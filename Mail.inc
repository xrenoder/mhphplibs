<?php
/**
 * Компоновка и отправка email
 * 
 * @author kpopov
 */
class Mail {
	const TYPE_HTML = "text/html";
	const TYPE_PLAIN = "text/plain";
	
	const CHARSET_UTF8 = "utf-8";
	const CHARSET_WIN1251 = "windows-1251";
	const CHARSET_ISO88591 = "iso-8859-1";
	
	const EOL = "\r\n";
	
	const FILE = "f";
	const FILENAME = "fn";
	const CONTENT = "c";
	const CONTENT_TYPE = "ct";
	const INLINE_ID = "i";
	
	private static $returnMail = "test@mail.com";
	private static $contentType = self::TYPE_HTML;
	private static $attach = array();				// массив вложений в двух вариантах форматов:
													// [$i]['f'] - путь к присоединяемому файлу
													// [$i]['fn'] - имя файла для отображения в письме (опционально, по умолчанию - используется имя присоединяемого файла)
													// [$i]["ct"] - MIME-тип вложения
													// или
													// [$i]['c'] - содержимое вложения
													// [$i]['fn'] - имя файла для отображения в письме
													// [$i]["ct"] - MIME-тип вложения
	private static $charset = self::CHARSET_UTF8;
	
	/**
	 * Инициализация - установка обратного почтового адреса
	 * @param string $mail
	 */
	public static function init($mail) {
		static::setReturnMail($mail);
	}

	/**
	 * Скомпоновать и отправить мыло с использованием шаблонизатора
	 * 
	 * @param string $mail			адрес email
	 * @param string $tplSubj		имя файла шаблона заголовка email
	 * @param string $tplBody		имя файла шаблона тела email
	 */
//	public static function send($mail, $tplSubj, $tplBody, $themeTpl = null, $themeLoc = null, $returnMail = null)
	public static function send($mail, $subj, $body, $returnMail = null)
	{
/*		
		if (!XrenValid::check($mail, XrenValid::EMAIL, 1, false)) {
			XrenExcept::errStrSec("Invalid email '$mail'");
		}
		
		if ($themeTpl) {
			$oldTplTheme = XrenTpl::getTplTheme();
			XrenTpl::setTplTheme($themeTpl);
		}
		
		if ($themeLoc) {
			$oldTextTheme = XrenTpl::getTextTheme();
			XrenTpl::setTextTheme($themeLoc);
		}
		
		XrenTpl::tplCommentsOff();
		XrenEcho::useCacheOff();
		XrenTpl::jsonOff();
		$subj = XrenTpl::fromFile($tplSubj);
		$body = XrenTpl::fromFile($tplBody);
		XrenTpl::jsonRestore();
		XrenEcho::useCacheRestore();
		XrenTpl::tplCommentsRestore();
*/

		if ($returnMail) {
			$oldReturnMail = static::$returnMail;
			static::$returnMail = $returnMail;
		}
		
		static::createAndSend($mail, $subj, $body);
		
/*		
		if ($themeTpl) {
			XrenTpl::setTplTheme($oldTplTheme);
		}
		
		if ($themeLoc) {
			XrenTpl::setTextTheme($oldTextTheme);
		}
*/		
		if ($returnMail) {
			static::$returnMail = $oldReturnMail;
		}
	}
	
	/**
	 * Установка обратного адреса
	 * 
	 * @param string $mail
	 */
	public static function setReturnMail($mail)
	{
/*		
		if (!XrenValid::check($mail, XrenValid::EMAIL)) {
			XrenExcept::errStrSec("Invalid return email '$mail'");
		}
*/		
 		static::$returnMail = $mail;
	}
	
	/**
	 * Установка типа письма
	 * 
	 * @param string $type
	 */
	public static function setContentType($type)
	{
		static::$contentType = $type;
	}
	
	/**
	 * Установка кодировки письма
	 * 
	 * @param string $charset
	 */
	public static function setCharset($charset)
	{
		static::$charset = $charset;
	}
	
	/**
	 * Добавление файлов вложений 
	 * 
	 * @param string $file
	 * @param string $type
	 * @param string $fileName
	 */
	public static function addAttachFile($file, $type, $fileName = null, $inlineId = null)
	{
		$attach = array();
		$attach[static::FILE] = $file;
		$attach[static::CONTENT_TYPE] = $type;
		if ($fileName) {
			$attach[static::FILENAME] = $fileName;
		}
		
		if ($inlineId) {
			$attach[static::INLINE_ID] = $inlineId;
		}
		
		static::$attach[] = $attach;
	}
	
	/**
	 * Добавление вложений напрямую, без файлов
	 * 
	 * @param string $content
	 * @param string $type
	 * @param string $fileName
	 */
	public static function addAttachContent($content, $type, $fileName, $inlineId = null)
	{
		$attach = array();
		$attach[static::CONTENT] = $content;
		$attach[static::CONTENT_TYPE] = $type;
		$attach[static::FILENAME] = $fileName;
		
		if ($inlineId) {
			$attach[static::INLINE_ID] = $inlineId;
		}
	
		static::$attach[] = $attach;
	}
	
	/**
	 * Удаление вложений
	 */
	public static function dropAttach()
	{
		static::$attach = array();
	}

	public static function createAndSend($mail, $subj, $body)
	{
		$mimeBoundary = md5(microtime());
		$headers = "";

//Common Headers
		$headers .= 'From: ' . static::$returnMail . self::EOL;
		$headers .= 'Reply-To: ' . static::$returnMail . self::EOL;
		$headers .= 'Return-Path: ' . static::$returnMail . self::EOL;    // these two to set reply address

//Boundry for marking the split & Multitype Headers
		$headers .= 'MIME-Version: 1.0' . self::EOL;
//		$headers .= "Content-Type: multipart/mixed; boundary=\"" . $mimeBoundary . "\"" . self::EOL;
		$headers .= "Content-Type: multipart/related; boundary=\"" . $mimeBoundary . "\"" . self::EOL;

		$msg = "";
		$msg .= "--" . $mimeBoundary . self::EOL;
		$msg .= "Content-Type: " . static::$contentType . "; charset=" . static::$charset . self::EOL;
		$msg .= "Content-Transfer-Encoding: 8bit" . self::EOL . self::EOL;
		$msg .= $body . self::EOL . self::EOL;

// Attachment
		foreach(static::$attach as $attach) {
			if (!isset($attach[static::CONTENT_TYPE])) {
				continue;
			}
		
			if (isset($attach[static::FILE]) && is_file($attach[static::FILE])) {
				if (isset($attach[static::FILENAME])) {
					$fileName = $attach[static::FILENAME];
				} else {
					$fileName = substr($attach[static::FILE], (strrpos($attach[static::FILE], "/") + 1));
				}
		
				$fp = fopen($attach[static::FILE], 'rb');
				$content = fread($fp, filesize($attach[static::FILE]));
				$content = chunk_split(base64_encode($content));				//Encode The Data For Transition using base64_encode();
				fclose($fp);
			} else if (isset($attach[static::CONTENT]) && isset($attach[static::FILENAME])) {
				$content = chunk_split(base64_encode($attach[static::CONTENT]));		//Encode The Data For Transition using base64_encode();
				$fileName = $attach[static::FILENAME];
			}
			
			if (isset($attach[static::INLINE_ID])) {
				$disposition = "inline"; 
			} else {
				$disposition = "attachment";
			}
		
			$msg .= "--" . $mimeBoundary . self::EOL;
			$msg .= "Content-Type: " . $attach[static::CONTENT_TYPE] . "; name=\"" . $fileName . "\"" . self::EOL;
			$msg .= "Content-Transfer-Encoding: base64" . self::EOL;
			
			if (isset($attach[static::INLINE_ID])) {
				$msg .= "Content-ID: <" . $attach[static::INLINE_ID] . ">" . self::EOL;
			}
			
			$msg .= "Content-Disposition: $disposition; filename=\"" . $fileName . "\"" . self::EOL . self::EOL; // !! This line needs TWO end of lines !! IMPORTANT !!
			$msg .= $content . self::EOL . self::EOL;
		}

// Finish message
		$msg .= "--" . $mimeBoundary . "--" . self::EOL . self::EOL;		// finish with two eol's for better security. see Injection.
		
//		XrenLog::file($msg);

// SEND
/*
		$logMessage = "Mail sended to '$mail' ('$subj')";
		XrenLog::log($logMessage, XrenLog::DEFAULT_MAIL_NODE, XrenLog::STATUS_MAIL);
*/		
		mail($mail, $subj, $msg, $headers, "-f" . static::$returnMail);

		return;
	}
}
