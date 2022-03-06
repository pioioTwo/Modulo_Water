#ifdef MQTT
#ifndef WiFi_h
#include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

unsigned long mqttTimeFail = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// EDIT THESE LINES TO MATCH YOUR SETUP
//#define MQTT_SERVER "192.168.1.201" //you MQTT IP Address
//#define mqtt_user "admin"
//#define mqtt_password "admin"
//#define mqtt_port 1883

#define CMD "cmd"
#define STATE "state"

void send_SwitchState(String _topic, boolean _val)
{
    String val = _val ? "ON" : "OFF";
    String _switch_state = _topic + STATE;
#ifdef DEBUG_SERIAL
    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc["state"] = _val ? "ON" : "OFF";
    serializeJson(doc, buffer);
    Serial.print("Send MQTT State Switch ");
    Serial.print(_switch_state);
    Serial.print(" > ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    // client.publish(_switch_set.c_str(), String(val).c_str());
    client.publish(_switch_state.c_str(), String(val).c_str());
}

void send_SwitchCmd(String _topic, boolean _val)
{
    String val = _val ? "ON" : "OFF";
    String _switch = _topic + CMD;
#ifdef DEBUG_SERIAL
    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc["set"] = _val ? "ON" : "OFF";
    serializeJson(doc, buffer);
    Serial.print("Send MQTT set Switch ");
    Serial.print(_switch);
    Serial.print(" > ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(_switch.c_str(), String(val).c_str()); // modelo normal
}

void send_SensorBinaryState(String _topic, String _sensor, boolean _state)
{
    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc[_sensor.c_str()] = _state ? "ON" : "OFF";
    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Send MQTT Sensor ");
    Serial.print(_topic);
    Serial.print(" > ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(_topic.c_str(), buffer, n);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String msg;
    // obtem a string do payload recebido
    for (size_t i = 0; i < length; i++)
    {
        char c = (char)payload[i];
        msg += c;
    }
    String topicStr = topic;
#ifdef DEBUG_SERIAL
    Serial.print("\nTopico Recebido: ");
    Serial.println(topicStr + ", Msg = " + msg);
#endif
    for (size_t i = 0; i < NUM_SWITCHS; i++)
    {
        String _switch_cmd, _switch_state;
        _switch_cmd = topicSwitch[i] + CMD;
        //_switch_state = topicSwitch[i] + STATE;
        if ((topicStr == _switch_cmd))
        { // recebi o topico da url
            stateOutput[i] = (msg == "ON") ? true : false;
        }
    }
}

// envia uma String, estou enviando o IP do dispositivo
void send_String(String _topic, String _value)
{
    String topicSend = _topic + STATE;
#ifdef DEBUG_SERIAL
    Serial.println(topicSend);
#endif
    client.publish(topicSend.c_str(), _value.c_str()); // modelo normal
}

/* Envia dados float
0 = Status do dispositivo
1 = Nivel do sinal do wifi
2 = Temperatura da placa
*/
void sendMQTT_Int(int _topic, int _val)
{
    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc[nameSensor[_topic].c_str()] = _val;
    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Send MQTT Int ");
    Serial.print(topicSensor[_topic]);
    Serial.print(" > ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(topicSensor[_topic].c_str(), buffer, n);
}

/* Envia dados float
0 = Status do dispositivo
1 = Nivel do sinal do wifi
2 = Temperatura da placa
*/
void sendMQTT_Float(int _topic, float _val)
{
    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc[nameSensor[_topic].c_str()] = float(_val);
    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Send MQTT Float ");
    Serial.print(topicSensor[_topic]);
    Serial.print(" > ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(topicSensor[_topic].c_str(), buffer, n);
}

/* envia descoberta dos sensores analogicos int ou float */
void sendMQTTDiscoverySensor(String _name_module, String _sensor, String _topicState, String _unit_mensure, String _dev_class)
{
    // String discoveryTopic = "homeassistant/sensor/ nome modulo " + String(sensorNumber) + "/humidity/config";
    String discoveryTopic = "homeassistant/sensor/" + _name_module + "/" + _sensor + "/config";
    int val_dev_class = _dev_class.toInt();

    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc["name"] = _name_module + "_" + _sensor;
    if (val_dev_class > 0)
        doc["dev_cla"] = dev_class[val_dev_class];
    if (sizeof(_unit_mensure) > 0)
        doc["unit_of_meas"] = _unit_mensure;
    doc["stat_t"] = _topicState;
    doc["frc_upd"] = "true";
    doc["val_tpl"] = "{{ value_json." + _sensor + "|default(0) }}";

    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Discovery topic = ");
    Serial.println(discoveryTopic);
    Serial.print("payload = ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(discoveryTopic.c_str(), buffer, n);
}

/* envia descoberta dos sensores binarios */
void sendMQTTDiscoverySensorBinary(String _name_module, String _sensor, String _topicState, String _dev_class)
{
    // String discoveryTopic = "homeassistant/sensor/ nome modulo " + String(sensorNumber) + "/humidity/config";
    String discoveryTopic = "homeassistant/binary_sensor/" + _name_module + "/" + _sensor + "/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];
    doc["name"] = _name_module + "_" + _sensor;
    // if (sizeof(_dev_class) > 0)
    //     doc["dev_cla"] = dev_class[_dev_class.toInt()];
    doc["stat_t"] = _topicState;
    doc["frc_upd"] = "true";
    // doc["ret"] = "false";
    // doc["val_tpl"] = "{{ value_json." + _sensor + "|upper }}";
    // doc["val_tpl"] = "{{ value_json." + _sensor + "|state }}";
    doc["val_tpl"] = "{{ value_json." + _sensor + "|default(0) }}";

    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Discovery topic = ");
    Serial.println(discoveryTopic);
    Serial.print("payload = ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(discoveryTopic.c_str(), buffer, n);
}

/* Envia json descoberta de switch */
void sendMQTTDiscoverySwitch(String _name_module, String _switch, String _topic)
{
    //"homeassistant/switch/bedroom/config" -m '{"name": "bedroom", "command_topic": "homeassistant/switch/bedroom/set", "payload_on": "ON", "payload_off": "OFF", "availability_topic": "homeassistant/switch/bedroom/available", "state_topic": "homeassistant/bedroom/state"}'
    String discoveryTopic = "homeassistant/switch/" + _name_module + "/" + _switch + "/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];
    //{"~": "homeassistant/switch/irrigation", "name": "garden", "cmd_t": "~/set", "stat_t": "~/state"}
    doc["name"] = _name_module + "_" + _switch;
    doc["cmd_t"] = _topic + CMD;
    doc["stat_t"] = _topic + STATE;
    doc["ret"] = "true";
    size_t n = serializeJson(doc, buffer);
#ifdef DEBUG_SERIAL
    Serial.print("Discovery topic = ");
    Serial.println(discoveryTopic);
    Serial.print("payload = ");
    String output;
    serializeJson(doc, output);
    Serial.println(output);
#endif
    client.publish(discoveryTopic.c_str(), buffer, n);
    delay(10);
    String _switch_topic_cmd;
    _switch_topic_cmd = _topic + CMD;
    client.subscribe(_switch_topic_cmd.c_str()); // assina o topic de comando

    //_switch_topic = _topic + "state";
    // client.subscribe(_switch_topic.c_str());
    // client.unsubscribe(_switch_topic.c_str());
}

/*
Inicia as variaveis de topico do mqtt
 */
void initMqttVars()
{

#ifdef SENSORS
    /* Topico dos Sensores
    ========================================================================================= */
#ifdef DEBUG_SERIAL
    Serial.println("\n\nCriando Topicos  Sensores: ");
#endif
    for (int i = 0; i < SENSORS_AVAILABLE; i++)
    {
        topicSensor[i] = configModulo.topicGroup;
        topicSensor[i] += "/";
        topicSensor[i] += configModulo.nameModule;
        topicSensor[i] += "/";
        topicSensor[i] += nameSensor[i];
        topicSensor[i] += "/";
#ifdef DEBUG_SERIAL
        Serial.println(topicSensor[i]);
#endif
    }
#endif


#ifdef SENSORS_BINARY
/* Topico dos Sensores Binarios
========================================================================================= */
#ifdef DEBUG_SERIAL
    Serial.println("\n\nCriando Topicos Sensores Binarios: ");
#endif

    for (size_t i = 0; i < NUM_SENSORS_BINARY; i++)
    {
        topicSensorBinary[i] = configModulo.topicGroup;
        topicSensorBinary[i] += "/";
        topicSensorBinary[i] += configModulo.nameModule;
        topicSensorBinary[i] += "/";
        topicSensorBinary[i] += nameSensorBinary;
        topicSensorBinary[i] += i;
        topicSensorBinary[i] += "/";
#ifdef DEBUG_SERIAL
        Serial.println(topicSensorBinary[i]);
#endif
    }

#ifdef DEBUG_SERIAL
    Serial.println("\n\nCriando Topicos de inputPressed: ");
#endif

    for (size_t i = 0; i < NUM_INPUTS; i++)
    {
        topicInputPressed[i] = topicGroup;
        topicInputPressed[i] += "/";
        topicInputPressed[i] += topicName;
        topicInputPressed[i] += "/";
        topicInputPressed[i] += nameInputPressed;
        topicInputPressed[i] += i;
        topicInputPressed[i] += "/";
#ifdef DEBUG_SERIAL
        Serial.println(topicInputPressed[i]);
#endif
    }
#endif

#ifdef SWITCHS
/* Topico dos Sensores Binarios
========================================================================================= */
#ifdef DEBUG_SERIAL
    Serial.println("\n\nCriando Topicos Switchs: ");
#endif

    for (size_t i = 0; i < NUM_SWITCHS; i++)
    {
        topicSwitch[i] = configModulo.topicGroup;
        topicSwitch[i] += "/";
        topicSwitch[i] += configModulo.nameModule;
        topicSwitch[i] += "/";
        topicSwitch[i] += nameSwitch;
        topicSwitch[i] += i;
        topicSwitch[i] += "/";
#ifdef DEBUG_SERIAL
        Serial.println(topicSwitch[i]);
#endif
    }
#endif

#ifdef DEBUG_SERIAL
    Serial.println("\n");
#endif
}

// Inicializa O Client MQTT
void initMqtt()
{
    client.setCallback(callback);
    client.setServer(configModulo.mqtt_server.c_str(), configModulo.mqtt_port.toInt());
}

/* Envia a tensão para MQTT */

boolean connectMQTT()
{
    if (!client.connected())
    {
#ifdef DEBUG_SERIAL
        Serial.println("Tentando conectar ao MQTT");
#endif
        if (client.connect((char *)configModulo.nameModule.c_str(), configModulo.mqtt_user.c_str(), configModulo.mqtt_pass.c_str()))
        {
#ifdef DEBUG_SERIAL
            Serial.println("\nConectado ao MQTT \n ");
#endif

// Sensores
#ifdef SENSORS
#ifdef DEBUG_SERIAL
            Serial.println("\nEnviando descoberta dos sensores analogicos ");
#endif

            // Enviando descoberta dos sensores analogicos
            for (size_t i = 0; i < SENSORS_AVAILABLE; i++)
            {
                sendMQTTDiscoverySensor(configModulo.nameModule, nameSensor[i], topicSensor[i], sensor_unit_of_mensure[0][i], sensor_unit_of_mensure[1][i]);
                delay(50);
            }
            send_String(topicSensor[0], WiFi.localIP().toString()); // envia o IP do Dispositivos
            sendMQTT_Int(1, getRSSIasQuality(WiFi.RSSI()));         // envia a primeira informação da rede
            sendMQTT_Float(3,litros[0]);
            sendMQTT_Float(4,litros[1]);
#endif

// Sensores Binarios
#ifdef SENSORS_BINARY
#ifdef DEBUG_SERIAL
            Serial.println("\nEnviando descoberta dos sensores do binarios ");
#endif
            // Envia a descoberta dos sensores binarios
            for (size_t i = 0; i < NUM_SENSORS_BINARY; i++)
            {
                sendMQTTDiscoverySensorBinary(topicName, nameSensorBinary + i, topicSensorBinary[i], String(27));
                delay(50);
                send_SensorBinaryState(topicSensorBinary[i], nameSensorBinary + i, false);
            }

#ifdef DEBUG_SERIAL
            Serial.println("\nEnviando descoberta dos sensores Pressed ");
#endif
            for (size_t i = 0; i < NUM_INPUTS; i++)
            {
                sendMQTTDiscoverySensorBinary(topicName, nameInputPressed + i, topicInputPressed[i], "");
                delay(50);
                send_SensorBinaryState(topicInputPressed[i], nameInputPressed + i, atmega.pressedInput[i]);
            }
#endif

// Switchs
#ifdef SWITCHS
#ifdef DEBUG_SERIAL
            Serial.println("\nEnviando descoberta dos Switchs ");
#endif
            for (size_t i = 0; i < NUM_SWITCHS; i++)
            {
                // send_SwitchCmd(topicSwitch[i], stateOutput[i]);
                // delay(10);
                // send_SwitchState(topicSwitch[i], stateOutput[i]);
                // delay(10);
                sendMQTTDiscoverySwitch(configModulo.nameModule, nameSwitch + i, topicSwitch[i]);
            }
#endif
        }
        else
        {
#ifdef DEBUG_SERIAL
            Serial.println("Não conectou no mqtt");
#endif
            return false;
        }
    }
    return true;
}

/* Verifica se esta conectado ao broker mqtt */
boolean checkMqtt()
{
    boolean check = false;
    if (!client.connected())
    {
        if (millis() - mqttTimeFail > 5000)
        {
#ifdef DEBUG_SERIAL
            unsigned long timeDebug = millis();
            check = connectMQTT();
            mqttTimeFail = millis();
            Serial.print("Tempo de conexão MQTT ");
            Serial.println((millis() - timeDebug));
#else
            check = connectMQTT();
            mqttTimeFail = millis();
#endif
        }
        return check;
    }
    else
    {
        client.loop();
        return true;
    }
}

#endif