int button = 12;

void setup() {
    pinMode(button, INPUT);
    Serial.begin(9600);

}

void loop() {
    int buttonState = digitalRead(button);
    if (buttonState == HIGH){
      while(digitalRead(button) == HIGH);
      Serial.print("Button pressed");
      }
}
