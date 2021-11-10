/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Modified 3 February 2018 by Justin Mattair
 *   for MattairTech boards (www.mattairtech.com)
 *
 * See README.md for documentation and pin mapping information
 */
 
 //DK modified for SAMMYC21 board and mapping to sercom5 - tested blink and hello world to screen OK!


#include "variant.h"

/*
 * Pins descriptions
 */           
extern const PinDescription g_APinDescription[]=
{
        // 0..11 - Analog capable pins
/*pwm*/  { PORTB,  8, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TC0_CH0, ADC_Channel4, EXTERNAL_INT_8, GCLK_CCL_NONE },
/*pwm*/  { PORTB,  9, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TC0_CH1, ADC_Channel5, EXTERNAL_INT_9, GCLK_CCL_NONE },
         { PORTA,  2, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_DAC|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel0, EXTERNAL_INT_NONE, GCLK_CCL_NONE },   
         { PORTA,  3, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_REF|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel1, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA,  4, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_REF|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel4, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA,  5, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel5, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA,  6, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel6, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA,  7, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_ADC|PIN_ATTR_DIGITAL), NOT_ON_TIMER, ADC_Channel7, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
        //I2C
/*sda*/  { PORTA,  8, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM), NOT_ON_TIMER, ADC_Channel10, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
/*scl*/  { PORTA,  9, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM), NOT_ON_TIMER, ADC_Channel11, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
        //Serial2
/*tx*/   { PORTA, 10, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_SERCOM_STD), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM), NOT_ON_TIMER, ADC_Channel10, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
/*rx*/   { PORTA, 11, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_SERCOM_STD), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), NOT_ON_TIMER, ADC_Channel11, EXTERNAL_INT_11, GCLK_CCL_NONE },

        // 12..23 - Digital functions (12-24 pwm), 12-15, SPI/SDcard
/*mosi*/ { PORTA, 12, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC2_CH0, No_ADC_Channel, EXTERNAL_INT_12, GCLK_CCL_NONE },
/*miso*/ { PORTA, 13, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC2_CH1, No_ADC_Channel, EXTERNAL_INT_13, GCLK_CCL_NONE },
/*sck*/  { PORTA, 14, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM), TCC0_CH0, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
/*cs*/   { PORTA, 15, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM), TC4_CH1, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
/*boot*/ { PORTA, 16, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_0, GCLK_CCL_NONE },
/*mosi1*/{ PORTA, 17, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC0_CH3, No_ADC_Channel, EXTERNAL_INT_1, GCLK_CCL_NONE },
/*miso1*/{ PORTA, 18, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TC4_CH0,  No_ADC_Channel, EXTERNAL_INT_2, GCLK_CCL_NONE },
/*sck1*/ { PORTA, 19, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC0_CH2, No_ADC_Channel, EXTERNAL_INT_3, GCLK_CCL_NONE },
/*cs1*/  { PORTA, 20, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC0_CH6, No_ADC_Channel, EXTERNAL_INT_4, GCLK_CCL_NONE },
         { PORTA, 21, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM|PIN_ATTR_EXTINT), TCC0_CH7, No_ADC_Channel, EXTERNAL_INT_5, GCLK_CCL_NONE },
         { PORTA, 22, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM), TCC0_CH4, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA, 23, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER|PIN_ATTR_SERCOM), TCC0_CH5, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },

        // 24..26 - Serial3 (pin 26 does not exist)
/*tx*/   { PORTA, 24, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_SERCOM_STD|PER_ATTR_TIMER_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_COM|PIN_ATTR_SERCOM|PIN_ATTR_TIMER), TC1_CH0, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, 
/*rx*/   { PORTA, 25, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_SERCOM_STD|PER_ATTR_TIMER_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_COM|PIN_ATTR_SERCOM|PIN_ATTR_TIMER), TC1_CH1, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, 
         { NOT_A_PORT,  0, PIO_NOT_A_PIN, PER_ATTR_NONE, PIN_ATTR_NONE, NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // Unused

        // 27..29 - Digital functions
         { PORTA, 27, PIO_MULTI, PER_ATTR_DRIVE_STRONG, PIN_ATTR_DIGITAL, NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { PORTA, 28, PIO_MULTI, PER_ATTR_DRIVE_STRONG, PIN_ATTR_DIGITAL, NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE },
         { NOT_A_PORT,  0, PIO_NOT_A_PIN, PER_ATTR_NONE, PIN_ATTR_NONE, NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // Unused

        // 30..31 - Digital functions / Debug interface (SWD CLK and SWD IO)
         { PORTA, 30, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), TCC1_CH0, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // SWD CLK
         { PORTA, 31, PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_TIMER_STD), (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), TCC1_CH1, No_ADC_Channel, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // SWD IO

        // 32..35 - Digital functions   Serial1 (USB) on 34 and 35 
         { PORTB, 22, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_DIGITAL|PIN_ATTR_EXTINT), NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_6, GCLK_CCL_NONE }, // 32
         { PORTB, 23, PIO_MULTI, PER_ATTR_DRIVE_STRONG, (PIN_ATTR_DIGITAL|PIN_ATTR_EXTINT), NOT_ON_TIMER, No_ADC_Channel, EXTERNAL_INT_7, GCLK_CCL_NONE }, // 33
         { PORTB, 2,  PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM), NOT_ON_TIMER, ADC_Channel2, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // 34, LED
         { PORTB, 3,  PIO_MULTI, (PER_ATTR_DRIVE_STRONG|PER_ATTR_ADC_ALT|PER_ATTR_SERCOM_ALT), (PIN_ATTR_ADC|PIN_ATTR_DIGITAL|PIN_ATTR_SERCOM), NOT_ON_TIMER, ADC_Channel3, EXTERNAL_INT_NONE, GCLK_CCL_NONE }, // 35
        //DK changed 34 and 35 for secom5 pins CANFDuino
};
const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TC0, TC1, TC2, TC3, TC4 } ;


// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ; //serial2
SERCOM sercom1( SERCOM1 ) ; //spi1
SERCOM sercom2( SERCOM2 ) ; //2Wire 
SERCOM sercom3( SERCOM3 ) ; //serial3
SERCOM sercom4( SERCOM4 ) ; //spi
SERCOM sercom5( SERCOM5 ) ; //serial1(USB)

#if defined(ONE_UART) || defined(TWO_UART) || defined(THREE_UART)
Uart Serial1( SERCOM_INSTANCE_SERIAL1, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;

    void SERCOM5_Handler()  {
      Serial1.IrqHandler();
    }
#endif

#if defined(TWO_UART) || defined(THREE_UART)
Uart Serial2( SERCOM_INSTANCE_SERIAL2, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

    void SERCOM0_Handler()  {
      Serial2.IrqHandler();
    }
#endif

#if defined(THREE_UART)
Uart Serial3( SERCOM_INSTANCE_SERIAL3, PIN_SERIAL3_RX, PIN_SERIAL3_TX, PAD_SERIAL3_RX, PAD_SERIAL3_TX ) ;

    void SERCOM3_Handler()  {
      Serial3.IrqHandler();
    }
#endif
