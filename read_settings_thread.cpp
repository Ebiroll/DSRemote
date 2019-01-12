/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017, 2018, 2019 Teunis van Beelen
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


#include "read_settings_thread.h"


read_settings_thread::read_settings_thread()
{
  device = NULL;

  err_str[0] = 0;

  err_num = -1;

  devparms = NULL;

  delay = 0;
}


void read_settings_thread::set_delay(int val)
{
  delay = val;
}


int read_settings_thread::get_error_num(void)
{
  return err_num;
}


void read_settings_thread::get_error_str(char *dest)
{
  strcpy(dest, err_str);
}


void read_settings_thread::set_device(struct tmcdev *dev)
{
  device = dev;
}


void read_settings_thread::set_devparm_ptr(struct device_settings *devp)
{
  devparms = devp;
}


void read_settings_thread::run()
{
  int chn, line=0;

  char str[512];

  err_num = -1;

  if(device == NULL) return;

  if(devparms == NULL) return;

  devparms->activechannel = -1;

  if(delay > 0)
  {
    sleep(delay);
  }

  for(chn=0; chn<devparms->channel_cnt; chn++)
  {
    sprintf(str, ":CHAN%i:BWL?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "20M"))
    {
      devparms->chanbwlimit[chn] = 20;
    }
    else if(!strcmp(device->buf, "250M"))
      {
        devparms->chanbwlimit[chn] = 250;
      }
      else if(!strcmp(device->buf, "OFF"))
        {
          devparms->chanbwlimit[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:COUP?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "AC"))
    {
      devparms->chancoupling[chn] = 2;
    }
    else if(!strcmp(device->buf, "DC"))
      {
        devparms->chancoupling[chn] = 1;
      }
      else if(!strcmp(device->buf, "GND"))
        {
          devparms->chancoupling[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:DISP?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->chandisplay[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->chandisplay[chn] = 1;

        if(devparms->activechannel == -1)
        {
          devparms->activechannel = chn;
        }
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

    if(devparms->modelserie != 1)
    {
      sprintf(str, ":CHAN%i:IMP?", chn + 1);

      usleep(TMC_GDS_DELAY);

      if(tmc_write(str) != 11)
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

      if(tmc_read() < 1)
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

      if(!strcmp(device->buf, "OMEG"))
      {
        devparms->chanimpedance[chn] = 0;
      }
      else if(!strcmp(device->buf, "FIFT"))
        {
          devparms->chanimpedance[chn] = 1;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }
    }

    sprintf(str, ":CHAN%i:INV?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->chaninvert[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->chaninvert[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

    sprintf(str, ":CHAN%i:OFFS?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->chanoffset[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:PROB?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->chanprobe[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:UNIT?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "VOLT"))
    {
      devparms->chanunit[chn] = 0;
    }
    else if(!strcmp(device->buf, "WATT"))
      {
        devparms->chanunit[chn] = 1;
      }
      else if(!strcmp(device->buf, "AMP"))
        {
          devparms->chanunit[chn] = 2;
        }
        else if(!strcmp(device->buf, "UNKN"))
          {
            devparms->chanunit[chn] = 3;
          }
          else
          {
            devparms->chanunit[chn] = 0;
          }

    sprintf(str, ":CHAN%i:SCAL?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->chanscale[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:VERN?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->chanvernier[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->chanvernier[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:OFFS?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->timebaseoffset = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:SCAL?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->timebasescale = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:ENAB?") != 14)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms->timebasedelayenable = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms->timebasedelayenable = 1;
    }
    else
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:OFFS?") != 14)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->timebasedelayoffset = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:SCAL?") != 14)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->timebasedelayscale = atof(device->buf);

  if(devparms->modelserie != 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:HREF:MODE?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "CENT"))
    {
      devparms->timebasehrefmode = 0;
    }
    else if(!strcmp(device->buf, "TPOS"))
      {
        devparms->timebasehrefmode = 1;
      }
      else if(!strcmp(device->buf, "USER"))
        {
          devparms->timebasehrefmode = 2;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:HREF:POS?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->timebasehrefpos = atoi(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:MODE?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "MAIN"))
  {
    devparms->timebasemode = 0;
  }
  else if(!strcmp(device->buf, "XY"))
    {
      devparms->timebasemode = 1;
    }
    else if(!strcmp(device->buf, "ROLL"))
      {
        devparms->timebasemode = 2;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

  if(devparms->modelserie != 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:VERN?") != 10)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->timebasevernier = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->timebasevernier = 1;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }
  }

  if((devparms->modelserie != 1) && (devparms->modelserie != 2))
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:XY1:DISP?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->timebasexy1display = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->timebasexy1display = 1;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:XY2:DISP?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms->timebasexy2display = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms->timebasexy2display = 1;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:COUP?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "AC"))
  {
    devparms->triggercoupling = 0;
  }
  else if(!strcmp(device->buf, "DC"))
    {
      devparms->triggercoupling = 1;
    }
    else if(!strcmp(device->buf, "LFR"))
      {
        devparms->triggercoupling = 2;
      }
      else if(!strcmp(device->buf, "HFR"))
        {
          devparms->triggercoupling = 3;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:SWE?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "AUTO"))
  {
    devparms->triggersweep = 0;
  }
  else if(!strcmp(device->buf, "NORM"))
    {
      devparms->triggersweep = 1;
    }
    else if(!strcmp(device->buf, "SING"))
      {
        devparms->triggersweep = 2;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:MODE?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "EDGE"))
  {
    devparms->triggermode = 0;
  }
  else if(!strcmp(device->buf, "PULS"))
    {
      devparms->triggermode = 1;
    }
    else if(!strcmp(device->buf, "SLOP"))
      {
        devparms->triggermode = 2;
      }
      else if(!strcmp(device->buf, "VID"))
        {
          devparms->triggermode = 3;
        }
        else if(!strcmp(device->buf, "PATT"))
          {
            devparms->triggermode = 4;
          }
          else if(!strcmp(device->buf, "RS232"))
            {
              devparms->triggermode = 5;
            }
            else if(!strcmp(device->buf, "IIC"))
              {
                devparms->triggermode = 6;
              }
              else if(!strcmp(device->buf, "SPI"))
                {
                  devparms->triggermode = 7;
                }
                else if(!strcmp(device->buf, "CAN"))
                  {
                    devparms->triggermode = 8;
                  }
                  else if(!strcmp(device->buf, "USB"))
                    {
                      devparms->triggermode = 9;
                    }
                    else
                    {
                      line = __LINE__;
                      goto GDS_OUT_ERROR;
                    }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:STAT?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "TD"))
  {
    devparms->triggerstatus = 0;
  }
  else if(!strcmp(device->buf, "WAIT"))
    {
      devparms->triggerstatus = 1;
    }
    else if(!strcmp(device->buf, "RUN"))
      {
        devparms->triggerstatus = 2;
      }
      else if(!strcmp(device->buf, "AUTO"))
        {
          devparms->triggerstatus = 3;
        }
        else if(!strcmp(device->buf, "FIN"))
          {
            devparms->triggerstatus = 4;
          }
          else if(!strcmp(device->buf, "STOP"))
            {
              devparms->triggerstatus = 5;
            }
            else
            {
              line = __LINE__;
              goto GDS_OUT_ERROR;
            }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:EDG:SLOP?") != 15)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "POS"))
  {
    devparms->triggeredgeslope = 0;
  }
  else if(!strcmp(device->buf, "NEG"))
    {
      devparms->triggeredgeslope = 1;
    }
    else if(!strcmp(device->buf, "RFAL"))
      {
        devparms->triggeredgeslope = 2;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:EDG:SOUR?") != 15)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->triggeredgesource = 0;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->triggeredgesource = 1;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->triggeredgesource = 2;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->triggeredgesource = 3;
        }
        else if(!strcmp(device->buf, "EXT"))
          {
            devparms->triggeredgesource = 4;
          }
          else if(!strcmp(device->buf, "EXT5"))
            {
              devparms->triggeredgesource = 5;
            }  // DS1000Z: "AC", DS6000: "ACL" !!
            else if((!strcmp(device->buf, "AC")) || (!strcmp(device->buf, "ACL")))
              {
                devparms->triggeredgesource = 6;
              }
              else
              {
                line = __LINE__;
                goto GDS_OUT_ERROR;
              }

  for(chn=0; chn<devparms->channel_cnt; chn++)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:LEV?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->triggeredgelevel[chn] = atof(device->buf);
  }

  if(devparms->triggeredgesource < 4)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", devparms->triggeredgesource + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(devparms->triggeredgesource== 4)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR EXT") != 18)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(devparms->triggeredgesource== 5)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR EXT5") != 19)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(devparms->triggeredgesource== 6)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR AC") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:HOLD?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->triggerholdoff = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:SRAT?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->samplerate = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:GRID?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "NONE"))
  {
    devparms->displaygrid = 0;
  }
  else if(!strcmp(device->buf, "HALF"))
    {
      devparms->displaygrid = 1;
    }
    else if(!strcmp(device->buf, "FULL"))
      {
        devparms->displaygrid = 2;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":MEAS:COUN:SOUR?") != 16)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "OFF"))
  {
    devparms->countersrc = 0;
  }
  else if(!strcmp(device->buf, "CHAN1"))
    {
      devparms->countersrc = 1;
    }
    else if(!strcmp(device->buf, "CHAN2"))
      {
        devparms->countersrc = 2;
      }
      else if(!strcmp(device->buf, "CHAN3"))
        {
          devparms->countersrc = 3;
        }
        else if(!strcmp(device->buf, "CHAN4"))
          {
            devparms->countersrc = 4;
          }
          else
          {
            line = __LINE__;
            goto GDS_OUT_ERROR;
          }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:TYPE?") != 11)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "VECT"))
  {
    devparms->displaytype = 0;
  }
  else if(!strcmp(device->buf, "DOTS"))
    {
      devparms->displaytype = 1;
    }
    else
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:TYPE?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "NORM"))
  {
    devparms->acquiretype = 0;
  }
  else if(!strcmp(device->buf, "AVER"))
    {
      devparms->acquiretype = 1;
    }
    else if(!strcmp(device->buf, "PEAK"))
      {
        devparms->acquiretype = 2;
      }
      else if(!strcmp(device->buf, "HRES"))
        {
          devparms->acquiretype = 3;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:AVER?") != 10)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->acquireaverages = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:GRAD:TIME?") != 16)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "MIN"))
  {
    devparms->displaygrading = 0;
  }
  else if(!strcmp(device->buf, "0.1"))
    {
      devparms->displaygrading = 1;
    }
    else if(!strcmp(device->buf, "0.2"))
      {
        devparms->displaygrading = 2;
      }
      else if(!strcmp(device->buf, "0.5"))
        {
          devparms->displaygrading = 5;
        }
        else if(!strcmp(device->buf, "1"))
          {
            devparms->displaygrading = 10;
          }
          else if(!strcmp(device->buf, "2"))
            {
              devparms->displaygrading = 20;
            }
            else if(!strcmp(device->buf, "5"))
              {
                devparms->displaygrading = 50;
              }
              else if(!strcmp(device->buf, "INF"))
                {
                  devparms->displaygrading = 10000;
                }
                else
                {
                  line = __LINE__;
                  goto GDS_OUT_ERROR;
                }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:SPL?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":MATH:FFT:SPL?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_fft_split = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:MODE?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "FFT"))
    {
      devparms->math_fft = 1;
    }
    else
    {
      devparms->math_fft = 0;
    }
  }
  else
  {
    if(tmc_write(":MATH:DISP?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->math_fft = atoi(device->buf);

    if(devparms->math_fft == 1)
    {
      usleep(TMC_GDS_DELAY);

      if(tmc_write(":MATH:OPER?") != 11)
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

      if(tmc_read() < 1)
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }

      if(!strcmp(device->buf, "FFT"))
      {
        devparms->math_fft = 1;
      }
      else
      {
        devparms->math_fft = 0;
      }
    }
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:VSM?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":MATH:FFT:UNIT?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "VRMS"))
  {
    devparms->fft_vscale = 0.5;

    devparms->fft_voffset = -2.0;

    devparms->math_fft_unit = 0;
  }
  else
  {
    devparms->fft_vscale = 10.0;

    devparms->fft_voffset = 20.0;

    devparms->math_fft_unit = 1;
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:SOUR?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":MATH:FFT:SOUR?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_fft_src = 0;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_fft_src = 1;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_fft_src = 2;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_fft_src = 3;
        }
        else
        {
          devparms->math_fft_src = 0;
        }

  usleep(TMC_GDS_DELAY);

  devparms->current_screen_sf = 100.0 / devparms->timebasescale;

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:HSP?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->math_fft_hscale = atof(device->buf);

//     if(tmc_write(":CALC:FFT:HSC?") != 14)
//     {
//       line = __LINE__;
//       goto GDS_OUT_ERROR;
//     }
//
//     if(tmc_read() < 1)
//     {
//       line = __LINE__;
//       goto GDS_OUT_ERROR;
//     }
//
//     switch(atoi(device->buf))
//     {
// //       case  0: devparms->math_fft_hscale = devparms->current_screen_sf / 80.0;
// //                break;
//       case  1: devparms->math_fft_hscale = devparms->current_screen_sf / 40.0;
//                break;
//       case  2: devparms->math_fft_hscale = devparms->current_screen_sf / 80.0;
//                break;
//       case  3: devparms->math_fft_hscale = devparms->current_screen_sf / 200.0;
//                break;
//       default: devparms->math_fft_hscale = devparms->current_screen_sf / 40.0;
//                break;
//     }
  }
  else
  {
    if(tmc_write(":MATH:FFT:HSC?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->math_fft_hscale = atof(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:HCEN?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":MATH:FFT:HCEN?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_fft_hcenter = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:VOFF?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->fft_voffset = atof(device->buf);
  }
  else
  {
    if(tmc_write(":MATH:OFFS?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->fft_voffset = atof(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":CALC:FFT:VSC?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(devparms->math_fft_unit == 1)
    {
      devparms->fft_vscale = atof(device->buf);
    }
    else
    {
      devparms->fft_vscale = atof(device->buf) * devparms->chanscale[devparms->math_fft_src];
    }
  }
  else
  {
    if(tmc_write(":MATH:SCAL?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->fft_vscale = atof(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:MODE?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:MODE?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "PAR"))
  {
    devparms->math_decode_mode = 0;
  }
  else if(!strcmp(device->buf, "UART"))
    {
      devparms->math_decode_mode = 1;
    }
    else if(!strcmp(device->buf, "RS232"))
      {
        devparms->math_decode_mode = 1;
      }
      else if(!strcmp(device->buf, "SPI"))
        {
          devparms->math_decode_mode = 2;
        }
        else if(!strcmp(device->buf, "IIC"))
          {
            devparms->math_decode_mode = 3;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:DISP?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:DISP?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_display = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:FORM?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:FORM?") != 11)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "HEX"))
  {
    devparms->math_decode_format = 0;
  }
  else if(!strcmp(device->buf, "ASC"))
    {
      devparms->math_decode_format = 1;
    }
    else if(!strcmp(device->buf, "DEC"))
      {
        devparms->math_decode_format = 2;
      }
      else if(!strcmp(device->buf, "BIN"))
        {
          devparms->math_decode_format = 3;
        }
        else if(!strcmp(device->buf, "LINE"))
          {
            devparms->math_decode_format = 4;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:OFFS?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:POS?") != 10)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_pos = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:MISO:THR?") != 19)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:THRE:CHAN1?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_threshold[0] = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:MOSI:THR?") != 19)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:THRE:CHAN2?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_threshold[1] = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SCLK:THR?") != 19)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:THRE:CHAN3?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_threshold[2] = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SS:THR?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:THRE:CHAN4?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_threshold[3] = atof(device->buf);

  if(devparms->modelserie != 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":BUS1:RS232:TTHR?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

//    devparms->math_decode_threshold_uart_tx = atof(device->buf);
    devparms->math_decode_threshold_uart_tx = atof(device->buf) * 10.0;  // hack for firmware bug!

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":BUS1:RS232:RTHR?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

//    devparms->math_decode_threshold_uart_rx = atof(device->buf);
    devparms->math_decode_threshold_uart_rx = atof(device->buf) * 10.0;  // hack for firmware bug!
  }

  if(devparms->modelserie == 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":DEC1:THRE:AUTO?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->math_decode_threshold_auto = atoi(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:RX?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:RX?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_uart_rx = 1;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_uart_rx = 2;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_uart_rx = 3;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_uart_rx = 4;
        }
        else if(!strcmp(device->buf, "OFF"))
          {
            devparms->math_decode_uart_rx = 0;
          }
          else
          {
            devparms->math_decode_uart_rx = 0;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:TX?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:TX?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_uart_tx = 1;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_uart_tx = 2;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_uart_tx = 3;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_uart_tx = 4;
        }
        else if(!strcmp(device->buf, "OFF"))
          {
            devparms->math_decode_uart_tx = 0;
          }
          else
          {
            devparms->math_decode_uart_tx = 0;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:POL?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:POL?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "POS"))
  {
    devparms->math_decode_uart_pol = 1;
  }
  else if(!strcmp(device->buf, "NEG"))
    {
      devparms->math_decode_uart_pol = 0;
    }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:END?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:END?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "MSB"))
  {
    devparms->math_decode_uart_pol = 1;
  }
  else if(!strcmp(device->buf, "LSB"))
    {
      devparms->math_decode_uart_pol = 0;
    }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:BAUD?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:BAUD?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

//FIXME  DEC1:UART:BAUD? can return also "USER" instead of a number!
  devparms->math_decode_uart_baud = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:DBIT?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:WIDT?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_uart_width = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:SBIT?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:STOP?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "1"))
  {
    devparms->math_decode_uart_stop = 0;
  }
  else if(!strcmp(device->buf, "1.5"))
    {
      devparms->math_decode_uart_stop = 1;
    }
    else if(!strcmp(device->buf, "2"))
      {
        devparms->math_decode_uart_stop = 2;
      }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:RS232:PAR?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:UART:PAR?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "ODD"))
  {
    devparms->math_decode_uart_par = 1;
  }
  else if(!strcmp(device->buf, "EVEN"))
    {
      devparms->math_decode_uart_par = 2;
    }
    else if(!strcmp(device->buf, "NONE"))
      {
        devparms->math_decode_uart_par = 0;
      }
      else
      {
        devparms->math_decode_uart_par = 0;
      }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SCLK:SOUR?") != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:CLK?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_spi_clk = 0;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_spi_clk = 1;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_spi_clk = 2;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_spi_clk = 3;
        }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:MISO:SOUR?") != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:MISO?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_spi_miso = 1;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_spi_miso = 2;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_spi_miso = 3;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_spi_miso = 4;
        }
        else if(!strcmp(device->buf, "OFF"))
          {
            devparms->math_decode_spi_miso = 0;
          }
          else
          {
            devparms->math_decode_spi_miso = 0;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:MOSI:SOUR?") != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:MOSI?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_spi_mosi = 1;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_spi_mosi = 2;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_spi_mosi = 3;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_spi_mosi = 4;
        }
        else if(!strcmp(device->buf, "OFF"))
          {
            devparms->math_decode_spi_mosi = 0;
          }
          else
          {
            devparms->math_decode_spi_mosi = 0;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SS:SOUR?") != 18)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:CS?") != 13)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms->math_decode_spi_cs = 1;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms->math_decode_spi_cs = 2;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms->math_decode_spi_cs = 3;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms->math_decode_spi_cs = 4;
        }
        else if(!strcmp(device->buf, "OFF"))
          {
            devparms->math_decode_spi_cs = 0;
          }
          else
          {
            devparms->math_decode_spi_cs = 0;
          }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SS:POL?") != 17)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:SEL?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "NCS"))
  {
    devparms->math_decode_spi_select = 0;
  }
  else if(!strcmp(device->buf, "CS"))
    {
      devparms->math_decode_spi_select = 1;
    }
    else if(!strcmp(device->buf, "NEG"))
    {
      devparms->math_decode_spi_select = 0;
    }
    else if(!strcmp(device->buf, "POS"))
      {
        devparms->math_decode_spi_select = 1;
      }

  if(devparms->modelserie == 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":DEC1:SPI:MODE?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "TIM"))
    {
      devparms->math_decode_spi_mode = 0;
    }
    else if(!strcmp(device->buf, "CS"))
      {
        devparms->math_decode_spi_mode = 1;
      }
  }

  if(devparms->modelserie == 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":DEC1:SPI:TIM?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->math_decode_spi_timeout = atof(device->buf);
  }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:MOSI:POL?") != 19)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:POL?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "NEG"))
  {
    devparms->math_decode_spi_pol = 0;
  }
  else if(!strcmp(device->buf, "POS"))
    {
      devparms->math_decode_spi_pol = 1;
    }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:SCLK:SLOP?") != 20)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:EDGE?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "NEG"))
  {
    devparms->math_decode_spi_edge = 0;
  }
  else if(!strcmp(device->buf, "POS"))
    {
      devparms->math_decode_spi_edge = 1;
    }
    else if(!strcmp(device->buf, "FALL"))
      {
        devparms->math_decode_spi_edge = 0;
      }
      else if(!strcmp(device->buf, "RISE"))
        {
          devparms->math_decode_spi_edge = 1;
        }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:DBIT?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:WIDT?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  devparms->math_decode_spi_width = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie != 1)
  {
    if(tmc_write(":BUS1:SPI:END?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }
  else
  {
    if(tmc_write(":DEC1:SPI:END?") != 14)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto GDS_OUT_ERROR;
  }

  if(!strcmp(device->buf, "LSB"))
  {
    devparms->math_decode_spi_end = 0;
  }
  else if(!strcmp(device->buf, "MSB"))
    {
      devparms->math_decode_spi_end = 1;
    }

  usleep(TMC_GDS_DELAY);

  if(devparms->modelserie == 1)
  {
    if(tmc_write(":FUNC:WREC:ENAB?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "1"))
    {
      devparms->func_wrec_enable = 1;
    }
    else if(!strcmp(device->buf, "0"))
      {
        devparms->func_wrec_enable = 0;
      }
      else
      {
        line = __LINE__;
        goto GDS_OUT_ERROR;
      }
  }
  else
  {
    if(tmc_write(":FUNC:WRM?") != 10)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(!strcmp(device->buf, "REC"))
    {
      devparms->func_wrec_enable = 1;
    }
    else if(!strcmp(device->buf, "PLAY"))
      {
        devparms->func_wrec_enable = 2;
      }
      else if(!strcmp(device->buf, "OFF"))
        {
          devparms->func_wrec_enable = 0;
        }
        else
        {
          line = __LINE__;
          goto GDS_OUT_ERROR;
        }
  }

  if(devparms->func_wrec_enable)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREC:FEND?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wrec_fend = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREC:FMAX?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wrec_fmax = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREC:FINT?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wrec_fintval = atof(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FST?") != 15)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wplay_fstart = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FEND?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wplay_fend = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FMAX?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wplay_fmax = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FINT?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wplay_fintval = atof(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FCUR?") != 16)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto GDS_OUT_ERROR;
    }

    devparms->func_wplay_fcur = atoi(device->buf);
  }

  err_num = 0;

  return;

GDS_OUT_ERROR:

  snprintf(err_str, 4095,
           "An error occurred while reading settings from device.\n"
           "File %s line %i", __FILE__, line);

  err_num = -1;

  return;
}

















