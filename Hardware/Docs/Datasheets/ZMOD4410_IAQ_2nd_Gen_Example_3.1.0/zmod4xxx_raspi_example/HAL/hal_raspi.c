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
 * @file    hal_raspi.c
 * @brief   Hardware abstraction layer for Raspi
 * @version 2.5.2
 */

#include "hal_raspi.h"

zmod4xxx_err init_hardware(zmod4xxx_dev_t *dev)
{
    raspi_i2c_setup();
    dev->read = raspi_i2c_read;
    dev->write = raspi_i2c_write;
    dev->delay_ms = raspi_delay;
    return ZMOD4XXX_OK;
}

zmod4xxx_err deinit_hardware()
{
    gpioTerminate();
    return ZMOD4XXX_OK;
}

int8_t is_key_pressed()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    /* Get parameters related to the FILE Descriptor fd */
    /* STDIN_FILENO is fd for STDIN buffer */
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    /* ICANON canonical mode and ECHO input characters */
    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
