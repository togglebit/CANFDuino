
int val;
byte pwm;

void setup()
{
    delay(5000);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW);
    Serial.begin(115200);
    Serial.println("system reset");
    
  
}


void loop()
{
    uint8_t ret;
    uint32_t id;
    uint8_t len;
    uint8_t buf[8];
    uint8_t i;
    
    delay(500);   
    //read analog inputs 
    val = analogRead(A0);  
    Serial.print("CH0 "); Serial.print(val);        
    val = analogRead(A1);  
    Serial.print(" CH1 "); Serial.print(val);    
    val = analogRead(A2);  
    Serial.print(" CH2 "); Serial.print(val);    
    val = analogRead(A3);  
    Serial.print(" CH3 "); Serial.print(val);    
    val = analogRead(A4);  
    Serial.print(" CH4 "); Serial.print(val);    
    val = analogRead(A5);  
    Serial.print(" CH5 "); Serial.print(val);    
    val = analogRead(A6);  
    Serial.print(" CH6 "); Serial.print(val);    
    val = analogRead(A7);  
    Serial.print(" CH7 "); Serial.print(val);
     val = analogRead(A8);  
    Serial.print(" CH8 "); Serial.print(val);
     val = analogRead(A9);  
    Serial.print(" CH9 "); Serial.println(val);
    
  
}
