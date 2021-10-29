#include "samc21_can.h"

SAMC21_CAN can(0);
int val;
byte pwm;

void setup()
{
  delay(5000);
          pinMode(12, OUTPUT);
          pinMode(13, OUTPUT);
          pinMode(14, OUTPUT);
          pinMode(15, OUTPUT);
          pinMode(16, OUTPUT);
          pinMode(17, OUTPUT);
          pinMode(18, OUTPUT);
          pinMode(21, OUTPUT);
          pinMode(22, OUTPUT);
          pinMode(23, OUTPUT);

           // initialize digital pin LED_BUILTIN as an output.
          pinMode(28, OUTPUT);
          digitalWrite(28, LOW);
          Serial.begin(19200);
          Serial.println("system reset");
    
    uint8_t ret;
    ret = can.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
    if (ret == CAN_OK) {
        Serial.println("CAN Initialized Successfully!");
    } else {
        Serial.println("Error Initializing CAN...");
    }
}


void loop()
{
    uint8_t ret;
    uint32_t id;
    uint8_t len;
    uint8_t buf[8];
    uint8_t i;
    
    //write to all PWM outputs
    delay(500);
    pwm +=1; 
    analogWrite(12, pwm);
    analogWrite(13, pwm);
    analogWrite(14, pwm);
    analogWrite(15, pwm);
    analogWrite(16, pwm);
    analogWrite(17, pwm);  
    analogWrite(18, pwm);
    analogWrite(20, pwm);
    analogWrite(21, pwm);
    analogWrite(22, pwm);
    analogWrite(23, pwm);

    Serial.print("PWM Outputs At: "); Serial.println(pwm); 
    
   
    //check CAN0 & CAN1 for RX 
    ret = can.readMsgBuf(&id, &len, buf);
    if (ret == CAN_OK) {
        Serial.print("Got a message from: ");
        Serial.print(id);
        Serial.print("  Length: ");
        Serial.print(len);
        Serial.print("  Data: ");
        for (i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);
        }
        Serial.println("");
    }
}
