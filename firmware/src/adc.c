#include "adc.h"
#include <util/delay.h>

volatile avg_t adc[ADC_LAST_CHANNEL + 1];

/**
 * @brief inicializa o ADC, configurado para conversão engatilhada com o timer0.
 */
void adc_init(void)
{

	ADMUX = (0 << REFS1) // AVcc with external capacitor at AREF pin
			| (1 << REFS0)
#ifdef ADC_8BITS
			| (1 << ADLAR); // ADC left adjusted -> using 8bits ADCH only
#else
			| (0 << ADLAR);						// ADC left adjusted -> using all 10 bits
#endif

	ADMUX = (ADMUX & 0xF8);

	ADCSRA = (1 << ADATE)	  // ADC Auto Trigger Enable
			 | (1 << ADIE)	  // ADC Interrupt Enable
			 | (1 << ADEN)	  // ADC Enable
			 | (1 << ADSC)	  // Do the first Start of Conversion
			 | (1 << ADPS2)	  // ADC Prescaller = 128;
			 | (1 << ADPS1) | (1 << ADPS0);

	ADCSRB = (0 << ADTS2) // Auto-trigger source: timer0 Compare Match A
			 | (1 << ADTS1) | (1 << ADTS0);
	// TIMER configurations

	// clr_bit(PRR0, PRTIM0);                          // Activates clock to timer0
	//  MODE 2 -> CTC with TOP on OCR1
	TCCR0A = (1 << WGM01) | (0 << WGM00)	  // mode 2
			 | (0 << COM0B1) | (0 << COM0B0)  // do nothing
			 | (0 << COM0A1) | (1 << COM0A0); // do nothing

	TCCR0B =
#if ADC_TIMER_PRESCALER == 1
		(0 << CS02) | (0 << CS01) | (1 << CS00) // Prescaler N=1
#elif ADC_TIMER_PRESCALER == 8
		(0 << CS02) | (1 << CS01) | (0 << CS00) // Prescaler N=8
#elif ADC_TIMER_PRESCALER == 64
		(0 << CS02) | (1 << CS01) | (1 << CS00) // Prescaler N=64
#elif ADC_TIMER_PRESCALER == 256
		(1 << CS02) | (0 << CS01) | (0 << CS00) // Prescaler N=256
#elif ADC_TIMER_PRESCALER == 1024
		(1 << CS02) | (0 << CS01) | (1 << CS00) // Prescaler N=1024
#else
		0
#endif
		| (0 << WGM02); // mode 2
	TCNT0 = 0;
	OCR0A = ADC_TIMER_TOP; // OCR2A = TOP = fcpu/(N*2*f) -1

	TIMSK0 = (1 << OCIE0A); // Ativa a interrupcao na igualdade de comparação do TC0 com OCR0A

	set_bit(DDRD, PD5);
}

/**
 * @brief MUX do ADC
 */
ISR(ADC_vect)
{
	cli();
	TIFR0 = (1 << OCF0A);
	uint8_t channel = ADMUX & 0x07; // read channel

	// Check overflow
	if ((uint32_t)(adc[channel].sum + ADC) > adc[channel].sum)
	{
		// Acquire sample
		adc[channel].sum += ADC;
		adc[channel].samples++;
	}

	if (channel++ >= ADC_LAST_CHANNEL)
		channel = ADC0;

	ADMUX = (ADMUX & 0xF8) | channel; // select next channel
	sei();
}

uint16_t adc_get_measurement_avg(adc_channels_t channel)
{
	if (channel > ADC_LAST_CHANNEL)
		return 0;

	// when are samples compute average, else use last average
	if (adc[channel].samples)
	{
		adc[channel].avg = adc[channel].sum / adc[channel].samples;
		// Clear variables
		adc[channel].sum = adc[channel].samples = 0;
	}
	return adc[channel].avg;
}

/**
 * @brief ISR necessária para auto-trigger do ADC. Caso contrário, dispara
 * BADISR_vect.
 */
ISR(TIMER0_COMPA_vect)
{
}
