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
#include "rcc.h"
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
#include "config.h"

// Definitions
// ----------------------------------------------------------------------------
#define APP_EEPROM_CONFIG_PAGE			(4U)
#define APP_EEPROM_BUFF_SIZE			(32U)
#define APP_DISPLAY_REFRESH_RATE		(20U)
#define APP_PID_ALLOWED_ERR_TIME		(200U)

#define RAW_LEFT_SENSOR					(PowerBoard.Ain[0])
#define RAW_RIGHT_SENSOR				(PowerBoard.Ain[1])

#define PWM_OUT_B1						(PowerBoard.Out[0].DutyCycle)
#define PWM_OUT_A1						(PowerBoard.Out[2].DutyCycle)
#define PWM_OUT_B2						(PowerBoard.Out[1].DutyCycle)
#define PWM_OUT_A2						(PowerBoard.Out[3].DutyCycle)

#define PWM_LEFT_OUT_UP					PWM_OUT_B1
#define PWM_LEFT_OUT_DOWN				PWM_OUT_A1
#define PWM_RIGHT_OUT_UP				PWM_OUT_B2
#define PWM_RIGHT_OUT_DOWN				PWM_OUT_A2

// Application public variables
// -----------------------------------------------------------------------------
bool APP_AutoMode = false;
bool APP_SaveConfigRequest = false;
bool APP_DefaultConfigRequest = false;
bool APP_SimuMode = false;
bool APP_WorkingPosition = false;
bool APP_ConfigInProgress = false;
bool APP_LeftSensorInv = CONFIG_SENSOR_INV_LEFT;
bool APP_RightSensorInv = CONFIG_SENSOR_INV_RIGHT;
bool APP_SerialMode = false;
// ----------------------------------------------
uint8_t APP_ErrorCode = 0;
uint8_t APP_StorageByte;
uint8_t APP_Eeprom[APP_EEPROM_BUFF_SIZE];
// ----------------------------------------------
uint16_t APP_LeftSensorMin = CONFIG_SENSOR_MIN_VALUE;
uint16_t APP_LeftSensorMax = CONFIG_SENSOR_MAX_VALUE;
uint16_t APP_RightSensorMin = CONFIG_SENSOR_MIN_VALUE;
uint16_t APP_RightSensorMax = CONFIG_SENSOR_MAX_VALUE;
uint16_t APP_LiftingTime = CONFIG_LIFTING_TIME;
// ----------------------------------------------
int32_t APP_AngleSensorLeft;
int32_t APP_AngleSensorRight;
int32_t APP_DisplaySensorLeft;
int32_t APP_DisplaySensorRight;
int32_t APP_Setpoint = CONFIG_ANGLE_SETPOINT;
// ----------------------------------------------
int8_t APP_LeftArm;
int8_t APP_RightArm;
uint16_t APP_PwmUp = CONFIG_MANUAL_PWM_UP;
uint16_t APP_PwmDown = CONFIG_MANUAL_PWM_DOWN;
// ----------------------------------------------
uint16_t APP_PidMaxInt = CONFIG_PID_MAX_INT;
uint16_t APP_PidOffset = CONFIG_PID_OFFSET;
uint16_t APP_PidKp = CONFIG_PID_KP;
uint16_t APP_PidKi = CONFIG_PID_KI;
uint16_t APP_PidKd = CONFIG_PID_KD;

// Private variables
// -----------------------------------------------------------------------------
PID_Position_t APP_PidLeft;
PID_Position_t APP_PidRight;
bool APP_PidEnabled = false;
bool APP_FirstExecution;

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
void app_AutoMode(void);
void app_ConfigureUART(void);
void app_DefaultConfiguration(void);

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
	app_ConfigureUART();
	UART_Enable(USART2);

	// Configure left arm PID
	APP_PidLeft.pEnable = &APP_PidEnabled;
	APP_PidLeft.Kp = APP_PidKp;
	APP_PidLeft.Ki = APP_PidKi;
	APP_PidLeft.Kd = APP_PidKd;
	APP_PidLeft.kpDiv = CONFIG_PID_P_DIVIDER;
	APP_PidLeft.kiDiv = CONFIG_PID_I_DIVIDER;
	APP_PidLeft.kdDiv = CONFIG_PID_D_DIVIDER;
	APP_PidLeft.maxIntegral = APP_PidMaxInt;
	APP_PidLeft.offset = APP_PidOffset;
	APP_PidLeft.pFeedback = &APP_AngleSensorLeft;
	APP_PidLeft.pSetpoint = &APP_Setpoint;
	APP_PidLeft.pOutA = &PWM_LEFT_OUT_UP;
	APP_PidLeft.pOutB = &PWM_LEFT_OUT_DOWN;

	// Configure right arm PID
	APP_PidRight.pEnable = &APP_PidEnabled;
	APP_PidRight.Kp = APP_PidKp;
	APP_PidRight.Ki = APP_PidKi;
	APP_PidRight.Kd = APP_PidKd;
	APP_PidRight.kpDiv = CONFIG_PID_P_DIVIDER;
	APP_PidRight.kiDiv = CONFIG_PID_I_DIVIDER;
	APP_PidRight.kdDiv = CONFIG_PID_D_DIVIDER;
	APP_PidRight.maxIntegral = APP_PidMaxInt;
	APP_PidRight.offset = APP_PidOffset;
	APP_PidRight.pFeedback = &APP_AngleSensorRight;
	APP_PidRight.pSetpoint = &APP_Setpoint;
	APP_PidRight.pOutA = &PWM_RIGHT_OUT_UP;
	APP_PidRight.pOutB = &PWM_RIGHT_OUT_DOWN;

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

	// Select operating mode
	if (APP_AutoMode)
		app_AutoMode();
	else
		app_ManualMode();

	// Report error code
	app_ErrorReport();

	// Update values ​​for on-screen display
	app_DisplayValues();

	// Save configuration
	if (APP_SaveConfigRequest)
		app_SaveConfiguration();

	// Load factory configuration
	if (APP_DefaultConfigRequest)
		app_DefaultConfiguration();
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
 * @brief 	Automatic mode operation
 * -----------------------------------------------------------------------------
 */
void app_AutoMode(void)
{
	static uint16_t timeCnt = 0;
	uint16_t waitingTime;

	// Check working position
	if (APP_WorkingPosition)
	{
		APP_PidEnabled = true;
		timeCnt = 0;
		return;
	}

	// Disable closed loop control
	APP_PidEnabled = false;

	// Time counter
	timeCnt++;
	waitingTime = APP_LiftingTime * 10;

	if (timeCnt <= waitingTime)
	{
		// Go to the top position
		PWM_LEFT_OUT_DOWN = 0;
		PWM_RIGHT_OUT_DOWN = 0;
		PWM_LEFT_OUT_UP = 800;
		PWM_RIGHT_OUT_UP = 800;
	}
	else
	{
		PWM_LEFT_OUT_UP = 0;
		PWM_RIGHT_OUT_UP = 0;
		timeCnt = waitingTime;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Manual mode operation
 * -----------------------------------------------------------------------------
 */
void app_ManualMode(void)
{
	// Disable closed loop control
	APP_PidEnabled = false;

	// Seeder Left Arm Control
	switch (APP_LeftArm)
	{
	case APP_ARM_HOLD:
		PWM_LEFT_OUT_UP = 0;
		PWM_LEFT_OUT_DOWN = 0;
		break;
	case APP_ARM_UP:
		PWM_LEFT_OUT_UP = APP_PwmUp;
		PWM_LEFT_OUT_DOWN = 0;
		break;
	case APP_ARM_DOWN:
		PWM_LEFT_OUT_UP = 0;
		PWM_LEFT_OUT_DOWN = APP_PwmDown;
		break;
	}

	// Seeder Right Arm Control
	switch (APP_RightArm)
	{
	case APP_ARM_HOLD:
		PWM_RIGHT_OUT_UP = 0;
		PWM_RIGHT_OUT_DOWN = 0;
		break;
	case APP_ARM_UP:
		PWM_RIGHT_OUT_UP = APP_PwmUp;
		PWM_RIGHT_OUT_DOWN = 0;
		break;
	case APP_ARM_DOWN:
		PWM_RIGHT_OUT_UP = 0;
		PWM_RIGHT_OUT_DOWN = APP_PwmDown;
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
	EEPROM_StoreWord(&APP_Eeprom[22], APP_LiftingTime);
	EEPROM_StoreWord(&APP_Eeprom[24], (uint16_t) APP_Setpoint);
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

	// Read boolean variables
	APP_StorageByte = APP_Eeprom[31];
	app_ReadStorageByte();

	// Check if the program is running on the ECU for the first time.
	if (APP_FirstExecution)
	{
		APP_FirstExecution = false;
		app_SaveConfiguration();
	}

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
	APP_LiftingTime = EEPROM_GetWord(&APP_Eeprom[22]);
	APP_Setpoint = (uint16_t) EEPROM_GetWord(&APP_Eeprom[24]);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Load default configuration
 * -----------------------------------------------------------------------------
 */
void app_DefaultConfiguration(void)
{
	APP_DefaultConfigRequest = false;

	// Set default values
	APP_LeftSensorMin = CONFIG_SENSOR_MIN_VALUE;
	APP_LeftSensorMax = CONFIG_SENSOR_MAX_VALUE;
	APP_RightSensorMin = CONFIG_SENSOR_MIN_VALUE;
	APP_RightSensorMax = CONFIG_SENSOR_MAX_VALUE;
	APP_PidMaxInt = CONFIG_PID_MAX_INT;
	APP_PidOffset = CONFIG_PID_OFFSET;
	APP_PidKp = CONFIG_PID_KP;
	APP_PidKi = CONFIG_PID_KI;
	APP_PidKd = CONFIG_PID_KD;
	APP_PwmUp = CONFIG_MANUAL_PWM_UP;
	APP_PwmDown = CONFIG_MANUAL_PWM_DOWN;
	APP_LiftingTime = CONFIG_LIFTING_TIME;
	APP_Setpoint = CONFIG_ANGLE_SETPOINT;

	// Save parameters in the EEPROM
	app_SaveConfiguration();
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
 * @brief	Write storage byte
 * -----------------------------------------------------------------------------
 */
void app_WriteStorageByte(void)
{
	EEPROM_PackByte(&APP_StorageByte, APP_LeftSensorInv, 0);
	EEPROM_PackByte(&APP_StorageByte, APP_RightSensorInv, 1);
	EEPROM_PackByte(&APP_StorageByte, APP_SerialMode, 2);
	EEPROM_PackByte(&APP_StorageByte, APP_FirstExecution, 7);
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Read storage byte
 * -----------------------------------------------------------------------------
 */
void app_ReadStorageByte(void)
{
	APP_LeftSensorInv = EEPROM_UnpackByte(APP_StorageByte, 0);
	APP_RightSensorInv = EEPROM_UnpackByte(APP_StorageByte, 1);
	APP_SerialMode = EEPROM_UnpackByte(APP_StorageByte, 2);
	APP_FirstExecution = EEPROM_UnpackByte(APP_StorageByte, 7);
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
	APP_AngleSensorLeft = (int32_t) lintrafo(RAW_LEFT_SENSOR, APP_LeftSensorMin, APP_LeftSensorMax, 0, CONFIG_SENSOR_ANGLE_RANGE);
	if (APP_LeftSensorInv)
		APP_AngleSensorLeft = CONFIG_SENSOR_ANGLE_RANGE - APP_AngleSensorLeft;

	// Read right angle sensor value
	APP_AngleSensorRight = (int32_t) lintrafo(RAW_RIGHT_SENSOR, APP_RightSensorMin, APP_RightSensorMax, 0, CONFIG_SENSOR_ANGLE_RANGE);
	if (APP_RightSensorInv)
		APP_AngleSensorRight = CONFIG_SENSOR_ANGLE_RANGE - APP_AngleSensorRight;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Set the error code variable
 * -----------------------------------------------------------------------------
 */
void app_ErrorReport(void)
{
	// Check temperature
	if (PowerBoard.Status.Temp >= (CONFIG_TEMP_ERR_VALUE * 10))
		APP_ErrorCode |= APP_ERR_CODE_TEMP;
	else
		APP_ErrorCode &= ~APP_ERR_CODE_TEMP;

	// Check left sensor (SA1)
	if (RAW_LEFT_SENSOR <= CONFIG_SENSOR_ERR_VALUE)
		APP_ErrorCode |= APP_ERR_CODE_SA1;
	else
		APP_ErrorCode &= ~APP_ERR_CODE_SA1;

	// Check right sensor (SA2)
	if (RAW_RIGHT_SENSOR <= CONFIG_SENSOR_ERR_VALUE)
		APP_ErrorCode |= APP_ERR_CODE_SA2;
	else
		APP_ErrorCode &= ~APP_ERR_CODE_SA2;

	// Check battery voltage
	if (PowerBoard.Status.VBus < CONFIG_VOLTAGE_MIN_VALUE || PowerBoard.Status.VBus > CONFIG_VOLTAGE_MAX_VALUE)
		APP_ErrorCode |= APP_ERR_CODE_BAT;
	else
		APP_ErrorCode &= ~APP_ERR_CODE_BAT;

	// Overcurrent
	if (PowerBoard.Status.Current >= CONFIG_OVERCURRENT_ERR_VALUE)
		APP_ErrorCode |= APP_ERR_CODE_OVC;

}

// -----------------------------------------------------------------------------
// Indicators section
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
	if (APP_ErrorCode)
		Indicator.Led.Error = LED_BLINK;
	else
		Indicator.Led.Error = LED_OFF;
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
		emafilter(APP_AngleSensorLeft, &APP_DisplaySensorLeft, 200);
		emafilter(APP_AngleSensorRight, &APP_DisplaySensorRight, 200);

		displayCounter = 0;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Send plotter data to the RS232 port
 * -----------------------------------------------------------------------------
 */
void APP_SendPlotterData(void)
{

	// Build data to send (Better Serial Plotter Windows Application)
	if (APP_SerialMode)
		// Setpoint mode
		sprintf(UART_TxBuffer, " %d %d %d\n", (int) APP_Setpoint, (int) APP_AngleSensorLeft, (int) APP_AngleSensorRight);
	else
		// PID mode
		sprintf(UART_TxBuffer, " %d %d %d %d %d\n", (int) APP_Setpoint, (int) APP_AngleSensorLeft, (int) APP_PidLeft.priv.propTerm, (int) APP_PidLeft.priv.intgTerm, (int) APP_PidLeft.priv.dervTerm);

	// Clear transfer complete flag
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

	// Start DMA transfer
	DMA1_Stream6->NDTR = strlen(UART_TxBuffer);
	DMA_Stream_Enable(DMA1_Stream6);
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

void app_ConfigureUART(void)
{
	UART_Config_t serialPort;

	// UART Configuration
	// ------------------
	// UART2 Module
	UART_Config_Reset(&serialPort);
	serialPort.module = USART2;
	serialPort.periph_clk = (uint32_t) RCC_APB1_CLK;
	serialPort.baudrate = BR_115200;
	serialPort.dir_tx = true;
	serialPort.dir_rx = true;
	serialPort.dma_tx = true;
	serialPort.dma_rx = true;
	serialPort.rxneie = false;
	serialPort.tcie = false;
	serialPort.idleie = false;
	UART_Init(serialPort);
}
