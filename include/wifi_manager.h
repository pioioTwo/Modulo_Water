
#ifdef SERVER
/* inicia o servidor assicrono para atualizar via ota */
void initServer()
{
  AsyncElegantOTA.setID(configModulo.nameModule.c_str());
#ifndef SENHA_OTA
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
#else
  AsyncElegantOTA.begin(&server, ota_login.c_str(), ota_password.c_str()); // Start ElegantOTA com senha
#endif

  // index
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              //String html = "Hello, world";
              //request->send(200, "text/plain", html);
              request->send(LittleFS, "/index.html", "text/html", false); });
  // color

  // These two callbacks are required for gen1 and gen3 compatibility
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                       {
    if (request->url() == "/colordata")
    {
      String body = String((char *)data);
      Serial.print(body);
      Serial.print(" Da url =");
      Serial.println(request->url());

      request->send(204, "");
      return;
    }
    // Handle any other body request here... });
  server.onNotFound([](AsyncWebServerRequest *request)
                    {
      String body = (request->hasParam("body", true)) ? request->getParam("body", true)->value() : String();

      // Handle not found request here... });
      initWebSocket();
#ifdef LITTLE_FS
      server.serveStatic("/", LittleFS, "/");
#endif
      server.begin();
} /* Fim do initServer */
#endif

#ifdef GERENCIADOR_WIFI
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

WiFiManager wm; // global wm instance

WiFiManagerParameter custom_TimeUpdate; //("Cycle", "TimeUpdate", TimeUpdate, 6);
WiFiManagerParameter custom_CycleTime;  //("CycleTime", "CycleTime", CycleTime, 6);
WiFiManagerParameter custom_IOT_CT_ID;  //("CT_ID", "DeviceID", DeviceID, 20);
WiFiManagerParameter custom_IOT_MAQ_ID; // Numero da maquina

String getParam(String name) // retorna a string do parametro adicionado no custom html
{
      // read parameter from server, for custom hmtl input
      String value;
      if (wm.server->hasArg(name))
      {
        value = wm.server->arg(name);
      }
      return value;
}
#else

#endif

/* checa a conexão wifi e gerencia */
void wifiConnection()
{
      if (WiFi.status() == WL_CONNECTED)
      {
        wifiConnected = true;
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
      }
      else
      {

#ifdef DEBUG_SERIAL
        if (wifiConnected)
        {
          Serial.println("Perda de Conexaõ Wifi\n");
        }
#endif
        wifiConnected = false;
        firstInit = false;
        static unsigned long timeConnectWifi = millis();
        if (millis() - timeConnectWifi > 12000)
        { // tenta reconectar se perda a conexão apos 12S
#ifdef DEBUG_SERIAL
          unsigned long timedebug = millis();
          Serial.println("Tentando reconectar forcado\n");
#endif
          WiFi.reconnect();
          timeConnectWifi = millis();
#ifdef DEBUG_SERIAL
          Serial.print("Time wifi = ");
          Serial.println(millis() - timedebug);
#endif
        }
      }

      // se o wifi esta conectado
      if (wifiConnected)
      {
        if (!firstInit)
        {
          // initServer(); // Se conectado ao WIfi faça inicio ou reinicio dos serviçoes de rede
#ifdef MQTT
          initMqtt();
          mqttTimeFail = millis();
#endif

#ifdef DEBUG_SERIAL
          Serial.printf("this is from %s\n", WiFi.localIP().toString().c_str());
#endif

          firstInit = true;
          for (size_t i = 0; i < 20; i++)
          {
            digitalWrite(LED, !digitalRead(LED));
            delay(50);
          }
        }
        else
        {
          // updateFirmwareloop();
        }
      }

// informações Necessarias não retire deste local
#ifdef MQTT
      if (wifiConnected)
      {
        mqttConnected = checkMqtt();
        if (mqttConnected)
        {
          static unsigned long timeSend = millis();
          unsigned timetoSend = 0;
          timetoSend = 30000; // 30s

          /* Envia os dados a cada x tempo */
          if ((millis() - timeSend > timetoSend))
          {
            sendMQTT_Int(1, getRSSIasQuality(WiFi.RSSI()));
            timeSend = millis();
          }
        }
      }
#endif
}