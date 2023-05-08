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
  * @file    iaq_2nd_gen.h
 * @author Renesas Electronics Corporation
 * @version 3.1.0
  * @brief   This file contains the data structure definitions and
  *          the function definitions for the 2nd generation IAQ algorithm.
  * @details The library contains an algorithm to calculate an EtOH, TVOC and
  *          IAQ index from ZMOD4410 measurements.
  *          The implementation is made to allow more than one sensor.
  *
  */

#ifndef IAQ_2ND_GEN_H_
#define IAQ_2ND_GEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>
#include "zmod4xxx_types.h"

/**
* @brief Variables that describe the library version
*/
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} algorithm_version;

/** \addtogroup RetCodes Return codes of the algorithm functions.
 *  @{
 */
#define IAQ_2ND_GEN_OK            (0) /**< everything okay */
#define IAQ_2ND_GEN_STABILIZATION (1) /**< sensor in stabilization */
#define IAQ_2ND_GEN_DAMAGE        (-102) /**< sensor damaged */
/** @}*/

/**
* @brief Variables that describe the sensor or the algorithm state.
*/
typedef struct {
    float log_rcda[9]; /**< log10 of CDA resistances. */
    float rh_cda;
    float t_cda;
    uint32_t sample_counter;
    float rg_mean;
    float var_log10_rel_iaq;
    uint8_t need_filter_init;
    float rel_iaq_smooth;
    float rel_iaq_deltafilter;
    float eco2_untracked;
    float min_tracking;
    float acchw;
    float accow;
    float etoh;
    float iaq;
} iaq_2nd_gen_handle_t;

/**
* @brief Variables that receive the algorithm outputs.
*/
typedef struct {
    float rmox[13]; /**< MOx resistance. */
    float log_rcda; /**< log10 of CDA resistance. */
    float rhtr; /**< heater resistance. */
    float temperature; /**< ambient temperature (degC). */
    float iaq; /**< IAQ index. */
    float tvoc; /**< TVOC concentration (mg/m^3). */
    float etoh; /**< EtOH concentration (ppm). */
    float eco2; /**< eCO2 concentration (ppm). */
} iaq_2nd_gen_results_t;

/**
* @brief Variables that are needed for algorithm
 * @param   [in] adc_result Value from read_adc_result function
 * @param   [in] humidity_pct relative ambient humidity (%)
 * @param   [in] temperature_degc ambient temperature (degC)
*/
typedef struct {
    uint8_t *adc_result;
    float humidity_pct;
    float temperature_degc;
} iaq_2nd_gen_inputs_t;

/**
 * @brief   calculates algorithm results from present sample.
 * @param   [in] handle Pointer to algorithm state variable.
 * @param   [in] dev Pointer to the device.
 * @param   [in] algo_input Structure containing inputs required for algo calculation.
 * @param   [out] results Pointer for storing the algorithm results.
 * @return  error code.
 */
int8_t calc_iaq_2nd_gen(iaq_2nd_gen_handle_t *handle, const zmod4xxx_dev_t *dev,
                        const iaq_2nd_gen_inputs_t *algo_input,
                        iaq_2nd_gen_results_t *results);

/**
 * @brief   Initializes the algorithm.
 * @param   [out] handle Pointer to algorithm state variable.
 * @return  error code.
*/
int8_t init_iaq_2nd_gen(iaq_2nd_gen_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* IAQ_2ND_GEN_H_ */

