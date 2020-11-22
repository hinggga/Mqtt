#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTT.h>

WiFiClient net;
MQTTClient client;
Servo servo;

const char ssid[] = "Wifi.id_Indomaret_Sagan";
const char pass[] = "otomatisnyambung";
int tmpPos = 0;
String LDR_value;
String pesan;


void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("Iot", "hingga21", "hingga257")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe("/servo");
  
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  String posisi = payload;
  int tmp = posisi.toInt();
  LDR_value=analogRead(36);
  pesan = LDR_value;
  Serial.print("Cahaya: ");
  Serial.println(LDR_value);
  client.publish("lux",pesan);
  
  if (tmp >= tmpPos ) {
    for (int i = tmpPos ; i <= tmp; i++) {
      servo.write(i);
      delay(15);
    }
    tmpPos = tmp;
    Serial.println("Posisi: ");
    Serial.println(tmpPos);
  } else if (tmp < tmpPos && tmp >= 0) {
    for (int j = tmpPos ; j >= tmp ; j--) {
      servo.write(j);
      delay(15);
    }
    tmpPos = tmp; 
    Serial.println("Posisi: ");
    Serial.println(tmpPos);
  } else if (tmpPos == tmp) {
    servo.write(tmp); //stand still
  }
  else {}
}

void setup() {
  Serial.begin(115200);
  pinMode(36,INPUT); 
  servo.attach(5);
  WiFi.begin(ssid, pass);
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);  
  connect();
  
}

void loop() {
  client.loop();
  delay(10);
  
  if (!client.connected()) {
    connect();
  }
}
