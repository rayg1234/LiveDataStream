<?php
	include('config1.php');
    
	
	//$date = date_create_from_format('j-M-Y', '15-Feb-2009');
	//echo date_format($date, 'Y-m-d');
	//$date1 = strtotime($_GET["time"]);
	//echo date('Y/m/d H:i:s', $date1);
    	$date = date_create_from_format('Y-m-d:H:i:s', $_GET["time"]);
	//echo date_format($date, 'Y-m-d');
	if ($date == false)
		$newdate = '1';
	else
		$newdate = date_format($date, 'Y-m-d:H:i:s');

    $myquery = "
SELECT  `time`, `temp1`, `voltval`, `currentval` , `laserint` , `lasersn` FROM  `labdata` WHERE time > '" . $newdate . "'" . " ORDER BY time";
    $query = mysql_query($myquery);
    
    if ( ! $query ) {
        echo mysql_error();
        die;
    }
    
    $data = array();
    
    for ($x = 0; $x < mysql_num_rows($query); $x++) {
        $data[] = mysql_fetch_assoc($query);
    }
    
    echo json_encode($data);     
     
    mysql_close($server);
?>