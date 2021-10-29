int val;
byte i;
bool toggle;


void setup()
{
        for(i = 0; i<26; i++)
        {
          pinMode(i, OUTPUT);
        }
        //try these IO with pullups on
        pinMode(6,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(14,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(15,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(18,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(22,INPUT_PULLUP);
        
          // initialize digital pin LED_BUILTIN as an output.
          pinMode(28, OUTPUT);
          Serial.begin(115200);
          Serial.println("system reset");
}


void loop()
{

    //write to all outputs
    for(i = 0; i<26; i++)
        {
           digitalWrite(i, toggle);  
        }
    digitalWrite(28, toggle);
    toggle = toggle? 0:1;
    delay(500);
}
