<?php
	include('config1.php');


    $myquery = "
SELECT  `time`, `temp1`, `voltval`, `currentval` , `laserint` , `lasersn` FROM  `labdata` ORDER BY time DESC LIMIT " . intval($_GET["lim"])
;
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