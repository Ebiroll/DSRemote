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


#include "save_data_thread.h"





save_data_thread::save_data_thread(int job_s)
{
  job = job_s;

  hdl = -1;

  err_num = -1;

  err_str[0] = 0;

  n_bytes_rcvd = -1;

  devparms = NULL;

  datrecs = 0;

  smps_per_record = 0;
}


int save_data_thread::get_error_num(void)
{
  return err_num;
}


void save_data_thread::get_error_str(char *dest, int sz)
{
  strlcpy(dest, err_str, sz);
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
  msleep(100);

  n_bytes_rcvd = tmc_read();

  err_num = 0;
}


void save_data_thread::init_save_memory_edf_file(struct device_settings *devp, int hdl_s,
                                                 int records, int smpls,
                                                 short **wav)
{
  datrecs = records;

  devparms = devp;

  smps_per_record = smpls;

  wavbuf = wav;

  hdl = hdl_s;
}


void save_data_thread::save_memory_edf_file(void)
{
  int i, chn;

  if(devparms == NULL)
  {
    strlcpy(err_str, "save_memory_edf_file(): Invalid devparms pointer.", 4096);

    err_num = 1;

    msleep(200);

    return;
  }

  if(hdl < 0)
  {
    strlcpy(err_str, "save_memory_edf_file(): Invalid handel.", 4096);

    err_num = 2;

    msleep(200);

    return;
  }

  msleep(100);

  for(i=0; i<datrecs; i++)
  {
    for(chn=0; chn<MAX_CHNS; chn++)
    {
      if(!devparms->chandisplay[chn])
      {
        continue;
      }

      if(edfwrite_digital_short_samples(hdl, wavbuf[chn] + (i * smps_per_record)))
      {
        strlcpy(err_str, "A file write error occurred.", 4096);

        err_num = 3;

        return;
      }
    }
  }

  err_num = 0;
}



















