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
// ----------------------------------------------
extern uint8_t APP_ErrorCode;
// ----------------------------------------------
extern uint8_t APP_AngleSensorRight;
extern uint8_t APP_AngleSensorLeft;
// ----------------------------------------------
extern int8_t APP_LeftArm;
extern int8_t APP_RightArm;

// Public functions
// ----------------------------------------------------------------------------
void APP_Init(void);
void APP_User(void);

#endif /* APP_H_ */
