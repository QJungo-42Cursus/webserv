<?php
    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        header("Location: " . $_SERVER['PHP_SELF']);
        $input = fgets(STDIN);

        parse_str($input, $output);
        if (isset($output['fname']) && isset($output['lname'])) {
            $fname = $output['fname'];
            $lname = $output['lname'];

            $file = './www/user_data.txt';
            $current = file_get_contents($file);

            // Append new data to the file
            $current .= "First Name: $fname, Last Name: $lname\n";

            // Write contents back to the file
            file_put_contents($file, $current);
            echo '<html><body><h1>Success!</h1><p>First Name: ' . $fname . ', Last Name: ' . $lname . '</p>';
            echo '<button onclick="window.location.href=\'index.html\';">Go Back</button>';
            echo '</body></html>';
            echo '<script type="text/javascript">
            preventDefault();
            </script>';
            exit();
        }
    } elseif ($_SERVER['REQUEST_METHOD'] == 'GET') {
        $file = './www/user_data.txt';

        // If the file exists, display its contents
        if (file_exists($file)) {
			$contents = file_get_contents($file);
            echo "<html>\n<body>\n<h1>User Data:</h1>\n<p>";
            echo nl2br($contents);
            echo '<button onclick="window.location.href=\'index.html\';">Go Back</button>';
            echo "</p>\n</body>\n</html>";
        }
    } elseif ($_SERVER['REQUEST_METHOD'] == 'DELETE') {
        $file = './www/user_data.txt';
        if (file_exists($file)) {
            $lines = file($file);
            array_pop($lines); // remove the last line
            file_put_contents($file, $lines);

            // Redirect back to the script
            echo '<script type="text/javascript">
            window.location.reload(true);
       </script>';
            exit();
        }
    }
?>