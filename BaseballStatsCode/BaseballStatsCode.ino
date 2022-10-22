
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
#define STASSID "NotMntx"
#define STAPSK  "notpassword"
#endif

int pin1 = 15;
int pin2 = 13;
int pin3 = 12;

char* ssid = STASSID;//gotwins/Messiah Wifi
char* password = STAPSK;//ths6190501/acceptableC0ffee

const char* host = "20.102.87.150";//djxmmx.net
const uint16_t port = 8080;//17

void writeScreen(char* c, char* d){
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(0,10,c);
  u8g2.drawStr(0,26,d);// write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  
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
      delay(30000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  
  char data1[32] = {0};
  char data2[32] = {0};
  char base1[32] = {0};
  char base2[32] = {0};
  char base3[32] = {0};
  client.readBytesUntil('\n',data1, sizeof(data1));
  client.readBytesUntil('\n',data2, sizeof(data2));
  client.readBytesUntil('\n',base1, sizeof(base1));
  client.readBytesUntil('\n',base2, sizeof(base2));
  client.readBytesUntil('\n',base3, sizeof(base3));
  int baseint1 = (int)base1;
  int baseint2 = (int)base2;
  int baseint3 = (int)base3;
  Serial.println(data1);
  Serial.println(data2);
  Serial.println(baseint1);
  Serial.println(baseint2);
  Serial.println(baseint3);
  writeScreen(data1, data2);
  if (baseint1 == 1){
    digitalWrite(pin1, HIGH);
  } else {
    digitalWrite(pin1, LOW);
  }
  if (baseint2 == 1){
    digitalWrite(pin2, HIGH);
  } else {
    digitalWrite(pin2, LOW);
  }
  if (baseint3 == 1){
    digitalWrite(pin3, HIGH);
  } else {
    digitalWrite(pin3, LOW);
  }

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();

  
  if (wait) {
    delay(30000); // execute once every 5 minutes, don't flood remote service
  }
  wait = true;
  
}
