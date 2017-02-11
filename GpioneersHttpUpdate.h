#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>

String HTTP_HEADER_LINEFOOT = "\r\n";

// gpioneers.com -> httpUpdate
const char* CURRENT_VERSION = "0.004.000"; // the most minimal version number available
const char* UPDATE_API_DOMAIN = "esp.gpioneers.com";
const String SERVER_UPDATE_URI = "http://" + String(UPDATE_API_DOMAIN) + "/device/update/" + String(WiFi.macAddress());

const int MAX_HTTP_CONNECTION_TRIALS = 11;

WiFiClient wifiClient;

boolean connectToHttpHost(const char* httpHost);
String getApiDefaultHeaderFields();
void sendUpdateAuthorizationRequest();
void sendUpdateRequest();
void httpUpdate();
