#include <SPI.h>
#include <Ethernet.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#define MS_PROXY "quickstart.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_MAX_PACKET_SIZE 100

// Update these with values suitable for your network.
byte mac[] = { 0x00, 0xFF, 0xBB, 0xCC, 0xDE, 0x02 };
//The convention to be followed is d:quickstart:iotsample-arduino:<MAC Address>

#define MQTT_CLIENTID "d:quickstart:iotsample-arduino:00ffbbccde02"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
IPAddress ip(192, 168, 1, 177);

EthernetClient c; // replace by a YunClient if running on a Yun
IPStack ipstack(c);

MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

String deviceEvent;

void setup() {
  analogReference(INTERNAL);
  Serial.begin(9600);
  Ethernet.begin(mac, ip); //For Arduino Yun, use Bridge.begin()
  delay(1000);
}

void loop() {
  int rc = -1;
  if (!client.isConnected()) {
    Serial.println("Connecting to IoT Foundation for publishing Temperature");
    while (rc != 0) {
      rc = ipstack.connect(MS_PROXY, MQTT_PORT);
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = MQTT_CLIENTID;
    rc = -1;
    while ((rc = client.connect(data)) != 0)
    ;
    Serial.println("Connected successfully\n");
    Serial.println("Temperature(in C)\tDevice Event (JSON)");
    Serial.println("____________________________________________________________________________");
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0;
  message.retained = false;
  deviceEvent = String("{\"d\":{\"SavageIOT\":\"Arduino Uno\",\"temperature\":");
  char buffer[60];
  dtostrf(getTemp(),1,2, buffer);
  deviceEvent += buffer;
  deviceEvent += "}}";
  Serial.print("\t");
  Serial.print(buffer);
  Serial.print("\t\t");
  deviceEvent.toCharArray(buffer, 60);
  Serial.println(buffer);
  message.payload = buffer;
  message.payloadlen = strlen(buffer);
  rc = client.publish(MQTT_TOPIC, message);

  if (rc != 0) {
    Serial.print("return code from publish was ");
    Serial.println(rc);
  }
  client.yield(1000);

  delay(2000);
}

/*
This function is reproduced as is from Arduino site => http://playground.arduino.cc/Main/InternalTemperatureSensor
*/
double getTemp(void) {
  int lightLvl = analogRead(0);
  double tempC = analogRead(1) / 9.31;
  double tempF = 1.8 * tempC + 32;
  return (tempF);
}






