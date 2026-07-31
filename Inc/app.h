/**
 ******************************************************************************
 * @file        : app.h
 * @author      : Estudiez, Ivan
 * @brief       : User Application (Terrain follower)
 * @date        : 24/06/2026
 * @version     : 1.0
 ******************************************************************************
 *	(24/06/2026)
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>
#include "pid.h"

// Definitions
// ----------------------------------------------------------------------------
#define APP_ERROR_PID					(0x01)
#define APP_ERROR_GPS					(0x02)
#define APP_ERROR_CAN					(0x04)
#define APP_ERROR_TEMP					(0x08)
// ----------------------------------------------------------------------------
#define APP_ARM_UP						(1)
#define APP_ARM_HOLD					(0)
#define APP_ARM_DOWN					(-1)
// ----------------------------------------------------------------------------
extern bool APP_AutoMode;
extern bool APP_SaveConfigRequest;
extern bool APP_SimuMode;
extern bool APP_WorkingPosition;
extern bool APP_ConfigInProgress;
extern bool APP_LeftSensorInv;
extern bool APP_RightSensorInv;
// ----------------------------------------------
extern uint8_t APP_ErrorCode;
// ----------------------------------------------
extern int32_t APP_AngleSensorRight;
extern int32_t APP_AngleSensorLeft;
extern int32_t APP_LeftSetpoint;
extern int32_t APP_RightSetpoint;
// ---------------------------------------------
extern uint16_t APP_LeftSensorMin;
extern uint16_t APP_LeftSensorMax;
extern uint16_t APP_RightSensorMin;
extern uint16_t APP_RightSensorMax;
// ----------------------------------------------
extern int8_t APP_LeftArm;
extern int8_t APP_RightArm;
extern uint16_t APP_PwmUp;
extern uint16_t APP_PwmDown;
// ----------------------------------------------
extern uint16_t APP_PidMaxInt;
extern uint16_t APP_PidOffset;
extern uint16_t APP_PidKp;
extern uint16_t APP_PidKi;
extern uint16_t APP_PidKd;

// Public functions
// ----------------------------------------------------------------------------
void APP_Init(void);
void APP_User(void);
void APP_SendPlotterData(void);

#endif /* APP_H_ */
