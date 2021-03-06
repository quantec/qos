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
 * @file    qnvm_memory.h
 * @brief   NVM emulation through memory block.
 *
 * @addtogroup NVM_MEMORY
 * @{
 */

#ifndef _QNVM_MEMORY_H_
#define _QNVM_MEMORY_H_

#if HAL_USE_NVM_MEMORY || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    NVM_MEMORY configuration options
 * @{
 */
/**
 * @brief   Enables the @p nvmmemoryAcquireBus() and
 *          @p nvmmemoryReleaseBus() APIs.
 * @note    Disabling this option saves both code and data space.
 */
#if !defined(NVM_MEMORY_USE_MUTUAL_EXCLUSION) || defined(__DOXYGEN__)
#define NVM_MEMORY_USE_MUTUAL_EXCLUSION       TRUE
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Non volatile memory emulation driver configuration structure.
 */
typedef struct
{
    /**
    * @brief Pointer to memory block.
    */
    uint8_t* memoryp;
    /**
     * @brief Smallest erasable sector size in bytes.
     */
    uint32_t sector_size;
    /**
     * @brief Total number of sectors.
     */
    uint32_t sector_num;
} NVMMemoryConfig;

/**
 * @brief   @p NVMMemoryDriver specific methods.
 */
#define _nvm_memory_driver_methods                                            \
    _base_nvm_device_methods

/**
 * @extends BaseNVMDeviceVMT
 *
 * @brief   @p NVMMemoryDriver virtual methods table.
 */
struct NVMMemoryDriverVMT
{
    _nvm_memory_driver_methods
};

/**
 * @extends BaseNVMDevice
 *
 * @brief   Structure representing a NVM memory driver.
 */
typedef struct
{
    /**
    * @brief Virtual Methods Table.
    */
    const struct NVMMemoryDriverVMT* vmt;
    _base_nvm_device_data
    /**
    * @brief Current configuration data.
    */
    const NVMMemoryConfig* config;
#if NVM_MEMORY_USE_MUTUAL_EXCLUSION || defined(__DOXYGEN__)
    /**
     * @brief mutex_t protecting the device.
     */
    mutex_t mutex;
#endif /* NVM_MEMORY_USE_MUTUAL_EXCLUSION */
} NVMMemoryDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
    void nvmmemoryInit(void);
    void nvmmemoryObjectInit(NVMMemoryDriver* nvmmemoryp);
    void nvmmemoryStart(NVMMemoryDriver* nvmmemoryp,
            const NVMMemoryConfig* config);
    void nvmmemoryStop(NVMMemoryDriver* nvmmemoryp);
    bool nvmmemoryRead(NVMMemoryDriver* nvmmemoryp, uint32_t startaddr,
            uint32_t n, uint8_t* buffer);
    bool nvmmemoryWrite(NVMMemoryDriver* nvmmemoryp, uint32_t startaddr,
            uint32_t n, const uint8_t* buffer);
    bool nvmmemoryErase(NVMMemoryDriver* nvmmemoryp, uint32_t startaddr,
            uint32_t n);
    bool nvmmemoryMassErase(NVMMemoryDriver* nvmmemoryp);
    bool nvmmemorySync(NVMMemoryDriver* nvmmemoryp);
    bool nvmmemoryGetInfo(NVMMemoryDriver* nvmmemoryp,
            NVMDeviceInfo* nvmdip);
    void nvmmemoryAcquireBus(NVMMemoryDriver* nvmmemoryp);
    void nvmmemoryReleaseBus(NVMMemoryDriver* nvmmemoryp);
    bool nvmmemoryWriteProtect(NVMMemoryDriver* nvmmemoryp,
            uint32_t startaddr, uint32_t n);
    bool nvmmemoryMassWriteProtect(NVMMemoryDriver* nvmmemoryp);
    bool nvmmemoryWriteUnprotect(NVMMemoryDriver* nvmmemoryp,
            uint32_t startaddr, uint32_t n);
    bool nvmmemoryMassWriteUnprotect(NVMMemoryDriver* nvmmemoryp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_NVM_MEMORY */

#endif /* _QNVM_MEMORY_H_ */

/** @} */
