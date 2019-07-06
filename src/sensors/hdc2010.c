/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdint.h>
#include <stddef.h>
#include "sb_codes.h"
#include "sb_i2c_inf.h"
#include "hdc2010.h"

int32_t HDC2010_getMeasurement(int i2cHandle,
                               HDC2010_MeasurementConfig* measureConfig,
                               HDC2010_Measurement* measurement)
{
    SensorBlast_I2CTrans sensorTrans;
    uint8_t writePayload[2];
    uint8_t readReg;
    uint16_t rawValue;

    /* Writing the configuration */
    writePayload[0] = HDC2010_MEASURE_CONFIG_REG;
    writePayload[1] = measureConfig->byte;
    sensorTrans.slaveAddress = HDC2010_SLAVE_ADDRESS;
    sensorTrans.writeBytes = 2;
    sensorTrans.readBytes = 0;
    sensorTrans.writeBuffer = writePayload;
    sensorTrans.i2cHandle = i2cHandle;

    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return SENSOR_BLAST_I2C_ERROR;
    }

    /* Reading the result */
    sensorTrans.writeBytes = 1;
    sensorTrans.readBytes = 1;
    sensorTrans.readBuffer = &readReg;
    writePayload[0] = HDC2010_TEMP_RES0_REG;

    /* Temperature LSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return SENSOR_BLAST_I2C_ERROR;
    }

    rawValue = readReg;
    writePayload[0] = HDC2010_TEMP_RES1_REG;

    /* Temperature MSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return SENSOR_BLAST_I2C_ERROR;
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
        return SENSOR_BLAST_I2C_ERROR;
    }

    rawValue = readReg;
    writePayload[0] = HDC2010_HUMID_RES1_REG;

    /* Humidity MSB */
    if(SensorBlast_I2CTransfer(&sensorTrans) != SENSOR_BLAST_OK)
    {
        return SENSOR_BLAST_I2C_ERROR;
    }

    rawValue |= (readReg << 8);

    /* Calculating the result */
    measurement->humidity = ((rawValue / 65536.0f) * 100.0f);

    return SENSOR_BLAST_OK;
}

int32_t HDC2010_openDriver(int i2cHandle, HDC2010_Config* config)
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
    configTrans.i2cHandle = i2cHandle;

    return SensorBlast_I2CTransfer(&configTrans);
}

