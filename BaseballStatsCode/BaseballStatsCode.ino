/*#include <MUIU8g2.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <splash.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0,  /*reset=/ 16,  /*clock=/ 5,  /*data=/ 4);
*/
/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "NotMntx"
#define STAPSK  "notpassword"
#endif

char* ssid = STASSID;//gotwins/Messiah Wifi
char* password = STAPSK;//ths6190501/acceptableC0ffee

const char* host = "20.102.87.150";//djxmmx.net
const uint16_t port = 8080;//17

void setup() {
  //u8g2.begin();
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

void loop() {
  static bool wait = false;

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
    client.println("GET / HTTP/1.1");
    client.println("Host: 20.102.87.150:8080");
    client.println("User-Agent: curl/7.83.1");
    client.println("Accept: */*");
    client.println("");
    //client.println(host);
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 15000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  /*while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }*/
  char data[32] = {0};
  client.readBytes(data, sizeof(data));
  Serial.println(data);

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  //displayStats("a", "b", 1, 2);
  
  if (wait) {
    delay(30000); // execute once every 5 minutes, don't flood remote service
  }
  wait = true;
  
}
/*
void displayStats(char* team1, char* team2, int score1, int score2){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //u8g2.drawStr(0,10, "a");  // write something to the internal memory
  u8g2.sendBuffer();
}*/
