/*
 * nvm_tools.c
 *
 *  Created on: 18.01.2014
 *      Author: vke
 */

#include "nvm_tools.h"

/**
 * @brief   Compares content of two BaseNVMDevice objects.
 *
 * @param[in] devap     pointer to the first @p BaseNVMDevice object
 * @param[in] devb      pointer to the second @p BaseNVMDevice object
 * @param[in] n         number of bytes to compare
 *
 * @return              The result of the comparison.
 * @retval true         The compared data is equal.
 * @retval false        The compared data is not equal or an error occurred.
 *
 * @api
 */
int nvmcmp(BaseNVMDevice* devap, BaseNVMDevice* devbp, uint32_t n)
{
    for (uint32_t i = 0; i < n; ++i)
    {
        uint8_t bytea;
        if (nvmRead(devap, i, 1, &bytea) != CH_SUCCESS)
            return -1;
        uint8_t byteb;
        if (nvmRead(devbp, i, 1, &byteb) != CH_SUCCESS)
            return -1;

        if (bytea != byteb)
            return 1;
    }
    return 0;
}

/**
 * @brief   Copies data between BaseNVMDevice objects.
 *
 * @param[in] dstp      pointer to the first @p BaseNVMDevice object
 * @param[in] srcp      pointer to the second @p BaseNVMDevice object
 * @param[in] n         number of bytes to copy
 *
 * @return              The result of the operation.
 * @retval true         The operation was successful.
 * @retval false        An error occurred.
 *
 * @api
 */
bool nvmcpy(BaseNVMDevice* dstp, BaseNVMDevice* srcp, uint32_t n)
{
    NVMDeviceInfo di;

    if (nvmGetInfo(dstp, &di) != CH_SUCCESS)
        return false;

    chDbgAssert(di.write_alignment <= 4, "nvmcpy(), #1",
            "unsupported write_alignment");

    if (di.write_alignment == 0)
        di.write_alignment = 1;

    for (uint32_t i = 0; i < n; i += di.write_alignment)
    {
        uint8_t temp[4] = { 0xff, 0xff, 0xff, 0xff };

        uint8_t chunk_n = di.write_alignment;
        if (n - i < di.write_alignment)
            chunk_n = n - i;

        if (nvmRead(srcp, i, chunk_n, temp) != CH_SUCCESS)
            return false;

        /* Note: Possibly remaining bytes are filled with 0xff. */
        if (nvmWrite(dstp, i, di.write_alignment, temp) != CH_SUCCESS)
            return false;
    }
    return true;
}

/**
 * @brief   Sets data of a BaseNVMDevice object to a desired pattern.
 *
 * @param[in] dstp      pointer to a @p BaseNVMDevice object
 * @param[in] pattern   pattern to set memory to
 * @param[in] n         number of bytes to set
 *
 * @return              The result of the operation.
 * @retval true         The operation was successful.
 * @retval false        An error occurred.
 *
 * @api
 */
bool nvmset(BaseNVMDevice* dstp, uint8_t pattern, uint32_t n)
{
    NVMDeviceInfo di;

    if (nvmGetInfo(dstp, &di) == CH_SUCCESS)
        return false;

    chDbgAssert(di.write_alignment <= 4, "nvmset(), #1",
            "unsupported write_alignment");

    const uint8_t temp[4] = { pattern, pattern, pattern, pattern };

    for (uint32_t i = 0; i < n; i += di.write_alignment)
    {
        if (nvmWrite(dstp, i, di.write_alignment, temp) != CH_SUCCESS)
            return false;
    }
    return true;
}