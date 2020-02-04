<?
	require_once(SCRIPT_DIR . "/loader.inc");

	Base::lock(LOCK_FILE);
	Base::setLogs(LOG_FILE, ERR_FILE);
	
try {
	require_once(SCRIPT_DIR . "/checker.inc");
	
	$now = time();
	$today = floor($now/DAYSEC) * DAYSEC;
	$yest = $today - DAYSEC;
	$preyest = $yest - DAYSEC;
	$tomor = $today + DAYSEC;
	
	$ecdsa = new Ecdsa();
	$crypto = new Crypto($ecdsa);
	$crypto->net = MHNET;
	
	$profitDate = gmdate("Y-M-d", ($today - (DAYSEC * 2)));
	$profitFile = PROFIT_DIR . "/" . $profitDate;
	
	if (NOAFF) {
		$profitFile = PROFIT_DIR . "/noaff";
	}
	
	if (!is_file($profitFile) || !filesize($profitFile)) {
		Base::log("All profits already shared");
		Base::end();
	}
	
	$dataPay = array();
	
	foreach($partners as $partner => $percent) {
		$partnerFile = PROFIT_DIR . "/" . $partner;
		if (!is_file($partnerFile)) {
			Base::end("No partner file for $partner");
		}
		
		$tmp = file($partnerFile);
		
		if (count($tmp) < 2) {
			Base::end("Bad partner file for $partner");
		}
		
		$dataPay[$partner] = array();
		$dataPay[$partner]['amount'] = 0;
		$dataPay[$partner]['desc'] = "";
		$dataPay[$partner]['addr'] = trim($tmp[0]);
		$dataPay[$partner]['mail'] = trim($tmp[1]);
		$dataPay[$partner]['problem'] = "";
	}
	
	$sumProfit = 0;
	$sumPartnersAmount = 0;
	$sumMainShared = 0;
	$sumMainAmount = 0;
	$sumStartBalance = 0;
	$sumEndBalance = 0;
	$sumPaid = 0;	
	$sumFee = 0;
	$sumEndBalance = 0;
	$problems = "";
	
	$okStr = "OK   ";
	$failStr = "FAIL ";
	$testStr = "TEST ";	
	
	$profits = file($profitFile);	
	
	foreach($profits as $val) {
		if (!trim($val)) {
			continue;
		}
		
		$tmp = explode("\t", trim($val));
		$nodeName = $tmp[0];
		$node = $tmp[1];
		$profit = $tmp[2];
		
		$keyFile = COMMON_DIR . "/" . $node . "/main_key"; 
		
		$nodePrivateKey = trim(Base::fileRead($keyFile));
	
		if (!$nodePrivateKey) {
			Base::err("Empty $node key file '$keyFile'");
		}
	
		$nodePublicKey = $ecdsa->privateToPublic($nodePrivateKey);
		$nodeAddress = $ecdsa->getAddress($nodePublicKey);
	
		$balanceData = $crypto->fetchBalance($nodeAddress);
		$nonce = (isset($balanceData['result']['count_spent'])) ? intval($balanceData['result']['count_spent']) + 1 : 1;
		$balance = $balanceData['result']['received'] - $balanceData['result']['spent'];
		
		while (!$balance) {
			Base::log("FAIL Cannot get balance. Change torrent.");
			sleep(SEND_SLEEP);
			$crypto->cleanHosts('TORRENT');
				
			$balanceData = $crypto->fetchBalance($nodeAddress);
			$nonce = (isset($balanceData['result']['count_spent'])) ? intval($balanceData['result']['count_spent']) + 1 : 1;
			$balance = $balanceData['result']['received'] - $balanceData['result']['spent'];
		}		
		
		if (NOAFF) {
			$profit = $balance;
		}
		
		$sumProfit += $profit;
		
		$dataPay[$node] = array();
		$dataPay[$node]['desc'] = $nodeName . " profit: " . ($profit /  MHDIV);
		$dataPay[$node][$mainPartner] = 0;
		$dataPay[$node]['start'] = 0;
		$dataPay[$node]['end'] = 0;
		$dataPay[$node]['paid'] = 0;
		$dataPay[$node]['fee'] = 0;
		
		if (!$profit) {
			Base::log("Zero profit at $node");
			continue;
		}
	
		$dataPay[$node]['start'] = $balance;
		$sumStartBalance += $balance;
		
		if ($profit > $balance) {
			Base::err("Not enough funds on $node: $balance less than $profit");
		}
		
// calculation
		
		$partnersAmount = 0;
		
		foreach($partners as $partner => $percent) {
			if ($partner == $mainPartner) {
				continue;
			}
			
			$amount = intval(ceil(($profit * $percent) / 100));
			$dataPay[$node][$partner] = $amount;
			$dataPay[$partner]['amount'] += $amount;
			$dataPay[$node]['desc'] .= " | $partner ($percent %) => " . ($amount / MHDIV);
			
			$partnersAmount += $amount;
		}
		
		$shared = intval($profit - $partnersAmount);
		$dataPay[$node]['desc'] .= " | $mainPartner => " . ($shared / MHDIV);
		$sumMainShared += $shared;
		
		$mainAmount = intval((floor(($balance - $partnersAmount) / MHDIV) * MHDIV));
		$dataPay[$node][$mainPartner] = $mainAmount;
		$dataPay[$mainPartner]['amount'] += $mainAmount;
		
		$sumPartnersAmount += $partnersAmount;
		$sumMainAmount += $mainAmount;
		
		foreach($partners as $partner => $percent) {
			if ($partner == $mainPartner) {
				continue;
			}
			
			$dataPay[$partner]['desc'] .= $dataPay[$node]['desc'] . "\n";
		}
		
		$dataPay[$mainPartner]['desc'] .= $dataPay[$node]['desc'] . " | Full $mainPartner Amount: " . ($mainAmount / MHDIV) . "\n";
		
//	make TX & send

		$nodePaid = 0;
		$nodeFee = 0;

		foreach($partners as $partner => $percent) {
			$statusStr = $okStr;
			
			$to = $dataPay[$partner]['addr'];
			$amount = $dataPay[$node][$partner];
			$dataStr = $dataPay[$node]['desc'];
			
			if (NOPAY) {
				Base::log("TX   " . $dataStr);
			}
			
			$fee = strlen($dataStr);
			$data = str2hex($dataStr);
			
			if (!NOPAY && $balance < ($amount + $fee)) {
				if ($balance < $amount) {
					$statusStr = $failStr;
					$dataPay[$partner]['problem'] = "Not enough funds on $node for pay " . ($amount / MHDIV) . " to '$partner'";
					$problems .= "\n" . $dataPay[$partner]['problem'];
					
					Base::log($statusStr . "Pay $amount to '$partner' fail (no funds)");
					
					continue;
				}
			
				$data = '';
				$fee = 0;			
			}
			
			$signText = $crypto->makeSign($to, strval($amount), strval($nonce), strval($fee), $data);
			$sign = $crypto->sign($signText, $nodePrivateKey);
			
			$notResolved = 1;
			
			while($notResolved) {
				$notResolved = 0;
		
				if (!NOPAY) {
					$res = $crypto->sendTx($to, $amount, $fee, $nonce, $data, $nodePublicKey, $sign);
				} else {
					$res = array();
					$res['result'] = 'ok';
					$res['params'] = '5bda789dee2675b0029cc6c19aeeba7c01c64123492417f21d377f34dab3acbd';
					$statusStr = $testStr;
				}
			
				if ($res['result'] != 'ok') {
					Base::log(var_export($res, true));
				
					$dataPay[$partner]['problem'] = "Sending from $node to '$partner' problem (" . $res['url'] . "): \n" . var_export($res, true);
					$problems .= "\n" . $dataPay[$partner]['problem'];

					$statusStr = $failStr;
				
					Base::log($statusStr . "Pay $amount to '$partner' from $node fail (can't send)");
				
					continue;
				}
			
				$txPay = $res['params'];
			
				if (!NOPAY) {
					sleep(SEND_SLEEP);
				}
		
				$res = $crypto->getTx($txPay);
			
				if (isset($res['error'])) {
					$historyData = $crypto->fetchHistory($nodeAddress);

					while(!isset($historyData['result'])) {
						Base::log("FAIL Cannot get fetch history. Change torrent.");
						sleep(SEND_SLEEP);
						$crypto->cleanHosts('TORRENT');
						$historyData = $crypto->fetchHistory($nodeAddress);
					}
					
					$res = $crypto->getTx($txPay);
					
					if (isset($res['error'])) {
						Base::log($statusStr . "Pay $amount from $node to '$partner' fail (not resolved '$txPay')");					
					
//						$dataPay[$partner]['problem'] = "Sending to '$partner' from $node problem (" . $res['url'] . "): transaction '$txPay' not resolved";
//						$problems .= "\n" . $dataPay[$partner]['problem'];
//						$statusStr = $failStr;
				
						$notResolved = 1;
						$crypto->cleanHosts('PROXY');
						continue;
					}					
				}
				
				Base::log($statusStr . "Pay $amount from $node to '$partner' suss ($txPay)");
			
				$dataPay[$node][$partner] = $txPay;
				$dataPay[$partner]['desc'] .= "$node Tx ID: $txPay\n";
			
				$dataPay[$node]['paid'] += $amount;
				$sumPaid += $amount;
				$dataPay[$node]['fee'] += $fee;
				$sumFee += $fee;
			
				$balanceData = $crypto->fetchBalance($nodeAddress);
				$nonce = (isset($balanceData['result']['count_spent'])) ? intval($balanceData['result']['count_spent']) + 1 : 1;
				$balance = $balanceData['result']['received'] - $balanceData['result']['spent'];
			
				while (!$balance) {
					Base::log("FAIL Cannot get balance. Change torrent.");
					sleep(SEND_SLEEP);
					$crypto->cleanHosts('TORRENT');
				
					$balanceData = $crypto->fetchBalance($nodeAddress);
					$nonce = (isset($balanceData['result']['count_spent'])) ? intval($balanceData['result']['count_spent']) + 1 : 1;
					$balance = $balanceData['result']['received'] - $balanceData['result']['spent'];
				}
			}
		}
		
		$dataPay[$node]['end'] = $balance;
		$sumEndBalance += $balance;
	}
	
	if (!$problems) {
		$problems = "NO PROBLEMS";
		
		if (!NOPAY && !NOAFF) {
			unlink($profitFile);
		}
	}
	
// mail	
	
	

	Mail::init(FROM_MAIL);
	Mail::setContentType(Mail::TYPE_PLAIN);
	$subjPrefix = NODENAME . " $profitDate for ";	
	
	foreach($partners as $partner => $percent) {
		$mail = $dataPay[$partner]['mail'];

		$mail = "";
		
		if (!$mail) {
			continue;
		}
		
		$reportStr = "Report for " . $profitDate;
		$reportStr .= "\nServer: " . NODENAME;
		$reportStr .= "\nPartner: " . $partner;
		$reportStr .= "\nPercent: " . $percent . "%";
		$reportStr .= "\nProblems: " . $problems;
		
		if ($partner == $mainPartner) {
			$reportStr .= "\nStart summary balance: " . ($sumStartBalance / MHDIV);
		}
		
		$reportStr .= "\nSummary profit: " . ($sumProfit / MHDIV);
		$reportStr .= "\nSummary minoritary shared: " . ($sumPartnersAmount / MHDIV);
		$reportStr .= "\nSummary majoritary shared: " . ($sumMainShared / MHDIV);
		
		if ($partner == $mainPartner) {
			$reportStr .= "\nSummary majoritary amount: " . ($sumMainAmount / MHDIV);
			$reportStr .= "\nSummary paid: " . ($sumPaid / MHDIV);
			$reportStr .= "\nSummary fee: " . ($sumFee / MHDIV);
			$reportStr .= "\nFinal summary balance: " . ($sumEndBalance / MHDIV);
		}
		
		$reportStr .= "\n\nDETAILS: \n" . $dataPay[$partner]['desc'];

		if ($partner == $mainPartner) {
			$reportStr .= "\n\nDUMP: \n" . var_export($dataPay, true);
		}
				
		Mail::send($mail, $subjPrefix . "$partner" , $reportStr);
		Base::log("Mail sended to '$partner' ($mail)");
	}
	
	Base::end();

} catch(Exception $e) {
	Base::err($e->getMessage());
}
