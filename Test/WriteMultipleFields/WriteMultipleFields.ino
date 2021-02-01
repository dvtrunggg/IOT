#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <String.h>
#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

const char* ssid = "Phuong Hien 2";
const char* password = "14129595";
const char* pass = "14129595";

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 1282766;
const char * myWriteAPIKey = "RMF501JWUVH75ZYU";



const char* mqtt_server = "broker.mqtt-dashboard.com";

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







// Initialize our values
//dữ liệu số từ cảm biến (nhiệt độ,...)
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
String myStatus = "";



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

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(message);
  
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

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  //thinkspeak
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  //wifi
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


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
}


void loop() {

  Counting();
  
  int out = in - count;
  String strCount = String(count);
  String strIn = String(in);
  
  
  
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



  //-----------------------think speak
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  //field 1 là number 1 (number 1 là độ ẩm,...)
  ThingSpeak.setField(1, count);
  ThingSpeak.setField(2, in);
  

  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  
  delay(20000); // Wait 20 seconds to update the channel again
 
}
