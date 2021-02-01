#include <SimpleTimer.h>

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <BlynkSimpleEsp8266.h>
#include "ThingSpeak.h"
#include <String.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


SimpleTimer timer;

int value_led = 1;

char token[] = "5_ksH1Dl4UIXwsE8w8iR5eeyX-sHyxJA";
// mqtt server
const char* mqtt_server = "broker.mqtt-dashboard.com";

// Thingspeak channel
unsigned long myChannelNumber = 1282766;
const char * myWriteAPIKey = "RMF501JWUVH75ZYU";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;




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



int l = D0;



/*
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
*/
/*
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
      client.subscribe("/home");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
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
      client.subscribe("/home");

      char* topic; 
      byte* payload; 
      unsigned int length;


       String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message); 
    if(message == "on"){
    lcd.backlight();
  }
  if(message == "off"){
    lcd.noBacklight();
  }
  
      

      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}



void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message); 
    if(message == "on"){
    lcd.noBacklight();
    delay(5000);
  }
  if(message == "off"){
    lcd.noBacklight();
    delay(5000);
  }
  
  
}






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



BLYNK_WRITE(V0) {
  value_led = param.asInt();
  Serial.print("LED LCD OFF");
  Serial.println(value_led);
  
}



void resetButtonLed(){
  value_led = 1;
  Serial.println("Value_led changed");
}


void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  
  wifiManager.resetSettings();
  wifiManager.autoConnect("Trungg");
  
  Serial.println("Connected...");
  Blynk.config(token);
  
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);

  // Thingspeak
  ThingSpeak.begin(espClient);




  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  timer.setInterval(5000L, resetButtonLed);

  /*LCD*/
  Wire.begin(D5, D6); //SDA, SCL
  lcd.clear();
  lcd.init();
  lcd.backlight();

  /*gas + buzzer*/
  pinMode (smokeA0,INPUT);
  pinMode(buzzer, OUTPUT);



  pinMode(l, OUTPUT);
}

void loop() {

  //bật tắt dèn LCD
  if(value_led == 0){
    lcd.noBacklight();
    timer.run();
  }

    if(value_led == 1){
    lcd.backlight();
    timer.run();
  }
  
  Counting();
  
  String strCount = String(count);
  String strIn = String(in);
  
  if (!client.connected()) {
    reconnect();
  }
  
  Blynk.run();
  //gửi email và thông báo cho người dùng khi khí gas ở mức cảnh báo
  if(checkGas >= sensorThres){

    Blynk.email("trungdinhviet1999@gmail.com", "Dangerous gas condition!!", "Dangerous!!!");
    Blynk.notify("Dangerous gas condition!!");
  }
  
  client.loop();
  // Publish on localhost using MQTT
  
  // DHT sensor code
  client.publish("/home/human", strCount.c_str());  
  client.publish("/home/human/come", strIn.c_str());  





  // Thingspeak
  //Write data to field
  ThingSpeak.setField(1, count);
  ThingSpeak.setField(2, in);

  
  if(analogRead(smokeA0) >15)
  {
    gas();
    String strGas = String(checkGas);
    client.publish("/home/gas", strGas.c_str());  
    ThingSpeak.setField(3, checkGas);

  } 
  
  int returncode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Check return Code
  if (returncode == 200) {
    Serial.println("Channel update sucessful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(returncode));
  }
 
  //Serial.println(ThingSpeak.readIntField(myChannelNumber, 3, myReadAPIKey));
  delay(200);
}
