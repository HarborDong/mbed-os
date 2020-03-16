/*
 * Copyright (c) 2016-2020 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_MPC.h"

#include "mpc_sie_drv.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

/* Driver version */
#define ARM_MPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_MPC_API_VERSION,
    ARM_MPC_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_MPC_GetVersion(void)
{
    return DriverVersion;
}

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref mpc_sie_error_t).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(enum mpc_sie_error_t err)
{
    switch(err) {
    case MPC_SIE_ERR_NONE:
        return ARM_DRIVER_OK;
    case MPC_SIE_INVALID_ARG:
        return ARM_DRIVER_ERROR_PARAMETER;
    case MPC_SIE_NOT_INIT:
        return ARM_MPC_ERR_NOT_INIT;
    case MPC_SIE_ERR_NOT_IN_RANGE:
        return ARM_MPC_ERR_NOT_IN_RANGE;
    case MPC_SIE_ERR_NOT_ALIGNED:
        return ARM_MPC_ERR_NOT_ALIGNED;
    case MPC_SIE_ERR_INVALID_RANGE:
        return ARM_MPC_ERR_INVALID_RANGE;
    case MPC_SIE_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE:
        return ARM_MPC_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    default:
        return ARM_MPC_ERR_UNSPECIFIED;
    }
}

#if (RTE_ISRAM0_MPC)
/* Ranges controlled by this ISRAM0_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_S = {
    .base         = MPC_ISRAM0_RANGE_BASE_S,
    .limit        = MPC_ISRAM0_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_NS = {
    .base         = MPC_ISRAM0_RANGE_BASE_NS,
    .limit        = MPC_ISRAM0_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM0_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM0_RANGE_LIST[MPC_ISRAM0_RANGE_LIST_LEN] = {
        &MPC_ISRAM0_RANGE_S,
        &MPC_ISRAM0_RANGE_NS
    };

/* ISRAM0_MPC Driver wrapper functions */
static int32_t ISRAM0_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_ISRAM0_DEV,
                       MPC_ISRAM0_RANGE_LIST,
                       MPC_ISRAM0_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM0_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_ISRAM0_DEV, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_ISRAM0_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_ISRAM0_DEV, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_ISRAM0_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_ISRAM0_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_ISRAM0_DEV);

    return error_trans(ret);
}

static void ISRAM0_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_ISRAM0_DEV);
}


static void ISRAM0_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_ISRAM0_DEV);
}

static uint32_t ISRAM0_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_ISRAM0_DEV);
}

static int32_t ISRAM0_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_ISRAM0_DEV);
}

/* ISRAM0_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_ISRAM0_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM0_MPC_Initialize,
    .Uninitialize     = ISRAM0_MPC_Uninitialize,
    .GetBlockSize     = ISRAM0_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM0_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM0_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM0_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM0_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM0_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM0_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM0_MPC_ClearInterrupt,
    .InterruptState   = ISRAM0_MPC_InterruptState,
    .LockDown         = ISRAM0_MPC_LockDown,
};
#endif /* RTE_ISRAM0_MPC */

#if (RTE_ISRAM1_MPC)
/* Ranges controlled by this ISRAM1_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_S = {
    .base         = MPC_ISRAM1_RANGE_BASE_S,
    .limit        = MPC_ISRAM1_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_NS = {
    .base         = MPC_ISRAM1_RANGE_BASE_NS,
    .limit        = MPC_ISRAM1_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM1_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM1_RANGE_LIST[MPC_ISRAM1_RANGE_LIST_LEN] = {
        &MPC_ISRAM1_RANGE_S,
        &MPC_ISRAM1_RANGE_NS
    };

/* ISRAM1_MPC Driver wrapper functions */
static int32_t ISRAM1_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_ISRAM1_DEV,
                       MPC_ISRAM1_RANGE_LIST,
                       MPC_ISRAM1_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM1_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_ISRAM1_DEV, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_ISRAM1_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_ISRAM1_DEV, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_ISRAM1_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_ISRAM1_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_ISRAM1_DEV);

    return error_trans(ret);
}

static void ISRAM1_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_ISRAM1_DEV);
}


static void ISRAM1_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_ISRAM1_DEV);
}

static uint32_t ISRAM1_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_ISRAM1_DEV);
}

static int32_t ISRAM1_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_ISRAM1_DEV);
}

/* ISRAM1_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_ISRAM1_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM1_MPC_Initialize,
    .Uninitialize     = ISRAM1_MPC_Uninitialize,
    .GetBlockSize     = ISRAM1_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM1_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM1_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM1_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM1_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM1_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM1_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM1_MPC_ClearInterrupt,
    .InterruptState   = ISRAM1_MPC_InterruptState,
    .LockDown         = ISRAM1_MPC_LockDown,
};
#endif /* RTE_ISRAM1_MPC */

#if (RTE_ISRAM2_MPC)
/* Ranges controlled by this ISRAM2_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM2_RANGE_S = {
    .base         = MPC_ISRAM2_RANGE_BASE_S,
    .limit        = MPC_ISRAM2_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM2_RANGE_NS = {
    .base         = MPC_ISRAM2_RANGE_BASE_NS,
    .limit        = MPC_ISRAM2_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM2_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM2_RANGE_LIST[MPC_ISRAM2_RANGE_LIST_LEN] = {
        &MPC_ISRAM2_RANGE_S,
        &MPC_ISRAM2_RANGE_NS
    };

/* ISRAM2_MPC Driver wrapper functions */
static int32_t ISRAM2_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_ISRAM2_DEV,
                       MPC_ISRAM2_RANGE_LIST,
                       MPC_ISRAM2_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM2_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_ISRAM2_DEV, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_ISRAM2_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_ISRAM2_DEV, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_ISRAM2_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_ISRAM2_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_ISRAM2_DEV);

    return error_trans(ret);
}

static void ISRAM2_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_ISRAM2_DEV);
}


static void ISRAM2_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_ISRAM2_DEV);
}

static uint32_t ISRAM2_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_ISRAM2_DEV);
}

static int32_t ISRAM2_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_ISRAM2_DEV);
}

/* ISRAM2_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_ISRAM2_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM2_MPC_Initialize,
    .Uninitialize     = ISRAM2_MPC_Uninitialize,
    .GetBlockSize     = ISRAM2_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM2_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM2_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM2_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM2_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM2_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM2_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM2_MPC_ClearInterrupt,
    .InterruptState   = ISRAM2_MPC_InterruptState,
    .LockDown         = ISRAM2_MPC_LockDown,
};
#endif /* RTE_ISRAM2_MPC */

#if (RTE_ISRAM3_MPC)
/* Ranges controlled by this ISRAM3_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM3_RANGE_S = {
    .base         = MPC_ISRAM3_RANGE_BASE_S,
    .limit        = MPC_ISRAM3_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM3_RANGE_NS = {
    .base         = MPC_ISRAM3_RANGE_BASE_NS,
    .limit        = MPC_ISRAM3_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM3_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM3_RANGE_LIST[MPC_ISRAM3_RANGE_LIST_LEN] = {
        &MPC_ISRAM3_RANGE_S,
        &MPC_ISRAM3_RANGE_NS
    };

/* ISRAM3_MPC Driver wrapper functions */
static int32_t ISRAM3_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_ISRAM3_DEV,
                       MPC_ISRAM3_RANGE_LIST,
                       MPC_ISRAM3_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM3_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_ISRAM3_DEV, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_ISRAM3_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_ISRAM3_DEV, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_ISRAM3_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_ISRAM3_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_ISRAM3_DEV);

    return error_trans(ret);
}

static void ISRAM3_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_ISRAM3_DEV);
}


static void ISRAM3_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_ISRAM3_DEV);
}

static uint32_t ISRAM3_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_ISRAM3_DEV);
}

static int32_t ISRAM3_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_ISRAM3_DEV);
}

/* ISRAM3_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_ISRAM3_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM3_MPC_Initialize,
    .Uninitialize     = ISRAM3_MPC_Uninitialize,
    .GetBlockSize     = ISRAM3_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM3_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM3_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM3_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM3_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM3_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM3_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM3_MPC_ClearInterrupt,
    .InterruptState   = ISRAM3_MPC_InterruptState,
    .LockDown         = ISRAM3_MPC_LockDown,
};
#endif /* RTE_ISRAM3_MPC */

#if (RTE_CODE_SRAM_MPC)
/* Ranges controlled by this CODE_SRAM_MPC */
static const struct mpc_sie_memory_range_t MPC_CODE_SRAM_RANGE_S = {
    .base         = MPC_CODE_SRAM_RANGE_BASE_S,
    .limit        = MPC_CODE_SRAM_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_CODE_SRAM_RANGE_NS = {
    .base         = MPC_CODE_SRAM_RANGE_BASE_NS,
    .limit        = MPC_CODE_SRAM_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_CODE_SRAM_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_CODE_SRAM_RANGE_LIST[MPC_CODE_SRAM_RANGE_LIST_LEN] = {
        &MPC_CODE_SRAM_RANGE_S,
        &MPC_CODE_SRAM_RANGE_NS
    };

/* CODE_SRAM_MPC Driver wrapper functions */
static int32_t CODE_SRAM_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_CODE_SRAM_DEV,
                       MPC_CODE_SRAM_RANGE_LIST,
                       MPC_CODE_SRAM_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t CODE_SRAM_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_CODE_SRAM_DEV, blk_size);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_CODE_SRAM_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_CODE_SRAM_DEV, ctrl);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_CODE_SRAM_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_CODE_SRAM_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t CODE_SRAM_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_CODE_SRAM_DEV);

    return error_trans(ret);
}

static void CODE_SRAM_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_CODE_SRAM_DEV);
}


static void CODE_SRAM_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_CODE_SRAM_DEV);
}

static uint32_t CODE_SRAM_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_CODE_SRAM_DEV);
}

static int32_t CODE_SRAM_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_CODE_SRAM_DEV);
}

/* CODE_SRAM_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_CODE_SRAM_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = CODE_SRAM_MPC_Initialize,
    .Uninitialize     = CODE_SRAM_MPC_Uninitialize,
    .GetBlockSize     = CODE_SRAM_MPC_GetBlockSize,
    .GetCtrlConfig    = CODE_SRAM_MPC_GetCtrlConfig,
    .SetCtrlConfig    = CODE_SRAM_MPC_SetCtrlConfig,
    .ConfigRegion     = CODE_SRAM_MPC_ConfigRegion,
    .GetRegionConfig  = CODE_SRAM_MPC_GetRegionConfig,
    .EnableInterrupt  = CODE_SRAM_MPC_EnableInterrupt,
    .DisableInterrupt = CODE_SRAM_MPC_DisableInterrupt,
    .ClearInterrupt   = CODE_SRAM_MPC_ClearInterrupt,
    .InterruptState   = CODE_SRAM_MPC_InterruptState,
    .LockDown         = CODE_SRAM_MPC_LockDown,
};
#endif /* RTE_CODE_SRAM_MPC */

#if (RTE_QSPI_MPC)
/* Ranges controlled by this QSPI_MPC */
static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_S = {
    .base         = MPC_QSPI_RANGE_BASE_S,
    .limit        = MPC_QSPI_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_NS = {
    .base         = MPC_QSPI_RANGE_BASE_NS,
    .limit        = MPC_QSPI_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_QSPI_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_QSPI_RANGE_LIST[MPC_QSPI_RANGE_LIST_LEN] = {
        &MPC_QSPI_RANGE_S,
        &MPC_QSPI_RANGE_NS
    };

/* QSPI_MPC Driver wrapper functions */
static int32_t QSPI_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_QSPI_DEV,
                       MPC_QSPI_RANGE_LIST,
                       MPC_QSPI_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t QSPI_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t QSPI_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_QSPI_DEV, blk_size);

    return error_trans(ret);
}

static int32_t QSPI_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_QSPI_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t QSPI_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_QSPI_DEV, ctrl);

    return error_trans(ret);
}

static int32_t QSPI_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_QSPI_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t QSPI_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_QSPI_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t QSPI_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_QSPI_DEV);

    return error_trans(ret);
}

static void QSPI_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_QSPI_DEV);
}


static void QSPI_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_QSPI_DEV);
}

static uint32_t QSPI_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_QSPI_DEV);
}

static int32_t QSPI_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_QSPI_DEV);
}

/* QSPI_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_QSPI_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = QSPI_MPC_Initialize,
    .Uninitialize     = QSPI_MPC_Uninitialize,
    .GetBlockSize     = QSPI_MPC_GetBlockSize,
    .GetCtrlConfig    = QSPI_MPC_GetCtrlConfig,
    .SetCtrlConfig    = QSPI_MPC_SetCtrlConfig,
    .ConfigRegion     = QSPI_MPC_ConfigRegion,
    .GetRegionConfig  = QSPI_MPC_GetRegionConfig,
    .EnableInterrupt  = QSPI_MPC_EnableInterrupt,
    .DisableInterrupt = QSPI_MPC_DisableInterrupt,
    .ClearInterrupt   = QSPI_MPC_ClearInterrupt,
    .InterruptState   = QSPI_MPC_InterruptState,
    .LockDown         = QSPI_MPC_LockDown,
};
#endif /* RTE_QSPI_MPC */

#if (RTE_EFLASH0_MPC)
/* Ranges controlled by this EFLASH0_MPC */
static const struct mpc_sie_memory_range_t MPC_EFLASH0_RANGE_S = {
    .base         = MPC_EFLASH0_RANGE_BASE_S,
    .limit        = MPC_EFLASH0_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_EFLASH0_RANGE_NS = {
    .base         = MPC_EFLASH0_RANGE_BASE_NS,
    .limit        = MPC_EFLASH0_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_EFLASH0_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_EFLASH0_RANGE_LIST[MPC_EFLASH0_RANGE_LIST_LEN] = {
        &MPC_EFLASH0_RANGE_S,
        &MPC_EFLASH0_RANGE_NS
    };

/* EFLASH0_MPC Driver wrapper functions */
static int32_t EFLASH0_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_EFLASH0_DEV,
                       MPC_EFLASH0_RANGE_LIST,
                       MPC_EFLASH0_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t EFLASH0_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_EFLASH0_DEV, blk_size);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_EFLASH0_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_EFLASH0_DEV, ctrl);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_EFLASH0_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_EFLASH0_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t EFLASH0_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_EFLASH0_DEV);

    return error_trans(ret);
}

static void EFLASH0_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_EFLASH0_DEV);
}


static void EFLASH0_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_EFLASH0_DEV);
}

static uint32_t EFLASH0_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_EFLASH0_DEV);
}

static int32_t EFLASH0_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_EFLASH0_DEV);
}

/* EFLASH0_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_EFLASH0_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = EFLASH0_MPC_Initialize,
    .Uninitialize     = EFLASH0_MPC_Uninitialize,
    .GetBlockSize     = EFLASH0_MPC_GetBlockSize,
    .GetCtrlConfig    = EFLASH0_MPC_GetCtrlConfig,
    .SetCtrlConfig    = EFLASH0_MPC_SetCtrlConfig,
    .ConfigRegion     = EFLASH0_MPC_ConfigRegion,
    .GetRegionConfig  = EFLASH0_MPC_GetRegionConfig,
    .EnableInterrupt  = EFLASH0_MPC_EnableInterrupt,
    .DisableInterrupt = EFLASH0_MPC_DisableInterrupt,
    .ClearInterrupt   = EFLASH0_MPC_ClearInterrupt,
    .InterruptState   = EFLASH0_MPC_InterruptState,
    .LockDown         = EFLASH0_MPC_LockDown,
};
#endif /* RTE_EFLASH0_MPC */

#if (RTE_EFLASH1_MPC)
/* Ranges controlled by this EFLASH1_MPC */
static const struct mpc_sie_memory_range_t MPC_EFLASH1_RANGE_S = {
    .base         = MPC_EFLASH1_RANGE_BASE_S,
    .limit        = MPC_EFLASH1_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_EFLASH1_RANGE_NS = {
    .base         = MPC_EFLASH1_RANGE_BASE_NS,
    .limit        = MPC_EFLASH1_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_EFLASH1_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_EFLASH1_RANGE_LIST[MPC_EFLASH1_RANGE_LIST_LEN] = {
        &MPC_EFLASH1_RANGE_S,
        &MPC_EFLASH1_RANGE_NS
    };

/* EFLASH1_MPC Driver wrapper functions */
static int32_t EFLASH1_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_EFLASH1_DEV,
                       MPC_EFLASH1_RANGE_LIST,
                       MPC_EFLASH1_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t EFLASH1_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_EFLASH1_DEV, blk_size);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_EFLASH1_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_EFLASH1_DEV, ctrl);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_EFLASH1_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_EFLASH1_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t EFLASH1_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_EFLASH1_DEV);

    return error_trans(ret);
}

static void EFLASH1_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_EFLASH1_DEV);
}


static void EFLASH1_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_EFLASH1_DEV);
}

static uint32_t EFLASH1_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_EFLASH1_DEV);
}

static int32_t EFLASH1_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_EFLASH1_DEV);
}

/* EFLASH1_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_EFLASH1_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = EFLASH1_MPC_Initialize,
    .Uninitialize     = EFLASH1_MPC_Uninitialize,
    .GetBlockSize     = EFLASH1_MPC_GetBlockSize,
    .GetCtrlConfig    = EFLASH1_MPC_GetCtrlConfig,
    .SetCtrlConfig    = EFLASH1_MPC_SetCtrlConfig,
    .ConfigRegion     = EFLASH1_MPC_ConfigRegion,
    .GetRegionConfig  = EFLASH1_MPC_GetRegionConfig,
    .EnableInterrupt  = EFLASH1_MPC_EnableInterrupt,
    .DisableInterrupt = EFLASH1_MPC_DisableInterrupt,
    .ClearInterrupt   = EFLASH1_MPC_ClearInterrupt,
    .InterruptState   = EFLASH1_MPC_InterruptState,
    .LockDown         = EFLASH1_MPC_LockDown,
};
#endif /* RTE_EFLASH1_MPC */
