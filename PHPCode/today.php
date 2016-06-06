<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"       
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">       
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">       
  <head>       
    <title>DuckWatch</title>       
    <meta http-equiv="content-type"       
        content="text/html; charset=utf-8"/>       
  </head>       
  <body>       
    <p>     
      <?php       
       $servername = "159.203.220.177";
       $username = "duck";
       $password = "duckGOOSE";
       $dbname = "duckwatch";

       echo "<center>";

       $conn = new mysqli($servername, $username, $password, $dbname) or die("Database Unavailable<br>\n");

       /*if($conn->connect_error) {
        die("connection failed: " . $conn->connect_error);
       }*/

       $url = "http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";

       $parts = parse_url($url);
       parse_str($parts['query'], $query);

       $pool = $query['pool'];

       //echo "Connected successfully! ";

       $sql = "SELECT * FROM pond WHERE id = '$pool'";
       $result = $conn->query($sql);

       if($result->num_rows > 0) {
          $row = $result->fetch_assoc();

          echo "<table>\n";

          echo "<th>Pool Id: " . $row["id"] . "</th>\n";
          echo "<tr>\n<td>Surface Temp:</td>\n<td>" . $row["surface_H2O_temp"] / 100 . "F</td>\n</tr>\n";
          echo "<tr>\n<td>Underwater Temp:</td>\n<td>" . $row["sub_H2O_temp"] / 100 . "F</td>\n</tr>\n";
          echo "<tr>\n<td>Outside Temp:</td>\n<td>" . $row["exterior_temp"] / 100 . "F</td>\n</tr>\n";
          echo "<tr>\n<td>Humidity:</td>\n<td>" . round($row["humidity"] / 1024) . "%</td>\n</tr>\n";

          if($row["windy"] > 0){
            echo "<tr><td>It's currently windy.</td></tr>\n";
          }
          else {
            echo "<tr><td>It's currently not windy.</td></tr>\n";
          }

          echo "<tr>\n<td>UV Index:</td>\n<td>" . $row["uv_index"] / 100 . "</td>\n</tr>\n";

          echo "</table>\n";
       }

       echo "\n<br><br><br>\n";

       $sql = "SELECT * FROM lilypad WHERE pond_id = '$pool'";
       $result = $conn->query($sql);
       
       $laneArr = array();

       echo "<table><tr>";
       if($result->num_rows > 0){
        while($row = $result->fetch_assoc()){
          echo "<td><table>\n";
          echo "<th>Lane id: " . $row["id"] . "</th>\n";

          if($row["status"] > 3){
            echo "<tr>\n<td>This lane is <b><font color=\"red\">occupied</font></b>.</td>\n</tr>\n";
          }
          else {
            echo "<tr>\n<td>This lane is <b><font color=\"green\">unoccupied</font></b>.</td>\n</tr>\n";
          }

          $laneArr[] = $row["status"];

          echo "<tr>\n<td>This lane is " . $row["depth"] / 100 . " in. deep.</td>\n</tr>\n";
          echo "<tr>\n<td>This lane is " . $row["length"] / 100 . " in. long.</td>\n</tr>\n";
          echo "</td></table>\n<br>";
        }
       }
       echo "</tr>\n<br>";

       echo "<tr>\n";

       for($ndx = 0; $ndx < 2; $ndx++){

          echo"<td><center><div style=\"width:100px;height:250px;background-color:";

          if($laneArr[$ndx] > 3){
            echo "#FF0000";
          }
          else{
            echo "#00FF00";
          }

          echo "\"></div></center></td>";
       }

       echo "</tr>\n</table></center>";

       $conn->close();       
      ?>       
    </p>       
  </body>       
</html>