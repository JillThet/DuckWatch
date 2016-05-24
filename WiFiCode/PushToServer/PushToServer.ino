/*
 * Push Sensor information to mySQL Database
 * 
 * Created By:  Jill Thetford <jthetfor@calpoly.edu>
 * Date:        05/08/16
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


const char* ssid = "Jill's Phone";
const char* password = "7609022232";

/*
const char* ssid = "Infected";
const char* password = "9258302935";
*/

String url= "http://jillthetford.com/SeniorProject/";
String dbUpdateURL = "http://jillthetford.com/SeniorProject/databaseUpdate.php";
String lnUpdateURL = "http://jillthetford.com/SeniorProject/laneUpdate.php";

String surf_temp;
String sub_temp;
String ext_temp;
String humidity;
String windy;
String uv_ndx;
String ln_1_status;
String ln_2_status;

void setup() {
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * push_to_server takse in a url and establishes a connection using the url.
 */
void push_to_server(String url)
{  
  Serial.println();
  Serial.print("pushing url [");
  Serial.print(url);
  Serial.println("] to server...");
  // Create HTTP Client
  HTTPClient http;
  // setup connection
  http.begin(url);
  // actually start connection
  http.GET();
  // close connection
  http.end();
}

/*
 * read_pkt parses through a string to check if it is formatted correctly and
 * stores the values of the packet content appropriately
 * 
 * Expected packet format:
 * <[=surf_temp,sub_temp,ext_temp,humidity,windy,uv_ndx,ln_1_status,ln_2_status=]>
 */
bool read_pkt(String pkt) {
  int ndx_start = 0;
  int ndx_comma;
  int loop_cnt = 0;

  pkt.trim();
  
  // check for correct packet header and trailer
  if (pkt.startsWith("<[=") && pkt.endsWith("=]>"))
  {
    Serial.println("Packet formatted correctly!");
    // remove header from string
    pkt.remove(0,3);
    // remove trailor from string
    pkt.remove(pkt.indexOf("="));

    loop_cnt = 0;
    while ((loop_cnt < 8  && (ndx_comma = pkt.indexOf(",",ndx_start+1)) > ndx_start)
           || ((ndx_comma == -1) && (loop_cnt == 7)))
    {
      switch (loop_cnt)
      {
        case 0:
          surf_temp = pkt.substring(ndx_start, ndx_comma);
          break;
        case 1:
          sub_temp = pkt.substring(ndx_start, ndx_comma);
          break;
        case 2:
          ext_temp = pkt.substring(ndx_start, ndx_comma);
          break;
        case 3:
          humidity = pkt.substring(ndx_start, ndx_comma);
          break;
        case 4:
          windy = pkt.substring(ndx_start, ndx_comma);
          break;
        case 5:
          uv_ndx = pkt.substring(ndx_start, ndx_comma);
          break;
        case 6:
          ln_1_status = pkt.substring(ndx_start, ndx_comma);
          break;
        case 7:
          ln_2_status = pkt.substring(ndx_start);
          break;
        default:
          return false;
          break;
      }
      // update new start of substring
      ndx_start = ndx_comma + 1;
      // increment the loop count
      loop_cnt += 1;
    }
    // check that packet fully parsed
    if (loop_cnt < 8)
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

/*
 * build_and_send constructs the URLS for updating the databases on the webserver
 */
void build_and_send()
{
  // update the pond
  String pond_url = dbUpdateURL + "?pool=1&surf=" + surf_temp +
    "&sub=" + sub_temp + "&ext=" + ext_temp + "&hum=" + humidity +
    "&hum=" + humidity + "&wind=" + windy + "&uv=" + uv_ndx;
  push_to_server(pond_url);

  // update lillypad (lane) 1
  String lillypad_1_url = lnUpdateURL + "?pool=1&lane=1&stat=" + ln_1_status;
  push_to_server(lillypad_1_url);
  
  // update lillypad (lane) 2
  String lillypad_2_url = lnUpdateURL + "?pool=1&lane=2&stat=" + ln_2_status;
  push_to_server(lillypad_2_url);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0)
  {
    // allow data to fully be put onto line
    delay(1000);

    // read packet and check if formatted correctly
    if (read_pkt(Serial.readString()))
    {
      Serial.println("Packet Contents:");
      Serial.print("surf_temp: ");
      Serial.println(surf_temp);
      Serial.print("sub_temp: ");
      Serial.println(sub_temp);
      Serial.print("ext_temp: ");
      Serial.println(ext_temp);
      Serial.print("humidity: ");
      Serial.println(humidity);
      Serial.print("windy: ");
      Serial.println(windy);
      Serial.print("uv_ndx: ");
      Serial.println(uv_ndx);
      Serial.print("ln_1_status: ");
      Serial.println(ln_1_status);
      Serial.print("ln_2_status: ");
      Serial.println(ln_2_status);
      // build URLs and send to server
      build_and_send(); 
    }
  }
}
