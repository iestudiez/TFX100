/**
 ******************************************************************************
 * @file        : tfx100_main.h
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 29 jun 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef INC_TFX100_MAIN_H_
#define INC_TFX100_MAIN_H_

#include "stdbool.h"
#include "stdint.h"

// Public definitions
// ----------------------------------------------------------------------------
#define SENSOR_MIN_VALUE				(0U)
#define SENSOR_MAX_VALUE				(90U)

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct TFX100AppPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} TFX100AppPrivate_t;

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen for Fertilizer Spreader
 * ----------------------------------------------------------------------------
 */
typedef struct GUI_TFX100App_t
{
	TFX100AppPrivate_t priv;
	uint8_t *pAngleSensor1;
	uint8_t *pAngleSensor2;
	bool *pWorkingPosition;
	int8_t *pLeftArm;
	int8_t *pRightArm;
	bool *pAutoMode;
	void *pDebugScreen;
	void *pRet;
} GUI_TFX100App_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_TFX100MainScreen(GUI_TFX100App_t *ui);

#endif /* INC_TFX100_MAIN_H_ */
