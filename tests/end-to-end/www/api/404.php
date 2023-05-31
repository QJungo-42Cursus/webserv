<?php

$message = "<html><body><h1>404 Not Found (from php)</h1></body></html>";
echo "Status: 404 Not Found\r\n";
echo "content-type:text/html\r\n";
echo "content-length: ".strlen($message)."\r\n";
echo "\r\n";
echo $message;
?>