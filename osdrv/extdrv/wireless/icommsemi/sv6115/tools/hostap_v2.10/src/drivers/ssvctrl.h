#ifndef SSVCTRL_H
#define SSVCTRL_H

#define SSV_CTRL_NL_NAME			"SSV_WPAS_CTL"

#define SSV_CTRL_MAX_SSID_LEN			32

/// SSV wpa_supplicant control netlink command.
typedef enum _en_ssv_wpas_ctl_cmd
{
    E_SSV_WPAS_CTL_CMD_UNSPEC,
    E_SSV_WPAS_CTL_CMD_EXTAUTH,
    E_SSV_WPAS_CTL_CMD_MAX
} EN_SSV_WPAS_CTL_CMD;

/// SSV wpa_supplicant control netlink attribute.
typedef enum _en_ssv_wpas_ctl_attr 
{
    E_SSV_WPAS_CTL_ATTR_UNSPEC,
    E_SSV_WPAS_CTL_ATTR_EXTAUTH,
    E_SSV_WPAS_CTL_ATTR_EXTAUTH_RSP,
    E_SSV_WPAS_CTL_ATTR_MAX
} EN_SSV_WPAS_CTL_ATTR;

enum ssv_external_auth_action {
	SSV_EXTERNAL_AUTH_START,
	SSV_EXTERNAL_AUTH_ABORT,
};

struct ssv_ssid {
	u8 ssid[SSV_CTRL_MAX_SSID_LEN];
	u8 ssid_len;
};

struct ssv_external_auth_params {
	enum ssv_external_auth_action action;
	u8 bssid[ETH_ALEN];
	struct ssv_ssid ssid;
	unsigned int key_mgmt_suite;
	u16 status;
};

struct ssvctrl {
	struct netlink_data *netlink;
	struct nl_cb *nl_cb;
	struct nl_sock *nl;
	int ssvctrl_id;
	int nlctrl_id;
	struct nl_sock *nl_event;
};

#endif
