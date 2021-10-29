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
          Serial.begin(115200);
          Serial.println("system reset");
}


void loop()
{
    //write to all outputs
    for(i = 0; i<26; i++)
        {
           if(digitalRead(i))
           {
            // Serial.print("Input "); Serial.print(i,DEC); Serial.println(" is high");
            }else
           {
            Serial.print("Input "); Serial.print(i,DEC); Serial.println(" is low");
          
           }
         }
    digitalWrite(27, toggle);
    toggle = toggle? 0:1;
    delay(2000);
}
