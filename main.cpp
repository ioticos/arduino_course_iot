#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid     = "yoyatedespedi";
const char* password = "qwertyuiop";

const char *mqtt_server = "m24.cloudmqtt.com";
const int   mqtt_port = 12480;
const char *mqtt_user = "pmxomwei";
const char *mqtt_pass = "GguSEks38Tbp";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido bajo el tópico -> ");
  Serial.print(topic);
  Serial.print("\n");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("\n Led Apagado");
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println("\n Led Encendido");
  }

  Serial.println();
}

void reconnect(){

  while (!client.connected()) {
    Serial.println("Intentando Conexión MQTT");

    String clientId = "iot_1_";
    clientId = clientId + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conexión a MQTT exitosa!!!");
      client.publish("salida", "primer mensaje");
      client.subscribe("entrada");
    }else{
      Serial.println("Falló la conexión ");
      Serial.print(client.state());
      Serial.print(" Se intentará de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup_wifi(){
  // Conexión a la red Wifi
  Serial.println();
  Serial.println("Conectando a...");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if(client.connected()==false){
    reconnect();
  }

  client.loop();

  if (millis() - lastMsg > 2000){
    lastMsg = millis();

    //Este valor podría ser una temeperatura por ejemplo
    value++;
    String mes = "Valor -> " + String(value);
    mes.toCharArray(msg, 50);
    client.publish("salida", msg);
    Serial.println("Mensaje enviado -> " + String(value));
  }

}
