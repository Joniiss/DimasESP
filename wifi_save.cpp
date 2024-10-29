#include "wifi_save.h"

WiFiServer server(80);
int client_count = 0;

int record_rst_time()
{
    int rst_time = 0;
    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;                                 // 定义不透明句柄
    err = nvs_open("storage", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Read
        printf("Reading restart counter from NVS ... ");
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("Restart counter = %d\n", restart_counter);
            rst_time = restart_counter;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Write
        printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }

    printf("\n");
    return rst_time;
}

void record_wifi(char *ssid, char *password, char *nome, char *email)
{

    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Wifi (NVS) handle... ");
    nvs_handle my_handle;                              // 定义不透明句柄
    err = nvs_open("Wifi", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Write
        printf("Updating ssid in NVS ... ");
        err = nvs_set_str(my_handle, "ssid", ssid);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Updating password in NVS ... ");
        err = nvs_set_str(my_handle, "password", password);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    printf("\n");
    printf("Opening Non-Volatile Wifi (NVS) handle... ");
    nvs_handle my_handle2;                              // 定义不透明句柄
    err = nvs_open("Hub", NVS_READWRITE, &my_handle2); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Write
        printf("Updating HubId in NVS ... ");
        err = nvs_set_str(my_handle2, "hubid", nome);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Updating Hubmail in NVS ... ");

        char emailCerto[70];
        strcpy(emailCerto, String(email).substring(0, String(email).indexOf('%')).c_str());

        
        char emailCerto2[70];
        strcpy(emailCerto2, String(email).substring(String(email).indexOf('%') + 3).c_str());
        //char emailCerto2[70] = String(email).substring(String(email).indexOf('%') + 3)

        String mail = String(emailCerto) + "@" + String(emailCerto2);
        const char *email = mail.c_str();

        err = nvs_set_str(my_handle2, "hubmail", email);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle2);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle2);
    }


    printf("\n");
}

//检测ssid名称
void check_wifi(char *ssid, char *password)
{
    char saved_ssid[SSID_LENGTH];
    size_t ssid_length = SSID_LENGTH;
    char saved_password[SSID_LENGTH];
    size_t password_length = SSID_LENGTH;
    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Wifi (NVS) handle... \n");
    nvs_handle my_handle;                              // 定义不透明句柄
    err = nvs_open("Wifi", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Read
        printf("Reading ssid and password from NVS ... \n");

        err = nvs_get_str(my_handle, "ssid", saved_ssid, &ssid_length);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("ssid: %s\n", saved_ssid);
            printf("ssid length= %d\n", ssid_length);
            strcpy(ssid, saved_ssid);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        err = nvs_get_str(my_handle, "password", saved_password, &password_length);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("password: %s\n", saved_password);
            printf("password length= %d\n", password_length);
            strcpy(password, saved_password);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Close
        nvs_close(my_handle);
    }

    printf("\n");
    return;
}

void ap_init()
{
    //WiFi.softAP(ssid, password);
    Serial.println("Entrou aqui");
    WiFi.softAP("Dimas");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
}

int wifi_config_server()
{

    WiFiClient client = server.available(); // listen for incoming clients

    if (client) // if you get a client,
    {
        Serial.println("---------------------------------------------------");
        Serial.printf("Index:%d\n", client_count);
        client_count++;
        Serial.println("New Client."); // print a message out the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                if (c == '\n')
                { // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("<h1>Makerfabs</h1><br><h2>ESP32 WIFI CONFIG</h2><br>");
                        client.print("Click <a href=\"/wifi_set\">here</a> to set WIFI.<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
                //show wifiset page
                if (currentLine.endsWith("GET /wifi_set"))
                {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();

                    client.print("<h1>Makerfabs</h1><br><h2>ESP32 WIFI CONFIG</h2><br>");
                    client.print("<form action=\"/set_over\">SSID:<br><input type=\"text\" name=\"ssid\"><br>PASSWORD:<br><input type=\"text\" name=\"password\"><br>EMAIL:<br><input type=\"text\" name=\"email\"><br>NOME:<br><input type=\"text\" name=\"nome\"><br><br>");
                    client.print("<input type=\"submit\" value=\"Set\"></form>");
                    // The HTTP response ends with another blank line:
                    client.println();
                    // break out of the while loop:
                    break;
                }

                if (currentLine.endsWith("GET /set_over"))
                {
                    String get_request = "";
                    //read GET next line
                    while (1)
                    {
                        char c_get = client.read();
                        Serial.write(c_get);
                        if (c_get == '\n')
                        {
                            break;
                        }
                        else
                        {
                            get_request += c_get;
                        }
                    }

                    //set_wifi_from_url(server.uri());
                    set_wifi_from_url(get_request);

                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();

                    client.print("<h1>Makerfabs</h1><br><h2>ESP32 WIFI CONFIG</h2><br>");
                    client.print("Set Successful<br>");
                    client.println();

                    client.stop();
                    Serial.println("Client Disconnected.");

                    return 0;
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
    return 1;
}

void set_wifi_from_url(String get_url)
{
    //get_url = "http://192.168.4.1/set_over?ssid=Makerfabs&password=20160704"
    int str_len = 0;
    int ssid_add = 0;
    int pwd_add = 0;
    int end_add = 0;
    int email_add = 0;
    int nome_add = 0;
    int string3_end = 0;
    int string2_end = 0;

    String ssid = "";
    String pwd = "";
    String email = "";
    String nome = "";
    String string2 = "";
    String string3 = "";

    str_len = get_url.length();
    ssid_add = get_url.indexOf('?');
    pwd_add = get_url.indexOf('&');
    end_add = get_url.indexOf(' ');

    ssid = get_url.substring(ssid_add + 6, pwd_add);

    string2 = get_url.substring(pwd_add + 10, end_add);
    email_add = string2.indexOf('&');
    string2_end = string2.indexOf(' ');
    pwd = string2.substring(0, email_add);

    string3 = string2.substring(email_add + 7, string2_end);
    nome_add = string3.indexOf('&');
    string3_end = string3.indexOf(' ');
    email = string3.substring(0, nome_add);
    nome = string3.substring(nome_add + 6, string3_end);

    Serial.println();
    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    Serial.println("Get ssid and password from url:");
    Serial.println(get_url);
    Serial.println(ssid);
    Serial.println(pwd);
    Serial.println(email);
    Serial.println(nome);
    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

    record_wifi((char *)ssid.c_str(), (char *)pwd.c_str(), (char *)nome.c_str(), (char *)email.c_str());
}

int wifi_set_main()
{
    char ssid[SSID_LENGTH];
    char password[SSID_LENGTH];
    pinMode(WIFI_SET_PIN, INPUT_PULLUP);

    check_wifi(ssid, password);

    //3秒内拉低WIFI_SET_PIN则恢复出场设置并重启
    Serial.println("Check WIFI_SET_PIN");
    int runtime = millis();
    int starttime = runtime;
    while ((runtime - starttime) < 3000)
    {
        if (digitalRead(WIFI_SET_PIN) == LOW)
        {

            Serial.println("Please connect \"Dimas\".");
            Serial.println("And visit 192.168.4.1 to set WIFI.");
            ap_init();
            while (wifi_config_server())
                ;
            delay(3000);
            esp_restart();
            return 0;
        }
        Serial.print(".");
        delay(100);
        runtime = millis();
    }
    Serial.println();

    //Connect wifi
    Serial.println("Connecting WIFI");
    WiFi.begin(ssid, password);

    int connect_count = 0;

    //10S未连接上自动跳过并返回0
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        connect_count++;
        if (connect_count > 10)
            return 0;
    }

    // 成功连接上WIFI则输出IP并返回1
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    return 1;
}

void nvs_test()
{
    char ssid[SSID_LENGTH] = "";
    char password[SSID_LENGTH] = "";
    int rst_time = 0;

    check_wifi(ssid, password);
    rst_time = record_rst_time();

    sprintf(ssid, "ssid_%d", rst_time);
    sprintf(password, "password_%d", rst_time);

    record_wifi(ssid, password);

    // Restart module
    for (int i = 10; i >= 0; i--)
    {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}