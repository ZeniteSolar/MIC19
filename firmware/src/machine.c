#include "machine.h"

volatile state_machine_t state_machine;
volatile control_t control;
volatile pump_flags_t pump_flags;
volatile system_flags_t system_flags;
volatile error_flags_t error_flags;
volatile uint16_t charge_count_error;
volatile uint8_t relay_clk;
volatile uint8_t first_boat_off;
volatile uint8_t machine_clk;
volatile uint8_t machine_clk_divider;
volatile uint8_t total_errors; // Contagem de ERROS

volatile uint16_t print_clk_div;
volatile uint8_t led_clk_div;

// Controle da zenira
volatile uint8_t ctrl_bit_zenira;
volatile control_zenira_t control_zenira;
volatile system_flags_zenira_t system_flags_zenira;

/**
 * @brief
 */
void machine_init(void) {
  // clr_bit(PRR0, PRTIM2);                          // Activates clock

  // MODE 2 -> CTC with TOP on OCR1
  TCCR2A = (1 << WGM21) | (0 << WGM20)      // mode 2
           | (0 << COM2B1) | (0 << COM2B0)  // do nothing
           | (0 << COM2A1) | (0 << COM2A0); // do nothing

  TCCR2B =
#if MACHINE_TIMER_PRESCALER == 1
      (0 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=1
#elif MACHINE_TIMER_PRESCALER == 8
      (0 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=8
#elif MACHINE_TIMER_PRESCALER == 32
      (0 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=32
#elif MACHINE_TIMER_PRESCALER == 64
      (1 << CS22) | (0 << CS21) | (0 << CS20) // Prescaler N=64
#elif MACHINE_TIMER_PRESCALER == 128
      (1 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=128
#elif MACHINE_TIMER_PRESCALER == 256
      (1 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=256
#elif MACHINE_TIMER_PRESCALER == 1024
      (1 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=1024
#else
      0
#endif
      | (0 << WGM22); // mode 2

  OCR2A = MACHINE_TOP_CTC; // OCR2A = TOP = fcpu/(N*2*f) -1
                           // (TOP+1)*N*2/fcpu = 1/f

  TIMSK2 |= (1 << OCIE2A); // Activates interruption

  set_machine_initial_state();
  set_state_initializing();
}

/**
 * @brief set machine initial state
 */
inline void set_machine_initial_state(void) {
  error_flags.all = 0;
  machine_clk = machine_clk_divider = led_clk_div = 0;
}

/**
 * @brief set error state
 */
inline void set_state_error(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("\n>>>STATE ERROR\n"));
  state_machine = STATE_ERROR;
}

/**
 * @brief set initializing state
 */
inline void set_state_initializing(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("\n>>>INITIALIZING STATE\n"));
  state_machine = STATE_INITIALIZING;
}

/**
 * @brief set idle state
 */
inline void set_state_idle(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("\n>>>IDLE STATE\n"));
  state_machine = STATE_IDLE;
}

/**
 * @brief set running state
 */
inline void set_state_running(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RUNNING STATE\n"));
  state_machine = STATE_RUNNING;
}

/**
 * @brief set reset state
 */
inline void set_state_reset(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RESET STATE\n"));
  state_machine = STATE_RESET;
}

/**
 * @breif prints the configurations and definitions
 */
inline void print_configurations(void) {
  VERBOSE_MSG_MACHINE(usart_send_string("CONFIGURATIONS:\n"));

  VERBOSE_MSG_MACHINE(usart_send_string("\nadc_f: "));
  VERBOSE_MSG_MACHINE(usart_send_uint16(ADC_FREQUENCY));
  VERBOSE_MSG_MACHINE(usart_send_string(", adc0 size: "));
  VERBOSE_MSG_MACHINE(usart_send_uint16(cbuf_adc0_SIZE));
  VERBOSE_MSG_MACHINE(usart_send_string(", adc1 size: "));
  VERBOSE_MSG_MACHINE(usart_send_uint16(cbuf_adc0_SIZE));
  VERBOSE_MSG_MACHINE(usart_send_string("\nmachine_f: "));
  VERBOSE_MSG_MACHINE(usart_send_uint16(MACHINE_FREQUENCY));

  VERBOSE_MSG_MACHINE(usart_send_char('\n'));
}

/**
 * @brief prints the system flags
 */
inline void print_system_flags(void) {
  static uint8_t count = 0;

  switch (count++) {
  case 0:
    usart_send_string("B:");
    usart_send_char('0' + system_flags.boat_on);
    usart_send_string(" ");
    break;
  case 1:
    usart_send_string(",B_s:");
    usart_send_char('0' + system_flags.boat_switch_on);
    usart_send_string(" ");
    break;
  case 2:
    usart_send_string(",MAM:");
    usart_send_char('0' + system_flags.motor_on);
    usart_send_string(" ");
    break;
  case 3:
    usart_send_string(",MCS:");
    usart_send_char('0' + system_flags.MCS_on);
    usart_send_string(" ");
    break;
  case 4:
    usart_send_string(",MCC:");
    usart_send_char('0' + system_flags.MCC_on);
    usart_send_string(" ");
    break;
  case 5:
    usart_send_string(",DMS:");
    usart_send_char('0' + system_flags.dead_men_switch);
    usart_send_string(" ");
    break;
  case 6:
    usart_send_string(",emer:");
    usart_send_char('0' + system_flags.emergency);
    usart_send_string(" ");
    break;
  case 7:
    usart_send_string(",re:");
    usart_send_char('0' + system_flags.reverse);
    usart_send_string(" ");
    break;
  case 8:
    usart_send_string("V: ");
    usart_send_uint16(control.motor_PWM_target);
    usart_send_string(" ");
    break;
  case 9:
    usart_send_string("|A: ");
    usart_send_uint16(control.motor_RAMP_target);
    usart_send_string(" ");
    break;
  case 10:
    usart_send_string("|D: ");
    usart_send_uint16(control.mde_steering_wheel_position);
    usart_send_string(" ");
    break;

  default:
    usart_send_char('\n');
    count = 0;
    break;
  }
}

/**
 * @brief prints the error flags
 */
inline void print_error_flags(void) {
  // VERBOSE_MSG_MACHINE(usart_send_string(" errFl: "));
  // VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.no_canbus));
}

inline void read_and_check_adcs(void) {
  // Sensitivity thresholds and safety limits
  #define MOTOR_PWM_ZENIRA_THRESHOLD 15    // Minimum change to detect pilot input on throttle
  #define MDE_STEERING_ZENIRA_THRESHOLD 15 // Minimum change to detect pilot input on steering
  #define MDE_STEERING_MAX 950            // Maximum steering value to prevent jamming
  #define MOTOR_PWM_MAX 1023               // Maximum motor PWM value (10-bit ADC)

  static uint16_t motor_pwm_previous = 0;
  static uint16_t mde_steering_previous = 0;
  uint16_t motor_pwm_current;
  uint16_t mde_steering_current;
  uint16_t motor_pwm_delta;
  uint16_t mde_steering_delta;
  uint8_t motor_pwm_changed = 0;
  uint8_t mde_steering_changed = 0;

#ifdef ADC_ON
  // Read current ADC values
  motor_pwm_current = MA_MOTOR_PWM_TARGET;
  mde_steering_current = MA_MDE_POSITION_TARGET;
#else
  motor_pwm_current = control.motor_PWM_target;
  mde_steering_current = control.mde_steering_wheel_position;
#endif

  // Safety: Clamp steering to maximum value to prevent jamming
  if (mde_steering_current > MDE_STEERING_MAX) {
    mde_steering_current = MDE_STEERING_MAX;
  }

  // Safety: Clamp motor PWM to maximum value
  if (motor_pwm_current > MOTOR_PWM_MAX) {
    motor_pwm_current = MOTOR_PWM_MAX;
  }

  // Calculate change magnitude for motor PWM
  if (motor_pwm_current > motor_pwm_previous) {
    motor_pwm_delta = motor_pwm_current - motor_pwm_previous;
  } else {
    motor_pwm_delta = motor_pwm_previous - motor_pwm_current;
  }

  // Calculate change magnitude for steering position
  if (mde_steering_current > mde_steering_previous) {
    mde_steering_delta = mde_steering_current - mde_steering_previous;
  } else {
    mde_steering_delta = mde_steering_previous - mde_steering_current;
  }

  // Motor PWM logic
  if (ctrl_bit_zenira) {
    // Zenira is active: only override if pilot movement exceeds threshold
    if (motor_pwm_delta > MOTOR_PWM_ZENIRA_THRESHOLD) {
      if(motor_pwm_current < MOTOR_PWM_ZENIRA_THRESHOLD) {
        // Below threshold: cut motor command to zero
        control.motor_PWM_target = 0;
        control_zenira.motor_PWM_target_zenira = 0;
        motor_pwm_previous = 0;
      } 
      else {
        // Pilot moved significantly: accept pilot input
        control.motor_PWM_target = motor_pwm_current;
        control_zenira.motor_PWM_target_zenira = motor_pwm_current;
        motor_pwm_previous = motor_pwm_current;
      }
    } 
    else {
      // Small or no change: keep zenira control
      control.motor_PWM_target = control_zenira.motor_PWM_target_zenira;
    }
  } else {
    // Zenira is inactive: accept all pilot changes
    control.motor_PWM_target = motor_pwm_current;
    control_zenira.motor_PWM_target_zenira = motor_pwm_current;
    motor_pwm_previous = motor_pwm_current;
  }

  // Steering position logic
  if (ctrl_bit_zenira) {
    // Zenira is active: only override if pilot movement exceeds sensitivity
    if (mde_steering_delta > MDE_STEERING_ZENIRA_THRESHOLD) {
      // Pilot moved significantly: accept pilot input
      control.mde_steering_wheel_position = mde_steering_current;
      control_zenira.mde_steering_wheel_position_zenira = mde_steering_current;
      mde_steering_previous = mde_steering_current;
    } else {
      // Small or no change: keep zenira control
      control.mde_steering_wheel_position = control_zenira.mde_steering_wheel_position_zenira;
    }
  } else {
    // Zenira is inactive: accept all changes (even small ones)
    control.mde_steering_wheel_position = mde_steering_current;
    control_zenira.mde_steering_wheel_position_zenira = mde_steering_current;
    mde_steering_previous = mde_steering_current;
  }

  //   switch (state_machine) {
  //   case STATE_INITIALIZING:
  //     check_battery_voltage();

  //     break;
  //   case STATE_IDLE:
  //     check_battery_voltage();

  //     break;
  //   case STATE_RUNNING:
  //     check_battery_voltage();

  //     break;
  //   default:
  //     break;
  //   }
}

inline void task_initializing(void) {
#ifdef LED_ON
  set_led(LED1);
#endif

  set_machine_initial_state();

  VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
  set_state_idle();
}

/**
 * @brief waits for commands while checking the system
 */
inline void task_idle(void) {

#ifdef LED_ON
  if (led_clk_div++ >= IDLE_LED_CLK_DIV) {
    cpl_led(LED1);
    led_clk_div = 0;
  }
#endif
#ifdef BUZZER_ON
  buzzer(1, 4, 90);

#endif

  reset_switches();

  read_boat_on();

  read_pump_switches();

  // Read DMS to keep it updated for display
  read_switches();

#ifdef CHECK_MCS_ON
  if (system_flags.MCS_on && system_flags.boat_on && system_flags.dead_men_switch)
    set_state_running();
#else
  if (system_flags.boat_on && system_flags.dead_men_switch)
    set_state_running();
#endif
}

/**
 * @brief running task checks the system and apply the control action to pwm.
 */
inline void task_running(void) {

  read_switches();

  read_boat_on();

  read_pump_switches();

#ifdef LED_ON
  if (led_clk_div++ >= RUNNING_LED_CLK_DIV) {
    cpl_led(LED1);
    led_clk_div = 0;
  }
#endif // LED_ON
#ifdef BUZZER_ON
  buzzer(10, 1, 64);
  // buzzer(4, 8, 0);
#endif // BUZZER_ON

  // Dead Men Switch is mandatory during operation
  if (!system_flags.boat_on || !system_flags.dead_men_switch)
    set_state_idle();
}

inline void buzzer(uint8_t buzzer_frequency, uint8_t buzzer_rhythm_on,
                   uint8_t buzzer_rhythm_off) {
  static uint8_t buzzer_frequency_clk_div = 0;
  static uint8_t buzzer_rhythm_clk_div = 0;

  /*

          |---|   |---|           |---|   |---|
          |   |   |   |           |   |   |   |
          |   |   |   |           |   |   |   |
  ----|   |---|   |-----------|   |---|   |-----

  ^^           ^   ^          ^
  ||-----------|   |          |
  |buzzer_frequency|          |
  |                |          |
  |----------------|----------|
   buzzer_rhythm_on|buzzer_rhythm_off
  */

  if (++buzzer_rhythm_clk_div <= buzzer_rhythm_on) {
    if (++buzzer_frequency_clk_div >= buzzer_frequency) {
      buzzer_frequency_clk_div = 0;
      cpl_buzzer();
    }
  } else {
    clr_buzzer();
    if (buzzer_rhythm_clk_div >= buzzer_rhythm_off + buzzer_rhythm_on)
      buzzer_rhythm_clk_div = 0;
  }
}

inline void read_boat_on(void) {
  enum { ON, OFF };
  static uint8_t count_boat_state[2] = {0, 0};
  static uint8_t count_emergency_state[2] = {0, 0};
  static uint8_t boat_switch_on_previous = 0;  // To detect edge transitions
  static uint8_t boat_switch_on_previous_zenira = 0;  // To detect zenira activation
  static uint8_t ctrl_bit_zenira_previous = 0; // To detect zenira control changes
  uint8_t boat_switch_on_edge = 0;  // Flag for edge detection

  // BOAT SWITCH - Always read the physical switch
  if (tst_bit(CTRL_SWITCHES_PIN, BOAT_ON_SWITCH)) {
    if (++count_boat_state[ON] >= BOAT_ON_TO_UPDATE) {
      count_boat_state[OFF] = 0;
      system_flags.boat_switch_on = 1;
      system_flags_zenira.boat_switch_on_zenira = 1;
    }
  } else {
    if (++count_boat_state[OFF] >= BOAT_ON_TO_UPDATE) {
      count_boat_state[ON] = 0;
      system_flags.boat_switch_on = 0;
      system_flags_zenira.boat_switch_on_zenira = 0;
    }
  }

  // Detect edge transition (ON->OFF or OFF->ON) on physical switch
  if (system_flags.boat_switch_on != boat_switch_on_previous) {
    boat_switch_on_edge = 1;
    boat_switch_on_previous = system_flags.boat_switch_on;
  }

  // Sync boat_switch_on_previous when zenira state changes to avoid false edges
  if (system_flags_zenira.boat_on_zenira != boat_switch_on_previous_zenira) {
    boat_switch_on_previous = system_flags.boat_switch_on;
    boat_switch_on_previous_zenira = system_flags_zenira.boat_on_zenira;
  }

  if(ctrl_bit_zenira != ctrl_bit_zenira_previous) {
    // Zenira control state changed, sync previous switch state
    boat_switch_on_edge = 1; // Prevent edge detection on control change
    ctrl_bit_zenira_previous = ctrl_bit_zenira;
  }
  // END OF BOAT SWITCH

  // EMERGENCY SWITCH
  if (!tst_bit(CTRL_SWITCHES_PIN, EMERGENCY_SWITCH)) {
    if (++count_emergency_state[ON] >= EMERGENCY_ON_TO_UPDATE) {
      count_emergency_state[OFF] = 0;
      system_flags.emergency = 1;
      system_flags_zenira.emergency_zenira = 1;
    }
  } 
  else {
    if (++count_emergency_state[OFF] >= EMERGENCY_ON_TO_UPDATE) {
      count_emergency_state[ON] = 0;
      system_flags.emergency = 0;
      system_flags_zenira.emergency_zenira = 0;
    }
  }

  // EMERGENCY EXCLUSIVE ON HARDWARE TO MAC
  // This means emergency will be set as always "safe"=1
  // system_flags.emergency = 1;
  // END OF EMERGENCY SWITCH

  // boat_on logic: 
  // Zenira controls normally, but pilot switch edge transition has priority
  // When pilot moves the switch, boat_on follows the switch position once
  // Then zenira can control again
  
  if (system_flags.emergency) {
    // Emergency is active - allow boat control
    if (boat_switch_on_edge) {
      // Pilot moved the switch: follow it
      system_flags.boat_on = system_flags.boat_switch_on;
      system_flags_zenira.boat_on_zenira = system_flags.boat_on;
    } else if (ctrl_bit_zenira) {
      // No switch movement: zenira controls
      system_flags.boat_on = system_flags_zenira.boat_on_zenira;
    }
    // If ctrl_bit_zenira is not active and no edge, keep current state
  } else {
    // Emergency not active: boat must be OFF
    system_flags.boat_on = 0;
    system_flags_zenira.boat_on_zenira = 0;
  }
}

inline void reset_switches(void) {
  system_flags.motor_on = 0;
  system_flags.dead_men_switch = 0;
  system_flags.MCC_on = 0;
  system_flags.emergency = 0;

  system_flags_zenira.motor_on_zenira = 0;
  system_flags_zenira.dead_men_switch_zenira = 0;
  system_flags_zenira.MCC_on_zenira = 0;
  system_flags_zenira.emergency_zenira = 0;
}

inline void read_pump_switches(void) {

  if (tst_bit(PUMPS_SWITCHES_PIN, PUMP1_ON_SWITCH))
    pump_flags.pump1_on = 1;
  else
    pump_flags.pump1_on = 0;

  ctrl_bit_zenira = pump_flags.pump1_on;

  if (tst_bit(PUMPS_SWITCHES_PIN, PUMP2_ON_SWITCH))
    pump_flags.pump2_on = 0;
  else
    pump_flags.pump2_on = 1;
}

inline void read_switches(void) {
  enum { ON, OFF };
  static uint8_t count_motor_state[2] = {0, 0};
  static uint8_t count_DMS_state[2] = {0, 0};

  static uint8_t motor_on_current = 0;
  static uint8_t motor_on_previous = 0;  // To detect edge transitions on physical switch
  static uint8_t motor_on_previous_zenira = 0;  // To detect zenira state changes
  uint8_t motor_on_edge = 0;  // Flag for edge detection


  // TEST DIGITAL PINS AND FILTER THEM

  // MOTOR SWITCH
  if (tst_bit(CTRL_SWITCHES_PIN, MOTOR_ON_SWITCH)) {
    if (++count_motor_state[ON] >= MOTOR_ON_TO_UPDATE) {
      count_motor_state[OFF] = 0;
      motor_on_current = 1;
    }
  } else {
    if (++count_motor_state[OFF] >= MOTOR_ON_TO_UPDATE) {
      count_motor_state[ON] = 0;
      motor_on_current = 0;
    }
  }

  // Detect edge transition (ON->OFF or OFF->ON) on physical switch ONLY
  if (motor_on_current != motor_on_previous) {
    motor_on_edge = 1;
    motor_on_previous = motor_on_current;
    motor_on_previous_zenira = motor_on_current; // Sync zenira previous state
  }

  if (system_flags.emergency) {
    // Emergency is active - allow motor control
    if (motor_on_edge) {
      // Pilot moved the physical switch: pilot has priority
      system_flags.motor_on = motor_on_current;
      system_flags_zenira.motor_on_zenira = motor_on_current;

    } else if (ctrl_bit_zenira) {
      // No switch movement and zenira is active: zenira controls
      system_flags.motor_on = system_flags_zenira.motor_on_zenira;
    }
    // If ctrl_bit_zenira is not active and no edge, keep current state
  } else {
    // Emergency not active: motor must be OFF
    system_flags.motor_on = 0;
    system_flags_zenira.motor_on_zenira = 0;
  }
  // END OF MOTOR SWITCH

  // TEST DIGITAL PINS AND FILTER THEM

  // DEAD MEN SWITCH
  if (tst_bit(DMS_PIN, DMS)) {
    if (++count_DMS_state[ON] >= DEAD_MEN_TO_UPDATE) {
      count_DMS_state[OFF] = 0;
      system_flags.dead_men_switch = 1;
      system_flags_zenira.dead_men_switch_zenira = 1;
    }
  } else {
    if (++count_DMS_state[OFF] >= DEAD_MEN_TO_UPDATE) {
      count_DMS_state[ON] = 0;
      system_flags.dead_men_switch = 0;
      system_flags_zenira.dead_men_switch_zenira = 0;
    }
  }
  // DEADMAN EXCLUSIVE ON HARDWARE TO MAC
  // This means deadman will be set as always "safe"=1
  // system_flags.dead_men_switch = 1;
  // END OF DEAD MEN SWITCH

  // REVERSE SWITCH
  if (!tst_bit(REVERSE_SWITCH_PIN, REVERSE_SWITCH)) {
    system_flags.reverse = 1;
    system_flags_zenira.reverse_zenira = 1;
  } else {
    system_flags.reverse = 0;
    system_flags_zenira.reverse_zenira = 0;
  }
  // END OF REVERSE SWITCH

  if (tst_bit(CTRL_SWITCHES_PIN, MCC_ON_SWITCH)){
    system_flags.MCC_on = 1;
    system_flags_zenira.MCC_on_zenira = 1;
  }
  else {
    system_flags.MCC_on = 0;
    system_flags_zenira.MCC_on_zenira = 0;
  }
}

/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void) {
#ifdef LED_ON
  if (led_clk_div++ >= ERROR_LED_CLK_DIV) {
    cpl_led(LED1);
    led_clk_div = 0;
  }
#endif
#ifdef BUZZER_ON
  buzzer(4, 8, 0);
#endif
  set_state_initializing();

  total_errors++; // incrementa a contagem de erros
  VERBOSE_MSG_ERROR(usart_send_string("The error code is: "));
  VERBOSE_MSG_ERROR(usart_send_uint16(error_flags.all));
  VERBOSE_MSG_ERROR(usart_send_char('\n'));

  if (!error_flags.all)
    VERBOSE_MSG_ERROR(
        usart_send_string("\t - Oh no, it was some unknown error.\n"));

  VERBOSE_MSG_ERROR(usart_send_string("The error level is: "));
  VERBOSE_MSG_ERROR(usart_send_uint16(total_errors));
  VERBOSE_MSG_ERROR(usart_send_char('\n'));

  if (total_errors < 2) {
    VERBOSE_MSG_ERROR(usart_send_string("I will reset the machine state.\n"));
  }
  if (total_errors >= 20) {
    VERBOSE_MSG_ERROR(
        usart_send_string("The watchdog will reset the whole system.\n"));
    set_state_reset();
  }
}

/**
 * @brief reset error task just freezes the processor and waits for watchdog
 */
inline void task_reset(void) {
#ifndef WATCHDOG_ON
  // wdt_init();
#endif

  cli(); // disable interrupts

  VERBOSE_MSG_ERROR(usart_send_string("WAITING FOR A RESET!\n"));
  for (;;) {
#ifdef BUZZER_ON
    buzzer(2, 64, 250);
    _delay_ms(1);
#endif
  };
}

void print_infos(void) {
  static uint8_t i = 0;

  if (print_clk_div++ >= PRINT_INFOS_CLK_DIV) {
    print_clk_div = 0;
    usart_send_string("\nState: ");
    switch (state_machine) {
    case STATE_INITIALIZING:
      usart_send_string("INIT");
      break;
    case STATE_IDLE:
      usart_send_string("IDLE");
      break;
    case STATE_RUNNING:
      usart_send_string("RUNNING");
      break;
    case STATE_ERROR:
      usart_send_string("ERROR");
      break;
    case STATE_RESET:
      usart_send_string("RESET");
      break;
    default:
      usart_send_string("UNKNOWN");
      break;
    }
    usart_send_string(" -|| Target motor: ");
    usart_send_uint16(control.motor_PWM_target);
    usart_send_string(" -|| Target MDE: ");
    usart_send_uint16(control.mde_steering_wheel_position);
    usart_send_string(" -|| Boat on: ");
    usart_send_uint8(system_flags.boat_on);
    usart_send_string(" -|| Motor on: ");
    usart_send_uint8(system_flags.motor_on);
    usart_send_string(" -|| Rev. flag: ");
    usart_send_uint8(system_flags.reverse);
    usart_send_string(" -|| Motor on: ");
    usart_send_uint8(system_flags.motor_on);
    usart_send_string(" -|| emergency: ");
    usart_send_uint8(system_flags.emergency);
    usart_send_string(" -|| zenira_control: ");
    usart_send_uint8(ctrl_bit_zenira);
    usart_send_string(" -|| zenira boat on: ");
    usart_send_uint8(system_flags_zenira.boat_on_zenira);
    usart_send_string(" -|| zenira motor on: ");
    usart_send_uint8(system_flags_zenira.motor_on_zenira);  

    switch (i++) {
    case 0:
      // usart_send_string("\ntestando: ");
      // usart_send_float(measurements.bat_voltage);
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      // VERBOSE_MSG_MACHINE(usart_send_char('\n'));
      i = 0;
      break;
    }
  }
}

/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void) {
  // print_infos();
  // print_system_flags();
  if (machine_clk) {
    machine_clk = 0;

#ifdef ADC_ON
    if (adc_data_ready) {
      adc_data_ready = 0;
      read_and_check_adcs();
    }

    if (error_flags.all) {
      print_system_flags();
      print_error_flags();
      print_infos();
      set_state_error();
    }

    switch (state_machine) {
    case STATE_INITIALIZING:
      task_initializing();

      break;
    case STATE_IDLE:
      task_idle();
#ifdef PRINT_INFOS
      print_infos();
#endif /* PRINT_INFOS */
#ifdef CAN_ON
      can_app_task();
#endif /* CAN_ON */
      break;
    case STATE_RUNNING:
      task_running();
#ifdef PRINT_INFOS
      print_infos();
#endif /* PRINT_INFOS */
#ifdef CAN_ON
      can_app_task();
#endif /* CAN_ON */

      break;
    case STATE_ERROR:
      task_error();
      __attribute__((fallthrough));
    case STATE_RESET:
    default:
      task_reset();
      break;
    }
    // print_system_flags();
#endif /* ADC_ON */
  }
}

/**
 * @brief ISR para ações de controle
 */
ISR(TIMER2_COMPA_vect) { machine_clk = 1; }
