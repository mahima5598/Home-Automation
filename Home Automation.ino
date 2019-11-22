#include<ESP8266WiFi.h>/*for recieving and sending data ( eg WiFi.begin() is under this)*/
#include <WiFiClient.h>//
#include <ESP8266WebServer.h>
#include"Adafruit_MQTT.h"//MQTT, or message queue telemetry transport, is a protocol for device communication that Adafruit IO supports.
#include"Adafruit_MQTT_Client.h"//for client interface
#define Relay1 D0
#define Relay2 D1
#define Relay3 D2
#define LEDPin D5
#define LEDPin1 D6
#define LEDPin2 D7
#define LEDPin3 D8
//WLANDetails
#define WLAN_SSID "."//YourSSID
#define WLAN_PASS "aaaaaaaa"//Yourpassword
/*************************Adafruit.ioSetup*********************************/
#define AIO_SERVER "io.adafruit.com"//AdafruitServer
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "mahimac"//Username
#define AIO_KEY "2268d355faf74f91b15390982d8430c6"//AuthKey
//WIFICLIENT for voice control
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
String page = "";
WiFiClient client;// Create an ESP8266 WiFiClient class to connect to the MQTT server.
Adafruit_MQTT_Client mqtt(&client,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Subscribe Light=Adafruit_MQTT_Subscribe(&mqtt,
AIO_USERNAME"/feeds/relay1");//Feedsnameshouldbesameeverywhere
Adafruit_MQTT_Subscribe Lamp=Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME
"/feeds/relay2");//// Setup a feed called 'lamp' for subscribing to current device
Adafruit_MQTT_Subscribe Fan=Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME
"/feeds/relay3");
void MQTT_connect();
//access through web page
void setup(){
  //the HTML of the web page
  page = "<h1>Simple NodeMCU Web Server</h1><p><a href=\"l1off\"><button>ON</button></a>&nbsp;<a href=\"l1on\"><button>OFF</button></a></p>";
  //make the LED pin output and initially turned off
Serial.begin(115200);//starts serial communication at speed of 115200 bits per sec
pinMode(Relay1,OUTPUT);
pinMode(Relay2,OUTPUT);
pinMode(Relay3,OUTPUT);
digitalWrite(LEDPin, HIGH);//for state of led
digitalWrite(LEDPin1, LOW);
digitalWrite(LEDPin2, LOW);
digitalWrite(LEDPin3, LOW);
//ConnecttoWiFiaccesspoint for voice control
Serial.println();Serial.println();
Serial.print("Connectingto");
Serial.println(WLAN_SSID);
WiFi.begin(WLAN_SSID,WLAN_PASS);
while(WiFi.status()!=WL_CONNECTED){
delay(500);
Serial.begin(115200);//starts serial communication at speed of 115200 bits per sec
WiFi.begin(WLAN_SSID, WLAN_PASS); //begin WiFi connection
Serial.println("");//prints data to serial port
Serial.print(".");
}
Serial.println();
Serial.println("WiFiconnected");
Serial.println("IPaddress:");
Serial.println(WiFi.localIP());
mqtt.subscribe(&Light);
mqtt.subscribe(&Lamp);
mqtt.subscribe(&Fan);


  // Wait for connection on web
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");//println for new line
  Serial.print("Connected to ");
  Serial.println(WLAN_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());//prints local ip
   
  server.on("/", [](){
    server.send(200, "text/html", page);//send http status 200(200 is a way odf acknowledging)
  });
  server.on("/l1off", [](){//send http status 200 and digital data when pressed off
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
  digitalWrite(LEDPin1, LOW);
  digitalWrite(LEDPin2, LOW);
  digitalWrite(LEDPin3, LOW);
    delay(1000);
  });
  server.on("/l1on", [](){//send http status 200 and digital data when pressed on, server on sets up the callback function
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
  digitalWrite(LEDPin1, LOW);
  digitalWrite(LEDPin2, HIGH);
  digitalWrite(LEDPin3, LOW);
    delay(1000);
  });
  server.begin();
  Serial.println("Web server started!");
}
void loop(){
//for web access
server.handleClient();//does the needed http server stuff and calls the right callback function

//for voice access  
MQTT_connect();/* Ensure the connection to the MQTT server is alive (this will make the first
 connection and automatically reconnect when disconnected).*/
Adafruit_MQTT_Subscribe*subscription;
while((subscription=mqtt.readSubscription(20000))){//refreash add
if(subscription==&Light){
Serial.print(F("LightGot:"));
Serial.println((char*)Light.lastread);
int Light_State=atoi((char*)Light.lastread);
digitalWrite(Relay1,Light_State);
}
if(subscription==&Lamp){
Serial.print(F("LampGot:"));
Serial.println((char*)Lamp.lastread);
int Lamp_State=atoi((char*)Lamp.lastread);
digitalWrite(Relay2,Lamp_State);
}
if(subscription==&Fan){
Serial.print(F("FanGot:"));
Serial.println((char*)Fan.lastread);
int Fan_State=atoi((char*)Fan.lastread);
digitalWrite(Relay3,Fan_State);
}
}
}
void MQTT_connect(){
int8_t ret;//ask
if(mqtt.connected()){
return;
}
Serial.print("ConnectingtoMQTT...");
uint8_t retries=3;
while((ret=mqtt.connect())!=0){//returns 0 if connected
Serial.println(mqtt.connectErrorString(ret));
Serial.println("RetryingMQTTconnectionin2seconds...");
mqtt.disconnect();
delay(2000);
retries--;
if(retries==0){
while(1);
}
}
Serial.println("MQTTConnected!");
}
