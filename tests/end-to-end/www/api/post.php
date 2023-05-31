<?php

$line = trim(fgets(STDIN));
$message = "<html><body><h1>hello</h1><p>you sent a GET request, good job</p><p>you sent me: " . $line . "</p> </body></html>";

echo "Status: 200 OK\r\n";
echo "Content-Type: text/plain\r\n";
echo "Content-Length: " . strlen($message) . "\r\n";
echo "\r\n";
echo $message;

?>