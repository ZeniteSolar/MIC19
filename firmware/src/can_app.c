/*
########################################################################
EXAMPLE OF SEND adc
########################################################################

inline void can_app_send_bat(void)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MCS19_ADC;
    msg.length                              = CAN_LENGTH_MSG_MCS19_ADC;
    msg.flags.rtr = 0;

    uint16_t avg_adc0 =
        measurements.adc0_avg_sum / measurements.adc0_avg_sum_count;


    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]            = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_MCS19_BAT_AVG_L_BYTE] =  LOW(avg_adc0);
    msg.data[CAN_MSG_MCS19_BAT_AVG_H_BYTE] =  HIGH(avg_adc0);
    msg.data[CAN_MSG_MCS19_BAT_MIN_L_BYTE]  = LOW(measurements.adc0_min);
    msg.data[CAN_MSG_MCS19_BAT_MIN_H_BYTE]  = HIGH(measurements.adc0_min);
    msg.data[CAN_MSG_MCS19_BAT_MAX_L_BYTE]  = LOW(measurements.adc0_max);
    msg.data[CAN_MSG_MCS19_BAT_MAX_H_BYTE]  = HIGH(measurements.adc0_max);

    can_send_message(&msg);
#ifdef VERBOSE_MSG_CAN_APP
    VERBOSE_MSG_CAN_APP(usart_send_string("adc bat msg was sent.\n"));
//    VERBOSE_MSG_CAN_APP(can_app_print_msg(&msg));
#endif

    reset_measurements();
}
*/


/*
########################################################################
EXAMPLE OF extract mensage
########################################################################
inline void can_app_extractor_mic17_mcs(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC19){

        // can_app_checks_without_mic17_msg = 0;

        if(msg->data[CAN_MSG_MIC19_MCS_BOAT_ON_BYTE] == 0xFF){
            system_flags.boat_on = 1;
        }else if(msg->data[CAN_MSG_MIC19_MCS_BOAT_ON_BYTE] == 0x00){
            system_flags.boat_on = 0;
        }

        //system_flags.boat_on       = bit_is_set(msg->data[
        //    CAN_MSG_MIC19_MCS_BOAT_ON_BYTE],
        //    CAN_MSG_MIC19_MCS_BOAT_ON_BIT);


        VERBOSE_MSG_CAN_APP(usart_send_string("boat on bit: "));
        VERBOSE_MSG_CAN_APP(usart_send_uint16(system_flags.boat_on));
        VERBOSE_MSG_CAN_APP(usart_send_char('\n'));


    }
}

*/

#include "can_app.h"
#define MOTOR_VERBOSE
uint32_t can_app_send_state_clk_div;
uint32_t can_app_send_motor_clk_div;
uint32_t can_app_send_mde_clk_div;
uint32_t can_app_send_boat_clk_div;
uint32_t can_app_send_pumps_clk_div;

uint32_t can_app_send_state_clk_div_zenira;
uint32_t can_app_send_motor_clk_div_zenira;
uint32_t can_app_send_mde_clk_div_zenira;
uint32_t can_app_send_boat_clk_div_zenira;
uint32_t can_app_send_pumps_clk_div_zenira;

/**
 * @brief Prints a can message via usart
 */
inline void can_app_print_msg(can_t *msg)
{
#ifdef USART_ON
    usart_send_string("ID: ");
    usart_send_uint16(msg->id);
    usart_send_string(". D: ");

	for (uint8_t i = 0; i < msg->length; i++)
	{
      usart_send_uint16(msg->data[i]);
      usart_send_char(' ');
    }

    usart_send_string(". ERR: ");
    can_error_register_t err = can_read_error_register();
    usart_send_uint16(err.rx);
    usart_send_char(' ');
    usart_send_uint16(err.tx);
    usart_send_char('\n');
#endif
}

/**
* @brief Manages the canbus application protocol
*/
inline void can_app_task(void)
{
    check_can();

	if (can_app_send_state_clk_div++ >= CAN_APP_SEND_STATE_CLK_DIV)
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("state msg was sent.\n"));
        can_app_send_state();
        can_app_send_state_clk_div = 0;
    }

	if (can_app_send_motor_clk_div++ >= CAN_APP_SEND_MOTOR_CLK_DIV)
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("motor msg was sent.\n"));
        can_app_send_motor();
        can_app_send_motor_clk_div = 0;
    }

	if (can_app_send_boat_clk_div++ >= CAN_APP_SEND_BOAT_CLK_DIV)
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("boat msg was sent.\n"));
        can_app_send_boat();
        can_app_send_boat_clk_div = 0;
    }

	if (can_app_send_pumps_clk_div++ >= CAN_APP_SEND_PUMPS_CLK_DIV)
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("pumps msg was sent.\n"));
        can_app_send_pumps();
        can_app_send_pumps_clk_div = 0;
    }

	if (can_app_send_mde_clk_div++ >= CAN_APP_SEND_MDE_CLK_DIV)
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("steering wheel msg was sent.\n"));
        can_app_send_steering_wheel();
        can_app_send_mde_clk_div = 0;
    }
}

inline void can_app_send_state(void)
{
    can_t msg;
	msg.id = CAN_MSG_MIC19_STATE_ID;
	msg.length = CAN_MSG_GENERIC_STATE_LENGTH;
    msg.flags.rtr = 0;

	msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_SELF;
	msg.data[CAN_MSG_GENERIC_STATE_STATE_BYTE] = (uint8_t)state_machine;
	msg.data[CAN_MSG_GENERIC_STATE_ERROR_BYTE] = error_flags.all;

    can_send_message(&msg);

    // VERBOSE_MSG_CAN_APP(usart_send_string("state msg was send.\n"));
    // VERBOSE_MSG_CAN_APP(can_app_print_msg(&msg));
}

inline void can_app_send_motor(void)
{
    can_t msg;
	msg.id = CAN_MSG_MIC19_MOTOR_ID;
	msg.length = CAN_MSG_MIC19_MOTOR_LENGTH;
    msg.flags.rtr = 0;

	msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_SELF;
#ifdef ADC_8BITS
	msg.data[CAN_MSG_MIC19_MOTOR_D_BYTE] = control.motor_PWM_target;
	msg.data[CAN_MSG_MIC19_MOTOR_I_BYTE] = control.motor_RAMP_target;
#else
	msg.data[CAN_MSG_MIC19_MOTOR_D_BYTE] = (uint8_t)(control.motor_PWM_target >> 2);
	msg.data[CAN_MSG_MIC19_MOTOR_I_BYTE] = (uint8_t)(control.motor_RAMP_target >> 2);
#endif

    msg.data[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] =
        ((system_flags.motor_on) << CAN_MSG_MIC19_MOTOR_MOTOR_MOTOR_ON_BIT);

    msg.data[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        ((system_flags.dead_men_switch) << CAN_MSG_MIC19_MOTOR_MOTOR_DMS_ON_BIT);

    msg.data[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        ((system_flags.reverse) << CAN_MSG_MIC19_MOTOR_MOTOR_REVERSE_BIT);

    can_send_message(&msg);
}

inline void can_app_send_steering_wheel(void)
{
    can_t msg;
	msg.id = CAN_MSG_MIC19_MDE_ID;
	msg.length = CAN_MSG_MIC19_MDE_LENGTH;
    msg.flags.rtr = 0;

	msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_SELF;
	msg.data[CAN_MSG_MIC19_MDE_POSITION_L_BYTE] = LOW(control.mde_steering_wheel_position);
	msg.data[CAN_MSG_MIC19_MDE_POSITION_H_BYTE] = HIGH(control.mde_steering_wheel_position);
    can_send_message(&msg);
}

inline void can_app_send_boat(void)
{
    can_t msg;
	msg.id = CAN_MSG_MIC19_MCS_ID;
	msg.length = CAN_MSG_MIC19_MCS_LENGTH;
    msg.flags.rtr = 0;

	msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_SELF;
	if (system_flags.boat_on)
	{
        msg.data[CAN_MSG_MIC19_MCS_BOAT_ON_BYTE] = 0xFF;
	}
	else
	{
        msg.data[CAN_MSG_MIC19_MCS_BOAT_ON_BYTE] = 0x00;
    }

    can_send_message(&msg);
}

inline void can_app_send_pumps(void)
{

    can_t msg;
	msg.id = CAN_MSG_MIC19_PUMPS_ID;
	msg.length = CAN_MSG_MIC19_PUMPS_LENGTH;
    msg.flags.rtr = 0;

	msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_SELF;

    msg.data[CAN_MSG_MIC19_PUMPS_PUMPS_BYTE] = 0x00;

    msg.data[CAN_MSG_MIC19_PUMPS_PUMPS_BYTE] |=
    (pump_flags.pump1_on) << (CAN_MSG_MIC19_PUMPS_PUMPS_PUMP1_BIT);

    msg.data[CAN_MSG_MIC19_PUMPS_PUMPS_BYTE] |=
    (pump_flags.pump2_on) << (CAN_MSG_MIC19_PUMPS_PUMPS_PUMP2_BIT);

    can_send_message(&msg);
}

inline void can_app_extractor_mcs_relay(can_t *msg)
{
	if (msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MCS19)
	{

        // can_app_checks_without_mic17_msg = 0;

		if (msg->data[CAN_MSG_MCS19_START_STAGES_MAIN_RELAY_BYTE] == 0xFF)
		{
            system_flags.MCS_on = 1;
		}
		else if (msg->data[CAN_MSG_MCS19_START_STAGES_MAIN_RELAY_BYTE] == 0x00)
		{
            system_flags.MCS_on = 0;
        }

        // VERBOSE_MSG_CAN_APP(usart_send_string("boat on bit: "));
        // VERBOSE_MSG_CAN_APP(usart_send_uint16(system_flags.boat_on));
        // VERBOSE_MSG_CAN_APP(usart_send_char('\n'));
    }
}

inline void can_app_extractor_mcv25_state(can_t *msg){
    if (msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MCV25)
    {
    VERBOSE_MSG_CAN_APP(usart_send_string("can_app_send_motor_clk_div = "));
    VERBOSE_MSG_CAN_APP(usart_send_uint32(can_app_send_motor_clk_div));
    VERBOSE_MSG_CAN_APP(usart_send_char('\n'));
        // Exemplo: extrair estado do módulo de direção
        uint8_t state_byte = msg->data[CAN_MSG_MCV25_STATE_STATE_BYTE];
        uint8_t error_byte = msg->data[CAN_MSG_MCV25_STATE_ERROR_BYTE];

        // Atualiza máquina de estados local
        // control.mcv25_state = state_byte;
        // error_flags.mcv25 = error_byte;
        
        VERBOSE_MSG_CAN_APP(usart_send_string("MCV25 STATE updated\n"));
        VERBOSE_MSG_CAN_APP(usart_send_string("state_byte = "));
        VERBOSE_MSG_CAN_APP(usart_send_uint16(state_byte));
        VERBOSE_MSG_CAN_APP(usart_send_string(" error = "));
        VERBOSE_MSG_CAN_APP(usart_send_uint16(error_byte));
        VERBOSE_MSG_CAN_APP(usart_send_char('\n'));
    }
}

inline void can_app_extractor_mcv25_boat_state(can_t *msg){
    if (msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MCV25)
    {
        // Exemplo: extrair estado do módulo de direção
        system_flags_zenira.boat_on_zenira = msg->data[CAN_MSG_MCV25_BOAT_STATE_BOAT_ON_BYTE];
        uint8_t state_byte = msg->data[CAN_MSG_MCV25_STATE_STATE_BYTE];
        
        VERBOSE_MSG_CAN_APP(usart_send_string("MCV25 BOAT ON updated\n"));
        VERBOSE_MSG_CAN_APP(usart_send_string("state_byte = "));
        VERBOSE_MSG_CAN_APP(usart_send_uint16(state_byte));
        VERBOSE_MSG_CAN_APP(usart_send_string(" boat_on = "));
        VERBOSE_MSG_CAN_APP(usart_send_uint16(system_flags_zenira.boat_on_zenira));
        VERBOSE_MSG_CAN_APP(usart_send_char('\n'));
    }
}

/**
 * @brief extract the motor clk div from mcv25 mde message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_extractor_mcv25_motor(can_t *msg){
    // Valor é limitado em read_and_check_adcs do machine.c
    // Aqui é feita apenas a conversão para que os valores estejam condizentes
    #define MOTOR_PWM_MAX_VALUE 1023               // Maximum motor PWM value (10-bit ADC)

    if (msg->data[CAN_MSG_MCV25_MOTOR_SIGNATURE_BYTE] == CAN_SIGNATURE_MCV25){
        system_flags_zenira.motor_on_zenira = msg->data[CAN_MSG_MCV25_MOTOR_MOTOR_BYTE];
        // Convert from CAN range (0-100) to ADC range (0-1023)
        control_zenira.motor_PWM_target_zenira = ((uint16_t)msg->data[CAN_MSG_MCV25_MOTOR_D_BYTE] * MOTOR_PWM_MAX_VALUE) / 100;
        control_zenira.motor_RAMP_target_zenira = ((uint16_t)msg->data[CAN_MSG_MCV25_MOTOR_I_BYTE] * MOTOR_PWM_MAX_VALUE) / 100;
    }
    // control_zenira.motor_RAMP_target = (uint16_t)msg->data[CAN_MSG_MCV25_MOTOR_I_BYTE];
}

/**
 * @brief extracts the steering wheel position from a mcv25 mde message
 * @param *msg pointer to the message to be extracted
 * 
 * Message format:
 * - POSITION_H_BYTE: direction (0 = left/bombordo, 1 = right/estibordo)
 * - POSITION_L_BYTE: angle in degrees (0-90) - used as incremental delta
 * 
 * Center position (angle=0) = 512 (straight ahead)
 * The angle is applied as an increment from current position.
 * Direction indicates which way to move:
 * - 0 (left): decreases position (towards MIN_POSITION)
 * - 1 (right): increases position (towards MAX_POSITION)
 * 
 * Threshold prevents reaching the extreme bounds - valid range is [MIN_POS+THRESHOLD, MAX_POS-THRESHOLD]
 */
inline void can_app_extractor_mcv25_mde(can_t *msg)
{
    #define CENTER_POSITION 512
    #define MAX_POSITION 1024
    #define MIN_POSITION 0
    #define THRESHOLD 30

    #define MIN_ALLOWED (MIN_POSITION + THRESHOLD)
    #define MAX_ALLOWED (MAX_POSITION - THRESHOLD)

    uint8_t direction = msg->data[CAN_MSG_MCV25_MDE_POSITION_H_BYTE] & 0x01;
    uint8_t angle = msg->data[CAN_MSG_MCV25_MDE_POSITION_L_BYTE];
    
    // Clamp angle to maximum 90 degrees
    if (angle > 90) angle = 90;
    
    uint16_t current_pos = control_zenira.mde_steering_wheel_position_zenira;
    uint16_t new_pos = current_pos;
    
    // If angle is 0, set to center position (straight ahead)
    if (angle == 0) {
        new_pos = CENTER_POSITION;
    } else {
        // Convert angle (1-90 degrees) to delta value (1-512)
        uint16_t delta = ((uint16_t)angle * CENTER_POSITION) / 90;
        
        // Apply delta with clamping
        if (direction == 0) {
            // Left (bombordo)
            if (delta > (current_pos - MIN_ALLOWED)) {
                new_pos = MIN_ALLOWED; // Delta is too large, clamp to MIN_ALLOWED
            } else {
                new_pos = current_pos - delta; // Delta is safe, apply it
            }
        } 
        else {
            // Right (estibordo)
            if (delta > (MAX_ALLOWED - current_pos)) {
                new_pos = MAX_ALLOWED; // Delta is too large, clamp to MAX_ALLOWED
            } else {
                new_pos = current_pos + delta; // Delta is safe, apply it
            }
        }
    }
    
    // Final safety check (should never be needed with preventive clamping)
    if (new_pos < MIN_ALLOWED) new_pos = MIN_ALLOWED;
    if (new_pos > MAX_ALLOWED) new_pos = MAX_ALLOWED;
    
    control_zenira.mde_steering_wheel_position_zenira = new_pos;

#ifdef DEBUG_CAN
    usart_send_string("CAN MDE - Angle: ");
    usart_send_uint16(angle);
    usart_send_string("° Direction: ");
    usart_send_uint16(direction);
    usart_send_string(" (");
    usart_send_string(direction == 0 ? "left" : "right");
    usart_send_string(") Delta: ");
    usart_send_uint16((angle * CENTER_POSITION) / 90);
    usart_send_string(" Current: ");
    usart_send_uint16(current_pos);
    usart_send_string(" -> New: ");
    usart_send_uint16(new_pos);
    usart_send_char('\n');
#endif
}

/**
 * @brief redirects a specific message extractor to a given message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_msg_extractors_switch(can_t *msg)
{
	if (msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MCS19)
	{

		switch (msg->id)
		{

            case CAN_MSG_MCS19_START_STAGES_ID:
                // VERBOSE_MSG_CAN_APP(usart_send_string("got a mcs msg: "));
                // VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mcs_relay(msg);

                __attribute__((fallthrough));
            default:
#ifdef USART_ON
                //VERBOSE_MSG_CAN_APP(usart_send_string("got a unknown msg:\n "));
#endif
                //VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                break;
        }
    }
    else if (msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MCV25)
    {
        switch (msg->id)
        {
            case CAN_MSG_MCV25_STATE_ID:
                VERBOSE_MSG_CAN_APP(usart_send_string("got a mcv25 state msg: "));
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mcv25_state(msg);
                break;

            case CAN_MSG_MCV25_MOTOR_ID:
                VERBOSE_MSG_CAN_APP(usart_send_string("got a mcv25 motor msg: "));
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mcv25_motor(msg);
                break;

            case CAN_MSG_MCV25_MDE_ID:
                VERBOSE_MSG_CAN_APP(usart_send_string("got a mcv25 mde msg: "));
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mcv25_mde(msg);
                break;

            case CAN_MSG_MCV25_BOAT_STATE_ID:
                VERBOSE_MSG_CAN_APP(usart_send_string("got a mcv25 boat msg: "));
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mcv25_boat_state(msg);
                break;

            default:
#ifdef USART_ON
                VERBOSE_MSG_CAN_APP(usart_send_string("got unknown mcv25 msg:\n"));
#endif
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                break;
        }
    }
}

/**
 * @brief Manages to receive and extract specific messages from canbus
 */
inline void check_can(void)
{
	if (can_check_message())
	{
        // VERBOSE_MSG_CAN_APP(usart_send_string("Mensagem recebida:\n"));
        can_t msg;
		if (can_get_message(&msg))
		{
            // VERBOSE_MSG_CAN_APP(can_app_print_msg(&msg)); // imprime sempre
            can_app_msg_extractors_switch(&msg);
        }
    }
}
