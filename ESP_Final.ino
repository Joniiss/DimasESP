#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"
#include "DHT.h"
#include "wifi_save.h"

#define DHTPIN 5
#define DHTTYPE DHT22 

int thermoDO = 22;
int thermoCS = 23;
int thermoCLK = 4;

DHT dht(DHTPIN, DHTTYPE);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);


const char* ssid = "Casa-437";
const char* password = "93514509";

const char* serverName = "https://recebemedidas-5ajx76ozaq-rj.a.run.app";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

/*
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  */

  if (wifi_set_main())
    {
        Serial.println("Connect WIFI SUCCESS");
    }
    else
    {
        Serial.println("Connect WIFI FAULT");
    }

  delay(500);

  dht.begin();
  delay(2000);

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;


      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
    
      int val;
      int val1 = 0;
      for(int i = 0; i < 5; i++) {
        val1 += analogRead(35);
        delay(100);
      }
      val = val1 / 5;

      float tSolo = thermocouple.readCelsius();

      int wreskein = 2;

      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"s1\":" + String(t) + ",\"s2\":" + String(val) + ",\"s3\":" + String(tSolo) + "}");

      Serial.println("{\"s1\":" + String(t) + ",\"s2\":" + String(val) + ",\"s3\":" + String(tSolo) + "}");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}


/*
char* medeDHT() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
}*/