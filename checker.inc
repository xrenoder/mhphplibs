<?php
	if(!defined('NODENAME'))
		throw new Exception('`NODENAME` constant not defined');
	
	if(!defined('FROM_MAIL'))
		throw new Exception('`FROM_MAIL` constant not defined');	
		
	if(!defined('OWNER_KEY'))
		throw new Exception('`OWNER_KEY` constant not defined');
		
	if(!defined('MAX_CAP'))
		throw new Exception('`MAX_CAP` constant not defined');

	if(!defined('NODE_UP'))
		throw new Exception('`NODE_UP` constant not defined');		
		
	if(!defined('NOPAY'))
		throw new Exception('`NOPAY` constant not defined');
		
//	if(!defined('NOREWARD'))
//		throw new Exception('`NOREWARD` constant not defined');
		
//	if(!defined('SINGLE'))
//		throw new Exception('`SINGLE` constant not defined');
		
	if(!defined('SIMPLE_BONUS'))
		throw new Exception('`SIMPLE_BONUS` constant not defined');
		
	if(!defined('MHNET'))
		throw new Exception('`MHNET` constant not defined');
		
	if(!defined('MHDIV'))
		throw new Exception('`MHDIV` constant not defined');
		
	if(!defined('MHCRYPTO'))
		throw new Exception('`MHCRYPTO` constant not defined');
		
	if(!defined('SEND_SLEEP'))
		throw new Exception('`SEND_SLEEP` constant not defined');
		
	if(!defined('NODE_NAME_TPL'))
		throw new Exception('`NODE_NAME_TPL` constant not defined');
		
	if(!defined('NODE_REG_ADDR'))
		throw new Exception('`NODE_REG_ADDR` constant not defined');
		
	if(!defined('DAYSEC'))
		throw new Exception('`DAYSEC` constant not defined');
	
//	if(file_exists(DATA_DIR) == false)
//		throw new Exception('`data` directory not found');
	
//	if(file_exists(REPORTS_DIR) == false)
//		throw new Exception('`reports` directory not found');
	
	if(file_exists(KEY_FILE) == false)
		throw new Exception(KEY_FILE file not found');
	
	if(file_exists(OWNER_POOL_FILE) == false)
		throw new Exception('`owner_pool` file not found');	
		
//	if(file_exists(PREV_REWARD_FILE) == false)
//		throw new Exception('`prev_reward` file not found');
	
	if(floatval(phpversion()) < 7.1)
		throw new Exception('requirements PHP 7.1+, current ' . phpversion());
	
	if(extension_loaded('curl') == false)
		throw new Exception('php-curl extension not loaded ');
	
	if(in_array(MHNET, ['main', 'dev', 'test']) == false)
		throw new Exception('unsupported net value', 1);

	
	if(MHCRYPTO) {
		if(extension_loaded('mhcrypto') == false)
			throw new Exception('mhcrypto extension not loaded');
	} else {
		if(extension_loaded('gmp') == false)
			throw new Exception('php-gmp extension not loaded ');
		
		if(file_exists(SCRIPT_DIR . '/vendor/autoload.php') == false)
			throw new Exception('`vendor/autoload.php` not found  in ' . SCRIPT_DIR);
		
		if(file_exists(SCRIPT_DIR . '/vendor/mdanter/ecc/src/EccFactory.php') == false)
			throw new Exception('`mdanter/ecc` not found. Please run composer command `composer require mdanter/ecc:0.5.2`');
		
		require_once(SCRIPT_DIR  . '/vendor/autoload.php');
	}
