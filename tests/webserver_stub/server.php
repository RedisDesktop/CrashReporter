<?php

ini_set("display_errors", 1);
ini_set("display_startup_errors", 1);

class Server {

    /**
* Default config
* @var array
*/
    private $config = array(
        'removeFilesOnNewRequest' => true,
    );

    /**
* @var string
*/
    private $requestsDir;

    private $router;


    public function __construct($requestsDir, array $config, array $router)
    {
        $this->config = array_merge($this->config, $config);
        $this->requestsDir = $requestsDir;
        $this->router = $router;
    }

    /**
* @return bool
*/
    public function canProcessRequest()
    {
        if (preg_match("/.*\.exe$/i", $_SERVER['REQUEST_URI'])) {
            return false;
        }

        return true;
    }

    /**
*
*/
    public function processRequest()
    {
        if ($this->config['removeFilesOnNewRequest']) {
            $this->cleanRequestsDir();
        }

        foreach ($this->router as $pattern => $callback) {
            if (preg_match($pattern, $_SERVER['REQUEST_URI'])) {
                return call_user_func($callback, $this);
            }
        }

        $this->dumpRequestInfo();
    }


    /**
* Remove all files from request dir
*/
    private function cleanRequestsDir()
    {
        foreach (new DirectoryIterator($this->requestsDir) as $fileInfo) {
            if(!$fileInfo->isDot()) {
                unlink($this->requestsDir . '/' . $fileInfo->getFilename());
            }
        }
    }

    /**
* Save current request info
*/
    public function dumpRequestInfo()
    {
        $dump = function ($dumpVar) {
            $parts = array();
            foreach ($dumpVar as $var => $val) {
                $parts[] = $var . '=' . $val;
            }
            return implode('&', $parts);
        };

        $requestInfo = 'REQUEST_URI: ' . $_SERVER['REQUEST_URI'] . PHP_EOL
            . 'POST:' . $dump($_POST) . PHP_EOL
            . 'GET:' . $dump($_GET) . PHP_EOL;

        file_put_contents($this->requestsDir . '/last.request', $requestInfo);

        echo <<< HTML
<h1>OK</h1>
<pre>{$requestInfo}</pre>
HTML;

    }
}

// --------------------------------------------------

$REQUEST_DIR = realpath(__DIR__ . '/requests');

$router = array(
    "/\/crash-report*/i"  => function () use ($REQUEST_DIR) {

	if (empty($_FILES['upload_file_minidump']))
		return '';

	
	$dumpName = $REQUEST_DIR . '/' . "last.dmp";

	if (move_uploaded_file($_FILES['upload_file_minidump']['tmp_name'], $dumpName)) {
		echo '{"ok":"OK! Thanks!"}';
	}

    return 'error';

    }
);

$server = new Server($REQUEST_DIR, array(), $router);

if ($server->canProcessRequest()) {
    $server->processRequest();
} else {
    return false;
}