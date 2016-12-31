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




void UI_Mainwindow::navDialChanged(int npos)
{
  int mpr = 1;

  double val, lefttime, righttime, delayrange;

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

    if(devparms.modelserie == 1)
    {
      if(devparms.triggerholdoff < 1.7e-8)
      {
        devparms.triggerholdoff = 1.6e-8;
      }
    }
    else
    {
      if(devparms.triggerholdoff < 1.01e-7)
      {
        devparms.triggerholdoff = 1e-7;
      }
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

    set_cue_cmd(str);
  }
  else if(devparms.timebasedelayenable)
    {
      strcpy(str, "Delayed timebase position: ");

      convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

      strcat(str, "s");

      statusLabel->setText(str);

      sprintf(str, ":TIM:DEL:OFFS %e", devparms.timebasedelayoffset);

      set_cue_cmd(str);
    }

  waveForm->update();
}


void UI_Mainwindow::adjDialChanged(int new_pos)
{
  static int old_pos=50;

  int diff, dir;

  if(adjDialFunc == ADJ_DIAL_FUNC_NONE)
  {
    return;
  }

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
      if(devparms.modelserie == 1)
      {
        if(devparms.triggerholdoff < 1.7e-8)
        {
          devparms.triggerholdoff = 1.6e-8;

          old_pos = new_pos;

          return;
        }
      }
      else
      {
        if(devparms.triggerholdoff <= 1.01e-7)
        {
          devparms.triggerholdoff = 1e-7;

          old_pos = new_pos;

          return;
        }
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
  static int old_pos=50;

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

  trigAdjDial_timer->start(TMC_DIAL_TIMER_DELAY);

  old_pos = new_pos;

  waveForm->label_active = LABEL_ACTIVE_TRIG;

  label_timer->start(LABEL_TIMER_IVAL);

  waveForm->setTrigLineVisible();

  waveForm->update();
}


void UI_Mainwindow::horScaleDialChanged(int new_pos)
{
  static int old_pos=50;

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
        if(devparms.timebasedelayscale <= 5.001e-9)
        {
          devparms.timebasedelayscale = 5e-9;

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
      devparms.timebasedelayscale = round_up_step125(devparms.timebasedelayscale, NULL);
    }
    else
    {
      devparms.timebasedelayscale = round_down_step125(devparms.timebasedelayscale, NULL);
    }

    devparms.current_screen_sf = 100.0 / devparms.timebasedelayscale;

    strcpy(str, "Delayed timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayscale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horScaleDial_timer->start(TMC_DIAL_TIMER_DELAY);

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
      devparms.timebasescale = round_up_step125(devparms.timebasescale, NULL);
    }
    else
    {
      devparms.timebasescale = round_down_step125(devparms.timebasescale, NULL);
    }

    devparms.current_screen_sf = 100.0 / devparms.timebasescale;

    strcpy(str, "Timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasescale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horScaleDial_timer->start(TMC_DIAL_TIMER_DELAY);

    old_pos = new_pos;
  }

  waveForm->update();
}


void UI_Mainwindow::horPosDialChanged(int new_pos)
{
  static int old_pos=50;

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

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);

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

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);

    old_pos = new_pos;
  }

  waveForm->update();
}


void UI_Mainwindow::vertOffsetDialChanged(int new_pos)
{
  static int old_pos=50;

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

  val = round_up_step125(devparms.chanscale[chn], NULL) / 100;

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

  waveForm->label_active = chn + 1;

  label_timer->start(LABEL_TIMER_IVAL);

  vertOffsDial_timer->start(TMC_DIAL_TIMER_DELAY);

  old_pos = new_pos;

  waveForm->update();
}


void UI_Mainwindow::vertScaleDialChanged(int new_pos)
{
  static int old_pos=50;

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
    val = round_up_step125(devparms.chanscale[chn], NULL);
  }
  else
  {
    val = round_down_step125(devparms.chanscale[chn], NULL);
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

  ltmp /= devparms.chanscale[chn];

  devparms.chanoffset[chn] /= ltmp;

  sprintf(str, "Channel %i scale: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanscale[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:SCAL %e", chn + 1, devparms.chanscale[chn]);

  set_cue_cmd(str);

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

  QList<QAction *> actionList;

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
  actionList = submenuacquisition.actions();
  if(devparms.acquiretype == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else if(devparms.acquiretype == 1)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.acquiretype == 2)
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
      else if(devparms.acquiretype == 3)
        {
          actionList[3]->setCheckable(true);
          actionList[3]->setChecked(true);
        }
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
  actionList = submenumemdepth.actions();
  if(devparms.acquirememdepth == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  if(devparms.modelserie != 1)
  {
    if((devparms.acquirememdepth == 14000) || (devparms.acquirememdepth == 7000))
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if((devparms.acquirememdepth == 140000) || (devparms.acquirememdepth == 70000))
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
      else if((devparms.acquirememdepth == 1400000) || (devparms.acquirememdepth == 700000))
        {
          actionList[3]->setCheckable(true);
          actionList[3]->setChecked(true);
        }
        else if((devparms.acquirememdepth == 14000000) || (devparms.acquirememdepth == 7000000))
          {
            actionList[4]->setCheckable(true);
            actionList[4]->setChecked(true);
          }
          else if((devparms.acquirememdepth == 140000000) || (devparms.acquirememdepth == 70000000)  ||
                  (devparms.acquirememdepth == 56000000) || (devparms.acquirememdepth == 28000000))
            {
              actionList[5]->setCheckable(true);
              actionList[5]->setChecked(true);
            }
  }
  else
  {
    if((devparms.acquirememdepth == 12000) || (devparms.acquirememdepth == 6000) ||
       (devparms.acquirememdepth == 3000))
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if((devparms.acquirememdepth == 120000) || (devparms.acquirememdepth == 60000) ||
            (devparms.acquirememdepth == 30000))
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
      else if((devparms.acquirememdepth == 1200000) || (devparms.acquirememdepth == 600000) ||
              (devparms.acquirememdepth == 300000))
        {
          actionList[3]->setCheckable(true);
          actionList[3]->setChecked(true);
        }
        else if((devparms.acquirememdepth == 12000000) || (devparms.acquirememdepth == 6000000) ||
                (devparms.acquirememdepth == 3000000))
          {
            actionList[4]->setCheckable(true);
            actionList[4]->setChecked(true);
          }
          else if((devparms.acquirememdepth == 24000000) || (devparms.acquirememdepth == 12000000) ||
                  (devparms.acquirememdepth == 6000000))
            {
              actionList[5]->setCheckable(true);
              actionList[5]->setChecked(true);
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

    set_cue_cmd(":ACQ:MDEP AUTO");

    devparms.timebaseoffset = 0;

    usleep(20000);

    set_cue_cmd(":TIM:OFFS 0");

    return;
  }

  strcpy(str, "Memory depth: ");

  convert_to_metric_suffix(str + strlen(str), mdepth, 0);

  statusLabel->setText(str);

  sprintf(str, ":ACQ:MDEP %i", mdepth);

  set_cue_cmd(str);

  devparms.timebaseoffset = 0;

  usleep(20000);

  set_cue_cmd(":TIM:OFFS 0");
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

  set_cue_cmd(":ACQ:TYPE NORM");
}


void UI_Mainwindow::set_acq_peak()
{
  if(devparms.acquiretype == 2)
  {
    return;
  }

  devparms.acquiretype = 2;

  statusLabel->setText("Acquire: peak");

  set_cue_cmd(":ACQ:TYPE PEAK");
}


void UI_Mainwindow::set_acq_hres()
{
  if(devparms.acquiretype == 3)
  {
    return;
  }

  devparms.acquiretype = 3;

  statusLabel->setText("Acquire: high resolution");

  set_cue_cmd(":ACQ:TYPE HRES");
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

  set_cue_cmd(":ACQ:TYPE AVER");
}


void UI_Mainwindow::cursButtonClicked()
{
}


void UI_Mainwindow::saveButtonClicked()
{
  QMenu menu;

  menu.addAction("Save screen waveform", this, SLOT(save_screen_waveform()));
  menu.addAction("Save memory waveform", this, SLOT(save_mem_wav()));
  menu.addAction("Wave Inspector",       this, SLOT(analyze_mem_wav()));
  menu.addAction("Save screenshot",      this, SLOT(save_screenshot()));
  menu.addAction("Factory",              this, SLOT(set_to_factory()));

  menu.exec(saveButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::save_mem_wav()
{
  save_memory_waveform(1);
}


void UI_Mainwindow::analyze_mem_wav()
{
  save_memory_waveform(0);
}


void UI_Mainwindow::dispButtonClicked()
{
  QMenu menu,
        submenutype,
        submenugrid,
        submenugrading;

  QList<QAction *> actionList;

  submenutype.setTitle("Type");
  submenutype.addAction("Vectors", this, SLOT(set_grid_type_vectors()));
  submenutype.addAction("Dots",    this, SLOT(set_grid_type_dots()));
  actionList = submenutype.actions();
  if(devparms.displaytype == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else
  {
    actionList[1]->setCheckable(true);
    actionList[1]->setChecked(true);
  }
  menu.addMenu(&submenutype);

  submenugrid.setTitle("Grid");
  submenugrid.addAction("Full", this, SLOT(set_grid_full()));
  submenugrid.addAction("Half", this, SLOT(set_grid_half()));
  submenugrid.addAction("None", this, SLOT(set_grid_none()));
  actionList = submenugrid.actions();
  if(devparms.displaygrid == 2)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else if(devparms.displaygrid == 1)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.displaygrid == 0)
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
  menu.addMenu(&submenugrid);

  submenugrading.setTitle("Persistence");
  submenugrading.addAction("Minimum",  this, SLOT(set_grading_min()));
//   submenugrading.addAction("0.05",     this, SLOT(set_grading_005()));
  submenugrading.addAction("0.1",      this, SLOT(set_grading_01()));
  submenugrading.addAction("0.2",      this, SLOT(set_grading_02()));
  submenugrading.addAction("0.5",      this, SLOT(set_grading_05()));
  submenugrading.addAction("1",        this, SLOT(set_grading_1()));
  submenugrading.addAction("2",        this, SLOT(set_grading_2()));
  submenugrading.addAction("5",        this, SLOT(set_grading_5()));
//   submenugrading.addAction("10",       this, SLOT(set_grading_10()));
//   submenugrading.addAction("20",       this, SLOT(set_grading_20()));
  submenugrading.addAction("Infinite", this, SLOT(set_grading_inf()));
  actionList = submenugrading.actions();
  if(devparms.displaygrading == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else if(devparms.displaygrading == 1)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.displaygrading == 2)
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
      else if(devparms.displaygrading == 5)
        {
          actionList[3]->setCheckable(true);
          actionList[3]->setChecked(true);
        }
      else if(devparms.displaygrading == 10)
        {
          actionList[4]->setCheckable(true);
          actionList[4]->setChecked(true);
        }
        else if(devparms.displaygrading == 20)
          {
            actionList[5]->setCheckable(true);
            actionList[5]->setChecked(true);
          }
          else if(devparms.displaygrading == 50)
            {
              actionList[6]->setCheckable(true);
              actionList[6]->setChecked(true);
            }
            else if(devparms.displaygrading == 10000)
              {
                actionList[7]->setCheckable(true);
                actionList[7]->setChecked(true);
              }
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

  set_cue_cmd(":DISP:TYPE VECT");
}


void UI_Mainwindow::set_grid_type_dots()
{
  if(devparms.displaytype)
  {
    return;
  }

  devparms.displaytype = 1;

  statusLabel->setText("Display type: dots");

  set_cue_cmd(":DISP:TYPE DOTS");
}


void UI_Mainwindow::set_grid_full()
{
  if(devparms.displaygrid == 2)
  {
    return;
  }

  devparms.displaygrid = 2;

  statusLabel->setText("Display grid: full");

  set_cue_cmd(":DISP:GRID FULL");
}


void UI_Mainwindow::set_grid_half()
{
  if(devparms.displaygrid == 1)
  {
    return;
  }

  devparms.displaygrid = 1;

  statusLabel->setText("Display grid: half");

  set_cue_cmd(":DISP:GRID HALF");
}


void UI_Mainwindow::set_grid_none()
{
  if(devparms.displaygrid == 0)
  {
    return;
  }

  devparms.displaygrid = 0;

  statusLabel->setText("Display grid: none");

  set_cue_cmd(":DISP:GRID NONE");
}


void UI_Mainwindow::set_grading_min()
{
  if(devparms.displaygrading == 0)
  {
    return;
  }

  devparms.displaygrading = 0;

  statusLabel->setText("Display grading: Minimum");

  set_cue_cmd(":DISP:GRAD:TIME MIN");
}


void UI_Mainwindow::set_grading_005()
{
  statusLabel->setText("Display grading: 0.05 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 0.05");
}


void UI_Mainwindow::set_grading_01()
{
  if(devparms.displaygrading == 1)
  {
    return;
  }

  devparms.displaygrading = 1;

  statusLabel->setText("Display grading: 0.1 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 0.1");
}


void UI_Mainwindow::set_grading_02()
{
  if(devparms.displaygrading == 2)
  {
    return;
  }

  devparms.displaygrading = 2;

  statusLabel->setText("Display grading: 0.2 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 0.2");
}


void UI_Mainwindow::set_grading_05()
{
  if(devparms.displaygrading == 5)
  {
    return;
  }

  devparms.displaygrading = 5;

  statusLabel->setText("Display grading: 0.5 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 0.5");
}


void UI_Mainwindow::set_grading_1()
{
  if(devparms.displaygrading == 10)
  {
    return;
  }

  devparms.displaygrading = 10;

  statusLabel->setText("Display grading: 1 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 1");
}


void UI_Mainwindow::set_grading_2()
{
  if(devparms.displaygrading == 20)
  {
    return;
  }

  devparms.displaygrading = 20;

  statusLabel->setText("Display grading: 2 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 2");
}


void UI_Mainwindow::set_grading_5()
{
  if(devparms.displaygrading == 50)
  {
    return;
  }

  devparms.displaygrading = 50;

  statusLabel->setText("Display grading: 5 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 5");
}


void UI_Mainwindow::set_grading_10()
{
  statusLabel->setText("Display grading: 10 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 10");
}


void UI_Mainwindow::set_grading_20()
{
  statusLabel->setText("Display grading: 20 Sec.");

  set_cue_cmd(":DISP:GRAD:TIME 20");
}


void UI_Mainwindow::set_grading_inf()
{
  if(devparms.displaygrading == 10000)
  {
    return;
  }

  devparms.displaygrading = 10000;

  statusLabel->setText("Display grading: Infinite");

  set_cue_cmd(":DISP:GRAD:TIME INF");
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
  msgBox.setStandardButtons(QMessageBox::Close);
  msgBox.setText(
    "Use the mousewheel to change the dials. In order to simulate a push on a dial,"
    "click on it with the right mouse button.\n"
    "To toggle the delayed timebase, right-click on the timebase dial.\n"
    "To set the horizontal position to zero, right-click on the horizontal position dial.\n"
    "To set the vertical offset to zero, right-click on the vertical position dial.\n\n"
    "In addition of using the dials to change the scale and offset of the traces and the trigger position,"
    "you can use the mouse to drag the colored arrows aside of the plot.\n\n"
    "Keyboard shortcuts:\n"
    "PageUp: move traces 12 (or 14) divisions to the right.\n"
    "PageDn: move traces 12 (or 14) divisions to the left.\n"
    "Arrow left: move traces 1 division to the right.\n"
    "Arrow right: move traces 1 division to the left.\n"
    "Arrow up: move active trace 1 division up.\n"
    "Arrow down: move active trace 1 division down.\n"
    "Zoom In (decrease timebase): Ctl+\n"
    "Zoom Out (increase timebase): Ctl-\n"
    "Increase vertical scale: -\n"
    "Decrease vertical scale: +\n"
    "Press '1' to select or deselect channel 1\n"
    "Press '2' to select or deselect channel 2, etc.\n"
    "Press 'c' to center the horizontal position.\n"
    "Press 't' to center the trigger position.\n"
    "Press 'f' to toggle FFT.\n"
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

    set_cue_cmd(str);
  }
  else
  {
    devparms.chanvernier[chn] = 1;

    sprintf(str, "Channel %i vernier: on", chn + 1);

    statusLabel->setText(str);

    sprintf(str, ":CHAN%i:VERN 1", chn + 1);

    set_cue_cmd(str);
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

      set_cue_cmd(":CHAN1:DISP 0");

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

    set_cue_cmd(":CHAN1:DISP 1");

    ch1Button->setStyleSheet("background: #FFFF33;");

    devparms.activechannel = 0;
  }
}


void UI_Mainwindow::ch2ButtonClicked()
{
  if(devparms.channel_cnt < 2)
  {
    return;
  }

  if(devparms.chandisplay[1])
  {
    if(devparms.activechannel == 1)
    {
      devparms.chandisplay[1] = 0;

      statusLabel->setText("Channel 2 off");

      set_cue_cmd(":CHAN2:DISP 0");

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

    set_cue_cmd(":CHAN2:DISP 1");

    ch2Button->setStyleSheet("background: #33FFFF;");

    devparms.activechannel = 1;
  }
}


void UI_Mainwindow::ch3ButtonClicked()
{
  if(devparms.channel_cnt < 3)
  {
    return;
  }

  if(devparms.chandisplay[2])
  {
    if(devparms.activechannel == 2)
    {
      devparms.chandisplay[2] = 0;

      statusLabel->setText("Channel 3 off");

      set_cue_cmd(":CHAN3:DISP 0");

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

    set_cue_cmd(":CHAN3:DISP 1");

    ch3Button->setStyleSheet("background: #FF33FF;");

    devparms.activechannel = 2;
  }
}


void UI_Mainwindow::ch4ButtonClicked()
{
  if(devparms.channel_cnt < 4)
  {
    return;
  }

  if(devparms.chandisplay[3])
  {
    if(devparms.activechannel == 3)
    {
      devparms.chandisplay[3] = 0;

      statusLabel->setText("Channel 4 off");

      set_cue_cmd(":CHAN4:DISP 0");

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

    set_cue_cmd(":CHAN4:DISP 1");

    ch4Button->setStyleSheet("background: #0066CC;");

    devparms.activechannel = 3;
  }
}


void UI_Mainwindow::chan_menu()
{
  QMenu menu,
        submenubwl,
        submenucoupling,
        submenuinvert,
        submenuprobe;

  QList<QAction *> actionList;

  if((devparms.activechannel < 0) || (devparms.activechannel > MAX_CHNS))
  {
    return;
  }

  submenucoupling.setTitle("Coupling");
  submenucoupling.addAction("AC",  this, SLOT(chan_coupling_ac()));
  submenucoupling.addAction("DC",  this, SLOT(chan_coupling_dc()));
  submenucoupling.addAction("GND", this, SLOT(chan_coupling_gnd()));
  actionList = submenucoupling.actions();
  if(devparms.chancoupling[devparms.activechannel] == 0)
  {
    actionList[2]->setCheckable(true);
    actionList[2]->setChecked(true);
  }
  else if(devparms.chancoupling[devparms.activechannel] == 1)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.chancoupling[devparms.activechannel] == 2)
      {
        actionList[0]->setCheckable(true);
        actionList[0]->setChecked(true);
      }
  menu.addMenu(&submenucoupling);

  submenubwl.setTitle("BWL");
  submenubwl.addAction("Off",    this, SLOT(chan_bwl_off()));
  submenubwl.addAction("20MHz",  this, SLOT(chan_bwl_20()));
  if(devparms.modelserie == 6)
  {
    submenubwl.addAction("250MHz", this, SLOT(chan_bwl_250()));
  }
  actionList = submenubwl.actions();
  if(devparms.chanbwlimit[devparms.activechannel] == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else if(devparms.chanbwlimit[devparms.activechannel] == 20)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.modelserie == 6)
      {
        if(devparms.chanbwlimit[devparms.activechannel] == 250)
        {
          actionList[2]->setCheckable(true);
          actionList[2]->setChecked(true);
        }
      }
  menu.addMenu(&submenubwl);

  submenuprobe.setTitle("Probe");
  submenuprobe.addAction("0.1", this, SLOT(chan_probe_01()));
  if(devparms.modelserie != 6)
  {
    submenuprobe.addAction("0.2", this, SLOT(chan_probe_02()));
    submenuprobe.addAction("0.5", this, SLOT(chan_probe_05()));
  }
  submenuprobe.addAction("1",   this, SLOT(chan_probe_1()));
  if(devparms.modelserie != 6)
  {
    submenuprobe.addAction("2",   this, SLOT(chan_probe_2()));
    submenuprobe.addAction("5",   this, SLOT(chan_probe_5()));
  }
  submenuprobe.addAction("10",  this, SLOT(chan_probe_10()));
  if(devparms.modelserie != 6)
  {
    submenuprobe.addAction("20",  this, SLOT(chan_probe_20()));
    submenuprobe.addAction("50",  this, SLOT(chan_probe_50()));
  }
  submenuprobe.addAction("100", this, SLOT(chan_probe_100()));
  actionList = submenuprobe.actions();
  menu.addMenu(&submenuprobe);

  submenuinvert.setTitle("Invert");
  submenuinvert.addAction("On",  this, SLOT(chan_invert_on()));
  submenuinvert.addAction("Off", this, SLOT(chan_invert_off()));
  actionList = submenuinvert.actions();
  if(devparms.chaninvert[devparms.activechannel] == 1)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else
  {
    actionList[1]->setCheckable(true);
    actionList[1]->setChecked(true);
  }
  menu.addMenu(&submenuinvert);

  menu.exec(chanMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::math_menu()
{
  char str[1024];

  double val;

  QMenu menu,
        submenufft,
        submenufftctr,
        submenuffthzdiv,
        submenufftsrc,
        submenufftvscale,
        submenufftoffset;

  QList<QAction *> actionList;

  if((devparms.activechannel < 0) || (devparms.activechannel > MAX_CHNS))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    val = 100.0 / devparms.timebasedelayscale;
  }
  else
  {
    val = 100.0 / devparms.timebasescale;
  }

  submenufftctr.setTitle("Center");
  convert_to_metric_suffix(str, devparms.math_fft_hscale * 5.0 , 1);
  strcat(str, "Hz");
  submenufftctr.addAction(str, this, SLOT(select_fft_ctr_5()));
  convert_to_metric_suffix(str, devparms.math_fft_hscale * 6.0 , 1);
  strcat(str, "Hz");
  submenufftctr.addAction(str, this, SLOT(select_fft_ctr_6()));
  convert_to_metric_suffix(str, devparms.math_fft_hscale * 7.0 , 1);
  strcat(str, "Hz");
  submenufftctr.addAction(str, this, SLOT(select_fft_ctr_7()));
  convert_to_metric_suffix(str, devparms.math_fft_hscale * 8.0 , 1);
  strcat(str, "Hz");
  submenufftctr.addAction(str, this, SLOT(select_fft_ctr_8()));
  if((devparms.math_fft_hscale * 9.0) < (val * 0.40001))
  {
    convert_to_metric_suffix(str, devparms.math_fft_hscale * 9.0 , 1);
    strcat(str, "Hz");
    submenufftctr.addAction(str, this, SLOT(select_fft_ctr_9()));
    if((devparms.math_fft_hscale * 10.0) < (val * 0.40001))
    {
      convert_to_metric_suffix(str, devparms.math_fft_hscale * 10.0 , 1);
      strcat(str, "Hz");
      submenufftctr.addAction(str, this, SLOT(select_fft_ctr_10()));
      if((devparms.math_fft_hscale * 11.0) < (val * 0.40001))
      {
        convert_to_metric_suffix(str, devparms.math_fft_hscale * 11.0 , 1);
        strcat(str, "Hz");
        submenufftctr.addAction(str, this, SLOT(select_fft_ctr_11()));
        if((devparms.math_fft_hscale * 12.0) < (val * 0.40001))
        {
          convert_to_metric_suffix(str, devparms.math_fft_hscale * 12.0 , 1);
          strcat(str, "Hz");
          submenufftctr.addAction(str, this, SLOT(select_fft_ctr_12()));
        }
      }
    }
  }

  submenuffthzdiv.setTitle("Hz/Div");
//   if(devparms.modelserie == 6)
//   {
//     convert_to_metric_suffix(str, val / 40.0 , 2);
//     strcat(str, "Hz/Div");
//     submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_40()));
//     convert_to_metric_suffix(str, val / 80.0 , 2);
//     strcat(str, "Hz/Div");
//     submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_80()));
//     convert_to_metric_suffix(str, val / 200.0 , 2);
//     strcat(str, "Hz/Div");
//     submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_200()));
//   }
//   else
//   {
    convert_to_metric_suffix(str, val / 20.0 , 2);
    strcat(str, "Hz/Div");
    submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_20()));
    convert_to_metric_suffix(str, val / 40.0 , 2);
    strcat(str, "Hz/Div");
    submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_40()));
    convert_to_metric_suffix(str, val / 100.0 , 2);
    strcat(str, "Hz/Div");
    submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_100()));
    convert_to_metric_suffix(str, val / 200.0 , 2);
    strcat(str, "Hz/Div");
    submenuffthzdiv.addAction(str, this, SLOT(select_fft_hzdiv_200()));
//  }

  submenufftoffset.setTitle("Offset");
  if(devparms.math_fft_unit == 0)
  {
    convert_to_metric_suffix(str, devparms.fft_vscale * 4.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp4()));
    convert_to_metric_suffix(str, devparms.fft_vscale * 3.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp3()));
    convert_to_metric_suffix(str, devparms.fft_vscale * 2.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp2()));
    convert_to_metric_suffix(str, devparms.fft_vscale, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp1()));
    strcpy(str, "0V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffset0()));
    convert_to_metric_suffix(str, devparms.fft_vscale * -1.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm1()));
    convert_to_metric_suffix(str, devparms.fft_vscale * -2.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm2()));
    convert_to_metric_suffix(str, devparms.fft_vscale * -3.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm3()));
    convert_to_metric_suffix(str, devparms.fft_vscale * -4.0, 1);
    strcat(str, "V");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm4()));
  }
  else
  {
    sprintf(str, "%+.0fdB", devparms.fft_vscale * 4.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp4()));
    sprintf(str, "%+.0fdB", devparms.fft_vscale * 3.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp3()));
    sprintf(str, "%+.0fdB", devparms.fft_vscale * 2.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp2()));
    sprintf(str, "%+.0fdB", devparms.fft_vscale);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetp1()));
    strcpy(str, "0dB");
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffset0()));
    sprintf(str, "%.0fdB", devparms.fft_vscale * -1.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm1()));
    sprintf(str, "%.0fdB", devparms.fft_vscale * -2.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm2()));
    sprintf(str, "%.0fdB", devparms.fft_vscale * -3.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm3()));
    sprintf(str, "%.0fdB", devparms.fft_vscale * -4.0);
    submenufftoffset.addAction(str, this, SLOT(select_fft_voffsetm4()));
  }

  submenufftvscale.setTitle("Scale");
  if(devparms.math_fft_unit == 0)
  {
    submenufftvscale.addAction("1V/Div",  this, SLOT(select_fft_vscale1()));
    submenufftvscale.addAction("2V/Div",  this, SLOT(select_fft_vscale2()));
    submenufftvscale.addAction("5V/Div",  this, SLOT(select_fft_vscale5()));
    submenufftvscale.addAction("10V/Div",  this, SLOT(select_fft_vscale10()));
    submenufftvscale.addAction("20V/Div",  this, SLOT(select_fft_vscale20()));
  }
  else
  {
    submenufftvscale.addAction("1dB/Div",  this, SLOT(select_fft_vscale1()));
    submenufftvscale.addAction("2dB/Div",  this, SLOT(select_fft_vscale2()));
    submenufftvscale.addAction("5dB/Div",  this, SLOT(select_fft_vscale5()));
    submenufftvscale.addAction("10dB/Div",  this, SLOT(select_fft_vscale10()));
    submenufftvscale.addAction("20dB/Div",  this, SLOT(select_fft_vscale20()));
  }

  submenufftsrc.setTitle("Source");
  submenufftsrc.addAction("CH1",  this, SLOT(select_fft_ch1()));
  submenufftsrc.addAction("CH2",  this, SLOT(select_fft_ch2()));
  if(devparms.channel_cnt > 2)
  {
    submenufftsrc.addAction("CH3",  this, SLOT(select_fft_ch3()));
    submenufftsrc.addAction("CH4",  this, SLOT(select_fft_ch4()));
  }
  actionList = submenufftsrc.actions();
  if(devparms.math_fft_src == 0)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else if(devparms.math_fft_src == 1)
    {
      actionList[1]->setCheckable(true);
      actionList[1]->setChecked(true);
    }
    else if(devparms.math_fft_src == 2)
      {
        actionList[2]->setCheckable(true);
        actionList[2]->setChecked(true);
      }
      else if(devparms.math_fft_src == 3)
        {
          actionList[3]->setCheckable(true);
          actionList[3]->setChecked(true);
        }

  submenufft.setTitle("FFT");
  submenufft.addAction("On",     this, SLOT(toggle_fft()));
  submenufft.addAction("Off",    this, SLOT(toggle_fft()));
  submenufft.addAction("Full",   this, SLOT(toggle_fft_split()));
  submenufft.addAction("Half",   this, SLOT(toggle_fft_split()));
  submenufft.addAction("Vrms",   this, SLOT(toggle_fft_unit()));
  submenufft.addAction("dB/dBm", this, SLOT(toggle_fft_unit()));
  submenufft.addMenu(&submenufftsrc);
  submenufft.addMenu(&submenufftctr);
  submenufft.addMenu(&submenuffthzdiv);
  submenufft.addMenu(&submenufftoffset);
  submenufft.addMenu(&submenufftvscale);
  actionList = submenufft.actions();
  if(devparms.math_fft == 1)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else
  {
    actionList[1]->setCheckable(true);
    actionList[1]->setChecked(true);
  }
  if(devparms.math_fft_split == 0)
  {
    actionList[2]->setCheckable(true);
    actionList[2]->setChecked(true);
  }
  else
  {
    actionList[3]->setCheckable(true);
    actionList[3]->setChecked(true);
  }
  if(devparms.math_fft_unit == 0)
  {
    actionList[4]->setCheckable(true);
    actionList[4]->setChecked(true);
  }
  else
  {
    actionList[5]->setCheckable(true);
    actionList[5]->setChecked(true);
  }

  menu.addMenu(&submenufft);

  menu.addAction("Decode", this, SLOT(show_decode_window()));

  menu.exec(mathMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::chan_coupling_ac()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 2;

  sprintf(str, "Channel %i coupling: AC", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP AC", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::chan_coupling_dc()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 1;

  sprintf(str, "Channel %i coupling: DC", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP DC", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::chan_coupling_gnd()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 0;

  sprintf(str, "Channel %i coupling: GND", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:COUP GND", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::chan_probe_01()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 0.1;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 0.1X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_02()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 0.2;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 0.2X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_05()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 0.5;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 0.5X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_1()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 1;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 1X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_2()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 2;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 2X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_5()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 5;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 5X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_10()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 10;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 10X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_20()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 20;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 20X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_50()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 50;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 50X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_probe_100()
{
  char str[128];

  devparms.chanscale[devparms.activechannel] /= devparms.chanprobe[devparms.activechannel];

  devparms.chanprobe[devparms.activechannel] = 100;

  devparms.chanscale[devparms.activechannel] *= devparms.chanprobe[devparms.activechannel];

  sprintf(str, "Channel %i probe: 100X", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:PROB %e", devparms.activechannel + 1, devparms.chanprobe[devparms.activechannel]);

  set_cue_cmd(str);
}


void UI_Mainwindow::chan_bwl_off()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 0;

  sprintf(str, "Channel %i bandwidth limit: Off", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL OFF", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::chan_bwl_20()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 20;

  sprintf(str, "Channel %i bandwidth limit: 20MHz", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL 20M", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::chan_bwl_250()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 250;

  sprintf(str, "Channel %i bandwidth limit: 250MHz", devparms.activechannel + 1);

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:BWL 250M", devparms.activechannel + 1);

  set_cue_cmd(str);

  updateLabels();
}


void UI_Mainwindow::updateLabels()
{
  int chn;

  char str[128];

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    str[0] = 0;

    if(devparms.chancoupling[chn] == 2)
    {
      strcat(str, "AC");
    }

    if(devparms.chanimpedance[chn])
    {
      strcat(str, " 50");
    }

    if(devparms.chanbwlimit[chn])
    {
      strcat(str, " BW");
    }

    switch(chn)
    {
      case 0: ch1InputLabel->setText(str);
              break;
      case 1: ch2InputLabel->setText(str);
              break;
      case 2: ch3InputLabel->setText(str);
              break;
      case 3: ch4InputLabel->setText(str);
              break;
    }
  }
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

  set_cue_cmd(str);
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

  set_cue_cmd(str);
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

  set_cue_cmd(str);
}


void UI_Mainwindow::clearButtonClicked()
{
  statusLabel->setText("Display cleared");

  set_cue_cmd(":DISP:CLE");

  waveForm->clear();
}


void UI_Mainwindow::autoButtonClicked()
{
  if((device == NULL) || (!devparms.connected))
  {
    return;
  }

  scrn_timer->stop();

  scrn_thread->wait();

  statusLabel->setText("Auto settings");

  tmc_write(":AUT");

  qApp->processEvents();

  usleep(20000);

  qApp->processEvents();

  sleep(2);

  get_device_settings();

  scrn_timer->start(devparms.screentimerival);
}


void UI_Mainwindow::runButtonClicked()
{
  if(devparms.triggerstatus == 5)
  {
    statusLabel->setText("Trigger: run");

    set_cue_cmd(":RUN");
  }
  else
  {
    statusLabel->setText("Trigger: stop");

    set_cue_cmd(":STOP");
  }
}


void UI_Mainwindow::singleButtonClicked()
{
  statusLabel->setText("Trigger: single");

  set_cue_cmd(":SING");
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

  QList<QAction *> actionList;

  submenudelayed.setTitle("Delayed");
  submenudelayed.addAction("On",  this, SLOT(horizontal_delayed_on()));
  submenudelayed.addAction("Off", this, SLOT(horizontal_delayed_off()));
  actionList = submenudelayed.actions();
  if(devparms.timebasedelayenable == 1)
  {
    actionList[0]->setCheckable(true);
    actionList[0]->setChecked(true);
  }
  else
  {
    actionList[1]->setCheckable(true);
    actionList[1]->setChecked(true);
  }
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

  set_cue_cmd(":TIM:DEL:ENAB 1");

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

  set_cue_cmd(":TIM:DEL:ENAB 0");
}


void UI_Mainwindow::horizontal_delayed_toggle()
{
  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayenable = 0;

    statusLabel->setText("Delayed timebase disabled");

    set_cue_cmd(":TIM:DEL:ENAB 0");
  }
  else
  {
    devparms.timebasedelayenable = 1;

    statusLabel->setText("Delayed timebase enabled");

    set_cue_cmd(":TIM:DEL:ENAB 1");
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

    set_cue_cmd(str);
  }
  else
  {
    devparms.timebaseoffset = 0;

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:OFFS %e", devparms.timebaseoffset);

    set_cue_cmd(str);
  }
}


void UI_Mainwindow::horScaleDialClicked(QPoint)
{
  horizontal_delayed_toggle();
}


void UI_Mainwindow::measureButtonClicked()
{
  int i;

  QMenu menu,
        submenucounter;

  QList<QAction *> actionList;

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
  actionList = submenucounter.actions();
  for(i=0; i<5; i++)
  {
    if(devparms.countersrc == i)
    {
      actionList[i]->setCheckable(true);
      actionList[i]->setChecked(true);

      break;
    }
  }
  menu.addMenu(&submenucounter);

  menu.exec(measureButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::counter_off()
{
  devparms.countersrc = 0;

  statusLabel->setText("Freq. counter off");

  set_cue_cmd(":MEAS:COUN:SOUR OFF");
}


void UI_Mainwindow::counter_ch1()
{
  devparms.countersrc = 1;

  statusLabel->setText("Freq. counter channel 1");

  set_cue_cmd(":MEAS:COUN:SOUR CHAN1");
}


void UI_Mainwindow::counter_ch2()
{
  devparms.countersrc = 2;

  statusLabel->setText("Freq. counter channel 2");

  set_cue_cmd(":MEAS:COUN:SOUR CHAN2");
}


void UI_Mainwindow::counter_ch3()
{
  devparms.countersrc = 3;

  statusLabel->setText("Freq. counter channel 3");

  set_cue_cmd(":MEAS:COUN:SOUR CHAN3");
}


void UI_Mainwindow::counter_ch4()
{
  devparms.countersrc = 4;

  statusLabel->setText("Freq. counter channel 4");

  set_cue_cmd(":MEAS:COUN:SOUR CHAN4");
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
            set_cue_cmd(":TRIG:SWE AUTO");
            break;
    case 1: trigModeNormLed->setValue(true);
            trigModeAutoLed->setValue(false);
            statusLabel->setText("Trigger norm");
            set_cue_cmd(":TRIG:SWE NORM");
            break;
    case 2: trigModeSingLed->setValue(true);
            trigModeNormLed->setValue(false);
            statusLabel->setText("Trigger single");
            set_cue_cmd(":TRIG:SWE SING");
            break;
  }
}


void UI_Mainwindow::trigMenuButtonClicked()
{
  int i;

  char str[512];

  QMenu menu,
        submenusource,
        submenuslope,
        submenucoupling,
        submenusetting;

  QList<QAction *> actionList;

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
  if(devparms.modelserie == 6)
  {
    submenusource.addAction("EXT", this, SLOT(trigger_source_ext()));
    submenusource.addAction("EXT/ 5", this, SLOT(trigger_source_ext5()));
  }
  submenusource.addAction("AC Line", this, SLOT(trigger_source_acl()));
  actionList = submenusource.actions();
  if(devparms.modelserie == 6)
  {
    for(i=0; i<7; i++)
    {
      if(devparms.triggeredgesource == i)
      {
        actionList[i]->setCheckable(true);
        actionList[i]->setChecked(true);

        break;
      }
    }
  }
  else
  {
    for(i=0; i<4; i++)
    {
      if(devparms.triggeredgesource == i)
      {
        actionList[i]->setCheckable(true);
        actionList[i]->setChecked(true);

        break;
      }
    }

    if(devparms.triggeredgesource == 6)
    {
      actionList[4]->setCheckable(true);
      actionList[4]->setChecked(true);
    }
  }
  menu.addMenu(&submenusource);

  submenucoupling.setTitle("Coupling");
  submenucoupling.addAction("AC", this, SLOT(trigger_coupling_ac()));
  submenucoupling.addAction("DC", this, SLOT(trigger_coupling_dc()));
  submenucoupling.addAction("LF reject", this, SLOT(trigger_coupling_lfreject()));
  submenucoupling.addAction("HF reject", this, SLOT(trigger_coupling_hfreject()));
  actionList = submenucoupling.actions();
  for(i=0; i<4; i++)
  {
    if(devparms.triggercoupling == i)
    {
      actionList[i]->setCheckable(true);
      actionList[i]->setChecked(true);

      break;
    }
  }
  menu.addMenu(&submenucoupling);

  submenuslope.setTitle("Slope");
  submenuslope.addAction("Positive", this, SLOT(trigger_slope_pos()));
  submenuslope.addAction("Negative", this, SLOT(trigger_slope_neg()));
  submenuslope.addAction("Rise/Fal", this, SLOT(trigger_slope_rfal()));
  actionList = submenuslope.actions();
  for(i=0; i<3; i++)
  {
    if(devparms.triggeredgeslope == i)
    {
      actionList[i]->setCheckable(true);
      actionList[i]->setChecked(true);

      break;
    }
  }
  menu.addMenu(&submenuslope);

  submenusetting.setTitle("Setting");
  sprintf(str, "Holdoff ");
  convert_to_metric_suffix(str + strlen(str), devparms.triggerholdoff, 3);
  strcat(str, "S");
  submenusetting.addAction(str, this, SLOT(trigger_setting_holdoff()));
  menu.addMenu(&submenusetting);

  menu.exec(trigMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::trigger_source_ch1()
{
  devparms.triggeredgesource = 0;

  statusLabel->setText("Trigger source channel 1");

  set_cue_cmd(":TRIG:EDG:SOUR CHAN1");
}


void UI_Mainwindow::trigger_source_ch2()
{
  devparms.triggeredgesource = 1;

  statusLabel->setText("Trigger source channel 2");

  set_cue_cmd(":TRIG:EDG:SOUR CHAN2");
}


void UI_Mainwindow::trigger_source_ch3()
{
  devparms.triggeredgesource = 2;

  statusLabel->setText("Trigger source channel 3");

  set_cue_cmd(":TRIG:EDG:SOUR CHAN3");
}


void UI_Mainwindow::trigger_source_ch4()
{
  devparms.triggeredgesource = 3;

  statusLabel->setText("Trigger source channel 4");

  set_cue_cmd(":TRIG:EDG:SOUR CHAN4");
}


void UI_Mainwindow::trigger_source_ext()
{
  devparms.triggeredgesource = 4;

  statusLabel->setText("Trigger source extern");

  set_cue_cmd(":TRIG:EDG:SOUR EXT");
}


void UI_Mainwindow::trigger_source_ext5()
{
  devparms.triggeredgesource = 5;

  statusLabel->setText("Trigger source extern 5");

  set_cue_cmd(":TRIG:EDG:SOUR EXT5");
}


void UI_Mainwindow::trigger_source_acl()
{
  devparms.triggeredgesource = 6;

  statusLabel->setText("Trigger source AC powerline");

  if(devparms.modelserie == 6)
  {
    set_cue_cmd(":TRIG:EDG:SOUR ACL");
  }
  else
  {
    set_cue_cmd(":TRIG:EDG:SOUR AC");
  }
}


void UI_Mainwindow::trigger_coupling_ac()
{
  devparms.triggercoupling = 0;

  statusLabel->setText("Trigger coupling AC");

  set_cue_cmd(":TRIG:COUP AC");
}


void UI_Mainwindow::trigger_coupling_dc()
{
  devparms.triggercoupling = 1;

  statusLabel->setText("Trigger coupling DC");

  set_cue_cmd(":TRIG:COUP DC");
}


void UI_Mainwindow::trigger_coupling_lfreject()
{
  devparms.triggercoupling = 2;

  statusLabel->setText("Trigger LF reject");

  set_cue_cmd(":TRIG:COUP LFR");
}


void UI_Mainwindow::trigger_coupling_hfreject()
{
  devparms.triggercoupling = 3;

  statusLabel->setText("Trigger HF reject");

  set_cue_cmd(":TRIG:COUP HFR");
}


void UI_Mainwindow::trigger_slope_pos()
{
  devparms.triggeredgeslope = 0;

  statusLabel->setText("Trigger edge positive");

  set_cue_cmd(":TRIG:EDG:SLOP POS");
}


void UI_Mainwindow::trigger_slope_neg()
{
  devparms.triggeredgeslope = 1;

  statusLabel->setText("Trigger edge negative");

  set_cue_cmd(":TRIG:EDG:SLOP NEG");
}


void UI_Mainwindow::trigger_slope_rfal()
{
  devparms.triggeredgeslope = 2;

  statusLabel->setText("Trigger edge positive /negative");

  set_cue_cmd(":TRIG:EDG:SLOP RFAL");
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

  set_cue_cmd(":TFOR");
}


void UI_Mainwindow::trig50pctButtonClicked()
{
  statusLabel->setText("Trigger 50%");

  set_cue_cmd(":TLHA");

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

  set_cue_cmd(str);
}


void UI_Mainwindow::toggle_fft()
{
  if(devparms.math_fft == 1)
  {
    devparms.math_fft = 0;

    if(devparms.modelserie != 1)
    {
      set_cue_cmd(":CALC:MODE OFF");
    }
    else
    {
      set_cue_cmd(":MATH:DISP OFF");
    }

    statusLabel->setText("Math display off");
  }
  else
  {
    if(devparms.modelserie != 1)
    {
      set_cue_cmd(":CALC:MODE FFT");
    }
    else
    {
      set_cue_cmd(":MATH:OPER FFT");

      set_cue_cmd(":MATH:DISP ON");
    }

    devparms.math_fft = 1;

    statusLabel->setText("FFT on");
  }
}


void UI_Mainwindow::toggle_fft_split()
{
  QMessageBox msgBox;

  if(devparms.math_fft_split == 1)
  {
    if(devparms.vertdivisions == 10)
    {
      msgBox.setIcon(QMessageBox::NoIcon);
      msgBox.setText("Can not set FFT to fullscreen when extended vertical range is set.\n"
                     "Uncheck \"Use extended vertical range\" checkbox in the settings menu first.");
      msgBox.exec();
      return;
    }

    devparms.math_fft_split = 0;

    set_cue_cmd(":MATH:FFT:SPL OFF");

    statusLabel->setText("FFT fullscreen");
  }
  else
  {
    set_cue_cmd(":MATH:FFT:SPL ON");

    devparms.math_fft_split = 1;

    statusLabel->setText("FFT splitscreen");
  }
}


void UI_Mainwindow::toggle_fft_unit()
{
  char str[512];

  if(devparms.math_fft_unit == 1)
  {
    devparms.fft_vscale = 1.0;

    devparms.fft_voffset = 0.0;

    devparms.math_fft_unit = 0;

    if(devparms.modelserie != 1)
    {
      set_cue_cmd(":CALC:FFT:VSM VRMS");
    }
    else
    {
      set_cue_cmd(":MATH:FFT:UNIT VRMS");
    }

    sprintf(str, ":MATH:OFFS %e", devparms.fft_voffset);

    set_cue_cmd(str);

    sprintf(str, ":MATH:SCAL %e", devparms.fft_vscale);

    set_cue_cmd(str);

    sprintf(str, ":MATH:OFFS %e", devparms.fft_voffset);

    set_cue_cmd(str);

    statusLabel->setText("FFT unit: Vrms");
  }
  else
  {
    devparms.fft_vscale = 10.0;

    devparms.fft_voffset = 20.0;

    devparms.math_fft_unit = 1;

    if(devparms.modelserie != 1)
    {
      set_cue_cmd(":CALC:FFT:VSM DBVR");
    }
    else
    {
     set_cue_cmd(":MATH:FFT:UNIT DB");
    }

    sprintf(str, ":MATH:OFFS %e", devparms.fft_voffset);

    set_cue_cmd(str);

    sprintf(str, ":MATH:SCAL %e", devparms.fft_vscale);

    set_cue_cmd(str);

    sprintf(str, ":MATH:OFFS %e", devparms.fft_voffset);

    set_cue_cmd(str);

    statusLabel->setText("FFT unit: dB");
  }
}


void UI_Mainwindow::select_fft_ch1()
{
  if(devparms.modelserie != 6)
  {
    set_cue_cmd(":MATH:FFT:SOUR CHAN1");
  }

  devparms.math_fft_src = 0;

  statusLabel->setText("FFT source: CH1");
}


void UI_Mainwindow::select_fft_ch2()
{
  if(devparms.modelserie != 6)
  {
    set_cue_cmd(":MATH:FFT:SOUR CHAN2");
  }

  devparms.math_fft_src = 1;

  statusLabel->setText("FFT source: CH2");
}


void UI_Mainwindow::select_fft_ch3()
{
  if(devparms.modelserie != 6)
  {
    set_cue_cmd(":MATH:FFT:SOUR CHAN3");
  }

  devparms.math_fft_src = 2;

  statusLabel->setText("FFT source: CH3");
}


void UI_Mainwindow::select_fft_ch4()
{
  if(devparms.modelserie != 6)
  {
    set_cue_cmd(":MATH:FFT:SOUR CHAN4");
  }

  devparms.math_fft_src = 3;

  statusLabel->setText("FFT source: CH4");
}


void UI_Mainwindow::select_fft_hzdiv_20()
{
  set_fft_hzdiv(20.0);
}


void UI_Mainwindow::select_fft_hzdiv_40()
{
  set_fft_hzdiv(40.0);
}


void UI_Mainwindow::select_fft_hzdiv_80()
{
  set_fft_hzdiv(80.0);
}


void UI_Mainwindow::select_fft_hzdiv_100()
{
  set_fft_hzdiv(100.0);
}


void UI_Mainwindow::select_fft_hzdiv_200()
{
  set_fft_hzdiv(200.0);
}


void UI_Mainwindow::set_fft_hzdiv(double val)
{
  char str[512];

  if(devparms.timebasedelayenable)
  {
    devparms.math_fft_hscale = (100.0 / devparms.timebasedelayscale) / val;
  }
  else
  {
    devparms.math_fft_hscale = (100.0 / devparms.timebasescale) / val;
  }

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HSP %e", devparms.math_fft_hscale);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HSC %e", devparms.math_fft_hscale);
  }

  set_cue_cmd(str);

  strcpy(str, "FFT scale: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale, 2);

  strcat(str, "Hz/Div");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_5()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 5.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 5.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 5.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 5.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_6()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 6.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 6.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 6.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 6.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_7()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 7.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 7.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 7.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 7.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_8()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 8.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 8.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 8.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 8.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_9()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 9.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 9.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 9.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 9.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_10()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 10.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 10.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 10.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 10.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_11()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 11.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 11.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 11.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale *11.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_ctr_12()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:HCEN %e", devparms.math_fft_hscale * 12.0);
  }
  else
  {
    sprintf(str, ":MATH:FFT:HCEN %e", devparms.math_fft_hscale * 12.0);
  }

  set_cue_cmd(str);

  devparms.math_fft_hcenter = devparms.math_fft_hscale * 12.0;

  strcpy(str, "FFT center: ");

  convert_to_metric_suffix(str + strlen(str), devparms.math_fft_hscale * 12.0, 1);

  strcat(str, "Hz");

  statusLabel->setText(str);
}


void UI_Mainwindow::select_fft_vscale1()
{
  devparms.fft_vscale = 1.0;

  set_fft_vscale();
}


void UI_Mainwindow::select_fft_vscale2()
{
  devparms.fft_vscale = 2.0;

  set_fft_vscale();
}


void UI_Mainwindow::select_fft_vscale5()
{
  devparms.fft_vscale = 5.0;

  set_fft_vscale();
}


void UI_Mainwindow::select_fft_vscale10()
{
  devparms.fft_vscale = 10.0;

  set_fft_vscale();
}


void UI_Mainwindow::select_fft_vscale20()
{
  devparms.fft_vscale = 20.0;

  set_fft_vscale();
}


void UI_Mainwindow::set_fft_vscale()
{
  char str[512];

  if(device == NULL)
  {
    return;
  }

  if(!devparms.connected)
  {
    return;
  }

  if(devparms.activechannel < 0)
  {
    return;
  }

  if(devparms.fft_voffset > (devparms.fft_vscale * 4.0))
  {
    devparms.fft_voffset = (devparms.fft_vscale * 4.0);
  }

  if(devparms.fft_voffset < (devparms.fft_vscale * -4.0))
  {
    devparms.fft_voffset = (devparms.fft_vscale * -4.0);
  }

  if(devparms.modelserie != 1)
  {
    if(devparms.math_fft_unit == 1)
    {
      sprintf(str, ":CALC:FFT:VSC %e", devparms.fft_vscale);

      set_cue_cmd(str);
    }
    else
    {
      sprintf(str, ":CALC:FFT:VSC %e", devparms.fft_vscale / devparms.chanscale[devparms.math_fft_src]);

      set_cue_cmd(str);
    }
  }
  else
  {
    sprintf(str, ":MATH:SCAL %e", devparms.fft_vscale);

    set_cue_cmd(str);
  }

  if(devparms.math_fft_unit == 0)
  {
    strcpy(str, "FFT scale: ");

    convert_to_metric_suffix(str + strlen(str), devparms.fft_vscale, 1);

    strcat(str, "V");
  }
  else
  {
    sprintf(str, "FFT scale: %+.1fdB/Div", devparms.fft_vscale);
  }

  statusLabel->setText(str);

  waveForm->update();
}


void UI_Mainwindow::select_fft_voffsetp4()
{
  devparms.fft_voffset = devparms.fft_vscale * 4.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetp3()
{
  devparms.fft_voffset = devparms.fft_vscale * 3.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetp2()
{
  devparms.fft_voffset = devparms.fft_vscale * 2.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetp1()
{
  devparms.fft_voffset = devparms.fft_vscale;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffset0()
{
  devparms.fft_voffset = 0.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetm1()
{
  devparms.fft_voffset = devparms.fft_vscale * -1.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetm2()
{
  devparms.fft_voffset = devparms.fft_vscale * -2.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetm3()
{
  devparms.fft_voffset = devparms.fft_vscale * -3.0;

  set_fft_voffset();
}


void UI_Mainwindow::select_fft_voffsetm4()
{
  devparms.fft_voffset = devparms.fft_vscale * -4.0;

  set_fft_voffset();
}


void UI_Mainwindow::set_fft_voffset()
{
  char str[512];

  if(devparms.modelserie != 1)
  {
    sprintf(str, ":CALC:FFT:VOFF %e", devparms.fft_voffset);

    set_cue_cmd(str);
  }
  else
  {
    sprintf(str, ":MATH:OFFS %e", devparms.fft_voffset);

    set_cue_cmd(str);
  }

  if(devparms.math_fft_unit == 0)
  {
    strcpy(str, "FFT position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.fft_voffset, 1);

    strcat(str, "V");
  }
  else
  {
    sprintf(str, "FFT position: %+.0fdB", devparms.fft_voffset);
  }

  statusLabel->setText(str);

  waveForm->label_active = LABEL_ACTIVE_FFT;

  label_timer->start(LABEL_TIMER_IVAL);

  waveForm->update();
}


void UI_Mainwindow::show_decode_window()
{
  UI_decoder_window w(this);
}
















