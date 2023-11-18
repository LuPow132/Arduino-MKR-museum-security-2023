#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "TRIUMPHS";        // your network SSID (name)
char pass[] = "TMT27202";    // your network password (use for WPA, or use as key for WEP)
//wifi and pass U:TP-Link_CE5A P:16653092

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "91.121.93.94";
int        port     = 1883;
const char temperature_topic[]  = "sr/temperature";
const char humidity_topic[]  = "sr/humidity";
const char light_toggle_topic[] = "md/led";
const char light_color_code_topic[] = "db/color";

int r, g, b;

uint32_t white = carrier.leds.Color(255,255,255);

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  carrier.noCase();
  carrier.begin();
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  carrier.Buzzer.beep();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(light_toggle_topic);
  Serial.println(light_color_code_topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(light_toggle_topic);
  mqttClient.subscribe(light_color_code_topic);
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int temperature = carrier.Env.readTemperature();
    int humidity = carrier.Env.readHumidity();

    Serial.print("Sending message to topic: ");
    Serial.println(temperature_topic);
    Serial.println(temperature);

    Serial.print("Sending message to topic: ");
    Serial.println(humidity_topic);
    Serial.println(humidity);

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

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    char reply;
    reply = (char)mqttClient.read();
    Serial.print(reply);
    if (reply == 't'){
      carrier.leds.clear();
      carrier.leds.fill(white, 0, 5);
      carrier.leds.show();
    }else if(reply == 'f'){
      carrier.leds.clear();
      carrier.leds.fill((0,0,0), 0, 5);
      carrier.leds.show();
    }
  }
}
