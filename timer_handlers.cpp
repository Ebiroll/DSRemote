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




void UI_Mainwindow::test_timer_handler()
{
  printf("scr_update_cntr is: %u\n", waveForm->scr_update_cntr);

  waveForm->scr_update_cntr = 0;
}


void UI_Mainwindow::label_timer_handler()
{
  waveForm->label_active = LABEL_ACTIVE_NONE;
}


void UI_Mainwindow::navDial_timer_handler()
{
  if(navDial->isSliderDown() == true)
  {
    navDialChanged(navDial->value());
  }
  else
  {
    navDial->setSliderPosition(50);

    navDialFunc = NAV_DIAL_FUNC_NONE;

    if(adjdial_timer->isActive() == false)
    {
      adjDialFunc = ADJ_DIAL_FUNC_NONE;
    }

    if(devparms.screenupdates_on == 1)
    {
      scrn_timer->start(devparms.screentimerival);
    }
  }
}


void UI_Mainwindow::adjdial_timer_handler()
{
  char str[512];

  adjdial_timer->stop();

  adjDialLabel->setStyleSheet(def_stylesh);

  adjDialLabel->setStyleSheet("font: 7pt;");

  adjDialLabel->setText("");

  if(adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF)
  {
    strcpy(str, "Trigger holdoff: ");

    convert_to_metric_suffix(str + strlen(str), devparms.triggerholdoff, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TRIG:HOLD %e", devparms.triggerholdoff);

    tmc_write(str);

    if(devparms.modelserie == 6)
    {
      usleep(20000);

      tmc_write(":CLE");
    }
  }
  else if(adjDialFunc == ADJ_DIAL_FUNC_ACQ_AVG)
    {
      sprintf(str, "Acquire averages: %i", devparms.acquireaverages);

      statusLabel->setText(str);

      sprintf(str, ":ACQ:AVER %i", devparms.acquireaverages);

      tmc_write(str);
    }

  adjDialFunc = ADJ_DIAL_FUNC_NONE;

  if(navDial_timer->isActive() == false)
  {
    navDialFunc = NAV_DIAL_FUNC_NONE;
  }

  if(devparms.screenupdates_on == 1)
  {
    scrn_timer->start(devparms.screentimerival);
  }
}


void UI_Mainwindow::stat_timer_handler()
{
  int line;

  char str[512];

  if(tmc_write(":TRIG:STAT?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

//  old_stat = devparms.triggerstatus;

  runButton->setStyleSheet(def_stylesh);

  singleButton->setStyleSheet(def_stylesh);

  if(!strcmp(device->buf, "TD"))
  {
    devparms.triggerstatus = 0;

    runButton->setStyleSheet("background: #66FF99;");
  }
  else if(!strcmp(device->buf, "WAIT"))
    {
      devparms.triggerstatus = 1;

      singleButton->setStyleSheet("background: #FF9966;");
    }
    else if(!strcmp(device->buf, "RUN"))
      {
        devparms.triggerstatus = 2;

        runButton->setStyleSheet("background: #66FF99;");
      }
      else if(!strcmp(device->buf, "AUTO"))
        {
          devparms.triggerstatus = 3;

          runButton->setStyleSheet("background: #66FF99;");
        }
        else if(!strcmp(device->buf, "FIN"))
          {
            devparms.triggerstatus = 4;
          }
          else if(!strcmp(device->buf, "STOP"))
            {
              devparms.triggerstatus = 5;

              runButton->setStyleSheet("background: #FF0066;");
            }
            else
            {
              line = __LINE__;
              goto OUT_ERROR;
            }

  if(tmc_write(":TRIG:SWE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "AUTO"))
  {
    devparms.triggersweep = 0;

    trigModeAutoLed->setValue(true);
    trigModeNormLed->setValue(false);
    trigModeSingLed->setValue(false);
  }
  else if(!strcmp(device->buf, "NORM"))
    {
      devparms.triggersweep = 1;

      trigModeAutoLed->setValue(false);
      trigModeNormLed->setValue(true);
      trigModeSingLed->setValue(false);
    }
    else if(!strcmp(device->buf, "SING"))
      {
        devparms.triggersweep = 2;

        trigModeAutoLed->setValue(false);
        trigModeNormLed->setValue(false);
        trigModeSingLed->setValue(true);
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(tmc_write(":ACQ:SRAT?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.samplerate = atof(device->buf);

  if(tmc_write(":ACQ:MDEP?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.memdepth = atoi(device->buf);

  if(devparms.countersrc)
  {
    if(tmc_write(":MEAS:COUN:VAL?") != 15)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.counterfreq = atof(device->buf);
  }

  return;

OUT_ERROR:

  sprintf(str, "An error occurred while reading status from device.\n"
               "File %s line %i", __FILE__, line);

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();

  close_connection();
}


void UI_Mainwindow::scrn_timer_handler()
{
  int i, j, n=0, chns=0;

  char str[128];

  if(device == NULL)
  {
    return;
  }

  stat_timer_handler();

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    chns++;
  }

  if(!chns)
  {
    waveForm->clear();

    return;
  }

//struct waveform_preamble wfp;

  if(devparms.triggerstatus != 1)  // Don't download waveform data when triggerstatus is "wait"
  {
    for(i=0; i<MAX_CHNS; i++)
    {
      if(!devparms.chandisplay[i])  // Download data only when channel is switched on
      {
        continue;
      }

///////////////////////////////////////////////////////////

//     tmc_write(":WAV:PRE?");
//
//     n = tmc_read();
//
//     if(n < 0)
//     {
//       strcpy(str, "Can not read from device.");
//       goto OUT_ERROR;
//     }
//
//     printf("waveform preamble: %s\n", device->buf);
//
//     if(parse_preamble(device->buf, device->sz, &wfp, i))
//     {
//       strcpy(str, "Preamble parsing error.");
//       goto OUT_ERROR;
//     }
//
//     printf("waveform preamble:\n"
//            "format: %i\n"
//            "type: %i\n"
//            "points: %i\n"
//            "count: %i\n"
//            "xincrement: %e\n"
//            "xorigin: %e\n"
//            "xreference: %e\n"
//            "yincrement: %e\n"
//            "yorigin: %e\n"
//            "yreference: %i\n",
//            wfp.format, wfp.type, wfp.points, wfp.count,
//            wfp.xincrement[i], wfp.xorigin[i], wfp.xreference[i],
//            wfp.yincrement[i], wfp.yorigin[i], wfp.yreference[i]);
//
//     printf("chanoffset[] is %e\n", devparms.chanoffset[i]);

//     rec_len = wfp.xincrement[i] * wfp.points;

///////////////////////////////////////////////////////////

      sprintf(str, ":WAV:SOUR CHAN%i", i + 1);

      tmc_write(str);

      tmc_write(":WAV:FORM BYTE");

      tmc_write(":WAV:MODE NORM");

      tmc_write(":WAV:DATA?");

      n = tmc_read();

      if(n < 0)
      {
        printf("Can not read from device.\n");
        return;
      }

      if(n > WAVFRM_MAX_BUFSZ)
      {
        strcpy(str, "Datablock too big for buffer.");
        goto OUT_ERROR;
      }

      if(n < 32)
      {
        n = 0;
      }

      for(j=0; j<n; j++)
      {
        devparms.wavebuf[i][j] = (int)(((unsigned char *)device->buf)[j]) - 127;
      }
    }
  }

  devparms.wavebufsz = n;

  waveForm->drawCurve(&devparms, device, n);

  return;

OUT_ERROR:

  scrn_timer->stop();

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();
}

























