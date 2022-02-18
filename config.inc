<?
	define('NOPAY', false);
	define('SIMPLE_BONUS', 0);	// simple bonus for day delegation in mhc per 100k delegation coins
	define('MHCRYPTO', true);
	define('NOAFF', true);		// true, if node not pay to delegators
	
	$rewardTime = 0;
	
// all amounts in microMHC
	$rewardAmount = 0;
	$fullSum = 0;	
	$rawActualSum = 0;
	$actualSum = 0;
	$per100kNode = 0;
	$per100kDeleg = 0;

	define('MHNET', "main");					// test, dev, main	
	define('MHDIV', 1000000);					// max coin division
	define('MAX_CAP', 10000000000000);
	define('NODE_UP', 1000000000);			// MHC frozen at start node
	define('SEND_SLEEP', 10);
	define('DAYSEC', 86400);
	define('OWNER_KEY', "NodeOwner");	

	define('NODE_REG_ADDR', '0x007a1e062bdb4d57f9a93fd071f81f08ec8e6482c3135c55ea');	
//	define('NODE_NAME_TPL', "[ #NODE# ] == #P10000000#% share reward for over 1kk delegations (#DATE# paid #A10000000# mhc per 100k) __ #P1000000#% reward for less than 1kk (#DATE# paid #A1000000# mhc per 100k) ____ Autopay daily at 00:01 UTC @XreNodes");

	define('NODE_NAME_TPL', "[ #NODE# ] == we pay #P1000000#-#P10000000#% == @XreNodes");
	
	$percents = array();
	$percents[1000000] = 90;
	$percents[10000000] = 95;
	
	$maxPercent = $percents[10000000];

	define('CLASS_DIR', SCRIPT_DIR . '/classes');
	
	define('COMMON_DIR', SCRIPT_DIR . '/..');
	
	define('OWNER_POOL_FILE', COMMON_DIR . "/owner_pool");
	define('PREV_REWARD_FILE', COMMON_DIR . "/prev_reward");
	define('PROFIT_DIR', COMMON_DIR . "/profit");
	define('MAIL_DIR', COMMON_DIR . '/mail');
	
	$partners = array();
	$partners["Xrenoder"] = 80;
	$partners["Vattern"] = 20;
	
	$mainPartner = "Xrenoder";
	
	define('PARENT_DIR', ROOT_DIR . '/..');
	
	define('DATA_DIR', ROOT_DIR . '/data');
	define('REPORTS_DIR', ROOT_DIR . '/reports');
	
	define('LOCK_FILE', ROOT_DIR . "/lock");
	define('LOG_FILE', ROOT_DIR . "/work.log");
	define('ERR_FILE', ROOT_DIR . "/work.err");
	define('EXCL_FILE', ROOT_DIR . "/exclude");
	define('REWARDS_FILE', REPORTS_DIR . "/rewards");
  
