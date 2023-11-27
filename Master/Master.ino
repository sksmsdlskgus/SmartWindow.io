#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Wi-Fi
const char* ssid = "LikeCloudy";
const char* password = "leejy0719";

volatile short humi_temp_rain[9] = {0,};

// Web
ESP8266WebServer server(80);

void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }
}

void handleRoot() {
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(2);  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */

  Wire.requestFrom(8, 18); /* request & read data of size 13 from slave */
  int i = 0;
  while(Wire.available()){
    char c = Wire.read();
    ((char *)humi_temp_rain)[i++] = c;
  }
  // Serial.print("Humi: ");
  // Serial.println(humi_temp_rain[0]);
  // Serial.print("Temp: ");
  // Serial.println(humi_temp_rain[1]);
  // Serial.print("Rain: ");
  // Serial.println(humi_temp_rain[2]);
  
  server.send(200, "text/plain", "{\"humi\":[" + String(humi_temp_rain[0], DEC) + ", " + String(humi_temp_rain[6], DEC) + ", " + String(humi_temp_rain[3], DEC) + "],\"temp\":[" + String(humi_temp_rain[1], DEC) + ", " + String(humi_temp_rain[7], DEC) + ", " + String(humi_temp_rain[4], DEC) + "], \"rain\":[" + String(humi_temp_rain[2], DEC) + ", " + String(humi_temp_rain[8], DEC) + ", " + String(humi_temp_rain[5], DEC) + "]}\r\n");
}

void handleMotor() {
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(1);  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */

  Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
  while(Wire.available()){
    char c = Wire.read();
  }
  server.send(200, "text/plain", "{\"humi\":[" + String(humi_temp_rain[0], DEC) + ", " + String(humi_temp_rain[6], DEC) + ", " + String(humi_temp_rain[3], DEC) + "],\"temp\":[" + String(humi_temp_rain[1], DEC) + ", " + String(humi_temp_rain[7], DEC) + ", " + String(humi_temp_rain[4], DEC) + "], \"rain\":[" + String(humi_temp_rain[2], DEC) + ", " + String(humi_temp_rain[8], DEC) + ", " + String(humi_temp_rain[5], DEC) + "]}\r\n");
}

void initHttp() {
  server.on("/api/values", handleRoot);
  server.on("/api/motor", handleMotor);
  server.begin();
}

void setup() {
  Serial.begin(115200); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  initWifi();
  initHttp();
}

void loop() {
  server.handleClient();
  MDNS.update();
}