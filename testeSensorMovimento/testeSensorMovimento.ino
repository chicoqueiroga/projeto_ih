/*  
    Material usado: http://RandomNerdTutorials.com/pirsensor
    Modificado por Francisco Queiroga
*/
 
int led = 13;                // pino do led
int sensor = 2;              // pino do sensor
int state = LOW;             // padrao, sem movimento
int val = 0;                 // guarda o valor do sensor

void setup() {
  pinMode(led, OUTPUT);      
  pinMode(sensor, INPUT);    
  Serial.begin(9600);        
}

void loop(){
  val = digitalRead(sensor);
  if (val == HIGH) {           
    digitalWrite(led, HIGH);   // detectou movimento, led liga
    delay(100); 
    
    if (state == LOW) {
      Serial.println("Movimento detectado."); 
      state = HIGH;       // atualiza o estado
    }
  } 
  else {
      digitalWrite(led, LOW); // desliga o led se nao existe movimento
      delay(200);             
      
      if (state == HIGH){
        Serial.println("Movimento parou.");
        state = LOW;       // atualiza o estado
    }
  }
}


