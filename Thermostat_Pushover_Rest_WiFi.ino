#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define DHTPIN D2
#define DHTTYPE DHT22
#define RELAY_PIN D1

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

const char* rest_token = "your_REST_TOKEN";

const char* pushover_user = "your_PUSHOVER_USER";
const char* pushover_token = "your_PUSHOVER_TOKEN";
const char* pushover_url = "https://api.pushover.net/1/messages.json";

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
AsyncWebServer server(80);

int heat_temperature_setpoint = 68; // Set your desired temperature in Fahrenheit
long lastMsg = 0;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();

  setup_wifi();

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("token") && request->getParam("token")->value() == rest_token) {
      float t = dht.readTemperature(true);
      String json = "{\"temperature\": " + String(t) + "}";
      request->send(200, "application/json", json);
    } else {
      request->send(401, "text/plain", "Unauthorized");
    }
  });

  server.on("/setpoint", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("token") && request->getParam("token")->value() == rest_token) {
      if (request->hasParam("value", true)) {
        String value = request->getParam("value", true)->value();
        heat_temperature_setpoint = value.toInt();
        request->send(200, "text/plain", "Setpoint updated");
      } else {
        request->send(400, "text/plain", "Missing value parameter");
      }
    } else {
      request->send(401, "text/plain", "Unauthorized");
    }
  });

  server.begin();
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

void sendPushoverNotification(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(pushover_url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String post_data = "token=" + String(pushover_token) + "&user=" + String(pushover_user) + "&message=" + String(message);

    int httpCode = http.POST(post_data);

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.printf("Error sending Pushover notification: %s", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

void loop() {
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float t = dht.readTemperature(true);

    if (isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.print(t);
    lcd.print(" F");

    if (t < heat_temperature_setpoint) {
      digitalWrite(RELAY_PIN, LOW); // Turn on the relay (start heating)
      lcd.setCursor(0, 1);
      lcd.print("Heating: ON");
      if (now - lastMsg > 2000) {
        sendPushoverNotification("Heating turned ON");
      }
    } else {
      digitalWrite(RELAY_PIN, HIGH); // Turn off the relay (stop heating)
      lcd.setCursor(0, 1);
      lcd.print("Heating: OFF");
      if (now - lastMsg > 2000) {
        sendPushoverNotification("Heating turned OFF");
      }
    }
  }
}

