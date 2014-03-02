/**
 * @file    Posix/qhal_lld.h
 * @brief   Posix HAL subsystem low level driver header.
 *
 * @addtogroup QHAL
 * @{
 */

#ifndef _QHAL_LLD_H_
#define _QHAL_LLD_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Platform capabilities.                                                    */
/*===========================================================================*/

/*===========================================================================*/
/* Platform specific friendly IRQ names.                                     */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void qhal_lld_init(void);
#ifdef __cplusplus
}
#endif

#endif /* _QHAL_LLD_H_ */

/** @} */
