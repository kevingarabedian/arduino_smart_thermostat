#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define RELAY_PIN 3

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27, 16 chars and 2 line display

const int heat_temperature_setpoint = 20; // Set your desired temperature in Celsius

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Turn off the relay by default
  
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(2000); // Wait a few seconds between measurements

  // Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature(); // Read temperature in Celsius

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print temperature on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(t);
  lcd.print(" C");

  // Control the HVAC system
  if (t < heat_temperature_setpoint) {
    digitalWrite(RELAY_PIN, LOW); // Turn on the relay (start heating)
    lcd.setCursor(0, 1);
    lcd.print("Heating: ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Turn off the relay (stop heating)
    lcd.setCursor(0, 1);
    lcd.print("Heating: OFF");
  }
}
