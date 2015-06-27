/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015 Teunis van Beelen
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

struct tmcdev *usb_tmcdev;



struct tmcdev * tmc_open_usb(const char *device)
{
  tmc_connection_type = 0;

  usb_tmcdev = tmcdev_open(device);

  return usb_tmcdev;
}


void tmc_close(void)
{
  if(tmc_connection_type == 0)
  {
    tmcdev_close(usb_tmcdev);

    usb_tmcdev = NULL;
  }
}


int tmc_write(const char *msg)
{
  if(tmc_connection_type == 0)
  {
    return tmcdev_write(usb_tmcdev, msg);
  }

  return -1;
}


int tmc_read(void)
{
  if(tmc_connection_type == 0)
  {
    return tmcdev_read(usb_tmcdev);
  }

  return -1;
}








