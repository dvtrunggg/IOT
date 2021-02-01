#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int irPin1 = D1;
int irPin2 = D2;
int count = 0;
boolean state1 = true;
boolean state2 = true;
boolean insideState = false;
boolean outsideIr = false;
boolean isPeopleExiting = false;
int i = 1;

/*phần LCD*/
LiquidCrystal_I2C lcd (0x27, 16, 2); //phần mặc định cho lcd

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
void setup() {
  Serial.begin(9600);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);

  /*LCD*/
  Wire.begin(D5, D6); //SDA, SCL
  lcd.clear();
  lcd.init();
  lcd.backlight();
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
void loop() {

  Counting();
  //delay(1000);
  //lcd.noDisplay();
  //delay(500);

}
