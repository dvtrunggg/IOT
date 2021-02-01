
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <String.h>

#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//const char* ssid = "Phuong Hien 2";
//const char* password = "14129595";
const char* mqtt_server = "broker.mqtt-dashboard.com";
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned long myChannelNumber = 1282766;
const char * myReadAPIKey = "UDF4NX8JRZE9GKFH";

String myStatus = "";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];



/*phần counting*/
int irPin1 = D1;
int irPin2 = D2;
int count = 0;
boolean state1 = true;
boolean state2 = true;
boolean insideState = false;
boolean outsideIr = false;
boolean isPeopleExiting = false;
int i = 1;
int in = 0;

/*phần LCD*/
LiquidCrystal_I2C lcd (0x27, 16, 2); //phần mặc định cho lcd



//phần khí gas và buzzer
int smokeA0 = A0;
int buzzer = D8;
int checkGas;
//ngưỡng khí gas
int sensorThres = 300;




void Nobody()
{
  lcd.setCursor(2,0);
  lcd.print("No one inside");
  lcd.setCursor(0,1);
  lcd.print("Bonjour ^.^");
}

void Human()
{
  
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Humans Inside: ");
  lcd.setCursor(0,1);
  lcd.print(count);
}

void Counting()
{
    if (!digitalRead(irPin1) && i==1 && state1){
     outsideIr=true;
     delay(400);
     i++;
     state1 = false;
  }

   if (!digitalRead(irPin2) && i==2 &&   state2){
     Serial.println("Entering into room");
     outsideIr=true;
     delay(400);
     i = 1 ;
     count++;
     in++;
     Human();
     Serial.print("No of persons inside the room: ");
     Serial.println(count);
     state2 = false;
  }

   if (!digitalRead(irPin2) && i==1 && state2 ){
     outsideIr=true;
     delay(400);
     i = 2 ;
     state2 = false;
  }

  if (!digitalRead(irPin1) && i==2 && state1 ){
     Serial.println("Exiting from room");
     outsideIr=true;
     delay(400);
     count--;
     if(count <= 0)
      count = 0;
     Human();
     Serial.print("No of persons inside the room: ");
     Serial.println(count);
     i = 1;
     state1 = false;
  }  

    if (digitalRead(irPin1)){
     state1 = true;
    }

     if (digitalRead(irPin2)){
     state2 = true;
    }
  if(count == 0)
  {
    Nobody();
  }

}
/*
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
*/


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe("/home/bedroom/light");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/*
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message);
  
}
*/

//gas
void gas()
{
  checkGas = analogRead(smokeA0);
  Serial.print("Pin A0: ");
  Serial.println(checkGas);

  // Checks if it has reached the threshold value
  if (checkGas > sensorThres)
  {
    tone(buzzer, 1000, 800);
    
    delay(1000);
  }
  else{
    noTone(buzzer);  
    
  }
  delay(1000);
}


void setup() {
  
  WiFiManager wifiManager;
  //wifiManager.resetSettings();  //test thôi vì nó reset pass -> phải nhập lại:)
  wifiManager.autoConnect("Trunggg"); //phát wifi tên trungg
  
  Serial.println("Connected...");
  Serial.begin(115200);
  //setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);


  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  /*LCD*/
  Wire.begin(D5, D6); //SDA, SCL
  lcd.clear();
  lcd.init();
  lcd.backlight();

  /*gas + buzzer*/
  pinMode (smokeA0,INPUT);
  pinMode(buzzer, OUTPUT);

  //think speak
  ThingSpeak.begin(espClient);
}

void loop() {

  Counting();
  
  String strCount = String(count);
  String strIn = String(in);
  //ThingSpeak.readIntField(myChannelNumber, 1, myReadAPIKey);
  //ThingSpeak.readIntField(myChannelNumber, 2, myReadAPIKey);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  client.publish("/home/human", strCount.c_str());  
  client.publish("/home/human/come", strIn.c_str());  
    if(analogRead(smokeA0) >15)
  {
    gas();
    
    String strGas = String(checkGas);
    
    client.publish("/home/gas", strGas.c_str());  
    ThingSpeak.setField(3, checkGas);
  } 

  // set the fields with the values


  



  }
