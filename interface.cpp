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
  char str[512];

  if(navDial->isSliderDown() == true)
  {
    navDial_timer->start(100);
  }
  else
  {
    navDial_timer->start(300);
  }

  if(navDialFunc == NAV_DIAL_FUNC_HOLDOFF)
  {
    if(npos > 93)
      {
        devparms.triggerholdoff += 1e-1;
      }
      else if(npos > 86)
        {
          devparms.triggerholdoff += 1e-2;
        }
        else if(npos > 79)
          {
            devparms.triggerholdoff += 1e-3;
          }
          else if(npos > 72)
            {
              devparms.triggerholdoff += 1e-4;
            }
            else if(npos > 65)
              {
                devparms.triggerholdoff += 1e-5;
              }
              else if(npos > 58)
                {
                  devparms.triggerholdoff += 1e-6;
                }
                else if(npos > 51)
                  {
                    devparms.triggerholdoff += 1e-7;
                  }
                  else if(npos > 49)
                    {
                      return;
                    }
                    else if(npos > 42)
                      {
                        devparms.triggerholdoff -= 1e-7;
                      }
                      else if(npos > 35)
                        {
                          devparms.triggerholdoff -= 1e-6;
                        }
                        else if(npos > 28)
                          {
                            devparms.triggerholdoff -= 1e-5;
                          }
                          else if(npos > 21)
                            {
                              devparms.triggerholdoff -= 1e-4;
                            }
                            else if(npos > 14)
                              {
                                devparms.triggerholdoff -= 1e-3;
                              }
                              else if(npos > 7)
                                {
                                  devparms.triggerholdoff -= 1e-2;
                                }
                                else
                                {
                                  devparms.triggerholdoff -= 1e-1;
                                }


    if(devparms.triggerholdoff < 1e-7)
    {
      devparms.triggerholdoff = 1e-7;
    }

    if(devparms.triggerholdoff > 10)
    {
      devparms.triggerholdoff = 10;
    }

    strcpy(str, "Holdoff: ");

    convert_to_metric_suffix(str + strlen(str), devparms.triggerholdoff);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TRIG:HOLD %e", devparms.triggerholdoff);

    tmcdev_write(device, str);
  }
}


void UI_Mainwindow::navDialReleased()
{
  navDial->setSliderPosition(50);
}


void UI_Mainwindow::acqButtonClicked()
{
}


void UI_Mainwindow::cursButtonClicked()
{
}


void UI_Mainwindow::saveButtonClicked()
{
  QMenu menu;

  menu.addAction("Save waveform",   this, SLOT(save_waveform()));
  menu.addAction("Save screenshot", this, SLOT(save_screenshot()));

  menu.exec(saveButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::dispButtonClicked()
{
  QMenu menu,
        submenugrid,
        submenugrading;

  submenugrid.setTitle("Grid");
  submenugrid.addAction("Full", this, SLOT(set_grid_full()));
  submenugrid.addAction("Half", this, SLOT(set_grid_half()));
  submenugrid.addAction("None", this, SLOT(set_grid_none()));
  menu.addMenu(&submenugrid);

  submenugrading.setTitle("Grading");
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


void UI_Mainwindow::set_grid_full()
{
  devparms.displaygrid = 2;

  tmcdev_write(device, ":DISP:GRID FULL");
}


void UI_Mainwindow::set_grid_half()
{
  devparms.displaygrid = 1;

  tmcdev_write(device, ":DISP:GRID HALF");
}


void UI_Mainwindow::set_grid_none()
{
  devparms.displaygrid = 0;

  tmcdev_write(device, ":DISP:GRID NONE");
}


void UI_Mainwindow::set_grading_min()
{
  tmcdev_write(device, ":DISP:GRAD:TIM MIN");
}


void UI_Mainwindow::set_grading_005()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 0.05");
}


void UI_Mainwindow::set_grading_01()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 0.1");
}


void UI_Mainwindow::set_grading_02()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 0.2");
}


void UI_Mainwindow::set_grading_05()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 0.5");
}


void UI_Mainwindow::set_grading_1()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 1");
}


void UI_Mainwindow::set_grading_2()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 2");
}


void UI_Mainwindow::set_grading_5()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 5");
}


void UI_Mainwindow::set_grading_10()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 10");
}


void UI_Mainwindow::set_grading_20()
{
  tmcdev_write(device, ":DISP:GRAD:TIM 20");
}


void UI_Mainwindow::set_grading_inf()
{
  tmcdev_write(device, ":DISP:GRAD:TIM INF");
}


void UI_Mainwindow::utilButtonClicked()
{
}


void UI_Mainwindow::helpButtonClicked()
{
}


void UI_Mainwindow::show_about_dialog()
{
  UI_Aboutwindow aboutwindow;
}


void UI_Mainwindow::adjDialChanged(int new_pos)
{
  static int old_pos=0;

  int diff, dir;

  char str[512];

  adjdial_timer->stop();

  adjDialLabel->setStyleSheet(def_stylesh);

  adjDialLabel->setStyleSheet("font: 7pt;");

  if(adjDialFunc == ADJ_DIAL_FUNC_NONE)
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

  if(adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF)
  {
    if(dir)
    {
      if(devparms.triggerholdoff >= 10)
      {
        devparms.triggerholdoff = 10;

        old_pos = new_pos;

        return;
      }

      devparms.triggerholdoff += devparms.timebasescale / 10;
    }
    else
    {
      if(devparms.triggerholdoff <= 1e-7)
      {
        devparms.triggerholdoff = 1e-7;

        old_pos = new_pos;

        return;
      }

      devparms.triggerholdoff -= devparms.timebasescale / 10;
    }

    strcpy(str, "Holdoff: ");

    convert_to_metric_suffix(str + strlen(str), devparms.triggerholdoff);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TRIG:HOLD %e", devparms.triggerholdoff);

    tmcdev_write(device, str);
  }

  old_pos = new_pos;
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

  convert_to_metric_suffix(str + strlen(str), devparms.triggeredgelevel[chn]);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":TRIG:EDG:LEV %e", devparms.triggeredgelevel[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;
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
      if(devparms.bandwidth == 1000)
      {
        if(devparms.timebasedelayscale <= 5e-10)
        {
          devparms.timebasedelayscale = 5e-10;

          old_pos = new_pos;

          return;
        }
      }
      else
      {
        if(devparms.timebasedelayscale <= 1e-9)
        {
          devparms.timebasedelayscale = 1e-9;

          old_pos = new_pos;

          return;
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

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayscale);

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
      if(devparms.bandwidth == 1000)
      {
        if(devparms.timebasescale <= 5e-10)
        {
          devparms.timebasescale = 5e-10;

          old_pos = new_pos;

          return;
        }
      }
      else
      {
        if(devparms.timebasescale <= 1e-9)
        {
          devparms.timebasescale = 1e-9;

          old_pos = new_pos;

          return;
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

    convert_to_metric_suffix(str + strlen(str), devparms.timebasescale);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:SCAL %e", devparms.timebasescale);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }
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
      if(devparms.timebasedelayoffset >= ((7 * devparms.timebasescale) + devparms.timebaseoffset - (7 * devparms.timebasedelayscale)))
      {
        old_pos = new_pos;

        return;
      }

      devparms.timebasedelayoffset += (devparms.timebasedelayscale / 50);
    }
    else
    {
      if(devparms.timebasedelayoffset <= -((7 * devparms.timebasescale) - devparms.timebaseoffset - (7 * devparms.timebasedelayscale)))
      {
        old_pos = new_pos;

        return;
      }

      devparms.timebasedelayoffset -= (devparms.timebasedelayscale / 50);
    }

    strcpy(str, "Horizontal delay position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset);

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

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:OFFS %e", devparms.timebaseoffset);

    tmcdev_write(device, str);

    old_pos = new_pos;
  }
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

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn]);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms.chanoffset[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;
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
    if(devparms.chanscale[chn] <= 2e-2)
    {
      devparms.chanscale[chn] = 2e-2;

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

  convert_to_metric_suffix(str + strlen(str), devparms.chanscale[chn]);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:SCAL %e", chn + 1, devparms.chanscale[chn]);

  tmcdev_write(device, str);

  old_pos = new_pos;
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

  sprintf(str, ":CHAN%i:COUP AC", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_coupling_dc()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 1;

  sprintf(str, ":CHAN%i:COUP DC", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_coupling_gnd()
{
  char str[128];

  devparms.chancoupling[devparms.activechannel] = 0;

  sprintf(str, ":CHAN%i:COUP GND", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_off()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 0;

  sprintf(str, ":CHAN%i:BWL OFF", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_20()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 20;

  sprintf(str, ":CHAN%i:BWL 20M", devparms.activechannel + 1);

  tmcdev_write(device, str);
}


void UI_Mainwindow::chan_bwl_250()
{
  char str[128];

  devparms.chanbwlimit[devparms.activechannel] = 250;

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

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn]);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms.chanoffset[chn]);

  tmcdev_write(device, str);
}


void UI_Mainwindow::clearButtonClicked()
{
  tmcdev_write(device, ":DISP:CLE");
}


void UI_Mainwindow::autoButtonClicked()
{
  tmcdev_write(device, ":AUT");
}


void UI_Mainwindow::runButtonClicked()
{
  if(devparms.triggerstatus == 5)
  {
    tmcdev_write(device, ":RUN");
  }
  else
  {
    tmcdev_write(device, ":STOP");
  }
}


void UI_Mainwindow::singleButtonClicked()
{
  tmcdev_write(device, ":SING");
}


void UI_Mainwindow::adjustDialClicked(QPoint)
{
  if(adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF)
  {
    devparms.triggerholdoff = 1e-7;

    statusLabel->setText("Holdoff: 100ns");

    tmcdev_write(device, ":TRIG:HOLD 1e-7");
  }
}


void UI_Mainwindow::horMenuButtonClicked()
{
  QMenu menu;

  menu.addAction("Delayed", this, SLOT(horizontal_delayed()));

  menu.exec(horMenuButton->mapToGlobal(QPoint(0,0)));
}


void UI_Mainwindow::horizontal_delayed()
{
  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayenable = 0;

    statusLabel->setText("Delayed timebase disabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 0");
  }
  else
  {
    devparms.timebasedelayenable = 1;

    statusLabel->setText("Delayed timebase enabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 1");
  }
}


void UI_Mainwindow::horPosDialClicked(QPoint)
{
//   QMenu menu;
//
//   menu.addAction("Zero", this, SLOT(horizontal_position_zero()));
//
//   menu.exec(horPosDial->mapToGlobal(QPoint(0,0)));
  char str[512];

  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayoffset = 0;

    strcpy(str, "Horizontal delay position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:OFFS %e", devparms.timebasedelayoffset);

    tmcdev_write(device, str);
  }
  else
  {
    devparms.timebaseoffset = 0;

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:OFFS %e", devparms.timebaseoffset);

    tmcdev_write(device, str);
  }
}


void UI_Mainwindow::horScaleDialClicked(QPoint)
{
  if(devparms.timebasedelayenable)
  {
    devparms.timebasedelayenable = 0;

    statusLabel->setText("Delayed timebase disabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 0");
  }
  else
  {
    devparms.timebasedelayenable = 1;

    statusLabel->setText("Delayed timebase enabled");

    tmcdev_write(device, ":TIM:DEL:ENAB 1");
  }
}


void UI_Mainwindow::trigModeButtonClicked()
{
  devparms.triggersweep++;

  devparms.triggersweep %= 3;

  switch(devparms.triggersweep)
  {
    case 0: trigModeAutoLed->setValue(true);
            trigModeSingLed->setValue(false);
            tmcdev_write(device, ":TRIG:SWE AUTO");
            break;
    case 1: trigModeNormLed->setValue(true);
            trigModeAutoLed->setValue(false);
            tmcdev_write(device, ":TRIG:SWE NORM");
            break;
    case 2: trigModeSingLed->setValue(true);
            trigModeNormLed->setValue(false);
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

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN1");
}


void UI_Mainwindow::trigger_source_ch2()
{
  devparms.triggeredgesource = 1;

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN2");
}


void UI_Mainwindow::trigger_source_ch3()
{
  devparms.triggeredgesource = 2;

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN3");
}


void UI_Mainwindow::trigger_source_ch4()
{
  devparms.triggeredgesource = 3;

  tmcdev_write(device, ":TRIG:EDG:SOUR CHAN4");
}


void UI_Mainwindow::trigger_source_ext()
{
  devparms.triggeredgesource = 4;

  tmcdev_write(device, ":TRIG:EDG:SOUR EXT");
}


void UI_Mainwindow::trigger_source_ext5()
{
  devparms.triggeredgesource = 5;

  tmcdev_write(device, ":TRIG:EDG:SOUR EXT5");
}


void UI_Mainwindow::trigger_source_acl()
{
  devparms.triggeredgesource = 6;

  tmcdev_write(device, ":TRIG:EDG:SOUR ACL");
}


void UI_Mainwindow::trigger_coupling_ac()
{
  devparms.triggercoupling = 0;

  tmcdev_write(device, ":TRIG:COUP AC");
}


void UI_Mainwindow::trigger_coupling_dc()
{
  devparms.triggercoupling = 1;

  tmcdev_write(device, ":TRIG:COUP DC");
}


void UI_Mainwindow::trigger_coupling_lfreject()
{
  devparms.triggercoupling = 2;

  tmcdev_write(device, ":TRIG:COUP LFR");
}


void UI_Mainwindow::trigger_coupling_hfreject()
{
  devparms.triggercoupling = 3;

  tmcdev_write(device, ":TRIG:COUP HFR");
}


void UI_Mainwindow::trigger_slope_pos()
{
  devparms.triggeredgeslope = 0;

  tmcdev_write(device, ":TRIG:EDG:SLOP POS");
}


void UI_Mainwindow::trigger_slope_neg()
{
  devparms.triggeredgeslope = 1;

  tmcdev_write(device, ":TRIG:EDG:SLOP NEG");
}


void UI_Mainwindow::trigger_slope_rfal()
{
  devparms.triggeredgeslope = 2;

  tmcdev_write(device, ":TRIG:EDG:SLOP RFAL");
}


void UI_Mainwindow::trigger_setting_holdoff()
{
  navDialFunc = NAV_DIAL_FUNC_HOLDOFF;

  adjDialFunc = ADJ_DIAL_FUNC_HOLDOFF;

  adjDialLabel->setText("Holdoff");

  adjDialCnt = 0;

  adjdial_timer->start(ADJDIAL_TIMER_IVAL);
}


void UI_Mainwindow::trigForceButtonClicked()
{
  tmcdev_write(device, ":TFOR");
}


void UI_Mainwindow::trig50pctButtonClicked()
{
  tmcdev_write(device, ":TLHA");

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

  convert_to_metric_suffix(str + strlen(str), devparms.triggeredgelevel[devparms.triggeredgesource]);

  strcat(str, "V");

  statusLabel->setText(str);

  sprintf(str, ":TRIG:EDG:LEV %e", devparms.triggeredgelevel[devparms.triggeredgesource]);

  tmcdev_write(device, str);
}








