/**
 ******************************************************************************
 * @file        : tfx100_version.h
 * @author      : Ivan.Estudiez
 * @brief       : Version information screen
 * @date        : 8 jul 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef INC_TFX100_VERSION_H_
#define INC_TFX100_VERSION_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * ----------------------------------------------------------------------------
 * @brief  Version Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct TFX100VersionPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} TFX100VersionPrivate_t;

typedef struct GUI_TFX100Version_t
{
	TFX100VersionPrivate_t priv;
	char *sCustomer;
	char *sFirmware;
	char *sSoftware;
	char *sHardware1;
	char *sHardware2;
	void *pRet;
} GUI_TFX100Version_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_TFX100VersionScreen(GUI_TFX100Version_t *info);


#endif /* INC_TFX100_VERSION_H_ */
