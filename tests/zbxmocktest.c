/*
** Zabbix
** Copyright (C) 2001-2023 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/
#include "zbxmocktest.h"
#include "zbxmockdata.h"

#include "zbxnum.h"
#include "zbxtypes.h"
#include "log.h"

/* unresolved symbols needed for linking */

static unsigned char	program_type	= 0;

unsigned char	get_program_type(void)
{
	return program_type;
}

int	CONFIG_FORKS[ZBX_PROCESS_TYPE_COUNT] = {
	5, /* ZBX_PROCESS_TYPE_POLLER */
	1, /* ZBX_PROCESS_TYPE_UNREACHABLE */
	0, /* ZBX_PROCESS_TYPE_IPMIPOLLER */
	1, /* ZBX_PROCESS_TYPE_PINGER */
	0, /* ZBX_PROCESS_TYPE_JAVAPOLLER */
	1, /* ZBX_PROCESS_TYPE_HTTPPOLLER */
	5, /* ZBX_PROCESS_TYPE_TRAPPER */
	0, /* ZBX_PROCESS_TYPE_SNMPTRAPPER */
	1, /* ZBX_PROCESS_TYPE_PROXYPOLLER */
	1, /* ZBX_PROCESS_TYPE_ESCALATOR */
	4, /* ZBX_PROCESS_TYPE_HISTSYNCER */
	1, /* ZBX_PROCESS_TYPE_DISCOVERER */
	3, /* ZBX_PROCESS_TYPE_ALERTER */
	1, /* ZBX_PROCESS_TYPE_TIMER */
	1, /* ZBX_PROCESS_TYPE_HOUSEKEEPER */
	0, /* ZBX_PROCESS_TYPE_DATASENDER */
	1, /* ZBX_PROCESS_TYPE_CONFSYNCER */
	1, /* ZBX_PROCESS_TYPE_SELFMON */
	0, /* ZBX_PROCESS_TYPE_VMWARE */
	0, /* ZBX_PROCESS_TYPE_COLLECTOR */
	0, /* ZBX_PROCESS_TYPE_LISTENER */
	0, /* ZBX_PROCESS_TYPE_ACTIVE_CHECKS */
	1, /* ZBX_PROCESS_TYPE_TASKMANAGER */
	0, /* ZBX_PROCESS_TYPE_IPMIMANAGER */
	1, /* ZBX_PROCESS_TYPE_ALERTMANAGER */
	1, /* ZBX_PROCESS_TYPE_PREPROCMAN */
	3, /* ZBX_PROCESS_TYPE_PREPROCESSOR */
	1, /* ZBX_PROCESS_TYPE_LLDMANAGER */
	2, /* ZBX_PROCESS_TYPE_LLDWORKER */
	1, /* ZBX_PROCESS_TYPE_ALERTSYNCER */
	5, /* ZBX_PROCESS_TYPE_HISTORYPOLLER */
	1, /* ZBX_PROCESS_TYPE_AVAILMAN */
	0, /* ZBX_PROCESS_TYPE_REPORTMANAGER */
	0, /* ZBX_PROCESS_TYPE_REPORTWRITER */
	1, /* ZBX_PROCESS_TYPE_SERVICEMAN */
	1, /* ZBX_PROCESS_TYPE_TRIGGERHOUSEKEEPER */
	5, /* ZBX_PROCESS_TYPE_ODBCPOLLER */
	0, /* ZBX_PROCESS_TYPE_CONNECTORMANAGER */
};

int	CONFIG_LISTEN_PORT		= 0;
char	*CONFIG_LISTEN_IP		= NULL;
char	*CONFIG_SOURCE_IP		= NULL;
int	CONFIG_TRAPPER_TIMEOUT		= 300;

int	CONFIG_HOUSEKEEPING_FREQUENCY	= 1;
int	CONFIG_MAX_HOUSEKEEPER_DELETE	= 5000;		/* applies for every separate field value */
int	CONFIG_CONFSYNCER_FREQUENCY	= 60;
int	CONFIG_PROBLEMHOUSEKEEPING_FREQUENCY = 60;

int	CONFIG_VMWARE_FREQUENCY		= 60;
int	CONFIG_VMWARE_PERF_FREQUENCY	= 60;
int	CONFIG_VMWARE_TIMEOUT		= 10;

zbx_uint64_t	CONFIG_CONF_CACHE_SIZE		= 8 * 0;
zbx_uint64_t	CONFIG_HISTORY_CACHE_SIZE	= 16 * 0;
zbx_uint64_t	CONFIG_HISTORY_INDEX_CACHE_SIZE	= 4 * 0;
zbx_uint64_t	CONFIG_TRENDS_CACHE_SIZE	= 4 * 0;
zbx_uint64_t	CONFIG_VALUE_CACHE_SIZE		= 8 * 0;
zbx_uint64_t	CONFIG_VMWARE_CACHE_SIZE	= 8 * 0;
zbx_uint64_t	CONFIG_TREND_FUNC_CACHE_SIZE	= 0;

int	CONFIG_UNREACHABLE_PERIOD	= 45;
int	CONFIG_UNREACHABLE_DELAY	= 15;
int	CONFIG_LOG_LEVEL		= 0;
char	*CONFIG_EXTERNALSCRIPTS		= NULL;

char	*CONFIG_SNMPTRAP_FILE		= NULL;

char	*CONFIG_JAVA_GATEWAY		= NULL;
int	CONFIG_JAVA_GATEWAY_PORT	= 0;

char	*CONFIG_SSH_KEY_LOCATION	= NULL;

int	CONFIG_LOG_SLOW_QUERIES		= 0;	/* ms; 0 - disable */

/* how often Zabbix server sends configuration data to proxy, in seconds */
int	CONFIG_PROXYCONFIG_FREQUENCY	= 0;
int	CONFIG_PROXYDATA_FREQUENCY	= 1;	/* 1s */

int	CONFIG_ALLOW_UNSUPPORTED_DB_VERSIONS = 0;

char	*CONFIG_LOAD_MODULE_PATH	= NULL;
char	**CONFIG_LOAD_MODULE		= NULL;

char	*CONFIG_USER			= NULL;

/* web monitoring */
char	*CONFIG_SSL_CA_LOCATION		= NULL;
char	*CONFIG_SSL_CERT_LOCATION	= NULL;
char	*CONFIG_SSL_KEY_LOCATION	= NULL;

char	*CONFIG_SOCKET_PATH			= NULL;
char	*CONFIG_HISTORY_STORAGE_URL		= NULL;
char	*CONFIG_HISTORY_STORAGE_OPTS		= NULL;
int	CONFIG_HISTORY_STORAGE_PIPELINES	= 0;

/* not used in tests, defined for linking with comms.c */
int	CONFIG_TCP_MAX_BACKLOG_SIZE	= SOMAXCONN;

const char	title_message[] = "mock_title_message";
const char	*usage_message[] = {"mock_usage_message", NULL};
const char	*help_message[] = {"mock_help_message", NULL};
const char	*progname = "mock_progname";
const char	syslog_app_name[] = "mock_syslog_app_name";

char	*CONFIG_HOSTS_ALLOWED		= NULL;
char	*CONFIG_HOSTNAMES		= NULL;
char	*CONFIG_HOSTNAME_ITEM		= NULL;
char	*CONFIG_HOST_METADATA		= NULL;
char	*CONFIG_HOST_METADATA_ITEM	= NULL;

ZBX_THREAD_LOCAL char	*CONFIG_HOSTNAME	= NULL;

int	CONFIG_REFRESH_ACTIVE_CHECKS	= 120;

int	CONFIG_BUFFER_SIZE		= 100;
int	CONFIG_BUFFER_SEND		= 5;

int	CONFIG_MAX_LINES_PER_SECOND	= 20;

char	**CONFIG_ALIASES		= NULL;
char	**CONFIG_USER_PARAMETERS	= NULL;
#if defined(_WINDOWS)
char	**CONFIG_PERF_COUNTERS		= NULL;
char	**CONFIG_PERF_COUNTERS_EN	= NULL;
#endif

static int	zbx_config_timeout = 3;
int	get_zbx_config_timeout(void)
{
	return zbx_config_timeout;
}

static int	zbx_config_enable_remote_commands = 0;
int	get_zbx_config_enable_remote_commands(void)
{
	return zbx_config_enable_remote_commands;
}

static int	zbx_config_log_remote_commands = 0;
int	get_zbx_config_log_remote_commands(void)
{
	return zbx_config_log_remote_commands;
}

static int	zbx_config_unsafe_user_parameters = 0;
int	get_zbx_config_unsafe_user_parameters(void)
{
	return zbx_config_unsafe_user_parameters;
}

/* test itself */

int	main (void)
{
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test_setup_teardown(zbx_mock_test_entry, zbx_mock_data_init, zbx_mock_data_free)
	};

	zbx_log_level = LOG_LEVEL_INFORMATION;

	return cmocka_run_group_tests(tests, NULL, NULL);
}
