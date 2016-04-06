<?php
$page = $_SERVER['PHP_SELF'];
$sec = "3";
?>
<html>
    <head>
    <meta http-equiv="refresh" content="<?php echo $sec?>;URL='<?php echo $page?>'">
    </head>
    <body>
    <body bgcolor="#00FFFF">
    <font face="monospace" color="#000000">
    <link rel="stylesheet" type="text/css" href="style_01.css" />
    <h1>Igunos AP Combo Status</h1>
    <h2>Bandwidth</h2>
    <h3>
    <?php
	$A=file_get_contents('/media/webap/stbw');
        #echo nl2br(file_get_contents('/media/webap/stbw'));
        echo nl2br($A);
	echo("<title>$A</title>");
    ?>
    </h3>
    <h2>WVDIAL Keep Alive</h2>
    <?php
	/* Add redirection so we can get stderr. */
	$handle = popen('tail /tmp/runwvd.log 2>&1', 'r');
	//echo "'$handle'; " . gettype($handle) . "\n";
	$read = fread($handle, 2096);
	echo nl2br($read);
	pclose($handle);
    ?>
    <h2>WVDIAL</h2>
    <?php		
	/* Add redirection so we can get stderr. */
	$handle = popen('tail /tmp/wvdial.log 2>&1', 'r');
	//echo "'$handle'; " . gettype($handle) . "\n";
	$read = fread($handle, 2096);
	echo nl2br($read);
	pclose($handle);
    ?>
    <h2>Timer Switcher</h2>
    <?php		
	/* Add redirection so we can get stderr. */
	$handle = popen('tail /tmp/timerswitcher.log 2>&1', 'r');
	//echo "'$handle'; " . gettype($handle) . "\n";
	$read = fread($handle, 2096);
	echo nl2br($read);
	pclose($handle);
    ?>
    <h2>Leases</h2>
    <?php		
	/* Add redirection so we can get stderr. */
	$handle = popen('cat /var/lib/misc/dnsmasq.leases','r');
	//echo "'$handle'; " . gettype($handle) . "\n";
	$read = fread($handle, 2096);
	echo nl2br($read);
	pclose($handle);
    ?>
    </font>
    </body>
</html>
