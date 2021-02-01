#include<ESP8266WiFi.h>
  #include<ESP8266WebServer.h>
  #include<string.h>
  
  const char* ssid = "HCMUS-I92"; 
  const char* password = "phonghoc@i92"; 
  
  ESP8266WebServer server(80);
  
  void setup() {
    // put your setup code here, to run once:
    pinMode( D4 , OUTPUT);  
    Serial.begin(115200);

    // connect to wifi 
    Serial.println();
    Serial.println();
    Serial.print("Connect to ...");
    Serial.println(ssid);
  
    WiFi.begin(ssid,password); 
  
    while(WiFi.status() != WL_CONNECTED){
      delay(500); 
      Serial.print(".");
    }
     Serial.println();
     Serial.println("WiFi connected");
     Serial.println("IP address: ");
     Serial.println(WiFi.localIP());
  
    //
    server.on("/", handleRoot);
    server.on("/temperature", showTemperature);
    server.on("/batden", batDen);
     server.on("/tatden", tatDen);
    server.begin();
    Serial.println("Web Server started"); 
  }
  
  void loop() {
    server.handleClient();
  }
  
  void handleRoot(){
    String batDen = "<a href='http://10.124.5.239/batden'>Turn on</a><br>" ;
    String tatDen = "<a href='http://10.124.5.239/tatden'>Turn off</a>" ;
    String s = batDen + tatDen; 
    server.send(200, "text/html", s);
  }
  
  void showTemperature(){
    server.send(200, "text/html", "<h1>HIEN THI NHIET DO: 38 <h1>");
  }    

  void batDen() {
    server.send(200, "text/html", "Turn on LED<h1>");
    digitalWrite(D4, HIGH); 
    delay(1000); 
  }

   void tatDen() {
    server.send(200, "text/html", "Turn off LED<h1>");
    digitalWrite(D4, LOW); 
    delay(1000); 
  }
