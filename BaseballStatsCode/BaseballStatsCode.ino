
#include <ESP8266WiFi.h>


#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <splash.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4);

/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "gotwins";//gotwins/Messiah Wifi/NotMntx
#define STAPSK  "ths6190501";//ths6190501/acceptableC0ffee/notpassword
#endif

char* ssid = STASSID;//gotwins/Messiah Wifi
char* password = STAPSK;//ths6190501/acceptableC0ffee

const char* host = "teamtrees.org";//djxmmx.net/20.102.87.150
//const uint16_t port = 8080;//17
unsigned long requestDue = 0;
int delayBetweenRequests = 10 * 1000;

void writeScreen(char* c, char* d){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(0,10,c);
  u8g2.drawStr(0,26,d);// write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void setup() {
  u8g2.begin();
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void makeHTTPRequest() {
  if (client.connected()){
    client.stop();
  }
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);


    client.setInsecure();
    return;
  }

  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/");
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  //delay(200);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }

  char treeData[32] = {0};

  // The count for the Tree is a property of the Div that eventually displays the count.
  // the innerHTML of the div starts as 0

  // Skip to the text that comes after "data-count=\""
  char treeString[] = "data-count=\"";
  if (!client.find(treeString))
  {
    Serial.println(F("Found no trees"));
    writeScreen("Found no trees");
    return;
  } else {
    // copy the data from the stream til the next "
    // thats out tree data
    client.readBytesUntil('\"', treeData, sizeof(treeData));
    Serial.print("#TeamTrees: ");
    Serial.println(treeData);
    writeScreen("#TeamTrees:", treeData);
  }
}
void loop() {
  unsigned long now = millis();
  if(requestDue < now){
    makeHTTPRequest();
    requestDue = now + delayBetweenRequests;
  }
}
