#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"
#include "DHT.h"
#include "wifi_save.h"
#include "nvs_flash.h"
#include "nvs.h"

#define DHTPIN 5
#define DHTTYPE DHT22 

int thermoDO = 22;
int thermoCS = 23;
int thermoCLK = 4;

DHT dht(DHTPIN, DHTTYPE);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

char hubId[24] = "";


//const char* ssid = "Casa-437";
//const char* password = "93514509";

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

  if (wifi_set_main())
    {
        Serial.println("Connect WIFI SUCCESS");
    }
    else
    {
        Serial.println("Connect WIFI FAULT");
    }

  delay(500);

  
  char saved_hubId[24];
  size_t hubId_length = 24;
  char hubmail[70] = "";
  char saved_hubmail[70] = "";
  size_t hubmail_length = 70;

  esp_err_t erro = nvs_flash_init();
  if (erro == ESP_ERR_NVS_NO_FREE_PAGES || erro == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
      // 如果NVS分区被占用则对其进行擦除
      // 并再次初始化
      ESP_ERROR_CHECK(nvs_flash_erase());
      erro = nvs_flash_init();
  }
  ESP_ERROR_CHECK(erro);

  // Open 打开NVS文件
  printf("\n");
  printf("Opening Non-Volatile Wifi (NVS) handle... \n");
  nvs_handle my_handle;                              // 定义不透明句柄
  erro = nvs_open("Hub", NVS_READWRITE, &my_handle); // 打开文件
  if (erro != ESP_OK)
  {
      printf("Error (%s) opening NVS handle!\n", esp_err_to_name(erro));
  }
  else
  {
    printf("Done\n");

       // Read
    printf("Reading ssid and password from NVS ... \n");

    erro = nvs_get_str(my_handle, "hubid", saved_hubId, &hubId_length);
    switch (erro)
    {
      case ESP_OK:
        printf("Done\n");
        printf("hubId: %s\n", saved_hubId);
        strcpy(hubId, saved_hubId);
        break;
      case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");

        break;
      default:
        printf("Error (%s) reading!\n", esp_err_to_name(erro));
      }
    }

    erro = nvs_get_str(my_handle, "hubmail", saved_hubmail, &hubmail_length);
        switch (erro)
        {
        case ESP_OK:
            printf("Done\n");
            printf("hubmail: %s\n", saved_hubmail);
            strcpy(hubmail, saved_hubmail);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(erro));
        }
    Serial.print("TAMANHO DO HUBID");
    Serial.println(String(hubId).length());
    if (String(hubId).length() < 20) {
      HTTPClient http;
      http.begin("https://novousuario-5ajx76ozaq-rj.a.run.app");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST("{\"email\":\"" + String(hubmail) + "\"" + ",\"nome\":" + "\"" + String(hubId) + "\"" + "}");

      Serial.println("{\"email\":\"" + String(hubmail) + "\"" + ",\"nome\":" + "\"" + String(hubId) + "\"" + "}");

      Serial.println(httpCode);

      char novoId[24];
      strcpy(novoId, http.getString().c_str());
      strcpy(hubId, novoId);
      http.end();


      printf("Opening Non-Volatile Wifi (NVS) handle... ");
      nvs_handle my_handle2;                              // 定义不透明句柄
      erro = nvs_open("Hub", NVS_READWRITE, &my_handle2); // 打开文件
      if (erro != ESP_OK)
      {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(erro));
      }
      else
      {
        printf("Done\n");

        // Write
        printf("Updating HubId in NVS ... ");
        erro = nvs_set_str(my_handle2, "hubid", novoId);
        printf((erro != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Committing updates in NVS ... ");
        erro = nvs_commit(my_handle2);
        printf((erro != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle2);
      }
    }    
    delay(5000);


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
      /*
      http.begin("https://novousuario-5ajx76ozaq-rj.a.run.app");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST("{\"email\":" + String("\"joao.ferraz.balducci@gmail.com\"") + ",\"nome\":" + String("\"batatas\"") + "}");

      Serial.println("{\"email\":" + String("joao.ferraz.balducci@gmail.com") + ",\"nome\":" + String("batatas") + "}");

      Serial.println(httpCode);

      Serial.println(http.getString());

      http.end(); */

      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"tempAr\":" + String(t) + ",\"umAr\":" + String(h) + ",\"umSolo\":" + String(val) + ",\"tempSolo\":" + String(tSolo) + ",\"hubId\":" + "\"" + String(hubId) + "\"" + "}");

      Serial.println(http.getString());

      Serial.println("{\"tempAr\":" + String(t) + ",\"umAr\":" + String(h) + ",\"umSolo\":" + String(val) + ",\"tempSolo\":" + String(tSolo) + "}");

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