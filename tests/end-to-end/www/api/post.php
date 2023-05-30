<?php

$line = trim(fgets(STDIN));
echo "Status: 200 OK\r\n";
echo "Content-Type: text/plain\r\n";
echo "Content-Length: " . strlen($line) . "\r\n";
echo "\r\n";
echo $line; 

?>