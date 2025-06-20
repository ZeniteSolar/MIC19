/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "conf.h"

// safety limits for adc values
// ---

// PRINT INFOS CONSTANTS
#define PRINT_INFOS_TIME        2 // seconds for half a period 
#define PRINT_INFOS_CLK_DIV     PRINT_INFOS_TIME * MACHINE_FREQUENCY

// LED CONSTANTS
#define IDLE_LED_TIME           1 // half a period...time that the LED stays on
#define IDLE_LED_CLK_DIV        IDLE_LED_TIME * 2 * MACHINE_FREQUENCY //  
#define RUNNING_LED_TIME        0.25 // half a period...time that the LED stays on
#define RUNNING_LED_CLK_DIV     RUNNING_LED_TIME * 2 * MACHINE_FREQUENCY //  
#define ERROR_LED_TIME          0.05 // half a period...time that the LED stays on
#define ERROR_LED_CLK_DIV       ERROR_LED_TIME * 2 * MACHINE_FREQUENCY //  


#ifdef ADC_ON
#include "adc.h"
#define MA_MOTOR_PWM_TARGET         ma_adc0()
#define MA_MDE_POSITION_TARGET      ma_adc2()
#endif
#ifdef USART_ON
#include "usart.h"
#endif
#include "dbg_vrb.h"
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
extern const uint8_t can_filter[];
#endif

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
    STATE_RESET,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     boat_on                :1;
        uint8_t     boat_switch_on         :1;
        uint8_t     MCS_on                 :1;
        uint8_t     motor_on               :1;
        uint8_t     MCC_on                 :1;
        uint8_t     dead_men_switch        :1;
        uint8_t     emergency              :1;
        uint8_t     reverse                :1;
    };
    uint8_t   all__;
} system_flags_t;


typedef union pump_flags{
    struct{
        uint8_t     pump1_on               :1;
        uint8_t     pump2_on               :1;
    };
    uint8_t   all__;
}pump_flags_t;


typedef union error_flags{
    struct{
        uint8_t     no_canbus     :1;
    };
    uint8_t   all;
}error_flags_t;

typedef struct{
    uint32_t sum;
    uint16_t avg;
    uint32_t samples;
} sub_control_t;

typedef struct control
{
    uint16_t motor_PWM_target;
    uint16_t motor_RAMP_target;
    uint16_t MCC_POWER_target;
    uint16_t mde_steering_wheel_position;
}control_t;



// machine checks
void check_buffers(void);
void read_and_check_adcs(void);

// debug functions
void print_configurations(void);
void print_system_flags(void);
void print_error_flags(void);
void print_infos(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);
void task_reset(void);
void task_waiting_reset(void);

// the machine itself
void set_machine_initial_state(void);
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);
void set_state_reset(void);
void set_state_waiting_reset(void);

//input functions
void read_switches(void);
void read_potentiometers(void);
void read_boat_on(void);
void read_pump_switches(void);
void reset_switches(void);

void buzzer(uint8_t buzzer_frequency, uint8_t buzzer_rhythm_on, uint8_t buzzer_rhythm_off);

// machine variables
extern volatile state_machine_t state_machine;
extern volatile control_t control;
extern volatile pump_flags_t pump_flags;
extern volatile system_flags_t system_flags;
extern volatile error_flags_t error_flags;
extern volatile uint16_t charge_count_error;
extern volatile uint8_t relay_clk;
extern volatile uint8_t first_boat_off;
extern volatile uint8_t machine_clk;
extern volatile uint8_t machine_clk_divider;
extern volatile uint8_t total_errors;           // Contagem de ERROS
extern volatile uint8_t reset_clk;

// other variables
extern volatile uint8_t led_clk_div;

#endif /* ifndef MACHINE_H */
