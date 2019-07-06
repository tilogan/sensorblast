/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdint.h>

#ifndef _i2c_inf_h__
#define _i2c_inf_h__


typedef struct _SensorBlast_I2CTrans
{
    uint32_t writeBytes;
    void* writeBuffer;
    uint32_t readBytes;
    void* readBuffer;
    uint8_t slaveAddress;
    int i2cHandle;

} SensorBlast_I2CTrans;

/* Function Declarations */
extern int32_t SensorBlast_I2CTransfer(SensorBlast_I2CTrans* trans);

#endif
