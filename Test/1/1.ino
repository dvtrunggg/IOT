
#include <LiquidCrystal.h>
#include <Wire.h>

//boolean state = true;
//int irPin = D2;

//phần khí gas và buzzer
//int smokeA0 = A0;
//int buzzer = D7;
// ngưỡng khí gas
int sensorThres = 800;


//phần couting và lcd
int in = D1;
int out = D2;
int relay = D3;
int count = 0;
LiquidCrystal lcd(13,12,11,10,9,8);
//khi có người đi vào, count sẽ ++
void In()
{
    count++;
    lcd.clear();
    lcd.print("Person In Room: ");
    lcd.setCursor(0,1);
    lcd.print(count);
        Serial.print("Count: ");
    Serial.print(count);
    delay(100);
}

//khi có người đi ra, count sẽ -- 
void Out()
{
    if(count>=1)
      count--;
    lcd.clear();
    lcd.print("Person In Room: ");
    lcd.setCursor(0,1);
    lcd.print(count);
        Serial.print("Count: ");
    Serial.print(count);
    delay(100);
}
void setup() {
  //pinMode(D2,OUTPUT);
 // pinMode(D3,OUTPUT);
  
 // pinMode(irPin,INPUT);
  //pinMode (smokeA0,INPUT);
 // pinMode(buzzer, OUTPUT);

  /*countting và lcd*/
  pinMode(in, INPUT);
  pinMode(out, INPUT);
  pinMode(relay, OUTPUT);
  lcd.clear();
  lcd.print("Person In Room: ");
  lcd.setCursor(0,1);
  lcd.print(count);




 
  Serial.begin(9600);
  

}
/*
//phần cản hồng ngoại
void canHongNgoai()
{
  if(!digitalRead(irPin) && state)
  {
    count++;
    state = false;
    Serial.print("Count: ");
    Serial.print(count);
    delay(100);
    
  }
  if(digitalRead(irPin))
  {
    state = true;
    delay(100);
  }
}
void nhapNhay()
{
  digitalWrite(D2, HIGH); // phần sáng
  delay(1000);
  digitalWrite(D2, LOW);  //phần tắt
  delay(1000);
}

void sang()
{
  digitalWrite(D3, HIGH);
}

//buzer khi có gas
void Buzzer()
{
  digitalWrite(buzzer, HIGH); // phần sáng
  delay(1000);
  digitalWrite(buzzer, LOW);  //phần tắt
  delay(1000);

}

//gas
void gas()
{
  int analogSensor = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(analogSensor);

  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
    tone(buzzer, 1000, 200);
    delay(1000);
  }
  else{
    noTone(buzzer);  
    
  }
  delay(100);
}
*/
//khi count <= 0 (ko có người ở trong)
void Nobody()
{
  lcd.clear();
  digitalWrite(relay, LOW);
  lcd.clear();
  lcd.print("Nobody In Room");
  lcd.setCursor(0,1);
  lcd.print("Light Is Off");
  delay(200);
}

//phần như hàm main nè:))
void loop() {

 // nhapNhay();
 // sang();
 //canHongNgoai();
 //gas();

 if(digitalRead(in))
 {
  In();
 }

  if(digitalRead(out))
 {
  Out();
 }

  if(count <=0)
  {
    Nobody();
  }
  else
  {
    digitalWrite(relay, HIGH);
  }

}
