/**
 ******************************************************************************
 * @file        : app.c
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

#include "app.h"
#include "eeprom.h"
#include "indicators.h"
#include "pwrboard.h"
#include "gnss.h"
#include "global.h"
#include "spi.h"
#include "gpio.h"
#include "tim.h"
#include "utils.h"

// Definitions
// ----------------------------------------------------------------------------
#define APP_EEPROM_CONFIG_PAGE			(4U)
#define APP_EEPROM_BUFF_SIZE			(32U)
#define APP_DISPLAY_REFRESH_RATE		(50U)
#define APP_MAX_TEMPERATURE				(60U)
#define APP_PID_ALLOWED_ERR_TIME		(200U)

#define SENSOR_MIN_VALUE				(368U)
#define SENSOR_MAX_VALUE				(3680U)
#define SENSOR_ERROR_VALUE				(350U);

// Application public variables
// -----------------------------------------------------------------------------
bool APP_AutoMode = false;
bool APP_SaveConfigRequest = false;
bool APP_SimuMode = false;
bool APP_WorkingPosition = false;
bool APP_ConfigInProgress = false;
// ----------------------------------------------
uint8_t APP_ErrorCode = 0;
uint8_t APP_Eeprom[APP_EEPROM_BUFF_SIZE];
// ----------------------------------------------
uint8_t APP_AngleSensorRight;
uint8_t APP_AngleSensorLeft;
// ----------------------------------------------
int8_t APP_LeftArm = 0;
int8_t APP_RightArm = 0;

// Private variables
// -----------------------------------------------------------------------------

// Function Prototypes
// -----------------------------------------------------------------------------
void app_PID(void);
void app_SaveConfiguration(void);
void app_LoadConfiguration(void);
void app_UpdateParameters(void);
void app_IndicatorsUpdate(void);
void app_ErrorReport(void);
void app_DisplayValues(void);
void app_DigInputInit(void);
void app_PowerLed(void);
void app_MessageLed(void);
void app_ErrorLed(void);
void app_Buzzer(void);
void app_ReadSensors(void);

/**
 * -----------------------------------------------------------------------------
 * @brief 	User Application Initialization
 * -----------------------------------------------------------------------------
 */
void APP_Init(void)
{
	// Load EEPROM Saved Parameters
	// ----------------------------
	app_LoadConfiguration();

	// Configure digital input
	app_DigInputInit();

	// PID Configuration

	// Set the LEDs and the screen backlight.
	Indicator.Led.Power = LED_OFF;
	Indicator.Led.Message = LED_OFF;
	Indicator.Led.Error = LED_OFF;
	Indicator.Backlight.Green = LED_ON;
	Indicator.Backlight.Red = LED_OFF;
	Indicator.Backlight.Blue = LED_ON;

	// Sensors Power Supply
	PowerBoard.SensorSupply.External = PWR_ON;
	PowerBoard.SensorSupply.Internal = PWR_ON;

	// Enable analog input AIN[1]
	PowerBoard.InputsConfig.Ain1En = true;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	User application task (Fertilizer)
 * -----------------------------------------------------------------------------
 */
void APP_User(void)
{
	// Update LEDs status
	app_IndicatorsUpdate();

	// Update application parameters
	if (APP_ConfigInProgress)
		app_UpdateParameters();

	// Read sensors
	app_ReadSensors();

	// Perform PID control
	app_PID();

	// Report error code
	app_ErrorReport();

	// Update values ​​for on-screen display
	app_DisplayValues();

	// Save configuration
	if (APP_SaveConfigRequest)
		app_SaveConfiguration();
}

/**
 * -----------------------------------------------------------------------------
 * @brief	PID calculation task
 * -----------------------------------------------------------------------------
 */
void app_PID(void)
{
}

void app_SaveConfiguration(void)
{
	APP_SaveConfigRequest = false;

//	// Save data to EEPROM
//	EEPROM_Write(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

	// Update PID parameters
	app_UpdateParameters();
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Load parameters from EEPROM memory
 * -----------------------------------------------------------------------------
 */
void app_LoadConfiguration(void)
{
//	// Load data from EEPROM
//	EEPROM_Read(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

	// Extract data
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update application parameters
 * -----------------------------------------------------------------------------
 */
void app_UpdateParameters(void)
{
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void app_ReadSensors(void)
{
	// Read machine position
	APP_WorkingPosition = !GPIO_Read(GPIOC, 7);

	// Read left angle sensor value
	APP_AngleSensorLeft = (uint8_t) lintrafo(PowerBoard.Ain[0], SENSOR_MIN_VALUE, SENSOR_MAX_VALUE, 0, 90);

	// Read right angle sensor value
	APP_AngleSensorRight = (uint8_t) lintrafo(PowerBoard.Ain[1], SENSOR_MIN_VALUE, SENSOR_MAX_VALUE, 0, 90);
}

// -----------------------------------------------------------------------------
// LEDs section
// -----------------------------------------------------------------------------

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update LEDs status
 * -----------------------------------------------------------------------------
 */
void app_IndicatorsUpdate(void)
{
	app_PowerLed();
	app_MessageLed();
	app_ErrorLed();
	app_Buzzer();
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Power LED management
 * -----------------------------------------------------------------------------
 */
void app_PowerLed(void)
{
	if (APP_AutoMode)
		Indicator.Led.Power = LED_BLINK;
	else
		Indicator.Led.Power = LED_OFF;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Message LED management
 * -----------------------------------------------------------------------------
 */
void app_MessageLed(void)
{
	if(APP_WorkingPosition)
		Indicator.Led.Message = LED_ON;
	else
		Indicator.Led.Message = LED_OFF;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Error LED management
 * -----------------------------------------------------------------------------
 */
void app_ErrorLed(void)
{
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Buzzer
 * -----------------------------------------------------------------------------
 */
void app_Buzzer(void)
{

}

/**
 * -----------------------------------------------------------------------------
 * @brief	Set the error code variable
 * -----------------------------------------------------------------------------
 */
void app_ErrorReport(void)
{
	// Check PID error

	// Check temperature
	if (PowerBoard.Status.Temp >= (APP_MAX_TEMPERATURE * 10))
		APP_ErrorCode |= APP_ERROR_TEMP;
	else
		APP_ErrorCode &= ~APP_ERROR_TEMP;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Display the values ​​at a specific refresh rate
 * -----------------------------------------------------------------------------
 */
void app_DisplayValues(void)
{
	static uint8_t displayCounter = 0;

	displayCounter++;

	// Run averaging routines here

	if (displayCounter == APP_DISPLAY_REFRESH_RATE)
	{
		// Assignment of process variables to display variables

		displayCounter = 0;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief  Disable TIM8 and configure digital input
 * -----------------------------------------------------------------------------
 */
void app_DigInputInit(void)
{
	GPIO_Config_t dinConf;

	// PC6 (Digital Input)
	GPIO_Config_Reset(&dinConf);
	dinConf.port = GPIOC;
	dinConf.pin = 7;
	dinConf.mode = INPUT;
	GPIO_Init(dinConf);

	TIM_Disable(TIM8);
}
