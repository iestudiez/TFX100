/**
 ******************************************************************************
 * @file        : config.h
 * @author      : Ivan.Estudiez
 * @brief       : Configuration file
 * @date        : 21 ago 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef CONFIG_H_
#define CONFIG_H_

// System
// ----------------------------------------------------------------------------
#define CONFIG_TEMP_ERR_VALUE				(60)
#define CONFIG_SENSOR_ERR_VALUE				(260)
#define CONFIG_VOLTAGE_MIN_VALUE			(11000)
#define CONFIG_VOLTAGE_MAX_VALUE			(16000)
#define CONFIG_OVERCURRENT_ERR_VALUE		(6000)

// PID
// ----------------------------------------------------------------------------
#define CONFIG_PID_KP						(9000)
#define CONFIG_PID_KI						(1000)
#define CONFIG_PID_KD						(2000)
#define CONFIG_PID_MAX_INT					(20)
#define CONFIG_PID_OFFSET					(160)
#define CONFIG_PID_P_DIVIDER				(10000)
#define CONFIG_PID_I_DIVIDER				(10000)
#define CONFIG_PID_D_DIVIDER				(1000)

// Angle sensor
// ----------------------------------------------------------------------------
#define CONFIG_SENSOR_ANGLE_RANGE			(900)
#define CONFIG_SENSOR_MIN_VALUE				(368U)
#define CONFIG_SENSOR_MAX_VALUE				(3640U)
#define CONFIG_SENSOR_ERROR_VALUE			(350U)
#define CONFIG_SENSOR_INV_LEFT				(true)
#define CONFIG_SENSOR_INV_RIGHT				(false)

// Application
// ----------------------------------------------------------------------------
#define CONFIG_LIFTING_TIME					(50)
#define CONFIG_ANGLE_SETPOINT				(350)
#define CONFIG_MANUAL_PWM_UP				(500)
#define CONFIG_MANUAL_PWM_DOWN				(300)

#endif /* CONFIG_H_ */
