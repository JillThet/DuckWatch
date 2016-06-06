<?php
	$servername = "159.203.220.177";
    $username = "duck";
    $password = "duckGOOSE";
    $dbname = "duckwatch";

	// Create connection
	$conn = new mysqli($servername, $username, $password, $dbname);
	// Check connection
	if ($conn->connect_error) {
    	die("Connection failed: " . $conn->connect_error);
	} 

	$url = "http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
	//echo $url;

	$parts = parse_url($url);
	parse_str($parts['query'], $query);
	$id = $query['pool'];

	$surface_H2O_temp = $query['surf'];
	$sub_H2O_temp = $query['sub'];
	$exterior_temp = $query['ext'];
	$humidity = $query['hum'];
	$windy = $query['wind'];
	$uv_index = $query['uv'];

	//$lane = $query['lane'];
	//$status = $query['stat'];

	$sql = "UPDATE pond SET surface_H2O_temp = '$surface_H2O_temp' WHERE id = '$id'; ";
	$sql .= "UPDATE pond SET sub_H2O_temp = '$sub_H2O_temp' WHERE id = '$id'; ";
	$sql .= "UPDATE pond SET exterior_temp = '$exterior_temp' WHERE id = '$id'; ";
	$sql .= "UPDATE pond SET humidity = '$humidity' WHERE id = '$id'; ";
	$sql .= "UPDATE pond SET windy = '$windy' WHERE id = '$id';";
	$sql .= "UPDATE pond SET uv_index = '$uv_index' WHERE id = '$id'; ";

	if ($conn->multi_query($sql) === TRUE) {
    	echo "Updated successfully\n";
	} else {
    	echo "Error: " . $sql . "<br>" . $conn->error . "\n";
	}

	$conn->close();
?>