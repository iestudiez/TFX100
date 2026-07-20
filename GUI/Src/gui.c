/**
 ******************************************************************************
 * @file        : gui.c
 * @author      : Estudiez, Ivan
 * @brief       : Graphic User Interface
 * @date		: 07/05/2026
 * @version		: 1.0
 ******************************************************************************
 */

#include "stddef.h"
#include "gui_utils.h"
#include "pwrboard.h"
#include "menu_list.h"
#include "input_box.h"
#include "list_box.h"
#include "msgbox.h"
#include "app.h"
#include "tfx100_main.h"
#include "tfx100_info.h"
#include "tfx100_debug.h"
#include "tfx100_version.h"

// ----------------------------------------------------------------------------
// Components of the graphical user interface
// ----------------------------------------------------------------------------
GUI_MenuList_t MenuList_MainMenu;
GUI_MenuList_t MenuList_Config;
GUI_ListBox_t ListBox_Pid;
GUI_TFX100App_t Application_Screen;
GUI_TFX100Debug_t DebugScreen;
GUI_TFX100Info_t InfoScreen;
GUI_MsgBox_t MsgBox_SaveConf;
GUI_TFX100Version_t TFX100_VersionScreen;

/**
 * ----------------------------------------------------------------------------
 * @brief		Initialization of the graphical components
 * ----------------------------------------------------------------------------
 */
void GUI_Init(void)
{
	// ------------------------------------------------------------------------
	// TFX100 APPLICATION SCREEN
	// ------------------------------------------------------------------------
	Application_Screen.priv.status = GUI_STATUS_ENABLED;
	Application_Screen.pRet = &MenuList_MainMenu;
	Application_Screen.pDebugScreen = &DebugScreen;
	Application_Screen.pAngleSensor1 = &APP_AngleSensorLeft;
	Application_Screen.pAngleSensor2 = &APP_AngleSensorRight;
	Application_Screen.pWorkingPosition = &APP_WorkingPosition;
	Application_Screen.pAutoMode = &APP_AutoMode;
	Application_Screen.pLeftArm = &APP_LeftArm;
	Application_Screen.pRightArm = &APP_RightArm;

	// ------------------------------------------------------------------------
	// DEBUG SCREEN
	// ------------------------------------------------------------------------
	DebugScreen.pRet = &Application_Screen;

	// ------------------------------------------------------------------------
	// MAIN MENU
	// ------------------------------------------------------------------------
	MenuList_MainMenu.sTitle = "MENU PRINCIPAL";
	MenuList_MainMenu.pRet = &Application_Screen;
	MenuList_MainMenu.item[0].label = "PANTALLA APLICACION";
	MenuList_MainMenu.item[0].run = &Application_Screen;
	MenuList_MainMenu.item[1].label = "INFO SISTEMA";
	MenuList_MainMenu.item[1].run = &InfoScreen;
	MenuList_MainMenu.item[2].label = "CONFIGURACION";
	MenuList_MainMenu.item[2].run = &MenuList_Config;
	MenuList_MainMenu.item[3].label = "GUARDAR CONFIGURACION";
	MenuList_MainMenu.item[3].run = &MsgBox_SaveConf;
	MenuList_MainMenu.item[4].label = "VERSION";
	MenuList_MainMenu.item[4].run = &TFX100_VersionScreen;

	// ------------------------------------------------------------------------
	// LISTBOX CONFIGURATION
	// ------------------------------------------------------------------------
	MenuList_Config.sTitle = "CONFIGURACION";
	MenuList_Config.pRet = & MenuList_MainMenu;
	MenuList_Config.item[0].label = "CONFIGURACION PID";
	MenuList_Config.item[0].run = &ListBox_Pid;
	MenuList_Config.item[1].label = "CALIBRACION SENSORES";
	MenuList_Config.item[1].run = NULL;


	// ------------------------------------------------------------------------
	// TFX100 CALIBRATION SCREEN
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// TFX100 INFO SCREEN
	// ------------------------------------------------------------------------
	InfoScreen.pRet = &MenuList_MainMenu;
	InfoScreen.pVoltage = &PowerBoard.Status.VBus;
	InfoScreen.pTemp = &PowerBoard.Status.Temp;
	InfoScreen.pCurrent = &PowerBoard.Status.Current;
	InfoScreen.pErrorCode = &APP_ErrorCode;

	// ------------------------------------------------------------------------
	// LISTBOX PID CONFIGURATION
	// ------------------------------------------------------------------------
	ListBox_Pid.sTitle = "CONFIGURACION PID";
	ListBox_Pid.pRet = &MenuList_Config;

	// KP (Proportional term gain)
	ListBox_Pid.item[0].sTile = "KP (PROPORCIONAL)";
	ListBox_Pid.item[0].pInputVar = &APP_PidKp;
	ListBox_Pid.item[0].dataType = GUI_INT32;
	ListBox_Pid.item[0].maxValue = 10000;
	ListBox_Pid.item[0].decimalPos = 0;
	ListBox_Pid.item[0].numDigits = 5;

	// KI (Integral term gain)
	ListBox_Pid.item[1].sTile = "KI (INTEGRAL)";
	ListBox_Pid.item[1].pInputVar = &APP_PidKi;
	ListBox_Pid.item[1].dataType = GUI_INT32;
	ListBox_Pid.item[1].maxValue = 10000;
	ListBox_Pid.item[1].decimalPos = 0;
	ListBox_Pid.item[1].numDigits = 5;

	// KD (Derivative term gain)
	ListBox_Pid.item[2].sTile = "KD (DERIVATIVO)";
	ListBox_Pid.item[2].pInputVar = &APP_PidKd;
	ListBox_Pid.item[2].dataType = GUI_INT32;
	ListBox_Pid.item[2].maxValue = 10000;
	ListBox_Pid.item[2].decimalPos = 0;
	ListBox_Pid.item[2].numDigits = 5;

	// Maximum value of the integral term
	ListBox_Pid.item[3].sTile = "MAX. INTEGRAL";
	ListBox_Pid.item[3].pInputVar = &APP_PidMaxInt;
	ListBox_Pid.item[3].dataType = GUI_UINT16;
	ListBox_Pid.item[3].maxValue = 1000;
	ListBox_Pid.item[3].decimalPos = 0;
	ListBox_Pid.item[3].numDigits = 4;

	// Offset
	ListBox_Pid.item[4].sTile = "OFFSET";
	ListBox_Pid.item[4].pInputVar = &APP_PidOffset;
	ListBox_Pid.item[4].dataType = GUI_UINT16;
	ListBox_Pid.item[4].maxValue = 1000;
	ListBox_Pid.item[4].decimalPos = 0;
	ListBox_Pid.item[4].numDigits = 4;

	// ------------------------------------------------------------------------
	// MSGBOX SAVE CONFIGURATION
	// ------------------------------------------------------------------------
	MsgBox_SaveConf.pRet = &MenuList_MainMenu;
	MsgBox_SaveConf.type = MSGBOX_WARNING;
	MsgBox_SaveConf.sMsgLine1 = "Guardar";
	MsgBox_SaveConf.sMsgLine2 = "configuracion?";
	MsgBox_SaveConf.pValue = &APP_SaveConfigRequest;

	// ------------------------------------------------------------------------
	// SHOW VERSION SCREEN
	// ------------------------------------------------------------------------
	TFX100_VersionScreen.pRet = &MenuList_MainMenu;
	TFX100_VersionScreen.sCustomer = "GERONZI";
	TFX100_VersionScreen.sSoftware = "TFX100.VER.0.20";
	TFX100_VersionScreen.sFirmware = "CX100.VER.0.20";
	TFX100_VersionScreen.sHardware1 = "HE-PLAC-CX100C_B1";
	TFX100_VersionScreen.sHardware2 = "HE-PLAC-CX100P_A1M1";
}

/**
 * ----------------------------------------------------------------------------
 * @brief		Run the graphics components
 * ----------------------------------------------------------------------------
 */
void GUI_Update(void)
{
	GUI_TFX100MainScreen(&Application_Screen);
	GUI_TFX100InfoScreen(&InfoScreen);
	GUI_MenuList(&MenuList_MainMenu);
	GUI_MenuList(&MenuList_Config);
	GUI_ListBox(&ListBox_Pid);
	GUI_MsgBox(&MsgBox_SaveConf);
	GUI_TFX100DebugScreen(&DebugScreen);
	GUI_TFX100VersionScreen(&TFX100_VersionScreen);

	// Run all input boxes
	GUI_InputBox(GUI_InputBox_DefaultPointer);

//	// Set configuration in progress flag
//	if ((ListBox_Config.priv.status == GUI_STATUS_ENABLED) || (ListBox_Pid.priv.status == GUI_STATUS_ENABLED))
//		APP_ConfigInProgress = true;
//	else
//		APP_ConfigInProgress = false;
}
