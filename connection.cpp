/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016, 2017, 2018, 2019 Teunis van Beelen
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


#include "connection.h"



int tmc_connection_type;

struct tmcdev *tmc_device;



struct tmcdev * tmc_open_usb(const char *device)
{
  tmc_connection_type = 0;

  tmc_device = tmcdev_open(device);

  return tmc_device;
}


struct tmcdev * tmc_open_lan(const char *address)
{
  tmc_connection_type = 1;

  tmc_device =  tmclan_open(address);

  return tmc_device;
}


void tmc_close(void)
{
  if(tmc_connection_type == 0)
  {
    tmcdev_close(tmc_device);
  }
  else
  {
    tmclan_close(tmc_device);
  }

  tmc_device = NULL;
}


int tmc_write(const char *cmd)
{
  if(tmc_connection_type == 0)
  {
    return tmcdev_write(tmc_device, cmd);
  }
  else
  {
    return tmclan_write(tmc_device, cmd);
  }

  return -1;
}


int tmc_read(void)
{
  if(tmc_connection_type == 0)
  {
    return tmcdev_read(tmc_device);
  }
  else
  {
    return tmclan_read(tmc_device);
  }

  return -1;
}









