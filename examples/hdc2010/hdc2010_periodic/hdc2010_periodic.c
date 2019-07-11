/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include "hdc2010.h"
#include <fcntl.h>
#include "sb_codes.h"
#include <linux/i2c.h>

#define I2C_PATH "/dev/i2c-1"

/* Configuration Structures */
HDC2010_Config hdcConfig =
{
    .object =
    {
        .softReset = false,
        .autoMode = HDC2010_Auto_Disabled,
        .useHeater = true,
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

int main(void)
{
    HDC2010_Measurement curMeasure;
    int32_t status;
    uint32_t count = 0;
    int i2cHandle;

    printf("\n---Sensor Blast - HDC2010 Proof of Concept---");

    /* Open the I2C Bus */
    if ((i2cHandle = open(I2C_PATH, O_RDWR)) < 0)
    {
        printf("\nERROR: Could not open I2C driver. Try running as root?\n");
        return -1;
    }

    /* Opening the driver */
    HDC2010_openDriver(i2cHandle, &hdcConfig);

    while(1)
    {
        status = HDC2010_getManualMeasurement(i2cHandle, &hdcMeasureConfig, &curMeasure);

        if(status == SENSOR_BLAST_OK)
        {
            printf("\n--- Sensor Reading %d ---", count);
            printf("\nTemperature Reading %f", curMeasure.temperature);
            printf("\nHumidity Reading %f", curMeasure.humidity);
        }
        else
        {
            printf("\nERROR: Could not read HDC2010! (err 0x%x)\n", status);
            return status;
        }

        count++;

        sleep(1);
    }
}

