
long val;
byte i;
bool toggle;


void setup()
{
        for(i = 0; i<26; i++)
        {
          pinMode(i, INPUT_PULLUP);
        }
  
          // initialize digital pin LED_BUILTIN as an output.
          pinMode(27, OUTPUT);
          digitalWrite(27, LOW);
          Serial2.begin(115200);
          Serial3.begin(115200);        
          Serial.begin(115200);
          Serial.println("system reset");
}
String msg,msg2; 

void loop()
{
    //write to all outputs
    Serial2.print("Serial2Test");Serial2.print(i,DEC);
    msg = Serial2.readString();
      Serial.println(msg);
 
    Serial3.print("Serial3Test");Serial3.print(i,DEC);
    msg2 = Serial3.readString();
    Serial.println(msg2);
    
    digitalWrite(28, toggle);
    toggle = toggle? 0:1;
    i=i+1;
    delay(2000);
}
