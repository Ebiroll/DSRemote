/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015 - 2020 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/


#ifndef DSR_CONNECTION_H
#define DSR_CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global.h"
#include "tmc_dev.h"
#include "tmc_lan.h"
#include "utils.h"


struct tmcdev * tmc_open_usb(const char *);
void tmc_close(void);
int tmc_write(const char *);
int tmc_read(void);
struct tmcdev * tmc_open_lan(const char *);



#endif

