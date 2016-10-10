/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016 Teunis van Beelen
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


#include "save_data_thread.h"





save_data_thread::save_data_thread(int job_s, int hdl_s)
{
  job = job_s;

  hdl = hdl_s;

  err_num = -1;

  err_str[0] = 0;

  n_bytes_rcvd = -1;
}


int save_data_thread::get_error_num(void)
{
  return err_num;
}


void save_data_thread::get_error_str(char *dest)
{
  strcpy(dest, err_str);
}


int save_data_thread::get_num_bytes_rcvd(void)
{
  return n_bytes_rcvd;
}


void save_data_thread::run()
{
  err_str[0] = 0;

  switch(job)
  {
    case 0: read_data();
            break;
    case 1: save_memory_edf_file();
            break;
    default: err_num = -4;
            break;
  }
}


void save_data_thread::read_data(void)
{
  n_bytes_rcvd = tmc_read();

  err_num = 0;
}


void save_data_thread::save_memory_edf_file(void)
{
}



















