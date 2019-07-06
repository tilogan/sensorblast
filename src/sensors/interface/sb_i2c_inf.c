/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "sb_i2c_inf.h"
#include "sb_codes.h"

extern int32_t SensorBlast_I2CTransfer(SensorBlast_I2CTrans* trans)
{
    struct i2c_msg messages[2];
    struct i2c_rdwr_ioctl_data packets;
    uint8_t numPackets = 0;

    if(trans->writeBytes > 0)
    {
        messages[numPackets].addr = trans->slaveAddress;
        messages[numPackets].flags = 0;
        messages[numPackets].len = trans->writeBytes;
        messages[numPackets].buf = trans->writeBuffer;
        numPackets++;
    }

    if(trans->readBytes > 0)
    {
        messages[numPackets].addr = trans->slaveAddress;
        messages[numPackets].flags = I2C_M_RD;
        messages[numPackets].len = trans->readBytes;
        messages[numPackets].buf = trans->readBuffer;
        numPackets++;
    }

     /* Sending the request */
    packets.msgs = messages;
    packets.nmsgs = numPackets;

    if(ioctl(trans->i2cHandle, I2C_RDWR, &packets) < 0)
    {
        return (SENSOR_BLAST_I2C_ERROR);
    }

    return (SENSOR_BLAST_OK);
}
