<?php
if ($_GET) {
    $argument1 = $_GET['argument1'];
}
echo $argument1;
$myfile = fopen("WebServerResult.txt", "w") or die("Unable to open file!");
fwrite($myfile, $argument1);
fclose($myfile);
?>