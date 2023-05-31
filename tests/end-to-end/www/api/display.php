<?php

$response = "<html><head><title>ENV</title></head><body><h1>ENV</h1><p>";
foreach (getenv() as $key => $value) {
    $response .= $key . " = " . $value . "\n";
}
$response .= "</p></body></html>";
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html\r\n";
echo "Content-Length: " . strlen($response) . "\r\n";
echo "\r\n";
echo $response;

?>