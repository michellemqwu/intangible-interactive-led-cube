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

void setup() {
  Serial.begin(9600);
  while(!Serial);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
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

void loop() {
  int sensorValue = readSensor();

  if (sensorValue > 100) {
    if (!handDetected) {
      return;
    } else {
      post(0);
      handDetected = false;
      Serial.println("----------FLICKR-----------");
    }
  } else {
    if (handDetected) {
      if (!get()) Serial.println("--------FLICKER--------");
    } else {
      handDetected = true;
      post(1);
      if (get()) Serial.println("--------LIGHT UP LED CUBE--------");
    }
  }

  Serial.println("Wait 1.2 seconds\n");
  delay(1200);
}
