/**
 ******************************************************************************
 * @file        : tfx100_debug.h
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 7 jul 2026
 * @version     : 0.
 ******************************************************************************
 */
#ifndef INC_TFX100_DEBUG_H_
#define INC_TFX100_DEBUG_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * ----------------------------------------------------------------------------
 * @brief  Debug Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct TFX100DebugPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} TFX100DebugPrivate_t;

typedef struct GUI_TFX100Debug_t
{
	TFX100DebugPrivate_t priv;
	void *pRet;
} GUI_TFX100Debug_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_TFX100DebugScreen(GUI_TFX100Debug_t *this);


#endif /* INC_TFX100_DEBUG_H_ */
