/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016, 2017 Teunis van Beelen
*
* Email: teuniz@gmail.com
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


#ifndef TMC_LAN_H
#define TMC_LAN_H


#ifdef __cplusplus
extern "C" {
#endif


struct tmcdev * tmclan_open(const char *);
void tmclan_close(struct tmcdev *);
int tmclan_write(struct tmcdev *, const char *);
int tmclan_read(struct tmcdev *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif



















