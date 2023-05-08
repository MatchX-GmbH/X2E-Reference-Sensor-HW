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
 * @file    main.c
 * @brief   This is an example for the ZMOD4410 gas sensor using the iaq_2nd_gen library.
 * @version 3.1.0
 * @author Renesas Electronics Corporation
 **/

#include "zmod4410_config_iaq2.h"
#include "zmod4xxx.h"
#include "zmod4xxx_cleaning.h"
#include "zmod4xxx_hal.h"
#include "iaq_2nd_gen.h"

int main()
{
    int8_t ret;
    zmod4xxx_dev_t dev;
    

    /* Sensor specific variables */
    uint8_t zmod4xxx_status;
    uint8_t track_number[ZMOD4XXX_LEN_TRACKING];
    uint8_t adc_result[ZMOD4410_ADC_DATA_LEN];
    uint8_t prod_data[ZMOD4410_PROD_DATA_LEN];
    iaq_2nd_gen_handle_t algo_handle;
    iaq_2nd_gen_results_t algo_results;
    iaq_2nd_gen_inputs_t algo_input;

    /**** TARGET SPECIFIC FUNCTION ****/
    /*
     * To allow the example running on customer-specific hardware, the init_hardware
     * function must be adapted accordingly. The mandatory funtion pointers *read,
     * *write and *delay require to be passed to "dev" (reference files located
     * in "dependencies/zmod4xxx_api/HAL" directory). For more information, read
     * the Datasheet, section "I2C Interface and Data Transmission Protocol".
     */
    ret = init_hardware(&dev);
    if (ret) {
        printf("Error %d during initialize hardware, exiting program!\n", ret);
        goto exit;
    }
    /**** TARGET SPECIFIC FUNCTION ****/

    /* Sensor related data */
    dev.i2c_addr = ZMOD4410_I2C_ADDR;
    dev.pid = ZMOD4410_PID;
    dev.init_conf = &zmod_iaq2_sensor_cfg[INIT];
    dev.meas_conf = &zmod_iaq2_sensor_cfg[MEASUREMENT];
    dev.prod_data = prod_data;

    /* Read product ID and configuration parameters. */
    ret = zmod4xxx_read_sensor_info(&dev);
    if (ret) {
        printf("Error %d during reading sensor information, exiting program!\n",
               ret);
        goto exit;
    }

    /*
     * Retrieve sensors unique tracking number and individual trimming information.
     * Provide this information when requesting support from Renesas.
     */
    ret = zmod4xxx_read_tracking_number(&dev, track_number);
    if (ret) {
        printf("Error %d during reading tracking number, exiting program!\n",
               ret);
        goto exit;
    }
    printf("Sensor tracking number: x0000");
    for (int i = 0; i < sizeof(track_number); i++) {
        printf("%02X", track_number[i]);
    }
    printf("\n");
    printf("Sensor trimming data: ");
    for (int i = 0; i < sizeof(prod_data); i++) {
        printf(" %i", prod_data[i]);
    }
    printf("\n");

    /*
     * Start the cleaning procedure. Check the Programming Manual on indications
     * of usage. IMPORTANT NOTE: The cleaning procedure can be run only once
     * during the modules lifetime and takes 1 minute (blocking).
     */
    printf("Starting cleaning procedure. This might take up to 1 min ...\n");
    ret = zmod4xxx_cleaning_run(&dev);
    if (ERROR_CLEANING == ret) {
        printf("Skipping cleaning procedure. It has already been performed!\n");
    } else if (ret) {
        printf("Error %d during cleaning procedure, exiting program!\n", ret);
        goto exit;
    }

    /* Determine calibration parameters and configure measurement. */
    ret = zmod4xxx_prepare_sensor(&dev);
    if (ret) {
        printf("Error %d during preparation of the sensor, exiting program!\n",
               ret);
        goto exit;
    }

    /*
     * One-time initialization of the algorithm. Handle passed to calculation
     * function.
     */
    ret = init_iaq_2nd_gen(&algo_handle);
    if (ret) {
        printf("Error %d during initializing algorithm, exiting program!\n",
                ret);
        goto exit;
    }

    printf("Evaluate measurements in a loop. Press any key to quit.\n\n");
    do {
        /* Start a measurement. */
        ret = zmod4xxx_start_measurement(&dev);
        if (ret) {
            printf("Error %d during starting measurement, exiting program!\n",
                   ret);
            goto exit;
        }
        /*
         * Perform delay. Required to keep proper measurement timing and keep algorithm accuracy.
         * For more information, read the Programming Manual, section
         * "Interrupt Usage and Measurement Timing".
         */
        dev.delay_ms(ZMOD4410_IAQ2_SAMPLE_TIME);

        /* Verify completion of measurement sequence. */
        ret = zmod4xxx_read_status(&dev, &zmod4xxx_status);
        if (ret) {
            printf("Error %d during reading sensor status, exiting program!\n",
                   ret);
            goto exit;
        }
        /* Check if measurement is running. */
        if (zmod4xxx_status & STATUS_SEQUENCER_RUNNING_MASK) {
            /*
             * Check if reset during measurement occured. For more information,
             * read the Programming Manual, section "Error Codes".
             */
            ret = zmod4xxx_check_error_event(&dev);
            switch (ret) {
            case ERROR_POR_EVENT:
                printf(
                    "Measurement completion fault. Unexpected sensor reset.\n");
                break;
            case ZMOD4XXX_OK:
                printf(
                    "Measurement completion fault. Wrong sensor setup.\n");
                break;
            default:
                printf("Error during reading status register (%d)\n", ret);
                break;
            }
            goto exit;
        }
        /* Read sensor ADC output. */
        ret = zmod4xxx_read_adc_result(&dev, adc_result);
        if (ret) {
            printf("Error %d during reading of ADC results, exiting program!\n",
                   ret);
            goto exit;
        }

        /*
         * Check validity of the ADC results. For more information, read the
         * Programming Manual, section "Error Codes".
         */
        ret = zmod4xxx_check_error_event(&dev);
        if (ret) {
            printf("Error during reading status register (%d)\n", ret);
            goto exit;
        }
        
        /*
         * Assign algorithm inputs: raw sensor data and ambient conditions.
         * Production code should use measured temperature and humidity values.
         */
        algo_input.adc_result = adc_result;
        algo_input.humidity_pct = 50.0;
        algo_input.temperature_degc = 20.0;
        
        /* Calculate algorithm results. */
        ret = calc_iaq_2nd_gen(&algo_handle, &dev, &algo_input, &algo_results);
        
        /* Skip 100 stabilization samples for iaq_2nd_gen algorithm. */
        printf("*********** Measurements ***********\n");
        for (int i = 0; i < 13; i++) {
            printf(" Rmox[%d] = ", i);
            printf("%.3f kOhm\n", algo_results.rmox[i] / 1e3);
        }
        printf(" Rcda = %.3f kOhm \n", pow(10, algo_results.log_rcda) / 1e3);
        printf(" EtOH = %6.3f ppm\n", algo_results.etoh);
        printf(" TVOC = %6.3f mg/m^3\n", algo_results.tvoc);
        printf(" eCO2 = %4.0f ppm\n", algo_results.eco2);
        printf(" IAQ  = %4.1f\n", algo_results.iaq);

        /* Check validity of the algorithm results. */
        switch (ret) {
        case IAQ_2ND_GEN_STABILIZATION:
            printf("Warm-Up!\n");
            break;
        case IAQ_2ND_GEN_OK:
            printf("Valid!\n");
            break;
        /*
        * Notification from Sensor self-check. For more information, read the
        * Programming Manual, section "Troubleshoot Sensor Damage (Sensor Self-Check)".
        */
        case IAQ_2ND_GEN_DAMAGE:
            printf("Error: Sensor probably damaged. Algorithm results may be incorrect.\n");
            break;
        /* Exit program due to unexpected error. */
        default:
            printf("Unexpected Error during algorithm calculation: Exiting Program.\n");
            goto exit;
        }

        

    } while (!is_key_pressed());

exit:
    ret = deinit_hardware();
    if (ret) {
        printf("Error %d during deinitializing hardware, exiting program!\n",
               ret);
        return ret;
    }
    return 0;
}
