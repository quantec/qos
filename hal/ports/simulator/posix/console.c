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
 * @file    console.c
 * @brief   Simulator console driver code.
 * @{
 */

#include <stdio.h>
#include <errno.h>

#include "ch.h"
#include "hal.h"
#include "console.h"

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief Console driver 1.
 */
BaseChannel CD1;

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static size_t _write(void *ip, const uint8_t *bp, size_t n) {
  size_t ret;
  (void)ip;

  do
  {
    ret = fwrite(bp, 1, n, stdout);
    if (errno == EAGAIN || errno == EINTR)
      chThdSleepMilliseconds(1);
  } while (ret == 0 && errno == EAGAIN);

  fflush(stdout);

  return ret;
}

static size_t _read(void *ip, uint8_t *bp, size_t n) {
  size_t ret;
  (void)ip;

  do
  {
      ret = fread(bp, 1, n, stdin);
      if (errno == EAGAIN || errno == EINTR)
        chThdSleepMilliseconds(1);
  } while (ret == 0 &&
      (errno == EAGAIN || errno == EINTR));

  return ret;
}

static msg_t _put(void *ip, uint8_t b) {
  (void)ip;

  if (_write(ip, &b, 1) == 1)
    return MSG_OK;
  else
    return MSG_RESET;
}

static msg_t _get(void *ip) {
  (void)ip;

  uint8_t b;
  if (_read(ip, &b, 1) == 1)
    return MSG_OK;
  else
    return MSG_RESET;
}

static size_t _writet(void *ip, const uint8_t *bp, size_t n, sysinterval_t timeout) {
  systime_t start = chVTGetSystemTimeX();
  size_t ret;

  do
  {
    if (chVTTimeElapsedSinceX(start) > timeout)
      return MSG_TIMEOUT;
    ret = fwrite(bp, 1, n, stdout);
    if (errno == EAGAIN || errno == EINTR)
      chThdSleepMilliseconds(1);
  } while (ret == 0 &&
      (errno == EAGAIN || errno == EINTR));

  return ret;
}

static size_t _readt(void *ip, uint8_t *bp, size_t n, sysinterval_t timeout) {
    systime_t start = chVTGetSystemTimeX();
  size_t ret;

  do
  {
    if (chVTTimeElapsedSinceX(start) > timeout)
      return MSG_TIMEOUT;
    ret = fread(bp, 1, n, stdin);
    if (errno == EAGAIN || errno == EINTR)
      chThdSleepMilliseconds(1);
  } while (ret == 0 &&
      (errno == EAGAIN || errno == EINTR));

  return ret;
}

static msg_t _putt(void *ip, uint8_t b, sysinterval_t timeout) {
  (void)ip;

  if (_writet(ip, &b, 1, timeout) == 1)
    return MSG_OK;
  else
    return MSG_TIMEOUT;
}

static msg_t _gett(void *ip, sysinterval_t timeout) {
  (void)ip;

  uint8_t b;
  if (_readt(ip, &b, 1, timeout) == 1)
    return b;
  else
    return MSG_TIMEOUT;
}

static msg_t _ctl(void *ip, unsigned int operation, void *arg) {

  (void)ip;
  (void)operation;
  (void)arg;

  return MSG_OK;
}

static const struct BaseChannelVMT vmt = {
  (size_t)0,
  _write, _read, _put, _get,
  _putt, _gett, _writet, _readt,
  _ctl
};

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

void conInit(void) {

  CD1.vmt = &vmt;
}

/** @} */
