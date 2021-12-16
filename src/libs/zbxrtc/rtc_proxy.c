/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
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

#include "zbxipcservice.h"
#include "zbxrtc.h"
#include "rtc.h"

int	rtc_parse_options_ex(const char *opt, zbx_uint32_t *code, char **data, char **error)
{
	ZBX_UNUSED(opt);
	ZBX_UNUSED(code);
	ZBX_UNUSED(data);
	ZBX_UNUSED(error);

	return SUCCEED;
}

int	rtc_process_request_ex(int code, const unsigned char *data, char **result)
{
	ZBX_UNUSED(code);
	ZBX_UNUSED(data);
	ZBX_UNUSED(result);

	return FAIL;
}
