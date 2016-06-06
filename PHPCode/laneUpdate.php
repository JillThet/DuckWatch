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

	$parts = parse_url($url);
	parse_str($parts['query'], $query);
	$pond_id = $query['pool'];
	$lane_id = $query['lane'];
	$status = $query['stat'];

	$sql = "UPDATE lilypad SET status = '$status' WHERE pond_id = '$pond_id' AND lane_id = '$lane_id'; ";

	if ($conn->query($sql) === TRUE) {
    	echo "Updated successfully";
	} else {
    	echo "Error: " . $sql . "<br>" . $conn->error;
	}

	$conn->close();
?>