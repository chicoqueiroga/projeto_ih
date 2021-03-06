/**************************************************************
 * IoT Motion Detector with Blynk
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 * 
 * Developed by Marcelo Rovai - 30 November r
 **************************************************************/
#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>
#include "FS.h"
#include <SPI.h>

WidgetTerminal terminal(V1);
WidgetLCD lcd(V3);


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

Servo myservo; //Criando o objeto servo para controlar o servomotor
char filename[] = "f.txt";
char auth[] = "5312c01e8f224750b99c951d68a307c3";

bool lastPirState = false;
bool curPirState = false;

/* WiFi credentials */
char ssid[] = "GVT-FIONA";
char pass[] = "J531128550";

/* HC-SR501 Motion Detector */
#define ledPin D7
#define pirPin D1 // 180Input for HC-S501

int countDetection;
//long rssi;
int alimento;
int qtd_alimento;

/*----- novo codigo -----*/
// NTP Servers:
static const char ntpServerName[] = "0.br.pool.ntp.org";
const int timeZone = -3;     // Fuso horário do Brasil

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);
/*----- fim do novo codigo -----*/


/***************************************************
 * Get PIR data
 **************************************************/
void getPirValue(void)
{
  
  curPirState = digitalRead(pirPin);
  //digitalWrite(ledPin, pirValue); 
  
  if (curPirState != lastPirState) {
    

    lastPirState = curPirState;
    if (curPirState == true) {
        countDetection++;
        time_t t = now();
        File myDataFile = SPIFFS.open(filename, "w+");
        //myDataFile.println("Dados lidos do LOG");
        myDataFile.print("Moviment detectado em: ");
        myDataFile.print(hour(t));
        myDataFile.print(":");
        myDataFile.println(minute(t));  
        myDataFile.close();
  
        myDataFile = SPIFFS.open(filename, "r");
       
        while(myDataFile.available()) {
          Serial.write(myDataFile.read());
          
          terminal.println(countDetection);
        }
         
          myDataFile.close(); 
         
          //Seria digitalWrite(ledPin, pirValue); l.println("==> Motion detected");
          Blynk.notify("DETECTION"); 
      }
    }        
 }
 
void actServo(void)
{
 
 Blynk.virtualWrite(V0, alimento);
 
 Serial.println("Servo acionado com sucesso!");
 if (alimento > 0) {
   Serial.println("Servo acionado com sucesso!");
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
   alimento -= 20; 
  } else {
    Blynk.virtualWrite(V0, 0);
    }
}

void printDigits(int digits)

{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer tdigitalWrite(ledPin, pirValue); o hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

BLYNK_WRITE(V5) //Button Widget is writing to pin V5
{
  int pinData = param.asInt(); 
  if(pinData==1)
  {
    lcd.clear();
    lcd.print(0 ,0,"Detecções");                 // This fn displays the IP Address when a button is pressed from the Blynk App
    lcd.print(0 ,1,countDetection);
   
  }else
  {
    lcd.clear();
  }
  
}



void setup()
{

  SPIFFS.begin(); // inicializando sistema de arquivos
  
  myservo.attach(2);  // attaches the servo on GIO2 to the servo object 

  countDetection = 0;

  alimento = 200;
  
  Serial.begin(9600);
  delay(100);
  Blynk.begin(auth, ssid, pass);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, HIGH);
  Blynk.setProperty(V0, "label", "Alimento");

/*----- novo codigo -----*/
  Serial.println("TimeNTP Example");
  //Serial.print("IP number assigned by DHCP is ");
  //Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  Serial.println("waiting for sync");
  while (timeStatus() == timeNotSet);
  Serial.println("time sync ok!");  
  Alarm.timerRepeat(10,actServo);
  delay(1000);
}


void loop()
{
  
  Blynk.run();
  Alarm.delay(200);
  getPirValue();
  //rssi=WiFi.RSSI();                                        // Functions to display RSSI and Ip address
 // Serial.println(rssi);
  //delay(500);
  

}

