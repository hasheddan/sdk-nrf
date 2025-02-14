/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @brief Header containing FMAC interface specific declarations for the
 * Zephyr OS layer of the Wi-Fi driver.
 */

#ifndef __ZEPHYR_FMAC_MAIN_H__
#define __ZEPHYR_FMAC_MAIN_H__

#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#ifndef CONFIG_NRF700X_RADIO_TEST
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/ethernet.h>
#ifdef CONFIG_NETWORKING
#include <zephyr_net_if.h>
#endif /* CONFIG_NETWORKING */
#ifdef CONFIG_NRF700X_STA_MODE
#include <drivers/driver_zephyr.h>
#endif /* CONFIG_NRF700X_STA_MODE */
#endif /* !CONFIG_NRF700X_RADIO_TEST */

#include <fmac_api.h>
#include <host_rpu_umac_if.h>
#include "ncs_version.h"

#define NRF700X_DRIVER_VERSION "1."NCS_VERSION_STRING

#ifndef CONFIG_NRF700X_RADIO_TEST
/* Use same timeout as WPA supplicant, this is high mainly to handle
 * connected scan.
 */
#define WIFI_NRF_SCAN_TIMEOUT (K_SECONDS(30))

struct wifi_nrf_vif_ctx_zep {
	const struct device *zep_dev_ctx;
	struct net_if *zep_net_if_ctx;
	void *supp_drv_if_ctx;
	void *rpu_ctx_zep;
	unsigned char vif_idx;

	scan_result_cb_t disp_scan_cb;
	bool scan_in_progress;
	int scan_type;
	unsigned int scan_res_cnt;
	struct k_work_delayable scan_timeout_work;

	struct net_eth_addr mac_addr;
	int if_type;
	enum wifi_nrf_fmac_if_op_state if_op_state;
#ifdef CONFIG_NET_STATISTICS_ETHERNET
	struct net_stats_eth eth_stats;
#endif /* CONFIG_NET_STATISTICS_ETHERNET */
#ifdef CONFIG_NRF700X_STA_MODE
	unsigned int assoc_freq;
	enum wifi_nrf_fmac_if_carr_state if_carr_state;
	struct wpa_signal_info *signal_info;
	struct wpa_conn_info *conn_info;
	struct zep_wpa_supp_dev_callbk_fns supp_callbk_fns;
	unsigned char twt_flows_map;
	unsigned char twt_flow_in_progress_map;
	struct wifi_ps_config *ps_info;
	bool ps_config_info_evnt;
	bool authorized;
	struct wifi_nrf_ext_capa {
		enum nrf_wifi_iftype iftype;
		unsigned char *ext_capa, *ext_capa_mask;
		unsigned int ext_capa_len;
	} iface_ext_capa;
	bool cookie_resp_received;
#ifdef CONFIG_NRF700X_DATA_TX
	struct k_work wifi_nrf_net_iface_work;
#endif /* CONFIG_NRF700X_DATA_TX */
	unsigned long rssi_record_timestamp_us;
	signed short rssi;
#endif /* CONFIG_NRF700X_STA_MODE */
};

struct wifi_nrf_vif_ctx_map {
	const char *ifname;
	struct wifi_nrf_vif_ctx_zep *vif_ctx;
};
#endif /* !CONFIG_NRF700X_RADIO_TEST */

struct wifi_nrf_ctx_zep {
	void *drv_priv_zep;
	void *rpu_ctx;
#ifdef CONFIG_NRF700X_RADIO_TEST
	struct rpu_conf_params conf_params;
	bool rf_test_run;
	unsigned char rf_test;
#else /* CONFIG_NRF700X_RADIO_TEST */
	struct wifi_nrf_vif_ctx_zep vif_ctx_zep[MAX_NUM_VIFS];
#ifdef CONFIG_NRF700X_UTIL
	struct rpu_conf_params conf_params;
#endif /* CONFIG_NRF700X_UTIL */
#endif /* CONFIG_NRF700X_RADIO_TEST */
	unsigned char *extended_capa, *extended_capa_mask;
	unsigned int extended_capa_len;
};

struct wifi_nrf_drv_priv_zep {
	struct wifi_nrf_fmac_priv *fmac_priv;
	/* TODO: Replace with a linked list to handle unlimited RPUs */
	struct wifi_nrf_ctx_zep rpu_ctx_zep;
};

extern struct wifi_nrf_drv_priv_zep rpu_drv_priv_zep;

void wifi_nrf_scan_timeout_work(struct k_work *work);
const char *wifi_nrf_get_drv_version(void);
enum wifi_nrf_status wifi_nrf_fmac_dev_add_zep(struct wifi_nrf_drv_priv_zep *drv_priv_zep);
enum wifi_nrf_status wifi_nrf_fmac_dev_rem_zep(struct wifi_nrf_drv_priv_zep *drv_priv_zep);
enum wifi_nrf_status wifi_nrf_fw_load(void *rpu_ctx);
struct wifi_nrf_vif_ctx_zep *wifi_nrf_get_vif_ctx(struct net_if *iface);

#endif /* __ZEPHYR_FMAC_MAIN_H__ */
