#define BLYNK_TEMPLATE_ID "TMPL3agwXyTOh"
#define BLYNK_TEMPLATE_NAME "Low Cost Air Monitoring System"
#define BLYNK_AUTH_TOKEN "Sm_n92a31vfu65C9N3dLJZkpvnxjnje8"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- DHT ----------------
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---------------- MQ135 ----------------
#define MQPIN 34

// ---------------- WiFi ----------------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Timer
BlynkTimer timer;

void sendSensor()
{
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int air = analogRead(MQPIN);

  if (isnan(temp) || isnan(hum))
    return;

  String status;

  if (air < 1200)
    status = "Good";
  else if (air < 2500)
    status = "Moderate";
  else
    status = "Poor";

  // -------- OLED --------
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("Temp : ");
  display.print(temp);
  display.println(" C");

  display.setCursor(0,16);
  display.print("Hum  : ");
  display.print(hum);
  display.println("%");

  display.setCursor(0,32);
  display.print("Air  : ");
  display.println(air);

  display.setCursor(0,48);
  display.print("Status: ");
  display.println(status);

  display.display();

  // -------- Serial --------
  Serial.println("----------------------");
  Serial.print("Temperature : ");
  Serial.println(temp);

  Serial.print("Humidity    : ");
  Serial.println(hum);

  Serial.print("Air Quality : ");
  Serial.println(air);

  Serial.print("Status      : ");
  Serial.println(status);

  // -------- Blynk --------
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, air);
  Blynk.virtualWrite(V3, status);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21,22);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Failed");
    while (true);
  }

  display.clearDisplay();
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi Connected!");

  // Connect to Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);

  while (!Blynk.connect()) {
    delay(500);
  }

  Serial.println("Connected to Blynk!");

  timer.setInterval(2000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}