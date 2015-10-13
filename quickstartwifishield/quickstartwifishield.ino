#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#define MS_PROXY "quickstart.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_MAX_PACKET_SIZE 100

// Update these with values suitable for your network.
byte mac[] = { 0x11, 0xBB, 0xCC, 0xDD, 0xEF, 0x03 };
//The convention to be followed is d:quickstart:iotsample-arduino:<MAC Address>

#define MQTT_CLIENTID "d:quickstart:iotsample-arduino:11bbccddef03"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"

byte ip[]     = {192, 168, 1, 177 };
EthernetClient c; 
IPStack ipstack(c);

MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

String deviceEvent;
double tempF = 0.0;
double tempC = 0.0;
int lightLvl = 0;

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network\n");  
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
  Serial.println("\n");
  MQTT::Message message;
  message.qos = MQTT::QOS0;
  message.retained = false;
  deviceEvent = String("{\"d\":{\"myName\":\"Arduino Uno\",\"temperature\":");
  char buffer[60];
  // convert double to string 
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
}


double getTemp(void) {
  lightLvl = analogRead(0);
  tempC = analogRead(1) / 9.31;
  tempF = 1.8 * tempC + 32;
  return (tempF);
}
