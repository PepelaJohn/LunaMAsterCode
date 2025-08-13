#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TempAndHumidity.h>
#include <BarometricPressure.h>
#include <AccelAndGyro.h>
#include <myosa.h>
MYOSA myosa;
TempAndHumidity Th;
BarometricPressure Bp;
AccelAndGyro Ag;
WiFiClientSecure client;

unsigned long previousMillis = 0;
const long perModuleInterval = 1500;
uint8_t nScreen = 0u;

const char* ssid = "iPhone";
const char* password = "8765432185";

const char* serverUrl = "https://track.lunadrone.com/api/telemetry";
const char* droneId = "LNDR-001";

const char* root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIIFATCCA+mgAwIBAgISBdEa0v1LsP3J1mlqcHBa6q/PMA0GCSqGSIb3DQEBCwUA\n"
"MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD\n"
"EwNSMTEwHhcNMjUwODA0MDU1NTExWhcNMjUxMTAyMDU1NTEwWjAeMRwwGgYDVQQD\n"
"ExN0cmFjay5sdW5hZHJvbmUuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n"
"CgKCAQEAqkr2UUHyKWai4FA99/fQqa6mMrVhYqYuIN4DIySMkEKyjyp8HXjsS/Ug\n"
"MhtQr1f902XuP9q4ZDEm3cO9RwYb4KKLMOaKkr7FeGxL0GDo/4HhRS8iQ4EkFxsG\n"
"ynoubVRorJ7gcfEwT368Zb+rl6hAsmA6L/sDn8h2JeeSEW3IX9i7zhXL8QUqi0Aw\n"
"Nv4GFHqTxcThfLpwHsBu32NJ9169FA7tY08m+t2JL9JwgXe03tBBU0O5I7lvhXPi\n"
"odOhI4/nilX02ShU/lTwEdK0s+SxCiaE+Qtf733BrcWCc2qnBc1Tmz8eJxjmTkNG\n"
"cNOyPWgHXJUaZmcCeBR8gOG9KoODCQIDAQABo4ICIjCCAh4wDgYDVR0PAQH/BAQD\n"
"AgWgMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAMBgNVHRMBAf8EAjAA\n"
"MB0GA1UdDgQWBBSgoJkp9zj4EXS2TgwycK/MJ1nGRzAfBgNVHSMEGDAWgBTFz0ak\n"
"6vTDwHpslcQtsF6SLybjuTAzBggrBgEFBQcBAQQnMCUwIwYIKwYBBQUHMAKGF2h0\n"
"dHA6Ly9yMTEuaS5sZW5jci5vcmcvMB4GA1UdEQQXMBWCE3RyYWNrLmx1bmFkcm9u\n"
"ZS5jb20wEwYDVR0gBAwwCjAIBgZngQwBAgEwLgYDVR0fBCcwJTAjoCGgH4YdaHR0\n"
"cDovL3IxMS5jLmxlbmNyLm9yZy84MC5jcmwwggEDBgorBgEEAdZ5AgQCBIH0BIHx\n"
"AO8AdQDM+w9qhXEJZf6Vm1PO6bJ8IumFXA2XjbapflTA/kwNsAAAAZhz239LAAAE\n"
"AwBGMEQCIDA4Yn8DTXAkfpWSf7aJ41XBbSOY5cutKdGowHEVE4O1AiBRrmF6Vfqc\n"
"r6XqWfj0bEZZkTGc0M/kYcYWY/j7M+XmmAB2ABLxTjS9U3JMhAYZw48/ehP457Vi\n"
"h4icbTAFhOvlhiY6AAABmHPbfzsAAAQDAEcwRQIhAOtB76QFm0veMFNkkqm7XGik\n"
"OWtnqorP7tG41AgSUQpTAiBQLUR6ITTc1Y8IscGrL+51YxGzecY8m+QQegedtS36\n"
"TDANBgkqhkiG9w0BAQsFAAOCAQEAQIrw2eqsp9Cfs2OkiHLAPlpLjS6gU2ZDeTg4\n"
"6wXrp2CP2ncoWIzjMf0OJMEVg1m8sEXFlwgKnO4LR7MKtFTjTuaOakCMfZ2ZXoL8\n"
"FjssfEXd+vpDdHPEobOHBejqnmAMsumZBKrieJBJ9FGYyQJK8rBtUzjIm6PlnG2+\n"
"eL3Eb0dDh5hhiHWylmdD+uQOxFGOLcMKnrPGTvAEwZFfrQDVC+Ubs26qNi+okdrw\n"
"Vy22xi4119yCsCC4NUgZzQo7TJnMc7iCklbarrbJOEGSvMQ6z9JG4VY0ZH5Rw4x9\n"
"6bIqrr00l6q3JzmOkvune4Ul24isQCmMpOGyxc/c2QUUYuib7A==\n"
"-----END CERTIFICATE-----\n";


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Serial.println(myosa.begin());

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= perModuleInterval) {
    previousMillis = currentMillis;
    switch (nScreen) {
      case 0u:
        myosa.printAceelAndGyro();
        nScreen = 1u;
        break;
      case 1u:
        myosa.printAceelAndGyro();
        nScreen = 2u;
        break;
      case 2u:
        myosa.printAceelAndGyro();
        nScreen = 3u;
        break;
      case 3u:
        myosa.printAceelAndGyro();
        nScreen = 4u;
        break;
      case 4u:
        myosa.printAirQuality();
        nScreen = 5u;
        break;
      case 5u:
        myosa.printBarometricPressure();
        nScreen = 6u;
        break;
      case 6u:
        myosa.printBarometricPressure();
        nScreen = 7u;
        break;
      case 7u:
        myosa.printLightProximityAndGesture();
        nScreen = 8u;
        break;
      case 8u:
        myosa.printLightProximityAndGesture();
        nScreen = 9u;
        break;
      case 9u:
        myosa.printTempAndHumidity();
        nScreen = 10u;
        break;
      case 10u:
        myosa.printTempAndHumidity();
        nScreen = 0u;
        break;
      default: nScreen = 0u; break;
    }

      float temperature = Th.getTempC(false);
      float pressure = Bp.getPressure();
      float altitude = Bp.getAltitude(1013.25, false);
      float speed = Ag.getAccelX();

      StaticJsonDocument<256> jsonDoc;
      jsonDoc["drone_id"] = droneId;
      jsonDoc["temperature"] = temperature;
      jsonDoc["pressure"] = pressure;
      jsonDoc["altitude"] = altitude;
      jsonDoc["speed"] = speed;

      sendToServer(jsonDoc);
    }
  // delay(2000);
}

void sendToServer(StaticJsonDocument<256>& jsonDoc) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected!");
    return;
  }

  
  client.setCACert(root_ca);

  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  int httpResponseCode = http.POST(jsonStr);
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.println("Server response: ");
    Serial.println(payload);
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}