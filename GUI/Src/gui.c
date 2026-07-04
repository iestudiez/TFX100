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

// ----------------------------------------------------------------------------
// Components of the graphical user interface
// ----------------------------------------------------------------------------
GUI_MenuList_t MenuList_MainMenu;
GUI_TFX100App_t Application_Screen;
GUI_MsgBox_t MsgBox_SaveConf;

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
	Application_Screen.pAngleSensor1 = &APP_AngleSensorLeft;
	Application_Screen.pAngleSensor2 = &APP_AngleSensorRight;
	Application_Screen.pWorkingPosition = &APP_WorkingPosition;
	Application_Screen.pLeftArm = &APP_LeftArm;
	Application_Screen.pRightArm = &APP_RightArm;

	// ------------------------------------------------------------------------
	// MAIN MENU
	// ------------------------------------------------------------------------
	MenuList_MainMenu.sTitle = "MENU PRINCIPAL";
	MenuList_MainMenu.pRet = &Application_Screen;
	MenuList_MainMenu.item[0].label = "PANTALLA APLICACION";
	MenuList_MainMenu.item[0].run = &Application_Screen;
	MenuList_MainMenu.item[1].label = "CALIBRACION";
	MenuList_MainMenu.item[1].run = NULL;
	MenuList_MainMenu.item[2].label = "INFO SISTEMA";
	MenuList_MainMenu.item[2].run = NULL;
	MenuList_MainMenu.item[3].label = "CONFIGURACION GENERAL";
	MenuList_MainMenu.item[3].run = NULL;
	MenuList_MainMenu.item[4].label = "CONFIGURACION PID";
	MenuList_MainMenu.item[4].run = NULL;
	MenuList_MainMenu.item[5].label = "GUARDAR CONFIGURACION";
	MenuList_MainMenu.item[5].run = NULL;
	MenuList_MainMenu.item[6].label = "VERSION";
	MenuList_MainMenu.item[6].run = NULL;

	// ------------------------------------------------------------------------
	// LISTBOX CONFIGURATION
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// TFX100 CALIBRATION SCREEN
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// TFX100 INFO SCREEN
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// LISTBOX PID CONFIGURATION
	// ------------------------------------------------------------------------

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
//	FTX100_VersionScreen.pRet = &MenuList_MainMenu;
//	FTX100_VersionScreen.sCustomer = "GERONZI";
//	FTX100_VersionScreen.sSoftware = "TFX100.VER.0.20";
//	FTX100_VersionScreen.sFirmware = "CX100.VER.0.20";
//	FTX100_VersionScreen.sHardware1 = "HE-PLAC-CX100C_B1";
//	FTX100_VersionScreen.sHardware2 = "HE-PLAC-CX100P_A1";

	// ------------------------------------------------------------------------
	// DEBUG SCREEN
	// ------------------------------------------------------------------------

}

/**
 * ----------------------------------------------------------------------------
 * @brief		Run the graphics components
 * ----------------------------------------------------------------------------
 */
void GUI_Update(void)
{
	GUI_TFX100MainScreen(&Application_Screen);
	GUI_MenuList(&MenuList_MainMenu);
	GUI_MsgBox(&MsgBox_SaveConf);

//	// Run all input boxes
//	GUI_InputBox(GUI_InputBox_DefaultPointer);
//
//	// Set configuration in progress flag
//	if ((ListBox_Config.priv.status == GUI_STATUS_ENABLED) || (ListBox_Pid.priv.status == GUI_STATUS_ENABLED))
//		APP_ConfigInProgress = true;
//	else
//		APP_ConfigInProgress = false;
}
