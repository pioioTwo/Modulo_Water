
#ifdef SERVER

AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        if (strcmp((char *)data, "states") == 0)
        {
            //notifyClients(getOutputStates());
        }
        else
        {
            //int gpio = atoi((char *)data);
            //notifyClients(getOutputStates());
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

    switch (type)
    {
    case WS_EVT_CONNECT:
        //ip[0] = client->remoteIP().toString().c_str();
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebSocket()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}



/* Fica Buscando atualização no webserver */
void updateFirmwareloop()
{
    AsyncElegantOTA.loop();
}

#endif