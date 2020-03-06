#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC_module.h"
#include "TImer_Module.h"
#include "PWM_module.h"

#define INTERRUPT_INIT  SREG |= (1<<7) // GLOBAL interrupt enabled

#define SET_BIT(PORT,PIN)  PORT |= (1<<PIN)
#define CLR_BIT(PORT,PIN)  PORT &= ~(1<<PIN)
#define READ_BIT(PORT,PIN) (PORT & (1<<PIN))

struct{
uint16_t Timer_count;
volatile uint8_t index;
}Sunroof;


int main()
{
    uint16_t ADC_FINAL=0;

    CLR_BIT(DDRD,PD2);   //For Opening Of Switch. PD2 as input.
    SET_BIT(PORTD,PD2);  //PULL up configuration.
    CLR_BIT(DDRD,PD3);   //For Closing Of Switch. PD3 as Input
    SET_BIT(PORTD,PD3);  //PULL up configuration.
    CLR_BIT(DDRD,PD4);   //For Checking the status Of AC switch. PD4 as Input
    SET_BIT(PORTD,PD4);  //PULL up configuration
    CLR_BIT(DDRD,PD5);   // For Checking the status of Ignition. PD5 as Input.
    SET_BIT(PORTD,PD5);  //PULL up configuration
    CLR_BIT(DDRD,PD7);   // For Checking the status Of Rain. Making PD7 as Input
    SET_BIT(PORTD,PD7);  //PULL up configuration
    ADC_init();
    ENABLE_ADC;

    timer_init();
    PWM_init();
    TIMSK0 |= (1<<TOIE0);
    TIFR0 |= (1<<TOV0);
    //STOP_TIMER;
    INTERRUPT_INIT;

    while(1)         // Super Loop
    {
        if(!(READ_BIT(PIND,PD5))) //Checking the engine ignition
        {
            if(!(READ_BIT(PIND,PD2))) //For Opening //Test-Case-1
            {
                 if(!(READ_BIT(PIND,PD7)))  //For Rain. If Rain Started.. //Test-Case-2
                    {
                        OCR0A = 64;   //If Rain started, the sunroof will close. 25% duty cycle.
                    }


                       else if((READ_BIT(PIND,PD7)))  //If Rain Stopped
            {
                if (!(READ_BIT(PIND,PD4))) //For AC. If AC is ON. //Test-Case-3
                {


                        OCR0A = 64;  //If AC is On, the sunroof will close. For Closing 25% duty-cycle.
                }
                else
                    {
                    OCR0A = 192;
                }
            }
            else
            {
                OCR0A = 192;  //If none AC is On or it is not raining, If I press the switch to open, it will open. For Opening 50% duty-cycle.
            }

        }
            else if(!(READ_BIT(PIND,PD3))) //For Closing. Test-Case-4
            {
                ADC_FINAL = READ_ADC(); //For checking any object detect during Sunroof Panel Close. Test-case-5
                if(Sunroof.index==1)
                {
                 //Read ADC Value
                if(ADC_FINAL<=750) //If it is in the range of 0-750 it mean no object and it will continue Closing or else it will open.
                {
                    OCR0A = 64;
                }
            else
                {
                    OCR0A = 0; //Stop. If it detect some object, the sunroof will stop functioning there.
                }
                Sunroof.index = 0;
                }


            }




    }
}


    return 0;
}


ISR(TIMER0_OVF_vect)
{
    Sunroof.Timer_count++;          // Incrementing the timer count
    if(Sunroof.Timer_count>=1000)   // 4883 for 5 sec delay
    {
        Sunroof.Timer_count=0;
        Sunroof.index=1;
        //PORTD ^= (1<<7);
    }
}
