/*

*/
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include "Adafruit_VL6180X.h"

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char serverAddress[] = "dweet.io";  // server address
int port = 80;
bool handDetected = false;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

String getName = "miaoye-test";
String postName = "mmw451-test";

Adafruit_VL6180X vl = Adafruit_VL6180X();

int column[16] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, A6, A7 };
int layer[4] = { A3, A2, A1, A0 };

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  for (int i = 0; i < 16; i++) {
    pinMode(column[i], OUTPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(layer[i], OUTPUT);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);
    aroundEdgeDown();
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("Adafruit VL6180x test!");
  if (!vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1)
      ;
  }
  Serial.println("Sensor found!");

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  if (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);
    aroundEdgeDown();
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
  }

  int sensorValue = readSensor();

  if (sensorValue > 100) {
    if (!handDetected) {
      randomflicker();
    } else {
      post(0);
      handDetected = false;
      randomflicker();
    }
  } else {
    if (handDetected) {
      if (!get()) randomflicker();
      else {diagonalRectangle();}
    } else {
      handDetected = true;
      post(1);
      if (get()) diagonalRectangle();
    }
  }

  Serial.println("Wait 0.5 seconds\n");
  delay(50);
}

int readSensor() {
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  // if (status == VL6180X_ERROR_NONE) {
  //   Serial.print("Range: "); Serial.println(range);
  // }

  delay(10);
  return range;
}

void post(int value) {
  // assemble the path for the POST message:
  String postPath = "/dweet/for/" + postName;
  String contentType = "application/json";

  String postData = "{\"sensorValue\":\"";
  postData += value;
  postData += "\"}";

  Serial.println("making POST request");

  // send the POST request
  client.post(postPath, contentType, postData);
  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

bool get() {
  // assemble the path for the GET message:
  String getPath = "/get/latest/dweet/for/" + getName;

  // send the GET request
  Serial.println("making GET request");
  client.get(getPath);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  int labelStart = response.indexOf("content\":");
  // find the first { after "content":
  int contentStart = response.indexOf("{", labelStart);
  // find the following } and get what's between the braces:
  int contentEnd = response.indexOf("}", labelStart);
  String content = response.substring(contentStart + 1, contentEnd);
  Serial.println(content);

  // now get the value after the colon, and convert to an int:
  int valueStart = content.indexOf(":");
  String valueString = content.substring(valueStart + 1);
  int number = valueString.toInt();
  bool result = number == 1 ? true : false;
  return result;
}

void randomflicker() {
  turnEverythingOff();
  int x = 150;
  for (int i = 0; i != 50; i += 2) {
    int randomLayer = random(0, 4);
    int randomColumn = random(0, 16);

    digitalWrite(layer[randomLayer], 1);
    digitalWrite(column[randomColumn], 0);
    delay(x);
    digitalWrite(layer[randomLayer], 0);
    digitalWrite(column[randomColumn], 1);
    delay(x);
  }
}

void diagonalRectangle() {
  int x = 350;
  turnEverythingOff();
  for (int count = 0; count < 1; count++) {
    //top left
    for (int i = 0; i < 8; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[3], 1);
    digitalWrite(layer[2], 1);
    delay(x);
    turnEverythingOff();
    //middle middle
    for (int i = 4; i < 12; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[1], 1);
    digitalWrite(layer[2], 1);
    delay(x);
    turnEverythingOff();
    //bottom right
    for (int i = 8; i < 16; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[0], 1);
    digitalWrite(layer[1], 1);
    delay(x);
    turnEverythingOff();
    //bottom middle
    for (int i = 4; i < 12; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[0], 1);
    digitalWrite(layer[1], 1);
    delay(x);
    turnEverythingOff();
    //bottom left
    for (int i = 0; i < 8; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[0], 1);
    digitalWrite(layer[1], 1);
    delay(x);
    turnEverythingOff();
    //middle middle
    for (int i = 4; i < 12; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[1], 1);
    digitalWrite(layer[2], 1);
    delay(x);
    turnEverythingOff();
    //top right
    for (int i = 8; i < 16; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[2], 1);
    digitalWrite(layer[3], 1);
    delay(x);
    turnEverythingOff();
    //top middle
    for (int i = 4; i < 12; i++) {
      digitalWrite(column[i], 0);
    }
    digitalWrite(layer[2], 1);
    digitalWrite(layer[3], 1);
    delay(x);
    turnEverythingOff();
  }
  //top left
  for (int i = 0; i < 8; i++) {
    digitalWrite(column[i], 0);
  }
  digitalWrite(layer[3], 1);
  digitalWrite(layer[2], 1);
  delay(x);
  turnEverythingOff();
}

void turnEverythingOff() {
  for (int i = 0; i < 16; i++) {
    digitalWrite(column[i], 1);
  }
  for (int i = 0; i < 4; i++) {
    digitalWrite(layer[i], 0);
  }
}

void turnEverythingOn()
{
  for(int i = 0; i<16; i++)
  {
    digitalWrite(column[i], 0);
  }
  //turning on layers
  for(int i = 0; i<4; i++)
  {
    digitalWrite(layer[i], 1);
  }
}

void aroundEdgeDown() {
  for (int x = 50; x != 0; x -= 50) {
    turnEverythingOff();
    for (int i = 4; i != 0; i--) {
      digitalWrite(layer[i - 1], 1);
      digitalWrite(column[5], 0);
      digitalWrite(column[6], 0);
      digitalWrite(column[9], 0);
      digitalWrite(column[10], 0);

      digitalWrite(column[0], 0);
      delay(x);
      digitalWrite(column[0], 1);
      digitalWrite(column[4], 0);
      delay(x);
      digitalWrite(column[4], 1);
      digitalWrite(column[8], 0);
      delay(x);
      digitalWrite(column[8], 1);
      digitalWrite(column[12], 0);
      delay(x);
      digitalWrite(column[12], 1);
      digitalWrite(column[13], 0);
      delay(x);
      digitalWrite(column[13], 1);
      digitalWrite(column[15], 0);
      delay(x);
      digitalWrite(column[15], 1);
      digitalWrite(column[14], 0);
      delay(x);
      digitalWrite(column[14], 1);
      digitalWrite(column[11], 0);
      delay(x);
      digitalWrite(column[11], 1);
      digitalWrite(column[7], 0);
      delay(x);
      digitalWrite(column[7], 1);
      digitalWrite(column[3], 0);
      delay(x);
      digitalWrite(column[3], 1);
      digitalWrite(column[2], 0);
      delay(x);
      digitalWrite(column[2], 1);
      digitalWrite(column[1], 0);
      delay(x);
      digitalWrite(column[1], 1);
    }
  }
}