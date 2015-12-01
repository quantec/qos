/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    templates/chcore.h
 * @brief   Port related template macros and structures.
 * @details This file is a template of the system driver macros provided by
 *          a port.
 *
 * @addtogroup core
 * @{
 */

#ifndef _CHCORE_H_
#define _CHCORE_H_

#include "st_lld.h"

#include <ucontext.h>

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @name    Architecture and Compiler
 * @{
 */
/**
 * Macro defining a simulated architecture into x86.
 */
#define PORT_ARCHITECTURE_SIMIA32

/**
 * @brief   Name of the implemented architecture.
 */
#define PORT_ARCHITECTURE_NAME          "SIMIA32 Architecture"

/**
 * @brief   Compiler name and version.
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
#define PORT_COMPILER_NAME              "GCC " __VERSION__

#else
#error "unsupported compiler"
#endif

/**
 * @brief   This port supports a realtime counter.
 */
#define PORT_SUPPORTS_RT                FALSE

/**
 * @brief   Port-specific information string.
 */
#if (TIMER_TYPE == ITIMER_REAL) || defined(__DOXYGEN__)
#define CH_PORT_INFO                    "Preemption through ITIMER_REAL"
#elif (TIMER_TYPE == ITIMER_VIRTUAL) || defined(__DOXYGEN__)
#define CH_PORT_INFO                    "Preemption through ITIMER_VIRTUAL"
#elif (TIMER_TYPE == ITIMER_PROF) || defined(__DOXYGEN__)
#define CH_PORT_INFO                    "Preemption through ITIMER_PROF"
#endif
/** @} */

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Stack size for the system idle thread.
 * @details This size depends on the idle thread implementation, usually
 *          the idle thread should take no more space than those reserved
 *          by @p PORT_INT_REQUIRED_STACK.
 */
#if !defined(PORT_IDLE_THREAD_STACK_SIZE) || defined(__DOXYGEN__)
#define PORT_IDLE_THREAD_STACK_SIZE     256
#endif

/**
 * @brief   Per-thread stack overhead for interrupts servicing.
 * @details This constant is used in the calculation of the correct working
 *          area size.
 */
#if !defined(PORT_INT_REQUIRED_STACK) || defined(__DOXYGEN__)
#define PORT_INT_REQUIRED_STACK         16384
#endif

/**
 * @brief   Enables an alternative timer implementation.
 * @details Usually the port uses a timer interface defined in the file
 *          @p chcore_timer.h, if this option is enabled then the file
 *          @p chcore_timer_alt.h is included instead.
 */
#if !defined(PORT_USE_ALT_TIMER)
#define PORT_USE_ALT_TIMER              FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if CH_DBG_ENABLE_STACK_CHECK
#error "option CH_DBG_ENABLE_STACK_CHECK not supported by this port"
#endif

#if !defined(_GNU_SOURCE)
#error "this port requires you to add -D_GNU_SOURCE to your CFLAGS"
#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Type of stack and memory alignment enforcement.
 * @note    In this architecture the stack alignment is enforced to 64 bits.
 */
typedef struct {
  uint8_t a[16];
} stkalign_t __attribute__((aligned(16)));

/**
 * @brief   Interrupt saved context.
 * @details This structure represents the stack frame saved during a
 *          preemption-capable interrupt handler.
 */
struct port_extctx {
};

/**
 * @brief   System saved context.
 * @details This structure represents the inner stack frame during a context
 *          switching.
 */
struct port_intctx {
  ucontext_t uc;
};

/**
 * @brief   Platform dependent part of the @p thread_t structure.
 * @details This structure usually contains just the saved stack pointer
 *          defined as a pointer to a @p port_intctx structure.
 */
struct context {
  ucontext_t uc;
};

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Platform dependent part of the @p chThdCreateI() API.
 * @details This code usually setup the context switching frame represented
 *          by an @p port_intctx structure.
 */
#define PORT_SETUP_CONTEXT(tp, workspace, wsize, pf, arg) {             \
  if (getcontext(&tp->p_ctx.uc) < 0)                                    \
    chSysHalt("getcontext() failed");                                   \
  tp->p_ctx.uc.uc_stack.ss_sp = workspace;                              \
  tp->p_ctx.uc.uc_stack.ss_size = wsize;                                \
  tp->p_ctx.uc.uc_stack.ss_flags = 0;                                   \
  tp->p_ctx.uc.uc_mcontext.gregs[REG_ECX] = (uint32_t)pf;               \
  tp->p_ctx.uc.uc_mcontext.gregs[REG_EDX] = (uint32_t)arg;              \
  makecontext(&tp->p_ctx.uc, (void(*)(void))_port_thread_start, 0);     \
}

/**
 * @brief   Computes the thread working area global size.
 * @note    There is no need to perform alignments in this macro.
 */
#define PORT_WA_SIZE(n) (sizeof(struct port_intctx) +                       \
                         sizeof(struct port_extctx) +                       \
                         ((size_t)(n)) + ((size_t)(PORT_INT_REQUIRED_STACK)))

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_PRIORITY(n) false

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_KERNEL_PRIORITY(n) false

/**
 * @brief   IRQ prologue code.
 * @details This macro must be inserted at the start of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_PROLOGUE() {                                               \
  port_isr_context_flag = true;                                             \
}

/**
 * @brief   IRQ epilogue code.
 * @details This macro must be inserted at the end of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_EPILOGUE() {                                               \
  port_isr_context_flag = false;                                            \
  return chSchIsPreemptionRequired();                                       \
}


/**
 * @brief   IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#define PORT_IRQ_HANDLER(id) bool id(void)

/**
 * @brief   Fast IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#define PORT_FAST_IRQ_HANDLER(id) void id(void)

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 */
#if !CH_DBG_ENABLE_STACK_CHECK || defined(__DOXYGEN__)
#define port_switch(ntp, otp) _port_switch(ntp, otp)
#else
#define port_switch(ntp, otp) {                                             \
  register struct port_intctx *sp asm ("%r1");                              \
  if ((stkalign_t *)(sp - 1) < otp->p_stklimit)                             \
    chSysHalt("stack overflow");                                            \
  _port_switch(ntp, otp);                                                   \
}
#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern bool port_isr_context_flag;
extern syssts_t port_irq_sts;

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

#ifdef __cplusplus
extern "C" {
#endif
  /* Import from chsys.c */
  void chSysHalt(const char *reason);
  /* Imports from chcore.c */
  void _port_switch(thread_t *ntp, thread_t *otp);
  void _port_thread_start(void);
#ifdef __cplusplus
}
#endif

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Port-related initialization code.
 */
static inline void port_init(void) {

  port_irq_sts = (syssts_t)0;
  port_isr_context_flag = false;
}

/**
 * @brief   Returns a word encoding the current interrupts status.
 *
 * @return              The interrupts status.
 */
static inline syssts_t port_get_irq_status(void) {

  return 0;
}

/**
 * @brief   Checks the interrupt status.
 *
 * @param[in] sts       the interrupt status word
 *
 * @return              The interrupt status.
 * @retval false        the word specified a disabled interrupts status.
 * @retval true         the word specified an enabled interrupts status.
 */
static inline bool port_irq_enabled(syssts_t sts) {

  return sts == (syssts_t)0;
}

/**
 * @brief   Determines the current execution context.
 *
 * @return              The execution context.
 * @retval false        not running in ISR mode.
 * @retval true         running in ISR mode.
 */
static inline bool port_is_isr_context(void) {

  return port_isr_context_flag;
}

/**
 * @brief   Kernel-lock action.
 * @details Usually this function just disables interrupts but may perform more
 *          actions.
 */
static inline void port_lock(void) {
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  if (sigprocmask(SIG_BLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
  port_irq_sts = (syssts_t)1;
}

/**
 * @brief   Kernel-unlock action.
 * @details Usually this function just enables interrupts but may perform more
 *          actions.
 */
static inline void port_unlock(void) {
  port_irq_sts = (syssts_t)0;
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  if (sigprocmask(SIG_UNBLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
}

/**
 * @brief   Kernel-lock action from an interrupt handler.
 * @details This function is invoked before invoking I-class APIs from
 *          interrupt handlers. The implementation is architecture dependent,
 *          in its simplest form it is void.
 */
static inline void port_lock_from_isr(void) {
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  /* The timer signal is masked on entry of systick automatically. */
  if (sigdelset(&set, PORT_TIMER_SIGNAL))
    chSysHalt("sigdelset() failed");
  if (sigprocmask(SIG_BLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
  port_irq_sts = (syssts_t)1;
}

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 * @details This function is invoked after invoking I-class APIs from interrupt
 *          handlers. The implementation is architecture dependent, in its
 *          simplest form it is void.
 */
static inline void port_unlock_from_isr(void) {
  port_irq_sts = (syssts_t)0;
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  /* The timer signal is unmasked on exit of systick automatically. */
  if (sigdelset(&set, PORT_TIMER_SIGNAL))
    chSysHalt("sigdelset() failed");
  if (sigprocmask(SIG_UNBLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
}

/**
 * @brief   Disables all the interrupt sources.
 * @note    Of course non-maskable interrupt sources are not included.
 */
static inline void port_disable(void) {
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  if (sigprocmask(SIG_BLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
  port_irq_sts = (syssts_t)1;
}

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 * @note    Interrupt sources above kernel level remains enabled.
 */
static inline void port_suspend(void) {
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  if (sigprocmask(SIG_BLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
  port_irq_sts = (syssts_t)1;
}

/**
 * @brief   Enables all the interrupt sources.
 */
static inline void port_enable(void) {
  port_irq_sts = (syssts_t)0;
  sigset_t set;
  if (sigfillset(&set) < 0)
    chSysHalt("sigfillset() failed");
  if (sigprocmask(SIG_UNBLOCK, &set, NULL) > 0)
    chSysHalt("sigprocmask() failed");
}

/**
 * @brief   Enters an architecture-dependent IRQ-waiting mode.
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 */
static inline void port_wait_for_interrupt(void) {

}

/**
 * @brief   Returns the current value of the realtime counter.
 *
 * @return              The realtime counter value.
 */
static inline rtcnt_t port_rt_get_counter_value(void) {

  return 0;
}

/*===========================================================================*/
/* Module late inclusions.                                                   */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

#if CH_CFG_ST_TIMEDELTA > 0
#if !PORT_USE_ALT_TIMER
#include "chcore_timer.h"
#else /* PORT_USE_ALT_TIMER */
#include "chcore_timer_alt.h"
#endif /* PORT_USE_ALT_TIMER */
#endif /* CH_CFG_ST_TIMEDELTA > 0 */

#endif /* !defined(_FROM_ASM_) */

#endif /* _CHCORE_H_ */

/** @} */
