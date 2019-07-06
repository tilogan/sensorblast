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
    bool startTrigger :1;
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

typedef struct _HDC2010_Measurement
{
    float temperature;
    float humidity;
} HDC2010_Measurement;

typedef struct _HDC2010_DriverConfig
{
    HDC2010_MeasurementConfig* measureConfig;
    HDC2010_Config* config;
    int i2cHandle;
} HDC2010_DriverConfig;

typedef int HDC2010_Handle;

/* Register Definitions */
#define HDC2010_TEMP_RES0_REG 0x00
#define HDC2010_TEMP_RES1_REG 0x01
#define HDC2010_HUMID_RES0_REG 0x03
#define HDC2010_HUMID_RES1_REG 0x04
#define HDC2010_MEASURE_CONFIG_REG 0x0F
#define HDC2010_CONFIG_REG 0x0E

/* Function Definitions */
extern int32_t HDC2010_getMeasurement(int i2chandle,
                                      HDC2010_MeasurementConfig* measureConfig,
                                      HDC2010_Measurement* measurement);
extern int32_t HDC2010_openDriver(int i2cHandle, HDC2010_Config* config);

#endif
