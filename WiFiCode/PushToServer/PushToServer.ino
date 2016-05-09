/*
 * Push Sensor information to mySQL Database
 * 
 * Created By:  Jill Thetford <jthetfor@calpoly.edu>
 * Date:        05/08/16
 */

#include <ESP8266WiFi.h>

#define DEBUG_HTTPCLIENT(...) Serial.println( __VA_ARGS__ )

#include <ESP8266HTTPClient.h>

/*
const char* ssid = "Jill's Phone";
const char* password = "7609022232";
*/

const char* ssid = "Infected";
const char* password = "9258302935";

const char* url= "http://jillthetford.com/SeniorProject/";
const char* dbUpdateURL = "http://jillthetford.com/SeniorProject/databaseUpdate.php";
const char* lnUpdateURL = "http://jillthetford.com/SeniorProject/laneUpdate.php";

/*
 * TODO
 * http post formation
 * serial reading to read in values to post
 * format networking packet....
 */



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Create HTTP Client
  HTTPClient http;

  Serial.print("[HTTP] begin: ");
  Serial.println("http://jillthetford.com/SeniorProject/laneUpdate.php?pool=1&lane=1&stat=1");
  http.begin("http://jillthetford.com/SeniorProject/laneUpdate.php?pool=1&lane=1&stat=1");

  // start connection and check if file exists on server
  Serial.println("[HTTP] GET...");
  int httpCode = http.GET();

  if (http.connected() && httpCode >= 200 && httpCode <= 300)
  {
    Serial.print("[HTTP] GET Code: ");
    Serial.println(httpCode);
    /*
    Serial.println("[HTTP] POST...");
    int ret_stat = http.POST("pool=1&lane=1&stat=1");
    http.writeToStream(&Serial);
    Serial.print("[HTTP] POST Ret: ");
    Serial.println(ret_stat);
    */
  } else
  {
    Serial.print("[HTTP] GET Code: ");
    Serial.println(httpCode);
    Serial.print("Not connected to ");
    Serial.println(lnUpdateURL);
  }
 
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
