/**
 * @file conf.h
 *
 * @defgroup CONF Configurations
 *
 * @brief General configuration of the system.
 *
 */

#ifndef CONF_H
//#define CONF_H

#ifndef F_CPU
#define F_CPU 16000000UL // define a frequencia do microcontrolador - 16MHz
#endif                   /* ifndef F_CPU */

// CONFIGURACOES DE COMPILACAO
#define DEBUG_ON
#define VERBOSE_ON
//#define VERBOSE_ON_CAN_APP
//#define VERBOSE_ON_MACHINE
//#define VERBOSE_ON_ADC
#define VERBOSE_ON_INIT
#define VERBOSE_ON_ERROR
#define VERBOSE_ON_RELAY

#define CAN_SIGNATURE_SELF CAN_SIGNATURE_MIC19

// MODULES ACTIVATION
#define USART_ON
#define CAN_ON
#define CAN_DEPENDENT
#define ADC_ON
#define MACHINE_ON
#define LED_ON
#define BUZZER_ON
#define WATCHDOG_ON
//#define SLEEP_ON
//#define CHECK_MCS_ON

// PINS UPDATE FILTER CONFIGURATION
#define BOAT_ON_TO_UPDATE 10
#define MOTOR_ON_TO_UPDATE 10
#define DEAD_MEN_TO_UPDATE 10
#define EMERGENCY_ON_TO_UPDATE 10


#ifdef ADC_ON
// ADC CONFIGURATION
// note that changing ADC_FREQUENCY may cause problems with avg_sum_samples
// #define ADC_8BITS
#define ADC_FREQUENCY                       10000 // 20000
#define ADC_TIMER_PRESCALER                 64
#define ADC_TOP_CTC                         F_CPU/(ADC_TIMER_PRESCALER * 2UL * ADC_FREQUENCY) -1

#if ADC_TOP_CTC >= 256
    #error "Value for ADC timer top is greater than 8 bits"
#elif ADC_TOP_CTC < 2
    #error "Value for ADC timer top is too low, increase the prescaler"
#endif

#define ADC_AVG_VARIABLE_OVERFLOW_PROTECTION 4294967296 / 255 // 32bit variable/8bit variable(maximum value of adc)


/** @brief Circular buffer size definitions 
 * Using equal size for adc structures...
 * one could replicate the structure delacartion with different sizes
 * or even, % TODO use dynamic allocation for strucutres 
 */
#define ADC_AVG_SIZE_10                     64
#if (ADC_AVG_SIZE_10 == 0 || (ADC_AVG_SIZE_10 & (ADC_AVG_SIZE_10 - 1)) != 0)
    #error "ADC_AVG_SIZE_10 must be a power of 2!"
#endif
#define cbuf_adc0_SIZE                      ADC_AVG_SIZE_10  /**< Buffer size for ADC0 */
#define cbuf_adc0_SIZE_LOG2                 log2_function(cbuf_adc0_SIZE)   /**< Log2 of buffer size */
#define cbuf_adc1_SIZE                      ADC_AVG_SIZE_10  /**< Buffer size for ADC1 */
#define cbuf_adc1_SIZE_LOG2                 log2_function(cbuf_adc1_SIZE)    /**< Log2 of buffer size */
#endif //ADC_ON





#ifdef MACHINE_ON
// ----> Cbuf used + not a power module ---> no need for even numbers between ADC and Machine frequencies
// no need for clk divider 
// #define MACHINE_CLK_DIVIDER_VALUE           ((uint64_t)(uint32_t)MACHINE_FREQUENCY*(uint32_t)ADC_AVG_SIZE_10)/(ADC_FREQUENCY)           //<! machine_run clock divider
#define MACHINE_FREQUENCY                   100           //<! machine timer frequency in Hz
#define MACHINE_TIMER_PRESCALER             1024          //<! machine timer prescaler
#define MACHINE_TOP_CTC                     F_CPU/(MACHINE_TIMER_PRESCALER * 2UL * MACHINE_FREQUENCY) -1
#if MACHINE_TOP_CTC >= 256
    #error "Value for Machine timer top is greater than 8 bits"
#elif MACHINE_TOP_CTC < 2
    #error "Value for Machine timer top is too low, increase the prescaler"
#endif

// The machine frequency may not be superior of ADC_FREQUENCY/ADC_AVG_SIZE_10
#if MACHINE_FREQUENCY > (ADC_FREQUENCY/ADC_AVG_SIZE_10)
    #error "Machine runs faster than the required time to fill the ADC ring buffer"
#endif
#endif // MACHINE_ON

// INPUT PINS DEFINITIONS
/*EXAMPLE OF INPUT PIN DEFINITONS
#define     CHARGERELAY_PORT        PORTC
#define     CHARGERELAY_PIN         PINC
#define     CHARGERELAY_DDR         DDRC

#define     CHARGERELAY             PC2
#define     set_chargerelay()       set_bit(CHARGERELAY_PORT, CHARGERELAY)
#define     clr_chargerelay()       clr_bit(CHARGERELAY_PORT, CHARGERELAY)
*/

#define PUMPS_SWITCHES_PORT PORTC
#define PUMPS_SWITCHES_PIN PINC
#define PUMPS_SWITCHES_DDR DDRC
#define PUMP1_ON_SWITCH PC3
#define PUMP2_ON_SWITCH PC4

#define REVERSE_SWITCH_PORT PORTC
#define REVERSE_SWITCH_PIN PINC
#define REVERSE_SWITCH_DDR DDRC
#define REVERSE_SWITCH PC5

#define DMS_PORT PORTD
#define DMS_PIN PIND
#define DMS_DDR DDRD
#define DMS PD6

#define CTRL_SWITCHES_PORT PORTD
#define CTRL_SWITCHES_PIN PIND
#define CTRL_SWITCHES_DDR DDRD
#define EMERGENCY_SWITCH PD7
#define BOAT_ON_SWITCH PD5
#define MOTOR_ON_SWITCH PD3
#define MCC_ON_SWITCH PD2

#define MOTOR_PWM_POT ADC0
#define MOTOR_RAMP_POT ADC1
#define MDE_POSITION_POT ADC2
// #define 	MCC_POWER_POT			<Not used, it was replaced by MDE_POSITION_POT>

#ifdef LED_ON
#define     LED_PORT                PORTD
#define     LED_PIN                 PIND
#define     LED_DDR                 DDRD
#define     LED1                    PD6
#define     cpl_led(y)              cpl_bit(LED_PORT, y)
#define     set_led(y)              set_bit(LED_PORT, y)
#define     clr_led(y)              clr_bit(LED_PORT, y)
#else
#define     cpl_led()
#define     set_led()
#define     clr_led()
#endif // LED_ON

#ifdef BUZZER_ON
#define BUZZER_PORT PORTD
#define BUZZER_PIN PIND
#define BUZZER_DDR DDRD
#define BUZZER PD4
#define cpl_buzzer() cpl_bit(BUZZER_PORT, BUZZER)
#define set_buzzer() set_bit(BUZZER_PORT, BUZZER)
#define clr_buzzer() clr_bit(BUZZER_PORT, BUZZER)
#endif


#ifdef CAN_ON
#define SPI_ON
#define CAN_APP_SEND_STATE_FREQ     10//36000     //<! state msg frequency in Hz
#define CAN_APP_SEND_ADC_FREQ       50//6000      //<! adc msg frequency in Hz
// TODO others
#if MACHINE_FREQUENCY % CAN_APP_SEND_STATE_FREQ != 0
    #warning "CAN_APP_SEND_STATE_FREQ doesn't have a multiple equal to MACHINE_FREQUENCY, this frequency will be truncated"
#endif

#if MACHINE_FREQUENCY % CAN_APP_SEND_ADC_FREQ != 0
    #warning "CAN_APP_SEND_ADC_FREQ doesn't have a multiple equal to MACHINE_FREQUENCY, this frequency will be truncated"
#endif


// CANBUS DEFINITONS
// ----------------------------------------------------------------------------
/* Global settings for building the can-lib and application program.
 *
 * The following two #defines must be set identically for the can-lib and
 * your application program. They control the underlying CAN struct. If the
 * settings disagree, the underlying CAN struct will be broken, with
 * unpredictable results.
 * If can.h detects that any of the #defines is not defined, it will set them
 * to the default values shown here, so it is in your own interest to have a
 * consistent setting. Ommiting the #defines in both can-lib and application
 * program will apply the defaults in a consistent way too.
 *
 * Select if you want to use 29 bit identifiers.
 */
#define SUPPORT_EXTENDED_CANID 0

/* Select if you want to use timestamps.
 * Timestamps are sourced from a register internal to the AT90CAN.
 * Selecting them on any other controller will have no effect, they will
 * be 0 all the time.
 */
#define SUPPORT_TIMESTAMPS 0
#endif // CAN_ON

#endif /* ifndef CONF_H */
