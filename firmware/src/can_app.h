/**
 * @file can_app.h
 *
 * @defgroup CANAPP    Application Module for canbus
 *
 * @brief The application layer module for canbus
 *
 */

#ifndef CAN_APP_H
#define CAN_APP_H

#include "conf.h"
#include "dbg_vrb.h"
#include "../lib/bit_utils.h"
#include "can.h"
#include "can_ids.h"
#include "machine.h"
#include "usart.h"

void can_app_print_msg(can_t *msg);
void can_app_task(void);
void can_app_send_state(void);
void can_app_send_motor(void);
void can_app_send_boat(void);
void can_app_send_pumps(void);
void can_app_send_steering_wheel(void);
void can_app_extractor_mcs_relay(can_t *msg);
void can_app_msg_extractors_switch(can_t *msg);
// void can_app_extractor_mic17_state(can_t *msg);
// void can_app_extractor_mic17_mcs(can_t *msg);
void check_can(void);

#ifdef CAN_ON
#define CAN_APP_SEND_STATE_CLK_DIV (MACHINE_FREQUENCY / CAN_MSG_MIC19_STATE_FREQUENCY)
#define CAN_APP_SEND_MOTOR_CLK_DIV (MACHINE_FREQUENCY / CAN_MSG_MIC19_MOTOR_FREQUENCY)
#define CAN_APP_SEND_MDE_CLK_DIV (MACHINE_FREQUENCY / CAN_MSG_MIC19_MDE_FREQUENCY)
#define CAN_APP_SEND_BOAT_CLK_DIV (MACHINE_FREQUENCY / CAN_MSG_MIC19_MCS_FREQUENCY)
#define CAN_APP_SEND_PUMPS_CLK_DIV (MACHINE_FREQUENCY / CAN_MSG_MIC19_PUMPS_FREQUENCY)
#endif

extern uint32_t can_app_send_state_clk_div;
extern uint32_t can_app_send_motor_clk_div;
extern uint32_t can_app_send_mde_clk_div;
extern uint32_t can_app_send_boat_clk_div;
extern uint32_t can_app_send_pumps_clk_div;

#endif /* ifndef CAN_APP_H */
