/* GUIÃO PARA AULA DE IOT
Realizar o tutorial até finalizar a instalação e conhecer a IDE do Arduino.
https://www.usinainfo.com.br/blog/programar-esp32-com-a-ide-arduino-tutorial-completo/

a) Instalação da biblioteca ESP32.
    
Continuar o tutorial anterior.

 b) Instalação da biblioteca ESP8266:
https://www.robocore.net/tutorials/como-programar-nodemcu-arduino-ide

c) Instalar a biblioteca PubSubClient by Nick O’Leary

d) Instalar a biblioteca "ArduinoJson" -- Versão 7.3.1
*/

//UTILIZAR O NODE-MCU-1.0


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* mqtt_server = "broker.emqx.io";
const char* ssid = "agents";
const char* password = "QgC9O8VucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";

const int pino_led = D7;

WiFiClient espClient;
PubSubClient client(espClient);

long lastTime = 0;
const int Msg_Size = 50;
char msg[Msg_Size];
int value = 0;


void setup_wifi() {
  Serial.println("Starting Setup Wifi");
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to Wifi ");
   WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(String topic, byte* payload, unsigned int length) {
  
  String strPayload="";
  for (int i=0;i<length;i++) {
    strPayload = strPayload + (char)payload[i];  
  }
  
  Serial.println(strPayload);
  if (strPayload == "true") {
    digitalWrite(pino_led, HIGH);
  } else {
    digitalWrite(pino_led, LOW);
  }
  
  // Allocate the JSON document
  JsonDocument doc;
  
  DeserializationError error = deserializeJson(doc, strPayload);

  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.f_str());
    return;
  }

  if (topic == "IPB/IoT/Lab/Presence") {
    if (doc["Detection"] == 1) {
      digitalWrite(LED_BUILTIN, LOW);  // Turn on the LED
    } else {
      digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED
    }
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "AulaIoT-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if(client.connect(clientId.c_str())) {
      Serial.println("connected to the Broker");
      client.subscribe("IPB/IoT/Aula02/PG/LED"); //Tópico que o ESP subscreve. 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(pino_led, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  Serial.println("\n Start Setup");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}


void loop() {
  while(WiFi.status() != WL_CONNECTED){ //Check Wifi connection
    WiFi.reconnect(); //If disconnected, try to reconnect
  }
  if(!client.connected()) {//Check Check Broker connection
    Serial.println("Node disconnected of Broker. Trying to connect.. ");
    reconnect(); //try reconect to broker.    
  }
  client.loop();
  long now = millis();
  if (now - lastTime > 250) {
    lastTime = now;
    value = analogRead(A0);
    snprintf (msg, Msg_Size, "%d", value);
    Serial.println(msg);
    client.publish("IPB/IoT/Aula02/PG/Potenciometro", msg);
  }
}
