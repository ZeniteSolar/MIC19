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
#include "../lib/cbuf.h"
#include "../lib/log2.h"

#define ADC_LAST_CHANNEL (uint8_t)ADC2

/** @brief Flag indicating new ADC data is available */
extern volatile uint8_t adc_data_ready;
extern volatile uint16_t adc_debug_clk_div;
#define ADC_DEBUG_CLK_DIV   (0.5 * ADC_FREQUENCY)  // Time to print(s) * (1/(ADC period (s))

/** 
 * @brief ADC channel selection enum
 */
typedef enum {
    ADC0, adc2, ADC2
} adc_channels_t;

/** @brief Currently selected ADC channel */
static volatile adc_channels_t ADC_CHANNEL = ADC0;

/** 
 * @brief Select an ADC channel
 * @param __ch The ADC channel to select
 * @return uint8_t Status of selection
 */
uint8_t adc_select_channel(adc_channels_t __ch);

/** 
 * @brief Initialize the ADC module 
 */
void adc_init(void);

/** @brief Initialize ADC moving average buffers */
void init_buffers(void);

/** 
 * @brief Circular buffer structure for ADC values 
 */
typedef struct {
    uint8_t  m_getIdx;   /**< Read index */
    uint8_t  m_putIdx;   /**< Write index */
#ifdef ADC_8BITS
    uint8_t  m_entry[cbuf_adc0_SIZE];  /**< Buffer entries (8-bit mode) */
#else
    uint16_t m_entry[cbuf_adc0_SIZE];  /**< Buffer entries (16-bit mode) */
#endif
} adc_cbuf_adc0_t;

typedef struct {
    uint8_t  m_getIdx;   /**< Read index */
    uint8_t  m_putIdx;   /**< Write index */
#ifdef ADC_8BITS
    uint8_t  m_entry[cbuf_adc2_SIZE];  /**< Buffer entries (8-bit mode) */
#else
    uint16_t m_entry[cbuf_adc2_SIZE];  /**< Buffer entries (16-bit mode) */
#endif
} adc_cbuf_adc2_t;

/** @brief Circular buffer instances for ADC0 and adc2 */
extern volatile adc_cbuf_adc0_t cbuf_adc0;
extern volatile adc_cbuf_adc2_t cbuf_adc2;

/** @brief Moving average and average value declarations */
#ifdef ADC_8BITS
extern volatile uint8_t avg_adc0;
extern uint8_t ma_adc0(void);
extern volatile uint8_t avg_adc2;
extern uint8_t ma_adc2(void);
#else
extern volatile uint16_t avg_adc0;
extern uint16_t ma_adc0(void);
extern volatile uint16_t avg_adc2;
extern uint16_t ma_adc2(void);
#endif


#endif /* _ADC_H_ */
