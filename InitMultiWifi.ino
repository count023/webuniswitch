#include <Arduino.h>



boolean setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  int i;
  for (i = 0; i < WLAN_DATA_LENGTH; i += 2) {
    const char* ssid = WLAN_DATA[i];
    const char* pass = WLAN_DATA[1 + i];
    wifiMultiClient.addAP(ssid, pass);
  }

  Serial.println("\nConnecting to WLAN");
  delay(20);

  int wifiConnectTrials = 0;
  while (wifiMultiClient.run() != WL_CONNECTED && wifiConnectTrials <= MAX_WIFI_CONNECTION_TRIALS) {
    delay(500);
    Serial.print(".");
    wifiConnectTrials++;
  }
  Serial.println();
  return (WiFi.status() == WL_CONNECTED);
}
