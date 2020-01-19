<?php 
class Base
{
	private static $lock = null;
	private static $logFile = null;
	private static $errFile = null;
	
	public static function setLogs($logFile, $errFile) {
		static::$logFile = $logFile;
		static::$errFile = $errFile;
	}
	
	public static function isLocked() {
		if (static::$lock) return true;
		
		return false;
	}
	
	public static function lock($name) {
		if (static::$lock != null) {
				return;
		}
		
		static::$lock = fopen($name, "c+b");
		flock(static::$lock, LOCK_EX);
	}
	
	public static function end($message = null) {
		if ($message !== null) {
			static::log($message);
		}
		
		flock(static::$lock, LOCK_UN);
		fclose(static::$lock);

		exit;	
	}
	
	public static function log($message) {
		$dateStr = date("[Y-M-d H:i:s O] ");

		$fd = fopen(static::$logFile, "ab");
		flock($fd, LOCK_EX);
		fwrite($fd, $dateStr . $message . "\n");
		flock($fd, LOCK_UN);
		fclose($fd);
	}
	
	public static function err($message) {
		$dateStr = date("[Y-M-d H:i:s O] ");

		$fd = fopen(static::$errFile, "ab");
		flock($fd, LOCK_EX);
		fwrite($fd, $dateStr . $message . "\n");
		flock($fd, LOCK_UN);
		fclose($fd);
		
		static::end($message);
	}
	
	public static function fileWriteStr($str, $file)
	{
		if (is_file($file) && filesize($file)) {
			$str = "\n" . $str;
		}
				
		$fp = fopen($file, "a");
		flock($fp, LOCK_EX);
		fwrite($fp, $str);
		flock($fp, LOCK_UN);
		fclose($fp);
	}

	public static function fileRead($file)
	{
		if (!is_file($file)) return "";
//		clearstatcache();
		$len = filesize($file);
		if (!$len) return "";

		$fp = fopen($file, "r");
		flock($fp, LOCK_EX);
		$data = fread($fp, $len);
		flock($fp, LOCK_UN);
		fclose($fp);

		return $data;
	}
}
