#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include <Ultrasonic.h>

#define ECHO D1
#define TRIG D2
#define LED D8
#define SERVO D7

Servo myservo;

const char *ssid     = "MATHEUS";
const char *password = "SPQR1720";

WiFiUDP ntpUDP;

int16_t utc = -3; //UTC -3:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;

NTPClient timeClient(ntpUDP, "0.br.pool.ntp.org", utc*3600, 60000);

Ultrasonic ultrasonic(TRIG, ECHO); //Instância chamada ultrassonic com parâmetros (trig, echo)

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  timeClient.update();

  myservo.attach(SERVO); //Indicando porta digital onde encontra-se o servo
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop() {
  //Chama a verificacao de tempo
  //Medindo distância para acionar o servo e ligar o led
  Serial.print("Distância(cm): ");
  Serial.println(ultrasonic.distanceRead());
  if (ultrasonic.distanceRead() > 5){
    digitalWrite(LED, HIGH);
    actServo();
  }else{
    digitalWrite(LED, LOW);
  }
  delay(2000);
}
void forceUpdate(void) {
  timeClient.forceUpdate();
}
void checkOST(void) {
  currentMillis = millis();//Tempo atual em ms
  //Lógica de verificação do tempo
  if (currentMillis - previousMillis > 1000) {
    previousMillis = currentMillis;    // Salva o tempo atual
    //printf("Time Epoch: %d: ", timeClient.getEpochTime());
    Serial.println(timeClient.getFormattedTime());
  }
}
void actServo(void)
{   
   Serial.print("Servo acionado com sucesso às ");
   Serial.println(timeClient.getFormattedTime());
   int pos;
   
   for(pos = 0; pos <= 45; pos += 1) // goes from 0 degrees to 45 degrees 
   {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
   } 
   for(pos = 45; pos>=0; pos-=1)     // goes from 45 degrees to 0 degrees 
   {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
   }
}
