/**
 ******************************************************************************
 * @file        : tfx100_info.h
 * @author      : Ivan.Estudiez
 * @brief       : TFX100 Information Screen
 * @date        : 7 jul 2026
 * @version     : 0.
 ******************************************************************************
 */
#ifndef INC_TFX100_INFO_H_
#define INC_TFX100_INFO_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct TFX100InfoPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} TFX100InfoPrivate_t;

typedef struct GUI_TFX100Info_t
{
	TFX100InfoPrivate_t priv;
	uint16_t *pVoltage;
	uint16_t *pTemp;
	uint16_t *pCurrent;
	uint8_t *pErrorCode;
	void *pRet;
} GUI_TFX100Info_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_TFX100InfoScreen(GUI_TFX100Info_t *ui);


#endif /* INC_TFX100_INFO_H_ */
