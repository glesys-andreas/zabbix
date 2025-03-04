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

#ifndef ZABBIX_UPGRADE_H
#define ZABBIX_UPGRADE_H

#include "zbxcommon.h"
#include "zbxdbhigh.h"

typedef enum {
	ZBX_HA_MODE_STANDALONE,
	ZBX_HA_MODE_CLUSTER
}
zbx_ha_mode_t;

void	zbx_init_library_dbupgrade(zbx_get_program_type_f get_program_type_cb);

int	DBcheck_version(zbx_ha_mode_t ha_mode);

#endif
