#define pinPIR 7

void setup() {
  pinMode(pinPIR, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  bool valorPIR = digitalRead(pinPIR);

  if (valorPIR) {
    Serial.println("MOVIMENTO");
    digitalWrite(13, HIGH);
    } else {
      Serial.println("---");
      }
}
