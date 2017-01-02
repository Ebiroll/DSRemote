/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017 Teunis van Beelen
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


#include "lan_connect_thread.h"


lan_connect_thread::lan_connect_thread()
{
  device = NULL;

  dev_str[0] = 0;
}


void lan_connect_thread::run()
{
  msleep(300);

  if(dev_str[0] == 0) return;

  device = tmc_open_lan(dev_str);
}


struct tmcdev * lan_connect_thread::get_device(void)
{
  return device;
}


void lan_connect_thread::set_device_address(const char *addr)
{
  strncpy(dev_str, addr, 16);

  dev_str[15] = 0;
}













