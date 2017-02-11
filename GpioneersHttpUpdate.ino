#include <Arduino.h>


boolean connectToHttpHost(const char* httpHost) {
  int connectionTrials = 0;
  boolean isConnected = wifiClient.connect(UPDATE_API_DOMAIN, 80);
  while (!isConnected && connectionTrials < MAX_HTTP_CONNECTION_TRIALS) {
    connectionTrials++;
    delay(50);
    isConnected = wifiClient.connect(UPDATE_API_DOMAIN, 80);
  }
  return isConnected;
}

String getApiDefaultHeaderFields() {
  String defaultHeaderStrings = "";
  defaultHeaderStrings += "x-ESP8266-STA-MAC: " + WiFi.macAddress() + HTTP_HEADER_LINEFOOT;
  defaultHeaderStrings += "x-ESP8266-AP-MAC: " + WiFi.softAPmacAddress() + HTTP_HEADER_LINEFOOT;
  defaultHeaderStrings += "x-ESP8266-chip-size: " + String(ESP.getFlashChipRealSize()) + HTTP_HEADER_LINEFOOT;
  defaultHeaderStrings += "x-ESP8266-chip-id: " + String(ESP.getFlashChipId()) + HTTP_HEADER_LINEFOOT;
  defaultHeaderStrings += "x-ESP8266-version: " + String(CURRENT_VERSION) + HTTP_HEADER_LINEFOOT;
  Serial.println("defaultHeaderStrings: \n" + defaultHeaderStrings);
  return defaultHeaderStrings;
}

void sendUpdateAuthorizationRequest() {

  if (connectToHttpHost(UPDATE_API_DOMAIN)) {
    String request = "";
    request += "POST /device/authenticate/" + WiFi.macAddress() + " HTTP/1.1" + HTTP_HEADER_LINEFOOT;
    request += "Host: " + String(UPDATE_API_DOMAIN) + HTTP_HEADER_LINEFOOT;
    request += "Authorization: Basic " + DEVICE_UPDATE_API_PASSWORD + HTTP_HEADER_LINEFOOT;
    request += getApiDefaultHeaderFields();
    request += "Content-Length: 0" + HTTP_HEADER_LINEFOOT;
    request += "Connection: close" + HTTP_HEADER_LINEFOOT;
    request += HTTP_HEADER_LINEFOOT;
    wifiClient.print(request);
    Serial.println("Sent authorization-request to " + String(UPDATE_API_DOMAIN));
  } else {
    Serial.println("!!!! no connection to " + String(UPDATE_API_DOMAIN) + " possible !!!!");
  }
  wifiClient.stop();
}

void sendUpdateRequest() {

  t_httpUpdate_return ret = ESPhttpUpdate.update(SERVER_UPDATE_URI, CURRENT_VERSION);

  /* */
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;

    default:
      Serial.print("Unknown Error: ");
      Serial.println(ret);
      break;
  }
  /* */
}

void httpUpdate() {
  sendUpdateAuthorizationRequest();
  sendUpdateRequest();
}
