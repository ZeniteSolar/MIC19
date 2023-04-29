#include "adc.h"
#include <util/delay.h>

volatile adc_t adc;

/**
 * @brief Changes ADC channel
 * @param __ch is the channel to be switched to
 * @return return the selected channel
 */
inline uint8_t adc_select_channel(adc_channels_t __ch)
{
    if(__ch < ADC_LAST_CHANNEL ) adc.select = __ch;

    ADMUX = (ADMUX & 0xF8) | adc.select; // clears the bottom 3 bits before ORing
    return adc.select;
}

/**
 * @brief inicializa o ADC, configurado para conversão engatilhada com o timer0.
 */
void adc_init(void)
{
    adc.ready = 0;
    adc.select = ADC0;

    //clr_bit(PRR, PRADC);                           // Activates clock to adc

    // configuracao do ADC
    // clr_bit(PORTC,ADC0);
    // clr_bit(PORTC,ADC1);
    // clr_bit(PORTC,ADC2);                            // disables pull-up for adcs pins
    // DDRC    =   0b00000000;                         // all adcs as inputs
    // set_bit(DIDR0,ADC0);
    // set_bit(DIDR0,ADC1);
    // set_bit(DIDR0,ADC2);                            // ADC0 to ADC2 as adc (digital disable)

    ADMUX   =   (0 << REFS1)                        // AVcc with external capacitor at AREF pin
            | (1 << REFS0)
#ifdef ADC_8BITS
            | (1 << ADLAR);                         // ADC left adjusted -> using 8bits ADCH only
#else
            | (0 << ADLAR);                         // ADC left adjusted -> using all 10 bits
#endif

    adc_select_channel(ADC0);                       // Choose admux
    ADCSRA  =   (1 << ADATE)                        // ADC Auto Trigger Enable
            | (1 << ADIE)                           // ADC Interrupt Enable
            | (1 << ADEN)                           // ADC Enable
            | (1 << ADSC)                           // Do the first Start of Conversion
            | (1 << ADPS2)                          // ADC Prescaller = 128;
            | (1 << ADPS1)
            | (1 << ADPS0);


    ADCSRB  =   (0 << ADTS2)                        // Auto-trigger source: timer0 Compare Match A
            | (1 << ADTS1)
            | (1 << ADTS0);
    // TIMER configurations

    //clr_bit(PRR0, PRTIM0);                          // Activates clock to timer0
    // MODE 2 -> CTC with TOP on OCR1
    TCCR0A  =   (1 << WGM01) | (0 << WGM00)         // mode 2
            | (0 << COM0B1) | (0 << COM0B0)         // do nothing
            | (0 << COM0A1) | (1 << COM0A0);        // do nothing

    TCCR0B  =
#if ADC_TIMER_PRESCALER ==     1
                (0 << CS02) | (0 << CS01) | (1 << CS00) // Prescaler N=1
#elif ADC_TIMER_PRESCALER ==   8
                (0 << CS02) | (1 << CS01) | (0 << CS00) // Prescaler N=8
#elif ADC_TIMER_PRESCALER ==   64
                (0 << CS02) | (1 << CS01) | (1 << CS00) // Prescaler N=64
#elif ADC_TIMER_PRESCALER ==   256
                (1 << CS02) | (0 << CS01) | (0 << CS00) // Prescaler N=256
#elif ADC_TIMER_PRESCALER ==   1024
                (1 << CS02) | (0 << CS01) | (1 << CS00) // Prescaler N=1024
#else
                0
#endif
                | (0 << WGM02);      // mode 2
    TCNT0 = 0;
    OCR0A = ADC_TIMER_TOP;                       	// OCR2A = TOP = fcpu/(N*2*f) -1


    TIMSK0 =   (1 << OCIE0A);                      // Ativa a interrupcao na igualdade de comparação do TC0 com OCR0A

set_bit(DDRD, PD5);
}

/**
 * @brief MUX do ADC
 */
ISR(ADC_vect)
{
	cli();
    static const float vi_coeff = 0.06582490575070313f;
    static const float vo_coeff = 0.06717781789490249f;
    static const float io_coeff = 0.01599315004f;
	TIFR0 = (1 << OCF0A);
    uint16_t adc = ADC;                     // read adc
    uint8_t channel = ADMUX & 0x07;         // read channel

	   switch(channel){
        case ADC0:
            
            break;

        case ADC1:                       
            
            break;

        case ADC2: default:
            
            channel = 255;             // recycle
            break;
    }
	set_bit(PORTD, PD5);
	_delay_us(40);
	clr_bit(PORTD, PD5);
    ADMUX = (ADMUX & 0xF8) | ++channel;   // select next channel
	sei();
}

/**
 * @brief ISR necessária para auto-trigger do ADC. Caso contrário, dispara
 * BADISR_vect.
 */
ISR(TIMER0_COMPA_vect)
{

}
