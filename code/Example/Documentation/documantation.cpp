//This documentation wrote by LuPow132
//Feel free to use it
//This documentation is the merge of "MKR IoT Carrier Cheat Sheet" and "Sending Data over MQTT"

//IMPORT LIBRARY
//The library you need to import in case you want to use MKR board with carrier
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

//CARRIER
//defined Carrier in short
MKRIoTCarrier carrier;

//WIFI CONNECTION
//to connect wifi you need to give microcontroller wifi's Username and Password, so we need to defind it as a char
char ssid[] = "SSID";
char pass[] = "PASSWORD";

//MQTT, WIFI CLIENT
//Defind Wificlient and MQTT
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

//BROKER CONNECTION
/*broker is the cloud that we gonna send data to
  Normally we ues 
     => https://test.mosquitto.org/
      just for test with port of 1883 (MQTT, unencrypted, unauthenticated)

  for more infomation about how to use and port info please go to "https://test.mosquitto.org/"

*/

//example broker and port
const char broker[] = "91.121.93.94";
int        port     = 1883;

//CREATE TOPIC
//to defind topic we need to write it in char. You can use / to make a directory like github or folder!
const char topic_name[] = "topic_path";

//INTERVAL
//in this example we send data in every 1 second (can chnage)
const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

//SETUP
void setup() {
  //CARRIER SETUP
  carrier.noCase();
  carrier.begin();

  //SERIAL
  //Initialize serial and wait for port to open:
  //IMPORTANT Don't forget to remove while serial in case you connect it to other power source that not from pc
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  //WIFI
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  //CONNECTION
  //connection process
  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  //incase it can't connect it will give us an error to work with
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  //but if everything working well it will say success fully connect to network
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  //beep in case we don't have any display to display Serial so we can know it already connect to internet with no problem
  carrier.Buzzer.beep();

  //TOPIC SUBSCIBTION
  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  //subscribe to diffrent topic
  Serial.print("Subscribing to topic: ");
  Serial.println(light_toggle_topic);
  Serial.println();
  // subscribe to a topic
  mqttClient.subscribe(light_toggle_topic);
  mqttClient.subscribe(light_color_code_topic);

}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker and keep getting update info
  mqttClient.poll();

  //for interval
  unsigned long currentMillis = millis();

  //if it reach to interval time then we start publish data to topic
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //CARRIER SENSOR
    //get carrier temp and humid
    int temperature = carrier.Env.readTemperature();
    int humidity = carrier.Env.readHumidity();

    //PUBLISH INFO TO TOPIC
    //serial
    Serial.print("Sending message to topic: ");
    Serial.println(temperature_topic);
    Serial.println(temperature);
    
    Serial.print("Sending message to topic: ");
    Serial.println(humidity_topic);
    Serial.println(humidity);

    //MQTT
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(temperature_topic);
    mqttClient.print(temperature);
    mqttClient.endMessage();

    mqttClient.beginMessage(humidity_topic);
    mqttClient.print(humidity);
    mqttClient.endMessage();

    Serial.println();
  }
}

//On message it will awake this function "based on when you config the "mqttClient.onMessage(function_to_awake_when_message_in);"
void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
}
