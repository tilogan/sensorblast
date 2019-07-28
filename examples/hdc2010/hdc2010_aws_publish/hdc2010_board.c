/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdint.h>
#include "hdc2010.h"

HDC2010_Config hdcConfig =
{
    .object =
    {
        .softReset = false,
        .autoMode = HDC2010_Auto_Disabled,
        .useHeater = false,
        .enableInt = false,
        .activeHigh = false,
        .intMode = HDC2010_LevelSensitive
    }
};

HDC2010_MeasurementConfig hdcMeasureConfig =
{
    .object =
    {
        .tempResolution = HDC2010_Temp_14bit,
        .humidResolution = HDC2010_Humid_14bit,
        .mode = HDC2010_HumidityAndTemperature,
        .startTrigger = true
    }
};

HDC2010_InterruptConfig hdcInterruptConfig =
{
    .object =
    {
        .humidityLowEnable = false,
        .humidityHighEnable = false,
        .temperatureLowEnable = false,
        .temperatureHighEnable = false,
        .dataReadyEnable = false,
    }
};

