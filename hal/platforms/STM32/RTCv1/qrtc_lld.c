/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

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
 * @file    STM32/RTCv1/qrtc_lld.c
 * @brief   STM32 RTC subsystem low level driver header.
 *
 * @addtogroup RTC
 * @{
 */

#include "ch.h"
#include "qhal.h"

#if HAL_USE_RTC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Convert from RTCTime to struct tm.
 *
 * @param[in] timespec      pointer to RTCTime structure
 * @param[out] result       pointer to tm structure
 *
 * @api
 */
void rtcRTCTime2TM(const RTCTime *timespec, struct tm *result)
{
    gmtime_r((time_t *)&(timespec->tv_sec), result);
}

/**
 * @brief   Convert from struct tm to RTCTime.
 *
 * @param[in] result        pointer to tm structure
 * @param[out] timespec     pointer to RTCTime structure
 *
 * @api
 */
void rtcTM2RTCTime(struct tm *timespec, RTCTime *result)
{
    result->tv_sec = mktime(timespec);
}

#endif /* HAL_USE_RTC */

/** @} */
