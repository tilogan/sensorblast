/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef _hdc2010_h__
#define _hdc2010_h__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define HDC2010_SLAVE_ADDRESS 0x40

typedef enum
{
    HDC2010_Temp_14bit = 0x00,
    HDC2010_Temp_11bit = 0x01,
    HDC2010_Temp_9bit  = 0x02
} HDC2010_TempRes;

typedef enum
{
    HDC2010_Humid_14bit = 0x00,
    HDC2010_Humid_11bit = 0x01,
    HDC2010_Humid_9bit  = 0x02
} HDC2010_HumidRes;

typedef enum
{
    HDC2010_HumidityAndTemperature = 0x00,
    HDC2010_TemperatureOnly = 0x1,
} HDC2010_Mode;

typedef enum
{
    HDC2010_Auto_Disabled = 0x00,
    HDC2010_Auto_1_120_Hz = 0x01,
    HDC2010_Auto_1_160_Hz = 0x02,
    HDC2010_Auto_0_1_Hz = 0x03,
    HDC2010_Auto_0_2_Hz = 0x04,
    HDC2010_Auto_1_Hz = 0x05,
    HDC2010_Auto_2_Hz = 0x06,
    HDC2010_Auto_5_Hz = 0x07
} HDC2010_AutoMeasureMode;

typedef enum
{

    HDC2010_LevelSensitive = 0x00,
    HDC2010_ComparatorMode = 0x01
} HDC2010_InterruptMode;

typedef struct _HDC_ConfigStruct
{
    HDC2010_InterruptMode intMode : 1;
    bool activeHigh : 1;
    bool enableInt : 1;
    bool useHeater : 1;
    HDC2010_AutoMeasureMode autoMode : 3;
    bool softReset : 1;
} HDC2010_ConfigStruct;

typedef union _HDC2010_Config
{
    uint8_t byte;
    HDC2010_ConfigStruct object;
} HDC2010_Config;

typedef struct _HDC2010_MeasurementConfigStruct
{
    bool startTrigger : 1;
    HDC2010_Mode mode : 2;
    uint8_t reserved : 1;
    HDC2010_HumidRes humidResolution : 2;
    HDC2010_TempRes  tempResolution : 2;
} HDC2010_MeasurementConfigStruct;

typedef union _HDC2010_MeasurementConfig
{
    uint8_t byte;
    HDC2010_MeasurementConfigStruct object;
} HDC2010_MeasurementConfig;

typedef struct _HDC2010_InterruptConfigStruct
{
    bool humidityLowEnable :1;
    bool humidityHighEnable :1;
    bool temperatureLowEnable :1;
    bool temperatureHighEnable :1;
    bool dataReadyEnable :1;
    uint8_t reserverd :3;
} HDC2010_InterruptConfigStruct;

typedef union _HDC2010_InterruptConfig
{
    uint8_t byte;
    HDC2010_InterruptConfigStruct object;
} HDC2010_InterruptConfig;

typedef struct _HDC2010_Measurement
{
    /* Where the results are stored */
    float temperature;
    float humidity;
} HDC2010_Measurement;

typedef void (*HDC2010_Callback)(int32_t);

typedef struct _HDC2010_DriverConfig
{
    HDC2010_MeasurementConfig* measureConfig;
    HDC2010_Config* config;
} HDC2010_DriverConfig;

typedef struct _HDC2010_Interface
{
    int i2cHandle;
    int gpioHandle;
    bool stopInitiated;
    pthread_t pollThread;
    HDC2010_Callback cb;
} HDC2010_Interface;

typedef int HDC2010_Handle;

/* Register Definitions */
#define HDC2010_TEMP_RES0_REG 0x00
#define HDC2010_TEMP_RES1_REG 0x01
#define HDC2010_HUMID_RES0_REG 0x02
#define HDC2010_HUMID_RES1_REG 0x03
#define HDC2010_INTERRUPT_CONFIG_REG 0x07
#define HDC2010_MEASURE_CONFIG_REG 0x0F
#define HDC2010_CONFIG_REG 0x0E

/* Datasheet Definitions */
#define HDC2010_14BIT_HUMID_TIME 660000
#define HDC2010_11BIT_HUMID_TIME 400000
#define HDC2010_9BIT_HUMID_TIME 275000
#define HDC2010_14BIT_TEMP_TIME 610000
#define HDC2010_11BIT_TEMP_TIME 350000
#define HDC2010_9BIT_TEMP_TIME 225000

/* Function Definitions */
extern int32_t HDC2010_getManualMeasurement(HDC2010_Interface* infHandle,
                                      HDC2010_MeasurementConfig* measureConfig,
                                      HDC2010_Measurement* measurement);
extern int32_t HDC2010_getMeasurement(HDC2010_Interface* infHandle, 
                                      HDC2010_Measurement* measurement);
extern int32_t HDC2010_initInterfaceObject(HDC2010_Interface* infHandle);
extern int32_t HDC2010_initiateMeasurement(HDC2010_Interface* infHandle,
                                           HDC2010_MeasurementConfig* measureConfig);
extern int32_t HDC2010_openDriver(HDC2010_Interface* infHandle,
                                  HDC2010_Config* config);
extern int32_t HDC2010_setInterruptConfig(HDC2010_Interface* infHandle,
                                          HDC2010_InterruptConfig* intConfig);
extern int32_t HDC2010_startPolling(HDC2010_Interface* infHandle);
extern int32_t HDC2010_stopPolling(HDC2010_Interface* infHandle);
#endif
