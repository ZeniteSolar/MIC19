/**
 * @file adc.h
 *
 * @defgroup ADC    ADC Module
 *
 * @brief This module implements a simple ADC using a state machine to mux
 * between the adc channels.
 *
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "conf.h"
#include "dbg_vrb.h"
#include "usart.h"
#include "../lib/bit_utils.h"
#include "../lib/log2.h"

#define ADC_LAST_CHANNEL (uint8_t)ADC2

// Equations for mode 2 (CTC with TOP OCR2A)
// Note the resolution. For example.. at 150hz, ICR1 = PWM_TOP = 159, so it
//#define QUOTIENT  (((uint32_t)MACHINE_TIMER_PRESCALER)*((uint32_t)MACHINE_TIMER_FREQUENCY))
//#define ADC_TIMER_TOP (0.5*(F_CPU)/QUOTIENT)
#define ADC_TIMER_FREQUENCY     (uint32_t)((ADC_FREQUENCY) * (ADC_LAST_CHANNEL + 1))
#define ADC_TIMER_TOP           (uint8_t)(((F_CPU) / ((ADC_TIMER_PRESCALER) * (ADC_TIMER_FREQUENCY))) - 1)

typedef enum adc_channels{
    ADC0, ADC1 ,ADC2, ADC3, ADC4, ADC5
} adc_channels_t;                           //*< the adc_channel type

typedef struct{
    uint32_t sum;
    uint16_t avg;
    uint16_t samples;
} avg_t;


void adc_init(void);

/**
 * @brief 
 * 
 * @param channel 
 * @return uint16_t 
 */
uint16_t adc_get_measurement_avg(adc_channels_t channel);

#endif /* ifndef _ADC_H_ */
