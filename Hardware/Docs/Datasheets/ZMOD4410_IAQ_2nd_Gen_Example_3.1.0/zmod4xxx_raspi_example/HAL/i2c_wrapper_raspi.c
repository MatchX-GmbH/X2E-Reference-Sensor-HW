/*******************************************************************************
 * Copyright (c) 2023 Renesas Electronics Corporation
 * All Rights Reserved.
 *
 * This code is proprietary to Renesas, and is license pursuant to the terms and
 * conditions that may be accessed at:
 * https://www.renesas.com/eu/en/document/msc/renesas-software-license-terms-gas-sensor-software
 *
 ******************************************************************************/

/**
 * @file    i2c_wrapper_raspi.c
 * @brief   Raspi specific function using PIGPIO library
 * @version 2.5.2
 */

#include "i2c_wrapper_raspi.h"

#define I2CBus 1

int8_t raspi_i2c_setup()
{
    if (gpioInitialise() < 0) {
        return ERROR_I2C;
    }
    return ZMOD4XXX_OK;
}

void raspi_delay(uint32_t ms)
{
    usleep(ms * 1000);
}

int8_t raspi_i2c_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buf,
                      uint8_t len)
{
    int8_t handle, ret;

    handle = i2cOpen(I2CBus, i2c_addr, 0);
    if (handle < 0) {
        return ERROR_I2C;
    }
    ret = i2cReadI2CBlockData(handle, reg_addr, (char *)buf, len);
    if (len != ret) {
        return ERROR_I2C;
    }
    i2cClose(handle);
    return ZMOD4XXX_OK;
}

int8_t raspi_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buf,
                       uint8_t len)
{
    int8_t handle, ret;

    handle = i2cOpen(I2CBus, i2c_addr, 0);
    if (handle < 0) {
        return ERROR_I2C;
    }
    ret = i2cWriteI2CBlockData(handle, reg_addr, (char *)buf, len);
    if (ret) {
        return ERROR_I2C;
    }
    i2cClose(handle);
    return ZMOD4XXX_OK;
}
