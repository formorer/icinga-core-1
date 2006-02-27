/*****************************************************************************
 *
 * XSDDEFAULT.H - Header file for default status data routines
 *
 * Copyright (c) 1999-2006 Ethan Galstad (nagios@nagios.org)
 * Last Modified:   02-26-2006
 *
 * License:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****************************************************************************/

#ifdef NSCORE
int xsddefault_initialize_status_data(char *);
int xsddefault_cleanup_status_data(char *,int);
int xsddefault_save_status_data(void);
#endif

#ifdef NSCGI

#define XSDDEFAULT_NO_DATA          0
#define XSDDEFAULT_INFO_DATA        1
#define XSDDEFAULT_PROGRAM_DATA     2
#define XSDDEFAULT_HOST_DATA        3
#define XSDDEFAULT_SERVICE_DATA     4
#define XSDDEFAULT_CONTACT_DATA     5

int xsddefault_read_status_data(char *,int);
#endif

int xsddefault_grab_config_info(char *);
void xsddefault_grab_config_directives(char *);
