/*
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    qserial_virtual.h
 * @brief   Virtual serial driver macros and structures.
 *
 * @addtogroup SERIAL_VIRTUAL
 * @{
 */

#ifndef _QSERIAL_VIRTUAL_H_
#define _QSERIAL_VIRTUAL_H_

#if HAL_USE_SERIAL_VIRTUAL || defined(__DOXYGEN__)

#include "qsymqueue.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Virtual serial configuration options
 * @{
 */

/**
 * @brief   Virtual serial buffer size.
 * @note    The default is 256 bytes for both the transmit and receive
 *          buffers.
 */
#if !defined(SERIAL_VIRTUAL_BUFFER_SIZE) || defined(__DOXYGEN__)
#define SERIAL_VIRTUAL_BUFFER_SIZE  256
#endif

/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !CH_CFG_USE_EVENTS
#error "Virtual serial driver requires CH_CFG_USE_EVENTS"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief Driver state machine possible states.
 */
typedef enum
{
    SDVIRTUAL_UNINIT = 0,               /**< Not initialized.                */
    SDVIRTUAL_STOP = 1,                 /**< Stopped.                        */
    SDVIRTUAL_READY = 2                 /**< Ready.                          */
} sdvirtualstate_t;

/**
 * @brief   Structure representing a serial driver.
 */
typedef struct SerialVirtualDriver SerialVirtualDriver;

/**
 * @brief   Virtual serial driver configuration structure.
 * @details An instance of this structure must be passed to @p svirtualdStart()
 *          in order to configure and start the driver operations.
 */
typedef struct
{
    /* Pointer to the far end. */
    SerialVirtualDriver *farp;
} SerialVirtualConfig;

/**
 * @brief   @p SerialVirtualDriver specific data.
 */
#define _serial_virtual_driver_data                                           \
    _base_asynchronous_channel_data                                           \
    /* Driver state. */                                                       \
    sdvirtualstate_t state;                                                   \
    /* Incoming data queue.*/                                                 \
    symmetric_queue_t queue;                                                  \
    /* Input buffer.*/                                                        \
    uint8_t queuebuf[SERIAL_VIRTUAL_BUFFER_SIZE];

/**
 * @brief   @p SerialVirtualDriver specific methods.
 */
#define _serial_virtual_driver_methods                                        \
    _base_asynchronous_channel_methods

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p SerialVirtualDriver virtual methods table.
 */
struct SerialVirtualDriverVMT
{
    _serial_virtual_driver_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   Full duplex virtual serial driver class.
 * @details This class extends @p BaseAsynchronousChannel by adding virtual
 *          I/O queues.
 */
struct SerialVirtualDriver
{
    /** @brief Virtual Methods Table.*/
    const struct SerialVirtualDriverVMT *vmt;
    _serial_virtual_driver_data
    const SerialVirtualConfig *configp;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Returns the far pointer of a virtual serial channel end point.
 *
 * @param[in] ip        pointer to a @p SerialVirtualDriver or derived class
 * @return              pointer to a @p SerialVirtualDriver or derived class
 *
 * @api
 */
#define sdvirtualGetFarPoint(ip) ((ip)->configp->farp)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
    void sdvirtualInit(void);
    void sdvirtualObjectInit(SerialVirtualDriver *sdvirtualp);
    void sdvirtualStart(SerialVirtualDriver *sdvirtualp,
            const SerialVirtualConfig *configp);
    void sdvirtualStop(SerialVirtualDriver *sdvirtualp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_SERIAL_VIRTUAL */

#endif /* _QSERIAL_VIRTUAL_H_ */

/** @} */
