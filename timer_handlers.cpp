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


void UI_Mainwindow::navDial_timer_handler()
{
  if(navDial->isSliderDown() == true)
  {
    navDialChanged(navDial->value());
  }
  else
  {
    navDial->setSliderPosition(50);
  }
}


void UI_Mainwindow::adjdial_timer_handler()
{
  if(adjDialCnt % 2)
  {
    adjDialLabel->setStyleSheet("background: #66FF99; font: 7pt;");
  }
  else
  {
    adjDialLabel->setStyleSheet(def_stylesh);

    adjDialLabel->setStyleSheet("font: 7pt;");
  }

  if(++adjDialCnt > ADJ_DIAL_CNT_TIMEOUT)
  {
    adjdial_timer->stop();

    adjDialLabel->setStyleSheet(def_stylesh);

    adjDialLabel->setStyleSheet("font: 7pt;");
  }
}


void UI_Mainwindow::stat_timer_handler()
{
  int line;

  char str[512];

  if(tmcdev_write(device, ":TRIG:STAT?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
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
              device->hdrbuf[32] = 0;
              printf(" Status error hdrbuf: %s<-\n", device->hdrbuf);

              device->buf[32] = 0;
              printf(" Status error buf: %s<-\n", device->buf);

              for(int i=0; i<32; i++)
              {
                printf("  %i:%02X", i, device->buf[i]);
              }
              printf("\n");

              line = __LINE__;
              goto OUT_ERROR;
            }

  if(tmcdev_write(device, ":TRIG:SWE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
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

  if(tmcdev_write(device, ":ACQ:SRAT?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.samplerate = atof(device->buf);

  if(tmcdev_write(device, ":ACQ:MDEP?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.memdepth = atoi(device->buf);

//   if(old_stat != devparms.triggerstatus)
//   {
//     printf("Status change: %i\n", devparms.triggerstatus);
//   }

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

//   tmcdev_write(device, ":ACQ:SRAT?");
//
//   devparms.samplerate = atof(device->buf);
//
  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    sprintf(str, ":WAV:SOUR CHAN%i", i + 1);

    tmcdev_write(device, str);

//     tmcdev_write(device, ":WAV:YOR?");
//
//     n = tmcdev_read(device);
//
//     if(n < 0)
//     {
//       printf("Can not read from device.\n");
//       return;
//     }
//
//     devparms.preamble.yorigin[i] = atof(device->buf);

    tmcdev_write(device, ":WAV:FORM BYTE");

    tmcdev_write(device, ":WAV:MODE NORM");

//     tmcdev_write(device, ":WAV:PRE?");
//
//     n = tmcdev_read(device);
//
//     if(n < 0)
//     {
//       strcpy(str, "Can not read from device.");
//       goto OUT_ERROR;
//     }
//
//     printf("waveform preamble: %s\n", device->buf);

//     if(parse_preamble(device->buf, device->sz, &devparms.preamble, i))
//     {
//       strcpy(str, "Preamble parsing error.");
//       goto OUT_ERROR;
//     }

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
//            "yreference: %e\n",
//            devparms.preamble.format, devparms.preamble.type, devparms.preamble.points, devparms.preamble.count,
//            devparms.preamble.xincrement[i], devparms.preamble.xorigin[i], devparms.preamble.xreference[i],
//            devparms.preamble.yincrement[i], devparms.preamble.yorigin[i], devparms.preamble.yreference[i]);

    tmcdev_write(device, ":WAV:DATA?");

    n = tmcdev_read(device);

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

    if(n < 16)
    {
      return;
    }

//    yref = devparms.preamble.yreference[i];

    for(j=0; j<n; j++)
    {
      devparms.wavebuf[i][j] = (device->buf[j] + 128);
    }
  }

  waveForm->drawCurve(&devparms, device, n);

  return;

OUT_ERROR:

  scrn_timer->stop();

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();
}

























