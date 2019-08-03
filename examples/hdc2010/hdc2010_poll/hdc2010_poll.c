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
#include <semaphore.h>
#include "sb_codes.h"
#include "hdc2010.h"
#include "interface/sb_i2c_inf.h"

#define I2C_PATH "/dev/i2c-1"
#define GPIO_PATH "/sys/class/gpio/"
#define GPIO_PIN 17

/* Configuration Structures */
HDC2010_Config hdcConfig =
{
    .object =
    {
        .softReset = false,
        .autoMode = HDC2010_Auto_Disabled,
        .useHeater = false,
        .enableInt = true,
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
        .dataReadyEnable = true,
    }
};

/* Application variables/functions */
static sem_t mutex;
void hdc2010_callback_function(int32_t status);

int main(void)
{
    HDC2010_Measurement curMeasure;
    int32_t status;
    uint32_t count = 0;
    HDC2010_Interface hdcInf;
    int fd;
    int len;
    char writeBuf[64];

    printf("\n---Sensor Blast - Interrupt Polling---\n");

    /* Initializing the parameter structure */
    HDC2010_initInterfaceObject(&hdcInf);

    /* Open the I2C Bus */
    fd = open(I2C_PATH, O_RDWR);
    if (fd < 0)
    {
        printf("\nERROR: Could not open I2C driver. Try running as root?\n");
        return -1;
    }
    hdcInf.i2cHandle = fd;

    /* Exporting the GPIO handle */
    fd = SensorBlast_configureGPIOInput(GPIO_PATH, GPIO_PIN);

    if(fd == 0)
    {
        printf("\nERROR: Could not open GPIO driver\n");
        return -1;
    }
    hdcInf.gpioHandle = fd;

    /* Setting the callback */
    hdcInf.cb = hdc2010_callback_function;

    /* Initializing the semaphore */
    sem_init(&mutex, 0, 1);

    /* Opening the driver */
    HDC2010_openDriver(&hdcInf, &hdcConfig);

    /* Setting the interrupt configuration */
    HDC2010_setInterruptConfig(&hdcInf, &hdcInterruptConfig);

    /* Starting to poll */
    HDC2010_startPolling(&hdcInf);

    while(1)
    {
        /* Initiating the measurement and waiting */
        HDC2010_initiateMeasurement(&hdcInf, &hdcMeasureConfig);
        sem_wait(&mutex);

        /* Reading the result */
        HDC2010_getMeasurement(&hdcInf, &curMeasure);

        printf("--- Sensor Reading ---\n");
        printf("Temperature Reading %f\n", curMeasure.temperature);
        printf("Humidity Reading %f\n", curMeasure.humidity);

        sleep(1);
    }
}

void hdc2010_callback_function(int32_t status)
{
    if(status == SENSOR_BLAST_OK)
    {
        sem_post(&mutex);
    }
    else
    {
        printf("ERROR: Could not read HDC2010! (err 0x%x)\n", status);
    }
}
