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

#include <string.h>
#include <stdio.h>
#include "uart.h"
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
#define APP_PID_P_DIVIDER				(1000)
#define APP_PID_I_DIVIDER				(100000)
#define APP_PID_D_DIVIDER				(100)

#define SENSOR_ANGLE_RANGE				(900)
#define SENSOR_MIN_VALUE				(368U)
#define SENSOR_MAX_VALUE				(3640U)
#define SENSOR_ERROR_VALUE				(350U);

#define PWM_OUT_A1						(PowerBoard.Out[0].DutyCycle)
#define PWM_OUT_B1						(PowerBoard.Out[2].DutyCycle)
#define PWM_OUT_A2						(PowerBoard.Out[1].DutyCycle)
#define PWM_OUT_B2						(PowerBoard.Out[3].DutyCycle)

// Application public variables
// -----------------------------------------------------------------------------
bool APP_AutoMode = false;
bool APP_SaveConfigRequest = false;
bool APP_SimuMode = false;
bool APP_WorkingPosition = false;
bool APP_ConfigInProgress = false;
bool APP_LeftSensorInv;
bool APP_RightSensorInv;
// ----------------------------------------------
uint8_t APP_ErrorCode = 0;
uint8_t APP_StorageByte;
uint8_t APP_Eeprom[APP_EEPROM_BUFF_SIZE];
// ----------------------------------------------
uint16_t APP_LeftSensorMin;
uint16_t APP_LeftSensorMax;
uint16_t APP_RightSensorMin;
uint16_t APP_RightSensorMax;
// ----------------------------------------------
int32_t APP_AngleSensorRight;
int32_t APP_AngleSensorLeft;
int32_t APP_LeftSetpoint = 450;
int32_t APP_RightSetpoint = 450;
// ----------------------------------------------
int8_t APP_LeftArm;
int8_t APP_RightArm;
uint16_t APP_PwmUp;
uint16_t APP_PwmDown;
// ----------------------------------------------
uint16_t APP_PidMaxInt = 500;
uint16_t APP_PidOffset = 0;
uint16_t APP_PidKp;
uint16_t APP_PidKi;
uint16_t APP_PidKd;

// Private variables
// -----------------------------------------------------------------------------
PID_Position_t APP_PidLeft;
PID_Position_t APP_PidRight;

// Function Prototypes
// -----------------------------------------------------------------------------
void app_PID(void);
void app_SaveConfiguration(void);
void app_LoadConfiguration(void);
void app_UpdateParameters(void);
void app_WriteStorageByte(void);
void app_ReadStorageByte(void);
void app_IndicatorsUpdate(void);
void app_ErrorReport(void);
void app_DisplayValues(void);
void app_DigInputInit(void);
void app_PowerLed(void);
void app_MessageLed(void);
void app_ErrorLed(void);
void app_Buzzer(void);
void app_ReadSensors(void);
void app_ManualMode(void);

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

	// Enable USART2
	UART_Enable(USART2);

	// Configure left arm PID
	APP_PidLeft.pEnable = &APP_AutoMode;
	APP_PidLeft.Kp = APP_PidKp;
	APP_PidLeft.Ki = APP_PidKi;
	APP_PidLeft.Kd = APP_PidKd;
	APP_PidLeft.proDiv = APP_PID_P_DIVIDER;
	APP_PidLeft.intDiv = APP_PID_I_DIVIDER;
	APP_PidLeft.derDiv = APP_PID_D_DIVIDER;
	APP_PidLeft.maxIntegral = APP_PidMaxInt;
	APP_PidLeft.offset = APP_PidOffset;
	APP_PidLeft.pFeedback = &APP_AngleSensorLeft;
	APP_PidLeft.pSetpoint = &APP_LeftSetpoint;
	APP_PidLeft.pOutA = &PWM_OUT_A1;
	APP_PidLeft.pOutB = &PWM_OUT_B1;

	// Configure right arm PID
	APP_PidRight.pEnable = &APP_AutoMode;
	APP_PidRight.Kp = APP_PidKp;
	APP_PidRight.Ki = APP_PidKi;
	APP_PidRight.Kd = APP_PidKd;
	APP_PidRight.proDiv = APP_PID_P_DIVIDER;
	APP_PidRight.intDiv = APP_PID_I_DIVIDER;
	APP_PidRight.derDiv = APP_PID_D_DIVIDER;
	APP_PidRight.maxIntegral = APP_PidMaxInt;
	APP_PidRight.offset = APP_PidOffset;
	APP_PidRight.pFeedback = &APP_AngleSensorRight;
	APP_PidRight.pSetpoint = &APP_RightSetpoint;
	APP_PidRight.pOutA = &PWM_OUT_A2;
	APP_PidRight.pOutB = &PWM_OUT_B2;

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

	// Manual mode
	if (APP_AutoMode == false)
		app_ManualMode();

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
	PID_Position(&APP_PidLeft);
	PID_Position(&APP_PidRight);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Manual mode operation
 * -----------------------------------------------------------------------------
 */
void app_ManualMode(void)
{
	// Seeder Left Arm Control
	switch (APP_LeftArm)
	{
	case APP_ARM_HOLD:
		PWM_OUT_A1 = 0;
		PWM_OUT_B1 = 0;
		break;
	case APP_ARM_UP:
		PWM_OUT_A1 = APP_PwmUp;
		PWM_OUT_B1 = 0;
		break;
	case APP_ARM_DOWN:
		PWM_OUT_A1 = 0;
		PWM_OUT_B1 = APP_PwmDown;
		break;
	}

	// Seeder Right Arm Control
	switch (APP_RightArm)
	{
	case APP_ARM_HOLD:
		PWM_OUT_A2 = 0;
		PWM_OUT_B2 = 0;
		break;
	case APP_ARM_UP:
		PWM_OUT_A2 = APP_PwmUp;
		PWM_OUT_B2 = 0;
		break;
	case APP_ARM_DOWN:
		PWM_OUT_A2 = 0;
		PWM_OUT_B2 = APP_PwmDown;
		break;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Save parameters in the EEPROM memory
 * -----------------------------------------------------------------------------
 */
void app_SaveConfiguration(void)
{
	APP_SaveConfigRequest = false;

	app_WriteStorageByte();

	EEPROM_StoreWord(&APP_Eeprom[0], APP_LeftSensorMin);
	EEPROM_StoreWord(&APP_Eeprom[2], APP_LeftSensorMax);
	EEPROM_StoreWord(&APP_Eeprom[4], APP_RightSensorMin);
	EEPROM_StoreWord(&APP_Eeprom[6], APP_RightSensorMax);
	EEPROM_StoreWord(&APP_Eeprom[8], APP_PidMaxInt);
	EEPROM_StoreWord(&APP_Eeprom[10], APP_PidOffset);
	EEPROM_StoreWord(&APP_Eeprom[12], APP_PidKp);
	EEPROM_StoreWord(&APP_Eeprom[14], APP_PidKi);
	EEPROM_StoreWord(&APP_Eeprom[16], APP_PidKd);
	EEPROM_StoreWord(&APP_Eeprom[18], APP_PwmUp);
	EEPROM_StoreWord(&APP_Eeprom[20], APP_PwmDown);
	APP_Eeprom[31] = APP_StorageByte;

	// Save data to EEPROM
	EEPROM_Write(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

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
	// Load data from EEPROM
	EEPROM_Read(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

	// Extract data
	APP_LeftSensorMin = EEPROM_GetWord(&APP_Eeprom[0]);
	APP_LeftSensorMax = EEPROM_GetWord(&APP_Eeprom[2]);
	APP_RightSensorMin = EEPROM_GetWord(&APP_Eeprom[4]);
	APP_RightSensorMax = EEPROM_GetWord(&APP_Eeprom[6]);
	APP_PidMaxInt = EEPROM_GetWord(&APP_Eeprom[8]);
	APP_PidOffset = EEPROM_GetWord(&APP_Eeprom[10]);
	APP_PidKp = EEPROM_GetWord(&APP_Eeprom[12]);
	APP_PidKi = EEPROM_GetWord(&APP_Eeprom[14]);
	APP_PidKd = EEPROM_GetWord(&APP_Eeprom[16]);
	APP_PwmUp = EEPROM_GetWord(&APP_Eeprom[18]);
	APP_PwmDown = EEPROM_GetWord(&APP_Eeprom[20]);
	APP_StorageByte = APP_Eeprom[31];

	app_ReadStorageByte();
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update application parameters
 * -----------------------------------------------------------------------------
 */
void app_UpdateParameters(void)
{
	// Configure left arm PID
	APP_PidLeft.Kp = APP_PidKp;
	APP_PidLeft.Ki = APP_PidKi;
	APP_PidLeft.Kd = APP_PidKd;
	APP_PidLeft.maxIntegral = APP_PidMaxInt;
	APP_PidLeft.offset = APP_PidOffset;

	// Configure right arm PID
	APP_PidRight.Kp = APP_PidKp;
	APP_PidRight.Ki = APP_PidKi;
	APP_PidRight.Kd = APP_PidKd;
	APP_PidRight.maxIntegral = APP_PidMaxInt;
	APP_PidRight.offset = APP_PidOffset;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Write storage slot
 * -----------------------------------------------------------------------------
 */
void app_WriteStorageByte(void)
{
	EEPROM_PackByte(&APP_StorageByte, APP_LeftSensorInv, 0);
	EEPROM_PackByte(&APP_StorageByte, APP_RightSensorInv, 1);
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Read storage slot
 * -----------------------------------------------------------------------------
 */
void app_ReadStorageByte(void)
{
	APP_LeftSensorInv = EEPROM_UnpackByte(APP_StorageByte, 0);
	APP_RightSensorInv = EEPROM_UnpackByte(APP_StorageByte, 1);
}

/**
 * -----------------------------------------------------------------------------
 * @brief	System sensor readings
 * -----------------------------------------------------------------------------
 */
void app_ReadSensors(void)
{
	// Read machine position
	APP_WorkingPosition = !GPIO_Read(GPIOC, 7);

	// Read left angle sensor value
	APP_AngleSensorLeft = (int32_t) lintrafo(PowerBoard.Ain[0], APP_LeftSensorMin, APP_LeftSensorMax, 0, SENSOR_ANGLE_RANGE);
	if (APP_LeftSensorInv)
		APP_AngleSensorLeft = SENSOR_ANGLE_RANGE - APP_AngleSensorLeft;

	// Read right angle sensor value
	APP_AngleSensorRight = (int32_t) lintrafo(PowerBoard.Ain[1], APP_RightSensorMin, APP_RightSensorMax, 0, SENSOR_ANGLE_RANGE);
	if (APP_RightSensorInv)
		APP_AngleSensorRight = SENSOR_ANGLE_RANGE - APP_AngleSensorRight;
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
	if (APP_WorkingPosition)
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
 * @brief 			Send plotter data to the RS232 port
 * -----------------------------------------------------------------------------
 * @param msg		Message to send
 * -----------------------------------------------------------------------------
 */
void APP_SendPlotterData(void)
{
	sprintf(UART_TxBuffer, "$%d %d %d;", (int) APP_LeftSetpoint, (int) APP_AngleSensorLeft, (int) APP_AngleSensorRight);

	DMA1_Stream6->NDTR = strlen(UART_TxBuffer);
	DMA_Stream_Enable(DMA1_Stream6);

	// Wait for DMA1_Stream6 transmission complete
	while (!((DMA1->HISR) & DMA_HISR_TCIF6))
	{
	}
	// Clear transfer complete flag
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
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
