/*
 MQTT IOT Example
 - continuously obtains values from the Virtuabotix DHT11 temperature/humidity sensor
 - formats the results as a JSON string for the IBM IOT example
 - connects to an MQTT server (either local or at the IBM IOT Cloud)
 - and publishes the JSON String to the topic named quickstart:MY_MAC_ADDRESS
 */

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update this to either the MAC address found on the sticker on your ethernet shield (newer shields)
// or a different random hexadecimal value (change at least the last four bytes)
byte mac[]    = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xAA };
char macstr[] = "deedbafefeaa";
// Note this next value is only used if you intend to test against a local MQTT server
byte localserver[] = {192, 168, 1, 23 };
// Update this value to an appropriate open IP on your local network
byte ip[] = {192, 168, 1, 177};

 

char servername[]="f8j9h3.messaging.internetofthings.ibmcloud.com";
String clientName = String("d:f8j9h3:ArduinoUno:") + macstr;
String topicName = String("iot-2/evt/status/fmt/json");
char username[]="use-token-auth";
char password[]="ajOhZ+U4OY)+*gLKC7"; 

float tempF = 0.0;
float tempC = 0.0;
int lightLvl = 0;

unsigned long time = 0;

EthernetClient ethClient;
// Uncomment this next line and comment out the line after it to test against a local MQTT server
//PubSubClient client(localserver, 1883, 0, ethClient);
PubSubClient client(servername, 1883, 0, ethClient);

void setup()
{
  analogReference(INTERNAL);
  // Start the ethernet client, open up serial port for debugging, and attach the DHT11 sensor
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  
}

void loop()
{
  char clientStr[40];
  clientName.toCharArray(clientStr,40);
  char topicStr[26];
  topicName.toCharArray(topicStr,26);
  getData();
  if (!client.connected()) {
    Serial.print("Trying to connect to: ");
    Serial.println(clientStr);
    client.connect(clientStr,username,password);
  }
  if (client.connected() ) {
    String json = buildJson();
    char jsonStr[200];
    json.toCharArray(jsonStr,200);
    boolean pubresult = client.publish(topicStr,jsonStr);
    Serial.print("attempt to send ");
    Serial.println(jsonStr);
    Serial.print("to ");
    Serial.println(topicStr);
    if (pubresult)
      Serial.println("successfully sent");
    else
      Serial.println("unsuccessfully sent");
  }
  delay(20000);
}

String buildJson() {
  String data = "{\"d\": {""\"temperature (F)\": ";
  data+=(int)tempF;
  data+=",\"light level\": ";
  data+=(int)lightLvl;
  data+="} }";
  return data;
  //return ("{\"d\": {\"msg\": \"Hello World\"}");
}

void getData() {
  lightLvl = analogRead(0);
  tempC = analogRead(1) / 9.31;
  tempF = 1.8 * tempC + 32;
}






