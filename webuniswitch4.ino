/*
   webuniswitch4.ino on ESP-01 in preparation of http-update & multi wifi.
   war: webrcswitch3.ino for NodeMCU V3
   20.11.2016
   24.11.2016
   29.11.2016 // integrating infrared // for count023
     3.ino:  30.11.2016 with count 023
     4.ino:  11.02.2017 with count 023

   by taquma & count 023

   rcswitch based on:
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a switch depending on the request
      http://server_ip/sw1/0 will turn sw1 off
      http://server_ip/sw1/1 will turn sw1 on
      http://server_ip/swx/0 will turn sw1 to sw4 off
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
    https://github.com/thisoldgeek/ESP8266-RCSwitch/blob/master/esp8266_rcswitch.ino

    irswitch based on:
    IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend
   An IR LED must be connected to ESP8266 pin 0 (D3).
   Version 0.1 June, 2015
   Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
*/

#include "passwords.h"
#include "InitMultiWifi.h"
#include "GpioneersHttpUpdate.h"

#include <RCSwitch.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>

const int MAX_CLIENT_AVAILABLE = 200;
const int IR_LED_PIN = 0;
const int RC_SWITCH_PIN = 2;

RCSwitch mySwitch = RCSwitch();
IRsend irsend(IR_LED_PIN); //an IR led is connected to GPIO pin 0

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  irsend.begin();
  
  delay(10);

  // prepare GPIO0 for sending rcswitch-codes
  pinMode(RC_SWITCH_PIN, OUTPUT);

  // Transmitter is connected to ESP01 PIN 2 
  mySwitch.enableTransmit(RC_SWITCH_PIN);
  mySwitch.setPulseLength(350);

  // Connect to WiFi network
  wifiInitiated = setupWifi();
  if (wifiInitiated) {
    httpUpdate();
  }

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void loop() {

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  int clientAvailable = 0;
  while (!client.available()) {
    if (clientAvailable < MAX_CLIENT_AVAILABLE) {
      Serial.print("_");
      delay(3);
      clientAvailable++;
    } else {
      Serial.println();
      Serial.println("Aborting request");
      delay(10);
      return;
    }
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  int switchNumber = -1;
  if (req.indexOf("/sw1/0") != -1) {
    val = 0;
    switchNumber = 1;
    mySwitch.switchOff(1, 1);
  } else if (req.indexOf("/sw1/1") != -1) {
    val = 1;
    switchNumber = 1;
    mySwitch.switchOn(1, 1);
  }

  else if (req.indexOf("/sw2/0") != -1) {
    val = 0;
    switchNumber = 2;
    mySwitch.switchOff(1, 2);
    delay(300);
    Serial.println("LED-stripe AUS");
    irsend.sendNEC(0xFFF807, 36);
    Serial.println("TV AUS");
    irsend.sendNEC(0x804E7887, 36);  // TV aus
    delay(300);

  } else if (req.indexOf("/sw2/1") != -1) {
    val = 1;
    switchNumber = 2;
    mySwitch.switchOn(1, 2);
    delay(300);
    Serial.println("LED-stripe AN");
    irsend.sendNEC(0xFFB04F, 36);
    Serial.println("TV AN");
    delay(300);
    irsend.sendNEC(0x804E7887, 36);  // TV an
    delay(300);
  }

  else if (req.indexOf("/sw3/0") != -1) {
    val = 0;
    switchNumber = 3;
    mySwitch.switchOff(1, 3);
  } else if (req.indexOf("/sw3/1") != -1) {
    val = 1;
    switchNumber = 3;
    mySwitch.switchOn(1, 3);
  }

  else if (req.indexOf("/sw4/0") != -1) {
    val = 0;
    switchNumber = 4;
    mySwitch.switchOff(1, 4);
  } else if (req.indexOf("/sw4/1") != -1) {
    val = 1;
    switchNumber = 4;
    mySwitch.switchOn(1, 4);
  }

  else if (req.indexOf("/swx/0") != -1) {
    val = 0;
    switchNumber = -1;
    mySwitch.switchOff(1, 2);
    delay(500);
    mySwitch.switchOff(1, 3);
    Serial.write("All RCswitches off."); 
    Serial.print('/n');
    delay(500);
    Serial.println("LED-stripe AUS");
    irsend.sendNEC(0xFFF807, 36);
    mySwitch.switchOff(1, 1);
    delay(200);
    mySwitch.switchOff(1, 4);
    delay(500);
  }

  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html><head><title>webuniswitch</title><body><p>Switch";
  s += (switchNumber == -1) ? "es 1 to 4 are " : " " + String(switchNumber) + " is ";
  s += "now ";
  s += (val) ? "ON" : "OFF";
  s += "</p></body></html>\r\n";

  // Send the response to the client
  client.print(s);
  client.flush();
  delay(1);
  client.stop();
  Serial.println("Client disonnected");
  delay(10);

  // The client will actually be disconnected
  // when the function returns and 'client' object is destroyed
}

