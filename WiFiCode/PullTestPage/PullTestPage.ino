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

const char* host = "jillthetford.com";
const int httpPort = 80;
const int httpsPort = 443;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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

  Serial.println("[HTTP] begin...");
  http.begin("http://jillthetford.com/SeniorProject/today.php?pool=1");

  Serial.println("[HTTP] GET...");
  // Start connection and send HTTP Header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.print("[HTTP] GET... code: ");
    Serial.println(httpCode);
    

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
       String payload = http.getString();
       Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
