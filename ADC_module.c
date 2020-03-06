#include <avr/io.h>
#include "ADC_module.h"

void ADC_init()
{
    CLR_BIT(DDRC,PC5);   // this will be the input from Temperature and humidity sensor PC5 Is analog PIN
    ADMUX |= ((1<<REFS0));
    ADMUX |= ((1<<MUX0));
    ADMUX |= ((1<<MUX2)); // reference voltage and left adjust
    ADCSRA |= ((1<<ADPS1)|(1<<ADPS2)); //adc enable // division factor
}

int READ_ADC()
{
    START_ADC;
    while(ADCSRA & (1<<ADSC));
  //  Adc_val = ((ADCH<<8)&0xFF00)|(ADCL);
   // return Adc_val;
   return ADC;
}


