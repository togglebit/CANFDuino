/*
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
#ifndef SAMC21_TIMER_H
#define SAMC21_TIMER_H    

//dump to the screen
//#define DEBUG_PRINTRX

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "typedef.h"
/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

// Enable TC3 interrupts
#define ENABLE_TIMER     NVIC_EnableIRQ(TC3_IRQn)
#define DISABLE_TIMER    NVIC_DisableIRQ(TC3_IRQn)
                    

// Static function pointer declaration
static void (*timerCallback)(void) = NULL;


bool setupTC3TimerInterrupt(UINT32 time_uS, void (*handler)(void))
{
    
    bool retVal = true;
    if(handler)
    {


        // Enable GCLK for TC3 (TC3 is connected to GCLK0 by default)
        GCLK->PCHCTRL[TC3_GCLK_ID].reg = (uint16_t) (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0);
        while (GCLK->PCHCTRL[TC3_GCLK_ID].bit.CHEN == 0);

        //disable counter
        TC3->COUNT16.CTRLA.reg &= !TC_CTRLA_ENABLE;

        // Reset TC3
        TC3->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
        while (TC3->COUNT16.SYNCBUSY.bit.SWRST);
        while (TC3->COUNT16.CTRLA.bit.SWRST);

        // Configure TC3 for recurring compare capture interrupt 
        //set timer to prescale by 256, (48MHz/265) = 187.5kHz. 1/187.5kHz =  5.3uS per tick
        TC3->COUNT16.CTRLA.reg = 0x00001610; 
        TC3->COUNT16.WAVE.reg = 1;
        TC3->COUNT16.CC[0].reg = (time_uS / 5.3) -1 ;   
        TC3->COUNT16.INTENSET.reg = TC_INTENSET_MC0; 
        while (TC3->COUNT16.SYNCBUSY.bit.CC0);

        // Enable TC3
        TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
        while (TC3->COUNT16.SYNCBUSY.bit.ENABLE);

        timerCallback = handler;
        NVIC_EnableIRQ(TC3_IRQn);

    }else
    {
        retVal = false;
    }
    return(retVal);
}


 void TC3_Handler() 
{
  //if (TC3->COUNT16.INTFLAG.bit.OVF) 
  if (TC3->COUNT16.INTFLAG.bit.MC0) 
  {
      if(timerCallback);
      {

          timerCallback();
      }
      
      TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
  }
}


#endif
