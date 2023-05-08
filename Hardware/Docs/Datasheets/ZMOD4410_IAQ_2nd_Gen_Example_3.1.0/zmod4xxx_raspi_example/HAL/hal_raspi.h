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
 * @brief   Hardware abstraction layer for Raspi
 * @version 2.5.2
 */

#ifndef _HAL_RASPI_H
#define _HAL_RASPI_H

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "i2c_wrapper_raspi.h"
#include "zmod4xxx_types.h"

/**
 * @brief   Initialize the target hardware
 * @param   [in] dev pointer to the device
 * @return  error code
 * @retval  0 success
 * @retval  "!= 0" error
 */
zmod4xxx_err init_hardware(zmod4xxx_dev_t *dev);

/**
 * @brief   deinitialize hardware if it is required
 * @return  error code
 * @retval  0 success
 * @retval  "!= 0" error
 */
zmod4xxx_err deinit_hardware();

/**
 * @brief   Check if any key is pressed. This function checks
 *          for any key pressed in STDIN and prints it out.
 * @return  integer
 * @retval  1 when is pressed and 0 key is not pressed
 */
int8_t is_key_pressed();

#endif /* _HAL_RASPI_H */
