#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>

const char* HOSTNAME = "esp01-webuniswitch";
boolean wifiInitiated = false;
const int MAX_WIFI_CONNECTION_TRIALS = 42;

ESP8266WiFiMulti wifiMultiClient;

boolean setupWifi();

