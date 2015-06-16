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




void UI_Mainwindow::navDialChanged(int npos)
{
  int mpr = 1;

  double val, lefttime, righttime, delayrange;

  scrn_timer->stop();

  if(navDial->isSliderDown() == true)
  {
    navDial_timer->start(100);
  }
  else
  {
    navDial_timer->start(300);
  }

  if(npos > 93)
    {
      mpr = 64;
    }
    else if(npos > 86)
      {
        mpr = 32;
      }
      else if(npos > 79)
        {
          mpr = 16;
        }
        else if(npos > 72)
          {
            mpr = 8;
          }
          else if(npos > 65)
            {
              mpr = 4;
            }
            else if(npos > 58)
              {
                mpr = 2;
              }
              else if(npos > 51)
                {
                  mpr = 1;
                }
                else if(npos > 49)
                  {
                    return;
                  }
                  else if(npos > 42)
                    {
                      mpr = -1;
                    }
                    else if(npos > 35)
                      {
                        mpr = -2;
                      }
                      else if(npos > 28)
                        {
                          mpr = -4;
                        }
                        else if(npos > 21)
                          {
                            mpr = -8;
                          }
                          else if(npos > 14)
                            {
                              mpr = -16;
                            }
                            else if(npos > 7)
                              {
                                mpr = -32;
                              }
                              else
                              {
                                mpr = -64;
                              }

  if(navDialFunc == NAV_DIAL_FUNC_HOLDOFF)
  {
    adjdial_timer->start(ADJDIAL_TIMER_IVAL_2);

    val = get_stepsize_divide_by_1000(devparms.triggerholdoff);

    devparms.triggerholdoff += (val * mpr);

    if(devparms.triggerholdoff < 1e-7)
    {
      devparms.triggerholdoff = 1e-7;
    }

    if(devparms.triggerholdoff > 10)
    {
      devparms.triggerholdoff = 10;
    }
  }
  else if(devparms.timebasedelayenable)
    {
      val = devparms.timebasedelayoffset;

      if(val < 0)
      {
        val *= -1;
      }

      if(val < 2e-7)
      {
        val = 2e-7;
      }

      val = get_stepsize_divide_by_1000(val);

      devparms.timebasedelayoffset += (val * mpr);

      lefttime = ((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset;

      righttime = ((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset;

      delayrange = (devparms.hordivisions / 2) * devparms.timebasedelayscale;

      if(devparms.timebasedelayoffset < -(lefttime - delayrange))
      {
        devparms.timebasedelayoffset = -(lefttime - delayrange);
      }

      if(devparms.timebasedelayoffset > (righttime - delayrange))
      {
        devparms.timebasedelayoffset = (righttime - delayrange);
      }
    }

    waveForm->update();
}


void UI_Mainwindow::navDialReleased()
{
  char str[512];

  navDial->setSliderPosition(50);

  if(navDialFunc == NAV_DIAL_FUNC_HOLDOFF)
  {
    strcpy(str, "Trigger holdoff: ");

    convert_to_metric_suffix(str + strlen(str), devparms.triggerholdoff, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TRIG:HOLD %e", devparms.triggerholdoff);

    tmcdev_write(device, str);
  }
  else if(devparms.timebasedelayenable)
    {
      strcpy(str, "Delayed timebase position: ");

      convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

      strcat(str, "s");

      statusLabel->setText(str);

      sprintf(str, ":TIM:DEL:OFFS %e", devparms.timebasedelayoffset);

      tmcdev_write(device, str);
    }

  adjDialFunc = ADJ_DIAL_FUNC_NONE;

  if(devparms.screenupdates_on == 1)
  {
    scrn_timer->start(SCREEN_TIMER_IVAL);
  }

  waveForm->update();
}


void UI_Mainwindow::adjDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir;

  if(adjDialFunc == ADJ_DIAL_FUNC_NONE)
  {
    return;
  }

  scrn_timer->stop();

  adjdial_timer->start(ADJDIAL_TIMER_IVAL_2);

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  if(adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF)
  {
    if(!dir)
    {
      if(devparms.triggerholdoff >= 10)
      {
        devparms.triggerholdoff = 10;

        old_pos = new_pos;

        return;
      }

      devparms.triggerholdoff += get_stepsize_divide_by_1000(devparms.triggerholdoff);
    }
    else
    {
      if(devparms.triggerholdoff <= 1e-7)
      {
        devparms.triggerholdoff = 1e-7;

        old_pos = new_pos;

        return;
      }

      devparms.triggerholdoff -= get_stepsize_divide_by_1000(devparms.triggerholdoff);
    }
  }
  else if(adjDialFunc == ADJ_DIAL_FUNC_ACQ_AVG)
    {
      if(!dir)
      {
        if(devparms.acquireaverages >= 8192)
        {
          devparms.acquireaverages = 8192;

          old_pos = new_pos;

          return;
        }

        devparms.acquireaverages *= 2;
      }
      else
      {
        if(devparms.acquireaverages <= 2)
        {
          devparms.acquireaverages = 2;

          old_pos = new_pos;

          return;
        }

        devparms.acquireaverages /= 2;
      }
    }

  old_pos = new_pos;

  waveForm->update();
}


void UI_Mainwindow::trigAdjustDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir, chn;

  char str[512];

  if(devparms.activechannel < 0)
  {
    return;
  }

  chn = devparms.triggeredgesource;

  if((chn < 0) || (chn > 3))
  {
    return;
  }

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  if(dir)
  {
    if(devparms.triggeredgelevel[chn] <= (-6 * devparms.chanscale[chn]))
    {
      devparms.triggeredgelevel[chn] = -6 * devparms.chanscale[chn];

      old_pos = new_pos;

      return;
    }

    devparms.triggeredgelevel[chn] -= devparms.chanscale[chn] / 50;
  }
  else
  {
    if(devparms.triggeredgelevel[chn] >= (6 * devparms.chanscale[chn]))
    {
      devparms.triggeredgelevel[chn] = 6 * devparms.chanscale[chn];

      old_pos = new_pos;

      return;
    }

    devparms.triggeredgelevel[chn] += devparms.chanscale[chn] / 50;
  }

  strcpy(str, "Trigger level: ");

  convert_to_metric_suffix(str + strlen(str), devparms.triggeredgelevel[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":TRIG:EDG:LEV %e", devparms.triggeredgelevel[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;

  waveForm->update();
}


void UI_Mainwindow::horScaleDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir;

  char str[512];

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  if(devparms.timebasedelayenable)
  {
    if(dir)
    {
      if(devparms.timebasedelayscale >= devparms.timebasescale / 2)
      {
        devparms.timebasedelayscale = devparms.timebasescale / 2;

        old_pos = new_pos;

        return;
      }

      if(devparms.timebasedelayscale >= 0.1)
      {
        devparms.timebasedelayscale = 0.1;

        old_pos = new_pos;

        return;
      }
    }
    else
    {
      if(devparms.modelserie == 1)
      {
        if(devparms.timebasescale <= 5.001e-9)
        {
          devparms.timebasescale = 5e-9;

          old_pos = new_pos;

          return;
        }
      }
      else
      {
        if(devparms.bandwidth == 1000)
        {
          if(devparms.timebasedelayscale <= 5.001e-10)
          {
            devparms.timebasedelayscale = 5e-10;

            old_pos = new_pos;

            return;
          }
        }
        else
        {
          if(devparms.timebasedelayscale <= 1.001e-9)
          {
            devparms.timebasedelayscale = 1e-9;

            old_pos = new_pos;

            return;
          }
        }
      }
    }

    if(dir)
    {
      devparms.timebasedelayscale = round_up_step125(devparms.timebasedelayscale);
    }
    else
    {
      devparms.timebasedelayscale = round_down_step125(devparms.timebasedelayscale);
    }

    strcpy(str, "Delayed timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayscale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:SCAL %e", devparms.timebasedelayscale);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }
  else
  {
    if(dir)
    {
      if(devparms.timebasescale >= 10)
      {
        devparms.timebasescale = 10;

        old_pos = new_pos;

        return;
      }
    }
    else
    {
      if(devparms.modelserie == 1)
      {
        if(devparms.timebasescale <= 5.001e-9)
        {
          devparms.timebasescale = 5e-9;

          old_pos = new_pos;

          return;
        }
      }
      else
      {
        if(devparms.bandwidth == 1000)
        {
          if(devparms.timebasescale <= 5.001e-10)
          {
            devparms.timebasescale = 5e-10;

            old_pos = new_pos;

            return;
          }
        }
        else
        {
          if(devparms.timebasescale <= 1.001e-9)
          {
            devparms.timebasescale = 1e-9;

            old_pos = new_pos;

            return;
          }
        }
      }
    }

    if(dir)
    {
      devparms.timebasescale = round_up_step125(devparms.timebasescale);
    }
    else
    {
      devparms.timebasescale = round_down_step125(devparms.timebasescale);
    }

    strcpy(str, "Timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasescale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:SCAL %e", devparms.timebasescale);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }

  waveForm->update();
}


void UI_Mainwindow::horPosDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir;

  char str[512];

  if(devparms.activechannel < 0)
  {
    return;
  }

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  if(devparms.timebasedelayenable)
  {
    if(dir)
    {
      if(devparms.timebasedelayoffset >= (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
      {
        old_pos = new_pos;

        return;
      }

      devparms.timebasedelayoffset += (devparms.timebasedelayscale / 50);
    }
    else
    {
      if(devparms.timebasedelayoffset <= -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
      {
        old_pos = new_pos;

        return;
      }

      devparms.timebasedelayoffset -= (devparms.timebasedelayscale / 50);
    }

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:OFFS %e", devparms.timebasedelayoffset);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }
  else
  {
    if(dir)
    {
      if(devparms.timebaseoffset >= 1)
      {
        devparms.timebaseoffset = 1;

        old_pos = new_pos;

        return;
      }

      devparms.timebaseoffset += devparms.timebasescale / 50;
    }
    else
    {
      if(devparms.timebaseoffset <= -1)
      {
        devparms.timebaseoffset = -1;

        old_pos = new_pos;

        return;
      }

      devparms.timebaseoffset -= devparms.timebasescale / 50;
    }

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:OFFS %e", devparms.timebaseoffset);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }

  waveForm->update();
}


void UI_Mainwindow::vertOffsetDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir, chn;

  char str[512];

  double val;

  if(devparms.activechannel < 0)
  {
    return;
  }

  chn = devparms.activechannel;

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  val = round_up_step125(devparms.chanscale[chn]) / 100;

  if(dir)
  {
    if(devparms.chanoffset[chn] <= -20)
    {
      devparms.chanoffset[chn] = -20;

      old_pos = new_pos;

      return;
    }

    devparms.chanoffset[chn] -= val;
  }
  else
  {
    if(devparms.chanoffset[chn] >= 20)
    {
      devparms.chanoffset[chn] = 20;

      old_pos = new_pos;

      return;
    }

    devparms.chanoffset[chn] += val;
  }

  sprintf(str, "Channel %i offset: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms.chanoffset[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;

  waveForm->update();
}


void UI_Mainwindow::vertScaleDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir, chn;

  double val, ltmp;

  char str[512];

  if(devparms.activechannel < 0)
  {
    return;
  }

  chn = devparms.activechannel;

  diff = new_pos - old_pos;

  if(diff < 0)
  {
    diff *= -1;
  }

  if(diff < 6)
  {
    return;
  }

  if(new_pos > old_pos)
  {
    if(diff < 12)
    {
      dir = 0;
    }
    else
    {
      dir = 1;
    }
  }
  else
  {
    if(diff < 12)
    {
      dir = 1;
    }
    else
    {
      dir = 0;
    }
  }

  if(dir)
  {
    if(devparms.chanscale[chn] >= 20)
    {
      devparms.chanscale[chn] = 20;

      old_pos = new_pos;

      return;
    }
  }
  else
  {
    if(devparms.chanscale[chn] <= 1e-2)
    {
      devparms.chanscale[chn] = 1e-2;

      old_pos = new_pos;

      return;
    }
  }

  ltmp = devparms.chanscale[chn];

  if(dir || devparms.chanvernier[chn])
  {
    val = round_up_step125(devparms.chanscale[chn]);
  }
  else
  {
    val = round_down_step125(devparms.chanscale[chn]);
  }

  if(devparms.chanvernier[chn])
  {
    val /= 100;

    if(dir)
    {
      devparms.chanscale[chn] += val;
    }
    else
    {
      devparms.chanscale[chn] -= val;
    }
  }
  else
  {
    devparms.chanscale[chn] = val;
  }

  ltmp /= val;

  devparms.chanoffset[chn] /= ltmp;

  sprintf(str, "Channel %i scale: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanscale[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:SCAL %e", chn + 1, devparms.chanscale[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;

  waveForm->update();
}


void UI_Mainwindow::acqButtonClicked()
{
  int chn,
      chns_on=0,
      dual=0;

  QMenu menu,
        submenuacquisition,
        submenumemdepth;

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(devparms.chandisplay[chn])
    {
      chns_on++;
    }
  }

  if((devparms.chandisplay[0] && devparms.chandisplay[1]) || (devparms.chandisplay[2] && devparms.chandisplay[3]))
  {
    dual = 1;
  }

  submenuacquisition.setTitle("Mode");
  submenuacquisition.addAction("Normal",  this, SLOT(set_acq_normal()));
  submenuacquisition.addAction("Average", this, SLOT(set_acq_average()));
  submenuacquisition.addAction("Peak Detect",  this, SLOT(set_acq_peak()));
  submenuacquisition.addAction("High Resolution",  this, SLOT(set_acq_hres()));
  menu.addMenu(&submenuacquisition);

  submenumemdepth.setTitle("Mem Depth");
  submenumemdepth.addAction("Auto",  this, SLOT(set_memdepth_auto()));
  if(devparms.modelserie == 6)
  {
    if(!dual)
    {
      submenumemdepth.addAction("14K",  this, SLOT(set_memdepth_14k()));
      submenumemdepth.addAction("140K", this, SLOT(set_memdepth_140k()));
      submenumemdepth.addAction("1.4M", this, SLOT(set_memdepth_1400k()));
      submenumemdepth.addAction("14M",  this, SLOT(set_memdepth_14m()));
      submenumemdepth.addAction("140M", this, SLOT(set_memdepth_140m()));
    }
    else
    {
      submenumemdepth.addAction("7K",   this, SLOT(set_memdepth_7k()));
      submenumemdepth.addAction("70K",  this, SLOT(set_memdepth_70k()));
      submenumemdepth.addAction("700K", this, SLOT(set_memdepth_700k()));
      submenumemdepth.addAction("7M",   this, SLOT(set_memdepth_7m()));
      submenumemdepth.addAction("70M",  this, SLOT(set_memdepth_70m()));
    }
  }
  else if((devparms.modelserie == 2) || (devparms.modelserie == 4))
    {
      if(chns_on < 2)
      {
        submenumemdepth.addAction("14K",  this, SLOT(set_memdepth_14k()));
        submenumemdepth.addAction("140K", this, SLOT(set_memdepth_140k()));
        submenumemdepth.addAction("1.4M", this, SLOT(set_memdepth_1400k()));
        submenumemdepth.addAction("14M",  this, SLOT(set_memdepth_14m()));
        if(devparms.modelserie == 2)
        {
          submenumemdepth.addAction("56M", this, SLOT(set_memdepth_56m()));
        }
        else
        {
          submenumemdepth.addAction("140M", this, SLOT(set_memdepth_140m()));
        }
      }
      else
      {
        submenumemdepth.addAction("7K",   this, SLOT(set_memdepth_7k()));
        submenumemdepth.addAction("70K",  this, SLOT(set_memdepth_70k()));
        submenumemdepth.addAction("700K", this, SLOT(set_memdepth_700k()));
        submenumemdepth.addAction("7M",   this, SLOT(set_memdepth_7m()));
        if(devparms.modelserie == 2)
        {
          submenumemdepth.addAction("28M",  this, SLOT(set_memdepth_28m()));
        }
        else
        {
          submenumemdepth.addAction("70M",  this, SLOT(set_memdepth_70m()));
        }
      }
    }
    else if(devparms.modelserie == 1)
      {
      if(chns_on < 2)
      {
        submenumemdepth.addAction("12K",  this, SLOT(set_memdepth_12k()));
        submenumemdepth.addAction("120K", this, SLOT(set_memdepth_120k()));
        submenumemdepth.addAction("1.2M", this, SLOT(set_memdepth_1200k()));
        submenumemdepth.addAction("12M",  this, SLOT(set_memdepth_12m()));
        submenumemdepth.addAction("24M",  this, SLOT(set_memdepth_24m()));
      }
      else if(chns_on < 3)
        {
          submenumemdepth.addAction("6K",   this, SLOT(set_memdepth_6k()));
          submenumemdepth.addAction("60K",  this, SLOT(set_memdepth_60k()));
          submenumemdepth.addAction("600K", this, SLOT(set_memdepth_600k()));
          submenumemdepth.addAction("6M",   this, SLOT(set_memdepth_6m()));
          submenumemdepth.addAction("12M",  this, SLOT(set_memdepth_12m()));
        }
        else
        {
          submenumemdepth.addAction("3K",   this, SLOT(set_memdepth_3k()));
          submenumemdepth.addAction("30K",  this, SLOT(set_memdepth_30k()));
          submenumemdepth.addAction("300K", this, SLOT(set_memdepth_300k()));
          submenumemdepth.addAction("3M",   this, SLOT(set_memdepth_3m()));
          submenumemdepth.addAction("6M",   this, SLOT(set_memdepth_6m()));
        }
      }
  menu.addMenu(&submenumemdepth);

  menu.exec(acqButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::set_memdepth(int mdepth)
{
  char str[256];

  if(mdepth <= 0)
  {
    statusLabel->setText("Memory depth: auto");

    tmcdev_write(device, ":ACQ:MDEP AUTO");

    devparms.timebaseoffset = 0;

    usleep(20000);

    tmcdev_write(device, ":TIM:OFFS 0");

    return;
  }

  strcpy(str, "Memory depth: ");

  convert_to_metric_suffix(str + strlen(str), mdepth, 0);

  statusLabel->setText(str);

  sprintf(str, ":ACQ:MDEP %i", mdepth);

  tmcdev_write(device, str);

  devparms.timebaseoffset = 0;

  usleep(20000);

  tmcdev_write(device, ":TIM:OFFS 0");
}


void UI_Mainwindow::set_memdepth_auto()
{
  set_memdepth(0);
}


void UI_Mainwindow::set_memdepth_12k()
{
  set_memdepth(12000);
}


void UI_Mainwindow::set_memdepth_120k()
{
  set_memdepth(120000);
}


void UI_Mainwindow::set_memdepth_1200k()
{
  set_memdepth(1200000);
}


void UI_Mainwindow::set_memdepth_12m()
{
  set_memdepth(12000000);
}


void UI_Mainwindow::set_memdepth_24m()
{
  set_memdepth(24000000);
}


void UI_Mainwindow::set_memdepth_3k()
{
  set_memdepth(3000);
}


void UI_Mainwindow::set_memdepth_30k()
{
  set_memdepth(30000);
}


void UI_Mainwindow::set_memdepth_300k()
{
  set_memdepth(300000);
}


void UI_Mainwindow::set_memdepth_3m()
{
  set_memdepth(3000000);
}


void UI_Mainwindow::set_memdepth_6m()
{
  set_memdepth(6000000);
}


void UI_Mainwindow::set_memdepth_6k()
{
  set_memdepth(6000);
}


void UI_Mainwindow::set_memdepth_60k()
{
  set_memdepth(60000);
}


void UI_Mainwindow::set_memdepth_600k()
{
  set_memdepth(600000);
}


void UI_Mainwindow::set_memdepth_7k()
{
  set_memdepth(7000);
}


void UI_Mainwindow::set_memdepth_70k()
{
  set_memdepth(70000);
}


void UI_Mainwindow::set_memdepth_700k()
{
  set_memdepth(700000);
}


void UI_Mainwindow::set_memdepth_7m()
{
  set_memdepth(7000000);
}


void UI_Mainwindow::set_memdepth_70m()
{
  set_memdepth(70000000);
}


void UI_Mainwindow::set_memdepth_14k()
{
  set_memdepth(14000);
}


void UI_Mainwindow::set_memdepth_140k()
{
  set_memdepth(140000);
}


void UI_Mainwindow::set_memdepth_1400k()
{
  set_memdepth(1400000);
}


void UI_Mainwindow::set_memdepth_14m()
{
  set_memdepth(14000000);
}


void UI_Mainwindow::set_memdepth_140m()
{
  set_memdepth(140000000);
}


void UI_Mainwindow::set_memdepth_28m()
{
  set_memdepth(28000000);
}


void UI_Mainwindow::set_memdepth_56m()
{
  set_memdepth(56000000);
}


void UI_Mainwindow::set_acq_normal()
{
  if(devparms.acquiretype == 0)
  {
    return;
  }

  devparms.acquiretype = 0;

  statusLabel->setText("Acquire: normal");

  tmcdev_write(device, ":ACQ:TYPE NORM");
}


void UI_Mainwindow::set_acq_peak()
{
  if(devparms.acquiretype == 2)
  {
    return;
  }

  devparms.acquiretype = 2;

  statusLabel->setText("Acquire: peak");

  tmcdev_write(device, ":ACQ:TYPE PEAK");
}


void UI_Mainwindow::set_acq_hres()
{
  if(devparms.acquiretype == 3)
  {
    return;
  }

  devparms.acquiretype = 3;

  statusLabel->setText("Acquire: high resolution");

  tmcdev_write(device, ":ACQ:TYPE HRES");
}


void UI_Mainwindow::set_acq_average()
{
  adjDialFunc = ADJ_DIAL_FUNC_ACQ_AVG;

  adjDialLabel->setText("Averages");

  adjDialLabel->setStyleSheet("background: #66FF99; font: 7pt;");

  adjdial_timer->start(ADJDIAL_TIMER_IVAL_1);

  if(devparms.acquiretype == 1)
  {
    return;
  }

  devparms.acquiretype = 1;

  statusLabel->setText("Acquire: average");

  tmcdev_write(device, ":ACQ:TYPE AVER");
}


void UI_Mainwindow::cursButtonClicked()
{
}


void UI_Mainwindow::saveButtonClicked()
{
  QMenu menu;

  menu.addAction("Save screen waveform", this, SLOT(save_screen_waveform()));
  menu.addAction("Save memory waveform", this, SLOT(save_memory_waveform()));
  menu.addAction("Save screenshot",      this, SLOT(save_screenshot()));
  menu.addAction("Factory",              this, SLOT(set_to_factory()));

  menu.exec(saveButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::dispButtonClicked()
{
  QMenu menu,
        submenutype,
        submenugrid,
        submenugrading;

  submenutype.setTitle("Type");
  submenutype.addAction("Vectors", this, SLOT(set_grid_type_vectors()));
  submenutype.addAction("Dots",    this, SLOT(set_grid_type_dots()));
  menu.addMenu(&submenutype);

  submenugrid.setTitle("Grid");
  submenugrid.addAction("Full", this, SLOT(set_grid_full()));
  submenugrid.addAction("Half", this, SLOT(set_grid_half()));
  submenugrid.addAction("None", this, SLOT(set_grid_none()));
  menu.addMenu(&submenugrid);

  submenugrading.setTitle("Persistence");
  submenugrading.addAction("Minimum",  this, SLOT(set_grading_min()));
  submenugrading.addAction("0.05",     this, SLOT(set_grading_005()));
  submenugrading.addAction("0.1",      this, SLOT(set_grading_01()));
  submenugrading.addAction("0.2",      this, SLOT(set_grading_02()));
  submenugrading.addAction("0.5",      this, SLOT(set_grading_05()));
  submenugrading.addAction("1",        this, SLOT(set_grading_1()));
  submenugrading.addAction("2",        this, SLOT(set_grading_2()));
  submenugrading.addAction("5",        this, SLOT(set_grading_5()));
  submenugrading.addAction("10",       this, SLOT(set_grading_10()));
  submenugrading.addAction("20",       this, SLOT(set_grading_20()));
  submenugrading.addAction("Infinite", this, SLOT(set_grading_inf()));
  menu.addMenu(&submenugrading);

  menu.exec(dispButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::set_grid_type_vectors()
{
  if(!devparms.displaytype)
  {
    return;
  }

  devparms.displaytype = 0;

  statusLabel->setText("Display type: vectors");

  tmcdev_write(device, ":DISP:TYPE VECT");
}


void UI_Mainwindow::set_grid_type_dots()
{
  if(devparms.displaytype)
  {
    return;
  }

  devparms.displaytype = 1;

  statusLabel->setText("Display type: dots");

  tmcdev_write(device, ":DISP:TYPE DOTS");
}


void UI_Mainwindow::set_grid_full()
{
  if(devparms.displaygrid == 2)
  {
    return;
  }

  devparms.displaygrid = 2;

  statusLabel->setText("Display grid: full");

  tmcdev_write(device, ":DISP:GRID FULL");
}


void UI_Mainwindow::set_grid_half()
{
  if(devparms.displaygrid == 1)
  {
    return;
  }

  devparms.displaygrid = 1;

  statusLabel->setText("Display grid: half");

  tmcdev_write(device, ":DISP:GRID HALF");
}


void UI_Mainwindow::set_grid_none()
{
  if(devparms.displaygrid == 0)
  {
    return;
  }

  devparms.displaygrid = 0;

  statusLabel->setText("Display grid: none");

  tmcdev_write(device, ":DISP:GRID NONE");
}


void UI_Mainwindow::set_grading_min()
{
  statusLabel->setText("Display grading: Minimum");

  tmcdev_write(device, ":DISP:GRAD:TIME MIN");
}


void UI_Mainwindow::set_grading_005()
{
  statusLabel->setText("Display grading: 0.05 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 0.05");
}


void UI_Mainwindow::set_grading_01()
{
  statusLabel->setText("Display grading: 0.1 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 0.1");
}


void UI_Mainwindow::set_grading_02()
{
  statusLabel->setText("Display grading: 0.2 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 0.2");
}


void UI_Mainwindow::set_grading_05()
{
  statusLabel->setText("Display grading: 0.5 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 0.5");
}


void UI_Mainwindow::set_grading_1()
{
  statusLabel->setText("Display grading: 1 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 1");
}


void UI_Mainwindow::set_grading_2()
{
  statusLabel->setText("Display grading: 2 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 2");
}


void UI_Mainwindow::set_grading_5()
{
  statusLabel->setText("Display grading: 5 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 5");
}


void UI_Mainwindow::set_grading_10()
{
  statusLabel->setText("Display grading: 10 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 10");
}


void UI_Mainwindow::set_grading_20()
{
  statusLabel->setText("Display grading: 20 Sec.");

  tmcdev_write(device, ":DISP:GRAD:TIME 20");
}


void UI_Mainwindow::set_grading_inf()
{
  statusLabel->setText("Display grading: Infinite");

  tmcdev_write(device, ":DISP:GRAD:TIME INF");
}


void UI_Mainwindow::utilButtonClicked()
{
}


void UI_Mainwindow::helpButtonClicked()
{
  show_howto_operate();
}


void UI_Mainwindow::show_howto_operate()
{
  QMessageBox msgBox;
  msgBox.setText(
    "Use the mousewheel to change the dials. In order to simulate a push on a dial,"
    "click on it with the right mouse button.\n"
    "To toggle the delayed timebase, right-click on the timebase dial.\n"
    "To set the horizontal position to zero, right-click on the horizontal position dial.\n"
    "To set the vertical offset to zero, right-click on the vertical position dial.\n\n"
    "In addition of using the dials to change the scale and offset of the traces and the trigger position,"
    "you can use the mouse to drag the colored arrows aside of the plot.\n\n"
    "Keyboard shortcuts:\n"
    "PageUp: move trace 12 (or 14) divisions to the right.\n"
    "PageDn: move trace 12 (or 14) divisions to the left.\n"
    "Arrow left: move trace 1 division to the right.\n"
    "Arrow right: move trace 1 division to the left.\n"
    "Zoom In (decrease timebase): Ctl+\n"
    "Zoom Out (increase timebase): Ctl-\n"
    "Increase vertical scale: -\n"
    "Decrease vertical scale: +\n"
    );

  msgBox.exec();
}


void UI_Mainwindow::show_about_dialog()
{
  UI_Aboutwindow aboutwindow;
}


void UI_Mainwindow::vertScaleDialClicked(QPoint)
{
  int chn;

  char str[512];

  if(devparms.activechannel < 0)
  {
    return;
  }

  chn = devparms.activechannel;

  if(devparms.chanvernier[chn])
  {
    devparms.chanvernier[chn] = 0;

    sprintf(str, "Channel %i vernier: off", chn + 1);

    statusLabel->setText(str);

    sprintf(str, ":CHAN%i:VERN 0", chn + 1);

    tmcdev_write(device, str);
  }
  else
  {
    devparms.chanvernier[chn] = 1;

    sprintf(str, "Channel %i vernier: on", chn + 1);

    statusLabel->setText(str);

    sprintf(str, ":CHAN%i:VERN 1", chn + 1);

    tmcdev_write(device, str);
  }
}


void UI_Mainwindow::ch1ButtonClicked()
{
  if(devparms.chandisplay[0])
  {
    if(devparms.activechannel == 0)
    {
      devparms.chandisplay[0] = 0;

      statusLabel->setText("Channel 1 off");

      tmcdev_write(device, ":CHAN1:DISP 0");

      ch1Button->setStyleSheet(def_stylesh);

      devparms.activechannel = -1;

      for(int i=0; i<MAX_CHNS; i++)
      {
        if(devparms.chandisplay[i])
        {
          devparms.activechannel = i;

          break;
        }
      }
    }
    else
    {
      devparms.activechannel = 0;
    }
  }
  else
  {
    devparms.chandisplay[0] = 1;

    statusLabel->setText("Channel 1 on");

    tmcdev_write(device, ":CHAN1:DISP 1");

    ch1Button->setStyleSheet("background: #FFFF33;");

    devparms.activechannel = 0;
  }
}


void UI_Mainwindow::ch2ButtonClicked()
{
  if(devparms.chandisplay[1])
  {
    if(devparms.activechannel == 1)
    {
      devparms.chandisplay[1] = 0;

      statusLabel->setText("Channel 2 off");

      tmcdev_write(device, ":CHAN2:DISP 0");

      ch2Button->setStyleSheet(def_stylesh);

      devparms.activechannel = -1;

      for(int i=0; i<MAX_CHNS; i++)
      {
        if(devparms.chandisplay[i])
        {
          devparms.activechannel = i;

          break;
        }
      }
    }
    else
    {
      devparms.activechannel = 1;
    }
  }
  else
  {
    devparms.chandisplay[1] = 1;

    statusLabel->setText("Channel 2 on");

    tmcdev_write(device, ":CHAN2:DISP 1");

    ch2Button->setStyleSheet("background: #33FFFF;");

    devparms.activechannel = 1;
  }
}


void UI_Mainwindow::ch3ButtonClicked()
{
  if(devparms.chandisplay[2])
  {
    if(devparms.activechannel == 2)
    {
      devparms.chandisplay[2] = 0;

      statusLabel->setText("Channel 3 off");

      tmcdev_write(device, ":CHAN3:DISP 0");

      ch3Button->setStyleSheet(def_stylesh);

      devparms.activechannel = -1;

      for(int i=0; i<MAX_CHNS; i++)
      {
        if(devparms.chandisplay[i])
        {
          devparms.activechannel = i;

          break;
        }
      }
    }
    else
    {
      devparms.activechannel = 2;
    }
  }
  else
  {
    devparms.chandisplay[2] = 1;

    statusLabel->setText("Channel 3 on");

    tmcdev_write(device, ":CHAN3:DISP 1");

    ch3Button->setStyleSheet("background: #FF33FF;");

    devparms.activechannel = 2;
  }
}


void UI_Mainwindow::ch4ButtonClicked()
{
  if(devparms.chandisplay[3])
  {
    if(devparms.activechannel == 3)
    {
      devparms.chandisplay[3] = 0;

      statusLabel->setText("Channel 4 off");

      tmcdev_write(device, ":CHAN4:DISP 0");

      ch4Button->setStyleSheet(def_stylesh);

      devparms.activechannel = -1;

      for(int i=0; i<MAX_CHNS; i++)
      {
        if(devparms.chandisplay[i])
        {
          devparms.activechannel = i;

          break;
        }
      }
    }
    else
    {
      devparms.activechannel = 3;
    }
  }
  else
  {
    devparms.chandisplay[3] = 1;

    statusLabel->setText("Channel 4 on");

    tmcdev_write(device, ":CHAN4:DISP 1");

    ch4Button->setStyleSheet("background: #0066CC;");

    devparms.activechannel = 3;
  }
}


void UI_Mainwindow::chan_menu()
{
  QMenu menu,
        submenubwl,
        submenucoupling,
        submenuinvert;

  if((devparms.activechannel < 0) || (devparms.activechannel > MAX_CHNS))
  {
    return;
  }

  submenucoupling.setTitle("Coupling");
  submenucoupling.addAction("AC",  this, SLOT(chan_coupling_ac()));
  submenucoupling.addAction("DC",  this, SLOT(chan_coupling_dc()));
  submenucoupling.addAction("GND", this, SLOT(chan_coupling_gnd()));
  menu.addMenu(&submenucoupling);

  submenubwl.setTitle("BWL");
  submenubwl.addAction("Off",    this, SLOT(chan_bwl_off()));
  submenubwl.addAction("20MHz",  this, SLOT(chan_bwl_20()));
  submenubwl.addAction("250MHz", this, SLOT(chan_bwl_250()));
  menu.addMenu(&submenubwl);

  submenuinvert.setTitle("Invert");
  submenuinvert.addAction("On",  this, SLOT(chan_invert_on()));
  submenuinvert.addAction("Off", this, SLOT(chan_invert_off()));
  menu.addMenu(&submenuinvert);

  menu.exec(chanMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::chan_coupling_ac()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 2;

  sprintf(str, "Channel %i coupling: AC", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP AC", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_coupling_dc()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 1;

  sprintf(str, "Channel %i coupling: DC", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP DC", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_coupling_gnd()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 0;

  sprintf(str, "Channel %i coupling: GND", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP GND", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_off()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 0;

  sprintf(str, "Channel %i bandwidth limit: Off", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL OFF", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_20()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 20;

  sprintf(str, "Channel %i bandwidth limit: 20MHz", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL 20M", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_250()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 250;

  sprintf(str, "Channel %i bandwidth limit: 250MHz", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL 250M", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_invert_on()
{
  char str[128];

  if(!devparms.chaninvert[devparms.activechannel])
  {
    devparms.triggeredgelevel[devparms.activechannel] *= -1;
  }

  devparms.chaninvert[devparms.activechannel] = 1;

  sprintf(str, "Channel %i inverted: On", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:INV 1", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_invert_off()
{
  char str[128];

  if(devparms.chaninvert[devparms.activechannel])
  {
    devparms.triggeredgelevel[devparms.activechannel] *= -1;
  }

  devparms.chaninvert[devparms.activechannel] = 0;

  sprintf(str, "Channel %i inverted: Off", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:INV 0", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::vertOffsetDialClicked(QPoint)
{
//   QMenu menu;
//
//   menu.addAction("Zero", this, SLOT(vertical_position_zero()));
//
//   menu.exec(vertOffsetDial->mapToGlobal(QPoint(0,0)));
  int chn;

  char str[512];

  if(devparms.activechannel < 0)
  {
    return;
  }

  chn = devparms.activechannel;

  devparms.chanoffset[chn] = 0;

  sprintf(str, "Channel %i offset: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms.chanoffset[chn]);

  tmcdev_write(device, str);
}


void UI_Mainwindow::clearButtonClicked()
{
  statusLabel->setText("Display cleared");

  tmcdev_write(device, ":DISP:CLE");
}


void UI_Mainwindow::autoButtonClicked()
{
  statusLabel->setText("Auto settings");

  tmcdev_write(device, ":AUT");

  qApp->processEvents();

  usleep(20000);

  qApp->processEvents();

  sleep(2);

  get_device_settings();
}


void UI_Mainwindow::runButtonClicked()
{
  if(devparms.triggerstatus == 5)
  {
    statusLabel->setText("Trigger: run");

    tmcdev_write(device, ":RUN");
  }
  else
  {
    statusLabel->setText("Trigger: stop");

    tmcdev_write(device, ":STOP");
  }
}


void UI_Mainwindow::singleButtonClicked()
{
  statusLabel->setText("Trigger: single");

  tmcdev_write(device, ":SING");
}


void UI_Mainwindow::adjustDialClicked(QPoint)
{
  if(adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF)
  {
    if(devparms.modelserie == 1)
    {
      devparms.triggerholdoff = 1.6e-8;

      statusLabel->setText("Holdoff: 16ns");
    }
    else
    {
      devparms.triggerholdoff = 1e-7;

      statusLabel->setText("Holdoff: 100ns");
    }
  }
}


void UI_Mainwindow::horMenuButtonClicked()
{
  QMenu menu,
        submenudelayed;

  submenudelayed.setTitle("Delayed");
  submenudelayed.addAction("On",  this, SLOT(horizontal_delayed_on()));
  submenudelayed.addAction("Off", this, SLOT(horizontal_delayed_off()));
  menu.addMenu(&submenudelayed);

  menu.exec(horMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::horizontal_delayed_on()
{
  if(devparms.timebasedelayenable)
  {
    return;
  }

  devparms.timebasedelayenable = 1;

  statusLabel->setText("Delayed timebase enabled");

  tmcdev_write(device, ":TIM:DEL:ENAB 1");

  devparms.timebasedelayoffset = devparms.timebaseoffset;
}


void UI_Mainwindow::horizontal_delayed_off()
{
  if(!devparms.timebasedelayenable)
  {
    return;
  }

  devparms.timebasedelayenable = 0;

  statusLabel->setText("Delayed timebase disabled");

  tmcdev_write(device, ":TIM:DEL:ENAB 0");
}


void UI_Mainwindow::horizontal_delayed_toggle()
{
  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayenable = 0;

    statusLabel->setText("Delayed timebase disabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 0");
  }
  else
  {
    scrn_timer->stop();

    devparms.timebasedelayenable = 1;

    statusLabel->setText("Delayed timebase enabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 1");

    tmcdev_write(device, ":TIM:DEL:OFFS?");

    tmcdev_read(device);

    devparms.timebasedelayoffset = atof(device->buf);

    tmcdev_write(device, ":TIM:DEL:SCAL?");

    tmcdev_read(device);

    devparms.timebasedelayscale = atof(device->buf);

    scrn_timer->start(SCREEN_TIMER_IVAL);
  }
}


void UI_Mainwindow::horPosDialClicked(QPoint)
{
  char str[512];

  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayoffset = devparms.timebaseoffset;

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:OFFS %e", devparms.timebasedelayoffset);

    tmcdev_write(device, str);
  }
  else
  {
    devparms.timebaseoffset = 0;

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:OFFS %e", devparms.timebaseoffset);

    tmcdev_write(device, str);
  }
}


void UI_Mainwindow::horScaleDialClicked(QPoint)
{
  horizontal_delayed_toggle();
}


void UI_Mainwindow::measureButtonClicked()
{
  QMenu menu,
        submenucounter;

  submenucounter.setTitle("Counter");
  submenucounter.addAction("OFF", this, SLOT(counter_off()));
  submenucounter.addAction("CH1", this, SLOT(counter_ch1()));
  if(devparms.channel_cnt > 1)
  {
    submenucounter.addAction("CH2", this, SLOT(counter_ch2()));
  }
  if(devparms.channel_cnt > 2)
  {
    submenucounter.addAction("CH3", this, SLOT(counter_ch3()));
  }
  if(devparms.channel_cnt > 3)
  {
    submenucounter.addAction("CH4", this, SLOT(counter_ch4()));
  }

  menu.addMenu(&submenucounter);

  menu.exec(measureButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::counter_off()
{
  devparms.countersrc = 0;

  statusLabel->setText("Freq. counter off");

  tmcdev_write(device, ":MEAS:COUN:SOUR OFF");
}


void UI_Mainwindow::counter_ch1()
{
  devparms.countersrc = 1;

  statusLabel->setText("Freq. counter channel 1");

  tmcdev_write(device, ":MEAS:COUN:SOUR CHAN1");
}


void UI_Mainwindow::counter_ch2()
{
  devparms.countersrc = 2;

  statusLabel->setText("Freq. counter channel 2");

  tmcdev_write(device, ":MEAS:COUN:SOUR CHAN2");
}


void UI_Mainwindow::counter_ch3()
{
  devparms.countersrc = 3;

  statusLabel->setText("Freq. counter channel 3");

  tmcdev_write(device, ":MEAS:COUN:SOUR CHAN3");
}


void UI_Mainwindow::counter_ch4()
{
  devparms.countersrc = 4;

  statusLabel->setText("Freq. counter channel 4");

  tmcdev_write(device, ":MEAS:COUN:SOUR CHAN4");
}


void UI_Mainwindow::trigModeButtonClicked()
{
  devparms.triggersweep++;

  devparms.triggersweep %= 3;

  switch(devparms.triggersweep)
  {
    case 0: trigModeAutoLed->setValue(true);
            trigModeSingLed->setValue(false);
            statusLabel->setText("Trigger auto");
            tmcdev_write(device, ":TRIG:SWE AUTO");
            break;
    case 1: trigModeNormLed->setValue(true);
            trigModeAutoLed->setValue(false);
            statusLabel->setText("Trigger norm");
            tmcdev_write(device, ":TRIG:SWE NORM");
            break;
    case 2: trigModeSingLed->setValue(true);
            trigModeNormLed->setValue(false);
            statusLabel->setText("Trigger single");
            tmcdev_write(device, ":TRIG:SWE SING");
            break;
  }
}


void UI_Mainwindow::trigMenuButtonClicked()
{
  QMenu menu,
        submenusource,
        submenuslope,
        submenucoupling,
        submenusetting;

  submenusource.setTitle("Source");
  submenusource.addAction("CH1", this, SLOT(trigger_source_ch1()));
  if(devparms.channel_cnt > 1)
  {
    submenusource.addAction("CH2", this, SLOT(trigger_source_ch2()));
  }
  if(devparms.channel_cnt > 2)
  {
    submenusource.addAction("CH3", this, SLOT(trigger_source_ch3()));
  }
  if(devparms.channel_cnt > 3)
  {
    submenusource.addAction("CH4", this, SLOT(trigger_source_ch4()));
  }
  submenusource.addAction("EXT", this, SLOT(trigger_source_ext()));
  submenusource.addAction("EXT/ 5", this, SLOT(trigger_source_ext5()));
  submenusource.addAction("AC Line", this, SLOT(trigger_source_acl()));
  menu.addMenu(&submenusource);

  submenucoupling.setTitle("Coupling");
  submenucoupling.addAction("DC", this, SLOT(trigger_coupling_dc()));
  submenucoupling.addAction("AC", this, SLOT(trigger_coupling_ac()));
  submenucoupling.addAction("LF reject", this, SLOT(trigger_coupling_lfreject()));
  submenucoupling.addAction("HF reject", this, SLOT(trigger_coupling_hfreject()));
  menu.addMenu(&submenucoupling);

  submenuslope.setTitle("Slope");
  submenuslope.addAction("Positive", this, SLOT(trigger_slope_pos()));
  submenuslope.addAction("Negative", this, SLOT(trigger_slope_neg()));
  submenuslope.addAction("Rise/Fal", this, SLOT(trigger_slope_rfal()));
  menu.addMenu(&submenuslope);

  submenusetting.setTitle("Setting");
  submenusetting.addAction("Holdoff", this, SLOT(trigger_setting_holdoff()));
  menu.addMenu(&submenusetting);

  menu.exec(trigMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::trigger_source_ch1()
{
  devparms.triggeredgesource = 0;

  statusLabel->setText("Trigger source channel 1");

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN1");
}


void UI_Mainwindow::trigger_source_ch2()
{
  devparms.triggeredgesource = 1;

  statusLabel->setText("Trigger source channel 2");

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN2");
}


void UI_Mainwindow::trigger_source_ch3()
{
  devparms.triggeredgesource = 2;

  statusLabel->setText("Trigger source channel 3");

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN3");
}


void UI_Mainwindow::trigger_source_ch4()
{
  devparms.triggeredgesource = 3;

  statusLabel->setText("Trigger source channel 4");

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN4");
}


void UI_Mainwindow::trigger_source_ext()
{
  devparms.triggeredgesource = 4;

  statusLabel->setText("Trigger source extern");

  tmcdev_write(device, ":TRIG:EDG:SOUR EXT");
}


void UI_Mainwindow::trigger_source_ext5()
{
  devparms.triggeredgesource = 5;

  statusLabel->setText("Trigger source extern 5");

  tmcdev_write(device, ":TRIG:EDG:SOUR EXT5");
}


void UI_Mainwindow::trigger_source_acl()
{
  devparms.triggeredgesource = 6;

  statusLabel->setText("Trigger source AC powerline");

  tmcdev_write(device, ":TRIG:EDG:SOUR ACL");
}


void UI_Mainwindow::trigger_coupling_ac()
{
  devparms.triggercoupling = 0;

  statusLabel->setText("Trigger coupling AC");

  tmcdev_write(device, ":TRIG:COUP AC");
}


void UI_Mainwindow::trigger_coupling_dc()
{
  devparms.triggercoupling = 1;

  statusLabel->setText("Trigger coupling DC");

  tmcdev_write(device, ":TRIG:COUP DC");
}


void UI_Mainwindow::trigger_coupling_lfreject()
{
  devparms.triggercoupling = 2;

  statusLabel->setText("Trigger LF reject");

  tmcdev_write(device, ":TRIG:COUP LFR");
}


void UI_Mainwindow::trigger_coupling_hfreject()
{
  devparms.triggercoupling = 3;

  statusLabel->setText("Trigger HF reject");

  tmcdev_write(device, ":TRIG:COUP HFR");
}


void UI_Mainwindow::trigger_slope_pos()
{
  devparms.triggeredgeslope = 0;

  statusLabel->setText("Trigger edge positive");

  tmcdev_write(device, ":TRIG:EDG:SLOP POS");
}


void UI_Mainwindow::trigger_slope_neg()
{
  devparms.triggeredgeslope = 1;

  statusLabel->setText("Trigger edge negative");

  tmcdev_write(device, ":TRIG:EDG:SLOP NEG");
}


void UI_Mainwindow::trigger_slope_rfal()
{
  devparms.triggeredgeslope = 2;

  statusLabel->setText("Trigger edge positive /negative");

  tmcdev_write(device, ":TRIG:EDG:SLOP RFAL");
}


void UI_Mainwindow::trigger_setting_holdoff()
{
  navDialFunc = NAV_DIAL_FUNC_HOLDOFF;

  adjDialFunc = ADJ_DIAL_FUNC_HOLDOFF;

  adjDialLabel->setText("Holdoff");

  adjDialLabel->setStyleSheet("background: #66FF99; font: 7pt;");

  adjdial_timer->start(ADJDIAL_TIMER_IVAL_1);
}


void UI_Mainwindow::trigForceButtonClicked()
{
  statusLabel->setText("Trigger force");

  tmcdev_write(device, ":TFOR");
}


void UI_Mainwindow::trig50pctButtonClicked()
{
  statusLabel->setText("Trigger 50%");

  tmcdev_write(device, ":TLHA");

  usleep(30000);

  tmcdev_write(device, ":TRIG:EDG:LEV?");

  tmcdev_read(device);

  devparms.triggeredgelevel[devparms.triggeredgesource] = atof(device->buf);

  waveForm->setTrigLineVisible();
}


void UI_Mainwindow::trigAdjustDialClicked(QPoint)
{
  char str[512];

  devparms.triggeredgelevel[devparms.triggeredgesource] = 0;

  strcpy(str, "Trigger level: ");

  convert_to_metric_suffix(str + strlen(str), devparms.triggeredgelevel[devparms.triggeredgesource], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":TRIG:EDG:LEV %e", devparms.triggeredgelevel[devparms.triggeredgesource]);

  tmcdev_write(device, str);
}








