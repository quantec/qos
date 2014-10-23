/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file    qsymqueue.c
 * @brief   Symmetric Queues code.
 *
 * @addtogroup io_queues
 * @details Symmetric queues are to be used from threads only.
 * @pre     In order to use the symmetric queues the @p CH_USE_QUEUES option must
 *          be enabled in @p chconf.h.
 * @{
 */

#include "ch.h"
#include "qsymqueue.h"

#if CH_USE_QUEUES || defined(__DOXYGEN__)

/**
 * @brief   Puts the invoking thread into the queue's threads queue.
 *
 * @param[out] sqp      pointer to an @p SymmetricQueue structure
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              A message specifying how the invoking thread has been
 *                      released from threads queue.
 * @retval Q_OK         is the normal exit, thread signaled.
 * @retval Q_RESET      if the queue has been reset.
 * @retval Q_TIMEOUT    if the queue operation timed out.
 */
static msg_t qwait(SymmetricQueue *sqp, systime_t timeout)
{
    if (timeout == TIME_IMMEDIATE)
      return Q_TIMEOUT;
    currp->p_u.wtobjp = sqp;
    queue_insert(currp, &sqp->q_waiting);
    return chSchGoSleepTimeoutS(THD_STATE_WTQUEUE, timeout);
}

/**
 * @brief   Initializes an input queue.
 * @details A Semaphore is internally initialized and works as a counter of
 *          the bytes contained in the queue.
 * @note    The callback is invoked from within the S-Locked system state,
 *          see @ref system_states.
 *
 * @param[out] sqp      pointer to an @p SymmetricQueue structure
 * @param[in] bp        pointer to a memory area allocated as queue buffer
 * @param[in] size      size of the queue buffer
 *
 * @init
 */
void chSymQInit(SymmetricQueue *sqp, uint8_t *bp, size_t size)
{
    queue_init(&sqp->q_waiting);
    sqp->q_counter = 0;
    sqp->q_buffer = sqp->q_rdptr = sqp->q_wrptr = bp;
    sqp->q_top = bp + size;
}

/**
 * @brief   Resets an input queue.
 * @details All the data in the input queue is erased and lost, any waiting
 *          thread is resumed with status @p Q_RESET.
 * @note    A reset operation can be used by a low level driver in order to
 *          obtain immediate attention from the high level layers.
 *
 * @param[in] sqp       pointer to an @p SymmetricQueue structure
 *
 * @iclass
 */
void chSymQResetI(SymmetricQueue *sqp)
{
    chDbgCheckClassI();

    sqp->q_rdptr = sqp->q_wrptr = sqp->q_buffer;
    sqp->q_counter = 0;
    while (notempty(&sqp->q_waiting))
      chSchReadyI(fifo_remove(&sqp->q_waiting))->p_u.rdymsg = Q_RESET;
}

/**
 * @brief   Input queue read with timeout.
 * @details This function reads a byte value from an input queue. If the queue
 *          is empty then the calling thread is suspended until a byte arrives
 *          in the queue or a timeout occurs.
 *
 * @param[in] sqp       pointer to an @p SymmetricQueue structure
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              A byte value from the queue.
 * @retval Q_TIMEOUT    if the specified time expired.
 * @retval Q_RESET      if the queue has been reset.
 *
 * @api
 */
msg_t chSymQGetTimeout(SymmetricQueue *sqp, systime_t timeout)
{
    uint8_t b;

    chSysLock();
    while (chSymQIsEmptyI(sqp))
    {
        msg_t msg;
        if ((msg = qwait((SymmetricQueue *)sqp, timeout)) < Q_OK)
        {
            chSysUnlock();
            return msg;
        }
    }

    sqp->q_counter--;
    b = *sqp->q_rdptr++;
    if (sqp->q_rdptr >= sqp->q_top)
        sqp->q_rdptr = sqp->q_buffer;

    if (notempty(&sqp->q_waiting))
        chSchReadyI(fifo_remove(&sqp->q_waiting))->p_u.rdymsg = Q_OK;

    chSysUnlock();
    return b;
}

/**
 * @brief   Input queue read with timeout.
 * @details The function reads data from an input queue into a buffer. The
 *          operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 * @note    The function is not atomic, if you need atomicity it is suggested
 *          to use a semaphore or a mutex for mutual exclusion.
 *
 * @param[in] sqp       pointer to an @p SymmetricQueue structure
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 *
 * @api
 */
size_t chSymQReadTimeout(SymmetricQueue *sqp, uint8_t *bp,
                       size_t n, systime_t timeout)
{
    size_t r = 0;

    chDbgCheck(n > 0, "chSymQReadTimeout");

    chSysLock();
    while (TRUE)
    {
        while (chSymQIsEmptyI(sqp))
        {
            if (qwait((SymmetricQueue*)sqp, timeout) != Q_OK)
            {
                chSysUnlock();
                return r;
            }
        }

        sqp->q_counter--;
        *bp++ = *sqp->q_rdptr++;
        if (sqp->q_rdptr >= sqp->q_top)
            sqp->q_rdptr = sqp->q_buffer;

        if (notempty(&sqp->q_waiting))
            chSchReadyI(fifo_remove(&sqp->q_waiting))->p_u.rdymsg = Q_OK;

        chSysUnlock(); /* Gives a preemption chance in a controlled point.*/
        r++;
        if (--n == 0)
            return r;

        chSysLock();
    }
}

/**
 * @brief   Output queue write with timeout.
 * @details This function writes a byte value to an output queue. If the queue
 *          is full then the calling thread is suspended until there is space
 *          in the queue or a timeout occurs.
 *
 * @param[in] sqp       pointer to an @p SymmetricQueue structure
 * @param[in] b         the byte value to be written in the queue
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval Q_OK         if the operation succeeded.
 * @retval Q_TIMEOUT    if the specified time expired.
 * @retval Q_RESET      if the queue has been reset.
 *
 * @api
 */
msg_t chSymQPutTimeout(SymmetricQueue *sqp, uint8_t b, systime_t timeout)
{
    chSysLock();
    while (chSymQIsFullI(sqp))
    {
        msg_t msg;

        if ((msg = qwait((SymmetricQueue*)sqp, timeout)) < Q_OK)
        {
            chSysUnlock();
            return msg;
        }
    }

    sqp->q_counter++;
    *sqp->q_wrptr++ = b;
    if (sqp->q_wrptr >= sqp->q_top)
        sqp->q_wrptr = sqp->q_buffer;

    if (notempty(&sqp->q_waiting))
        chSchReadyI(fifo_remove(&sqp->q_waiting))->p_u.rdymsg = Q_OK;

    chSysUnlock();
    return Q_OK;
}

/**
 * @brief   Output queue write with timeout.
 * @details The function writes data from a buffer to an output queue. The
 *          operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 * @note    The function is not atomic, if you need atomicity it is suggested
 *          to use a semaphore or a mutex for mutual exclusion.
 *
 * @param[in] sqp       pointer to an @p SymmetricQueue structure
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 *
 * @api
 */
size_t chSymQWriteTimeout(SymmetricQueue *sqp, const uint8_t *bp,
                        size_t n, systime_t timeout)
{
    size_t w = 0;

    chDbgCheck(n > 0, "chSymQWriteTimeout");

    chSysLock();
    while (TRUE)
    {
        while (chSymQIsFullI(sqp))
        {
            if (qwait((SymmetricQueue*)sqp, timeout) != Q_OK)
            {
                chSysUnlock();
                return w;
            }
        }

        sqp->q_counter++;
        *sqp->q_wrptr++ = *bp++;
        if (sqp->q_wrptr >= sqp->q_top)
            sqp->q_wrptr = sqp->q_buffer;

        if (notempty(&sqp->q_waiting))
            chSchReadyI(fifo_remove(&sqp->q_waiting))->p_u.rdymsg = Q_OK;

        chSysUnlock(); /* Gives a preemption chance in a controlled point.*/
        w++;
        if (--n == 0)
            return w;
        chSysLock();
    }
}

#endif  /* CH_USE_QUEUES */

/** @} */