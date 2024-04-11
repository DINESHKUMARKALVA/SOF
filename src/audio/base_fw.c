// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2024 Intel Corporation.
//

#include <sof/audio/component.h>
#include <sof/lib/memory.h>
#include <sof/ut.h>
#include <sof/tlv.h>
#include <ipc4/base_fw.h>
#include <ipc4/base_fw_platform.h>
#include <ipc4/pipeline.h>
#include <ipc4/logging.h>
#include <ipc/topology.h>
#include <sof_versions.h>
#include <sof/lib/cpu-clk-manager.h>
#include <sof/lib/cpu.h>
#include <sof/platform.h>
#include <sof/lib_manager.h>
#include <rtos/init.h>
#include <platform/lib/clk.h>
#include <sof/audio/module_adapter/module/generic.h>
#include <sof/schedule/dp_schedule.h>
#include <sof/schedule/ll_schedule.h>
#include <sof/debug/telemetry/telemetry.h>
/* FIXME:
 * Builds for some platforms like tgl fail because their defines related to memory windows are
 * already defined somewhere else. Remove this ifdef after it's cleaned up
 */
#ifdef CONFIG_SOF_TELEMETRY
#include "mem_window.h"
#include "adsp_debug_window.h"
#endif

#include <zephyr/logging/log_ctrl.h>

LOG_MODULE_REGISTER(basefw, CONFIG_SOF_LOG_LEVEL);

/* 0e398c32-5ade-ba4b-93b1-c50432280ee4 */
DECLARE_SOF_RT_UUID("basefw", basefw_comp_uuid, 0xe398c32, 0x5ade, 0xba4b,
		    0x93, 0xb1, 0xc5, 0x04, 0x32, 0x28, 0x0e, 0xe4);
DECLARE_TR_CTX(basefw_comp_tr, SOF_UUID(basefw_comp_uuid), LOG_LEVEL_INFO);

static struct ipc4_system_time_info global_system_time_info;
static uint64_t global_cycle_delta;

static int basefw_config(uint32_t *data_offset, char *data)
{
	uint16_t version[4] = {SOF_MAJOR, SOF_MINOR, SOF_MICRO, SOF_BUILD};
	struct sof_tlv *tuple = (struct sof_tlv *)data;
	struct ipc4_scheduler_config sche_cfg;
	uint32_t plat_data_offset = 0;
	uint32_t log_bytes_size = 0;

	tlv_value_set(tuple, IPC4_FW_VERSION_FW_CFG, sizeof(version), version);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MEMORY_RECLAIMED_FW_CFG, 1);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_FAST_CLOCK_FREQ_HZ_FW_CFG, CLK_MAX_CPU_HZ);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple,
			     IPC4_SLOW_CLOCK_FREQ_HZ_FW_CFG,
			     clock_get_freq(CPU_LOWEST_FREQ_IDX));

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_DL_MAILBOX_BYTES_FW_CFG, MAILBOX_HOSTBOX_SIZE);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_UL_MAILBOX_BYTES_FW_CFG, MAILBOX_DSPBOX_SIZE);

	/* TODO: add log support */
	tuple = tlv_next(tuple);
#ifdef CONFIG_LOG_BACKEND_ADSP_MTRACE
	log_bytes_size = SOF_IPC4_LOGGING_MTRACE_PAGE_SIZE;
#endif
	tlv_value_uint32_set(tuple, IPC4_TRACE_LOG_BYTES_FW_CFG, log_bytes_size);


	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_PPL_CNT_FW_CFG, IPC4_MAX_PPL_COUNT);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_ASTATE_COUNT_FW_CFG, IPC4_MAX_CLK_STATES);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_MODULE_PIN_COUNT_FW_CFG, IPC4_MAX_SRC_QUEUE);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_MOD_INST_COUNT_FW_CFG, IPC4_MAX_MODULE_INSTANCES);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_LL_TASKS_PER_PRI_COUNT_FW_CFG,
			     IPC4_MAX_LL_TASKS_PER_PRI_COUNT);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_LL_PRI_COUNT, SOF_IPC4_MAX_PIPELINE_PRIORITY + 1);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_DP_TASKS_COUNT_FW_CFG, IPC4_MAX_DP_TASKS_COUNT);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MODULES_COUNT_FW_CFG, 5);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MAX_LIBS_COUNT_FW_CFG, IPC4_MAX_LIBS_COUNT);

	tuple = tlv_next(tuple);
	sche_cfg.sys_tick_cfg_length = 0;
	sche_cfg.sys_tick_divider = 1;
	sche_cfg.sys_tick_multiplier = 1;
	sche_cfg.sys_tick_source = SOF_SCHEDULE_LL_TIMER;
	tlv_value_set(tuple, IPC4_SCHEDULER_CONFIGURATION, sizeof(sche_cfg), &sche_cfg);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_FW_CONTEXT_SAVE,
			     IS_ENABLED(CONFIG_ADSP_IMR_CONTEXT_SAVE));

	tuple = tlv_next(tuple);

	/* add platform specific tuples */
	platform_basefw_fw_config(&plat_data_offset, (char *)tuple);

	*data_offset = (int)((char *)tuple - data) + plat_data_offset;

	return 0;
}

static int basefw_hw_config(uint32_t *data_offset, char *data)
{
	struct sof_tlv *tuple = (struct sof_tlv *)data;
	uint32_t plat_data_offset = 0;
	uint32_t value;

	tlv_value_uint32_set(tuple, IPC4_CAVS_VER_HW_CFG, HW_CFG_VERSION);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_DSP_CORES_HW_CFG, CONFIG_CORE_COUNT);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_MEM_PAGE_BYTES_HW_CFG, HOST_PAGE_SIZE);

	tuple = tlv_next(tuple);
	tlv_value_uint32_set(tuple, IPC4_EBB_SIZE_BYTES_HW_CFG, SRAM_BANK_SIZE);

	tuple = tlv_next(tuple);
	value = SOF_DIV_ROUND_UP(EBB_BANKS_IN_SEGMENT * SRAM_BANK_SIZE, HOST_PAGE_SIZE);
	tlv_value_uint32_set(tuple, IPC4_TOTAL_PHYS_MEM_PAGES_HW_CFG, value);

	tuple = tlv_next(tuple);

	/* add platform specific tuples */
	platform_basefw_hw_config(&plat_data_offset, (char *)tuple);

	*data_offset = (int)((char *)tuple - data) + plat_data_offset;

	return 0;
}

struct ipc4_system_time_info *basefw_get_system_time_info(void)
{
	return &global_system_time_info;
}

static log_timestamp_t basefw_get_timestamp(void)
{
	return sof_cycle_get_64() + global_cycle_delta;
}

static uint32_t basefw_set_system_time(uint32_t param_id,
				       bool first_block,
				       bool last_block,
				       uint32_t data_offset,
				       const char *data)
{
	uint64_t dsp_time;
	uint64_t dsp_cycle;
	uint64_t host_time;
	uint64_t host_cycle;

	if (!(first_block && last_block))
		return IPC4_INVALID_REQUEST;

	global_system_time_info.host_time.val_l = ((const struct ipc4_system_time *)data)->val_l;
	global_system_time_info.host_time.val_u = ((const struct ipc4_system_time *)data)->val_u;

	dsp_cycle = sof_cycle_get_64();
	dsp_time = k_cyc_to_us_floor64(dsp_cycle);

	global_system_time_info.dsp_time.val_l = (uint32_t)(dsp_time);
	global_system_time_info.dsp_time.val_u = (uint32_t)(dsp_time >> 32);

	/* use default timestamp if 64bit is not enabled since 64bit is necessary for host time */
	if (!IS_ENABLED(CONFIG_LOG_TIMESTAMP_64BIT)) {
		LOG_WRN("64bits timestamp is disabled, so use default timestamp");
		return IPC4_SUCCESS;
	}

	host_time = global_system_time_info.host_time.val_l |
			((uint64_t)global_system_time_info.host_time.val_u << 32);
	host_cycle = k_us_to_cyc_ceil64(host_time);
	global_cycle_delta = host_cycle - dsp_cycle;
	log_set_timestamp_func(basefw_get_timestamp,
			       sys_clock_hw_cycles_per_sec());

	return IPC4_SUCCESS;
}

static uint32_t basefw_get_system_time(uint32_t *data_offset, char *data)
{
	struct ipc4_system_time *system_time = (struct ipc4_system_time *)data;

	system_time->val_l = global_system_time_info.host_time.val_l;
	system_time->val_u = global_system_time_info.host_time.val_u;
	*data_offset = sizeof(struct ipc4_system_time);
	return IPC4_SUCCESS;
}

static int basefw_register_kcps(bool first_block,
				bool last_block,
				uint32_t data_offset_or_size,
				const char *data)
{
	if (!(first_block && last_block))
		return -EINVAL;

	/* value of kcps to request on core 0. Can be negative */
	if (core_kcps_adjust(0, *(int32_t *)data))
		return -EINVAL;

	return 0;
}

static int basefw_kcps_allocation_request(struct ipc4_resource_kcps *request)
{
	if (core_kcps_adjust(request->core_id, request->kcps))
		return -EINVAL;

	return 0;
}

static int basefw_resource_allocation_request(bool first_block,
					      bool last_block,
					      uint32_t data_offset_or_size,
					      const char *data)
{
	struct ipc4_resource_request *request;

	if (!(first_block && last_block))
		return -EINVAL;

	request = (struct ipc4_resource_request *)data;

	switch (request->ra_type) {
	case IPC4_RAT_DSP_KCPS:
		return basefw_kcps_allocation_request(&request->ra_data.kcps);
	case IPC4_RAT_MEMORY:
		return -EINVAL;
	default:
		return -EINVAL;
	}
}

static int basefw_power_state_info_get(uint32_t *data_offset, char *data)
{
	struct sof_tlv *tuple = (struct sof_tlv *)data;
	uint32_t core_kcps[CONFIG_CORE_COUNT] = {0};
	int core_id;

	tlv_value_uint32_set(tuple, IPC4_ACTIVE_CORES_MASK, cpu_enabled_cores());
	tuple = tlv_next(tuple);

	for (core_id = 0; core_id < CONFIG_CORE_COUNT; core_id++) {
		if (cpu_is_core_enabled(core_id))
			core_kcps[core_id] = core_kcps_get(core_id);
	}

	tlv_value_set(tuple, IPC4_CORE_KCPS, sizeof(core_kcps), core_kcps);
	tuple = tlv_next(tuple);
	*data_offset = (int)((char *)tuple - data);
	return 0;
}

static int basefw_libraries_info_get(uint32_t *data_offset, char *data)
{
	if (sizeof(struct ipc4_libraries_info) +
		    LIB_MANAGER_MAX_LIBS * sizeof(struct ipc4_library_props) >
	    SOF_IPC_MSG_MAX_SIZE) {
		tr_err(&basefw_comp_tr, "Error with message size");
		return -ENOMEM;
	}

	struct ipc4_libraries_info *const libs_info = (struct ipc4_libraries_info *)data;
	const struct sof_man_fw_desc *desc;
	int lib_counter = 0;

	for (int lib_id = 0; lib_id < LIB_MANAGER_MAX_LIBS; ++lib_id) {
		if (lib_id == 0) {
			desc = platform_base_fw_get_manifest();
		} else {
#if CONFIG_LIBRARY_MANAGER
			desc = (struct sof_man_fw_desc *)lib_manager_get_library_manifest(lib_id);
#else
			desc = NULL;
#endif
		}

		if (!desc)
			continue;

		libs_info->libraries[lib_id].id = lib_id;
		memcpy_s(libs_info->libraries[lib_counter].name,
			 SOF_MAN_FW_HDR_FW_NAME_LEN, desc->header.name, sizeof(desc->header.name));
		libs_info->libraries[lib_counter].major_version =
			desc->header.major_version;
		libs_info->libraries[lib_counter].minor_version =
			desc->header.minor_version;
		libs_info->libraries[lib_counter].hotfix_version =
			desc->header.hotfix_version;
		libs_info->libraries[lib_counter].build_version =
			desc->header.build_version;
		libs_info->libraries[lib_counter].num_module_entries =
			desc->header.num_module_entries;

		lib_counter++;
	}

	libs_info->library_count = lib_counter;
	*data_offset =
		sizeof(libs_info) + libs_info->library_count * sizeof(libs_info->libraries[0]);

	return 0;
}

static int basefw_modules_info_get(uint32_t *data_offset, char *data)
{
	return platform_basefw_modules_info_get(data_offset, data);
}

int schedulers_info_get(uint32_t *data_off_size,
			char *data,
			uint32_t core_id)
{
	/* TODO
	 * Core id parameter is not yet used. For now we only get scheduler info from current core
	 * Other cores info can be added by implementing idc request for this data.
	 * Do this if Schedulers info get ipc has uses for accurate info per core
	 */

	struct scheduler_props *scheduler_props;
	/* the internal structs have irregular sizes so we cannot use indexing, and have to
	 *  reassign pointers for each element
	 */
	struct schedulers_info *schedulers_info = (struct schedulers_info *)data;

	schedulers_info->scheduler_count = 0;

	/* smallest response possible is just zero schedulers count
	 * here we replace max_len from data_off_size to serve as output size
	 */
	*data_off_size = sizeof(struct schedulers_info);

	/* ===================== LL_TIMER SCHEDULER INFO ============================ */
	schedulers_info->scheduler_count++;
	scheduler_props = (struct scheduler_props *)(data + *data_off_size);
	scheduler_get_task_info_ll(scheduler_props, data_off_size);

	/* ===================== DP SCHEDULER INFO ============================ */
#if CONFIG_ZEPHYR_DP_SCHEDULER
	schedulers_info->scheduler_count++;
	scheduler_props = (struct scheduler_props *)(data + *data_off_size);
	scheduler_get_task_info_dp(scheduler_props, data_off_size);
#endif
	return 0;
}

static int basefw_pipeline_list_info_get(uint32_t *data_offset, char *data)
{
	struct ipc4_pipeline_set_state_data *ppl_data = (struct ipc4_pipeline_set_state_data *)data;

	struct ipc *ipc = ipc_get();
	struct ipc_comp_dev *ipc_pipe;
	const struct ipc4_pipeline_set_state_data *pipeline_data;

	pipeline_data = ipc4_get_pipeline_data_wrapper();
	ppl_data->pipelines_count = 0;

	for (int ppl = 0; ppl < pipeline_data->pipelines_count; ppl++) {
		ipc_pipe = ipc_get_pipeline_by_id(ipc, ppl);
		if (!ipc_pipe)
			tr_err(&ipc_tr, "No pipeline with instance_id = %d", ppl);
		else
			ppl_data->ppl_id[ppl_data->pipelines_count++] =
				ipc_pipe->pipeline->pipeline_id;
	}

	*data_offset = sizeof(ppl_data->pipelines_count) +
		       ppl_data->pipelines_count * sizeof(ppl_data->ppl_id[0]);
	return 0;
}

int set_perf_meas_state(const char *data)
{
#ifdef CONFIG_SOF_TELEMETRY
	enum ipc4_perf_measurements_state_set state = *data;

	struct telemetry_wnd_data *wnd_data =
			(struct telemetry_wnd_data *)ADSP_DW->slots[SOF_DW_TELEMETRY_SLOT];
	struct system_tick_info *systick_info =
			(struct system_tick_info *)wnd_data->system_tick_info;

	switch (state) {
	case IPC4_PERF_MEASUREMENTS_DISABLED:
		break;
	case IPC4_PERF_MEASUREMENTS_STOPPED:
		for (int i = 0; i < CONFIG_MAX_CORE_COUNT; i++)
			systick_info[i].peak_utilization = 0;
		break;
	case IPC4_PERF_MEASUREMENTS_STARTED:
	case IPC4_PERF_MEASUREMENTS_PAUSED:
		break;
	default:
		return -EINVAL;
	}
#endif
	return IPC4_SUCCESS;
}

static int basefw_get_large_config(struct comp_dev *dev,
				   uint32_t param_id,
				   bool first_block,
				   bool last_block,
				   uint32_t *data_offset,
				   char *data)
{
	/* We can use extended param id for both extended and standard param id */
	union ipc4_extended_param_id extended_param_id;

	extended_param_id.full = param_id;

	switch (extended_param_id.part.parameter_type) {
	case IPC4_PERF_MEASUREMENTS_STATE:
	case IPC4_GLOBAL_PERF_DATA:
		break;
	default:
		if (!first_block)
			return -EINVAL;
	}

	switch (extended_param_id.part.parameter_type) {
	case IPC4_FW_CONFIG:
		return basefw_config(data_offset, data);
	case IPC4_HW_CONFIG_GET:
		return basefw_hw_config(data_offset, data);
	case IPC4_SYSTEM_TIME:
		return basefw_get_system_time(data_offset, data);
	case IPC4_POWER_STATE_INFO_GET:
		return basefw_power_state_info_get(data_offset, data);
	case IPC4_SCHEDULERS_INFO_GET:
		return schedulers_info_get(data_offset, data,
					 extended_param_id.part.parameter_instance);
	case IPC4_PIPELINE_LIST_INFO_GET:
		return basefw_pipeline_list_info_get(data_offset, data);
	case IPC4_MODULES_INFO_GET:
		return basefw_modules_info_get(data_offset, data);
	case IPC4_LIBRARIES_INFO_GET:
		return basefw_libraries_info_get(data_offset, data);
	/* TODO: add more support */
	case IPC4_DSP_RESOURCE_STATE:
	case IPC4_NOTIFICATION_MASK:
	case IPC4_PIPELINE_PROPS_GET:
	case IPC4_GATEWAYS_INFO_GET:
	case IPC4_PERF_MEASUREMENTS_STATE:
	case IPC4_GLOBAL_PERF_DATA:
		COMPILER_FALLTHROUGH;
	default:
		break;
	}

	return platform_basefw_get_large_config(dev, param_id, first_block, last_block,
						data_offset, data);
};

static int basefw_set_large_config(struct comp_dev *dev,
				   uint32_t param_id,
				   bool first_block,
				   bool last_block,
				   uint32_t data_offset,
				   const char *data)
{
	switch (param_id) {
	case IPC4_PERF_MEASUREMENTS_STATE:
		return set_perf_meas_state(data);
	case IPC4_SYSTEM_TIME:
		return basefw_set_system_time(param_id, first_block,
						last_block, data_offset, data);
	case IPC4_ENABLE_LOGS:
		return ipc4_logging_enable_logs(first_block, last_block, data_offset, data);
	case IPC4_REGISTER_KCPS:
		return basefw_register_kcps(first_block, last_block, data_offset, data);
	case IPC4_RESOURCE_ALLOCATION_REQUEST:
		return basefw_resource_allocation_request(first_block, last_block, data_offset,
							  data);
	default:
		break;
	}

	return platform_basefw_set_large_config(dev, param_id, first_block, last_block,
						data_offset, data);
};

static const struct comp_driver comp_basefw = {
	.uid	= SOF_RT_UUID(basefw_comp_uuid),
	.tctx	= &basefw_comp_tr,
	.ops	= {
		.get_large_config = basefw_get_large_config,
		.set_large_config = basefw_set_large_config,
	},
};

static SHARED_DATA struct comp_driver_info comp_basefw_info = {
	.drv = &comp_basefw,
};

UT_STATIC void sys_comp_basefw_init(void)
{
	comp_register(platform_shared_get(&comp_basefw_info,
					  sizeof(comp_basefw_info)));
}

DECLARE_MODULE(sys_comp_basefw_init);
SOF_MODULE_INIT(basefw, sys_comp_basefw_init);
