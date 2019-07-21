/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <poll.h>
#include "sb_codes.h"
#include "hdc2010.h"

#define I2C_PATH "/dev/i2c-1"

/* Configuration Structures */
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

int main(void)
{
    HDC2010_Measurement curMeasure;
    int32_t status;
    uint32_t count = 0;
    HDC2010_Interface hdcInf;
    int fd;
    int len;
    char writeBuf[64];

    printf("\n---Sensor Blast - HDC2010 Proof of Concept---\n");

    /* Open the I2C Bus */
    if ((hdcInf.i2cHandle = open(I2C_PATH, O_RDWR)) < 0)
    {
        printf("\nERROR: Could not open I2C driver. Try running as root?\n");
        return -1;
    }

    /* Opening the driver */
    HDC2010_openDriver(&hdcInf, &hdcConfig);

    /* Setting the interrupt config */
    HDC2010_setInterruptConfig(&hdcInf, &hdcInterruptConfig);

    while(1)
    {
        status = HDC2010_getManualMeasurement(&hdcInf, &hdcMeasureConfig,
                                               &curMeasure);

        if(status == SENSOR_BLAST_OK)
        {
            printf("--- Sensor Reading %d ---\n", count);
            printf("Temperature Reading %f\n", curMeasure.temperature);
            printf("Humidity Reading %f\n", curMeasure.humidity);
        }
        else
        {
            printf("ERROR: Could not read HDC2010! (err 0x%x)\n", status);
            return status;
        }

        count++;

        sleep(1);
    }
}

