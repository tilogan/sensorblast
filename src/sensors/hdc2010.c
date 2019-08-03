/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "sb_codes.h"
#include "sb_logger.h"
#include "sb_i2c_inf.h"
#include "hdc2010.h"

/* Statics */
static void *HDC2010_pollThread(void *arg);

int32_t HDC2010_getManualMeasurement(HDC2010_Interface* infHandle,
                               HDC2010_MeasurementConfig* measureConfig,
                               HDC2010_Measurement* measurement)
{
    struct timespec sleepDelay;
    int32_t ret;

    ret = HDC2010_initiateMeasurement(infHandle, measureConfig);

    if(ret != SENSOR_BLAST_OK)
    {
        return ret;
    }

    /* Delaying for the time specified in the specification */
    sleepDelay.tv_sec = 0;
    if(measureConfig->object.mode == HDC2010_HumidityAndTemperature)
    {
        if(measureConfig->object.humidResolution == HDC2010_Humid_14bit)
        {
            sleepDelay.tv_nsec = HDC2010_14BIT_HUMID_TIME;
        }
        else if(measureConfig->object.humidResolution == HDC2010_Humid_11bit)
        {
            sleepDelay.tv_nsec = HDC2010_11BIT_HUMID_TIME;
        }
        else if(measureConfig->object.humidResolution == HDC2010_Humid_9bit)
        {
            sleepDelay.tv_nsec = HDC2010_9BIT_HUMID_TIME;
        }
        else
        {
            return (SENSOR_BLAST_PARAM_ERROR);
        }

    }
    else if(measureConfig->object.mode == HDC2010_TemperatureOnly)
    {
        if(measureConfig->object.tempResolution == HDC2010_Temp_14bit)
        {
            sleepDelay.tv_nsec = HDC2010_14BIT_TEMP_TIME;
        }
        else if(measureConfig->object.tempResolution == HDC2010_Temp_11bit)
        {
            sleepDelay.tv_nsec = HDC2010_11BIT_TEMP_TIME;
        }
        else if(measureConfig->object.tempResolution == HDC2010_Temp_9bit)
        {
            sleepDelay.tv_nsec = HDC2010_9BIT_TEMP_TIME;
        }
        else
        {
            return (SENSOR_BLAST_PARAM_ERROR);
        }
    }
    else
    {
        return (SENSOR_BLAST_PARAM_ERROR);
    }

    nanosleep(&sleepDelay, NULL);

    return HDC2010_getMeasurement(infHandle, measurement);
}

int32_t HDC2010_initiateMeasurement(HDC2010_Interface* infHandle,
                                    HDC2010_MeasurementConfig* measureConfig)
{
    SensorBlast_I2CTrans sensorTrans;
    uint8_t writePayload[2];

    /* Writing the configuration */
    writePayload[0] = HDC2010_MEASURE_CONFIG_REG;
    writePayload[1] = measureConfig->byte;
    sensorTrans.slaveAddress = HDC2010_SLAVE_ADDRESS;
    sensorTrans.writeBytes = 2;
    sensorTrans.readBytes = 0;
    sensorTrans.writeBuffer = writePayload;
    sensorTrans.i2cHandle = infHandle->i2cHandle;

    return SensorBlast_I2CTransfer(&sensorTrans);
}

int32_t HDC2010_getMeasurement(HDC2010_Interface* infHandle,
                               HDC2010_Measurement* measurement)
{
    uint16_t rawValue;
    SensorBlast_I2CTrans sensorTrans;
    uint8_t writePayload[2];
    uint8_t readReg;

    /* Reading the result */
    sensorTrans.slaveAddress = HDC2010_SLAVE_ADDRESS;
    sensorTrans.writeBytes = 1;
    sensorTrans.writeBuffer = writePayload;
    sensorTrans.readBytes = 1;
    sensorTrans.readBuffer = &readReg;
    sensorTrans.i2cHandle = infHandle->i2cHandle;
    writePayload[0] = HDC2010_TEMP_RES0_REG;

    /* Temperature LSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return (SENSOR_BLAST_I2C_ERROR);
    }

    rawValue = readReg;
    writePayload[0] = HDC2010_TEMP_RES1_REG;

    /* Temperature MSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return (SENSOR_BLAST_I2C_ERROR);
    }

    rawValue |= (readReg << 8);

    /* Calculating the result */
    measurement->temperature = ((rawValue / 65536.0f) * 165.0f) - 40.0f;

    /* Now the Humidity */
    sensorTrans.readBuffer = &readReg;
    writePayload[0] = HDC2010_HUMID_RES0_REG;

    /* Humidity LSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return (SENSOR_BLAST_I2C_ERROR);
    }

    rawValue = readReg;
    writePayload[0] = HDC2010_HUMID_RES1_REG;

    /* Humidity MSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return (SENSOR_BLAST_I2C_ERROR);
    }

    rawValue |= (readReg << 8);

    /* Calculating the result */
    measurement->humidity = ((rawValue / 65536.0f) * 100.0f);

    return (SENSOR_BLAST_OK);
}

int32_t HDC2010_openDriver(HDC2010_Interface* infHandle,
                            HDC2010_Config* config)
{
    SensorBlast_I2CTrans configTrans;
    uint8_t transPayload[1];

    /* Writing the configuration */
    transPayload[0] = HDC2010_CONFIG_REG;
    transPayload[1] = config->byte;
    configTrans.slaveAddress = HDC2010_SLAVE_ADDRESS;
    configTrans.writeBytes = 2;
    configTrans.readBytes = 0;
    configTrans.writeBuffer = transPayload;
    configTrans.i2cHandle = infHandle->i2cHandle;

    return SensorBlast_I2CTransfer(&configTrans);
}

int32_t HDC2010_startPolling(HDC2010_Interface* infHandle)
{
    int ret;
    char threadName[9];

    /* Creating the thread  to start polling the GPIO */
    snprintf(threadName, 9, "THREAD%d", infHandle->gpioHandle);
    ret = pthread_create(&infHandle->pollThread, NULL, HDC2010_pollThread,
                            (void*)infHandle);
}

int32_t HDC2010_stopPolling(HDC2010_Interface* infHandle)
{
    infHandle->stopInitiated == true;
    pthread_join(infHandle->pollThread, NULL);
}

static void *HDC2010_pollThread(void *arg)
{
    HDC2010_Interface* infHandle = (HDC2010_Interface*)arg;
    HDC2010_Measurement measurement;

    if(arg == NULL)
    {
        return (NULL);
    }

    while(1)
    {
        SensorBlast_pollGPIO(infHandle->gpioHandle, 1000);

        if(infHandle->stopInitiated == true)
        {
            break;
        }

        /* Invoking the user callback */
        infHandle->cb(SENSOR_BLAST_OK);
    }

}

int32_t HDC2010_setInterruptConfig(HDC2010_Interface* infHandle,
                                   HDC2010_InterruptConfig* intConfig)
{
    SensorBlast_I2CTrans configTrans;
    uint8_t transPayload[1];

    /* Writing the configuration */
    transPayload[0] = HDC2010_INTERRUPT_CONFIG_REG;
    transPayload[1] = intConfig->byte;
    configTrans.slaveAddress = HDC2010_SLAVE_ADDRESS;
    configTrans.writeBytes = 2;
    configTrans.readBytes = 0;
    configTrans.writeBuffer = transPayload;
    configTrans.i2cHandle = infHandle->i2cHandle;

    return SensorBlast_I2CTransfer(&configTrans);
}

int32_t HDC2010_initInterfaceObject(HDC2010_Interface* infHandle)
{
    infHandle->cb = NULL;
    infHandle->i2cHandle = 0;
    infHandle->gpioHandle = 0;
    infHandle->pollThread = (pthread_t)NULL;
    infHandle->stopInitiated = false;
}
