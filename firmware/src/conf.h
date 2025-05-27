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
//#define ADC_8BITS
// ADC CONFIGURATION
// note that changing ADC_FREQUENCY may cause problems with avg_sum_samples
#define ADC_FREQUENCY 1000U // 20000
#define ADC_TIMER_PRESCALER 64UL
#define ADC_AVG_SIZE_2 7    // in base 2
#define ADC_AVG_SIZE_10 128 // in base 10

#define ADC_AVG_VARIABLE_OVERFLOW_PROTECTION 4294967296 / 255 // 32bit variable/8bit variable(maximum value of adc)

//#define POTENTIOMETER_LOW_TRIGGER 2
//#define POTENTIOMETER_HIGH_TRIGGER 2

//#define FAKE_ADC_ON
#ifdef FAKE_ADC_ON
#define FAKE_ADC 1
#endif // FAKE_ADC_ON

#endif // ADC_ON

#ifdef MACHINE_ON
// The machine frequency may not be superior of ADC_FREQUENCY/ADC_AVG_SIZE_10
#define MACHINE_TIMER_FREQUENCY 120UL  //<! machine timer frequency in Hz
#define MACHINE_TIMER_PRESCALER 1024UL //<! machine timer prescaler
#define MACHINE_FREQUENCY MACHINE_TIMER_FREQUENCY

// SCALE TO CONVERT ADC DEFINITIONS
#define VSCALE (uint16_t)1000

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
#define PUMP2_ON_SWITCH PC4 // Will be used as an on/off switch for addon modules

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
#define LED_PORT PORTD
#define LED_PIN PIND
#define LED_DDR DDRD
#define LED1 PD6
#define cpl_led(y) cpl_bit(LED_PORT, y)
#define set_led(y) set_bit(LED_PORT, y)
#define clr_led(y) clr_bit(LED_PORT, y)
#else
#define cpl_led()
#define set_led()
#define clr_led()
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
