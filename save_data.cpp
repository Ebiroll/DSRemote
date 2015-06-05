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




void UI_Mainwindow::save_screenshot()
{
  int n;

  char str[128],
       opath[MAX_PATHLEN];

  QPainter painter;

  QPainterPath path;

  if(device == NULL)
  {
    return;
  }

  tmcdev_write(device, ":DISP:DATA?");

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  n = tmcdev_read(device);

  QApplication::restoreOverrideCursor();

  if(n < 0)
  {
    strcpy(str, "Can not read from device.\n"
                "Have you already patched the usbtmc driver?");
    goto OUT_ERROR;
  }

  if(device->sz != SCRN_SHOT_BMP_SZ)
  {
    strcpy(str, "Error, bitmap has wrong filesize\n");
    goto OUT_ERROR;
  }

  if(strncmp(device->buf, "BM", 2))
  {
    strcpy(str, "Error, file is not a bitmap\n");
    goto OUT_ERROR;
  }

  memcpy(devparms.screenshot_buf, device->buf, SCRN_SHOT_BMP_SZ);

  screenXpm.loadFromData((uchar *)(devparms.screenshot_buf), SCRN_SHOT_BMP_SZ);

  if(!strncmp(devparms.modelname, "DS6", 3))
  {
    painter.begin(&screenXpm);

    painter.fillRect(0, 0, 95, 29, QColor(48, 48, 48));

    path.addRoundedRect(5, 5, 85, 20, 3, 3);

    painter.fillPath(path, Qt::black);

    painter.setPen(Qt::white);

    painter.drawText(5, 5, 85, 20, Qt::AlignCenter, devparms.modelname);

    painter.end();
  }

  opath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(opath, recent_savedir);
    strcat(opath, "/");
  }
  strcat(opath, "screenshot.png");

  strcpy(opath, QFileDialog::getSaveFileName(this, "Save file", opath, "PNG files (*.png *.PNG)").toLocal8Bit().data());

  if(!strcmp(opath, ""))
  {
    return;
  }

  get_directory_from_path(recent_savedir, opath, MAX_PATHLEN);

//   FILE *f = fopen(opath, "wb");
//   if(f == NULL)
//   {
//     QMessageBox msgBox;
//     msgBox.setIcon(QMessageBox::Critical);
//     msgBox.setText("Can not open file for writing");
//     msgBox.exec();
//
//     return;
//   }
//
//   fwrite((uchar *)devparms.screenshot_buf, SCRN_SHOT_BMP_SZ, 1, f);
//
//   fclose(f);

  if(screenXpm.save(QString::fromLocal8Bit(opath), "PNG", 50) == false)
  {
    strcpy(str, "Could not save file (unknown error)");
    goto OUT_ERROR;
  }

  return;

OUT_ERROR:

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();
}


void UI_Mainwindow::save_memory_waveform()
{
//   int i, j, k, ready, n=0, chns=0, hdl=-1, yoffset[MAX_CHNS], bytes_rcvd;
//
//   char str[128],
//        opath[MAX_PATHLEN];
//
//   short *wavbuf[4];
//
//   double rec_len = 0;
//
// //  struct waveform_preamble wfp;
//
//   if(device == NULL)
//   {
//     return;
//   }
//
//   wavbuf[0] = NULL;
//   wavbuf[1] = NULL;
//   wavbuf[2] = NULL;
//   wavbuf[3] = NULL;
//
//   rec_len = devparms.memdepth / devparms.samplerate;
//
//   if(rec_len < 1e-6)
//   {
//     strcpy(str, "Can not save waveforms shorter than 1 uSec.");
//     goto OUT_ERROR;
//   }
//
//   for(i=0; i<MAX_CHNS; i++)
//   {
//     if(!devparms.chandisplay[i])  // Download data only when channel is switched on
//     {
//       continue;
//     }
//
//     wavbuf[i] = (short *)malloc(devparms.memdepth * sizeof(short));
//     if(wavbuf[i] == NULL)
//     {
//       strcpy(str, "Malloc error.");
//       goto OUT_ERROR;
//     }
//
//     chns++;
//   }
//
//   if(!chns)
//   {
//     strcpy(str, "No active channels.");
//     goto OUT_ERROR;
//   }
//
//   scrn_timer->stop();
//
//   stat_timer->stop();
//
//   tmcdev_write(device, ":STOP");
//
//   for(i=0; i<MAX_CHNS; i++)
//   {
//     if(!devparms.chandisplay[i])  // Download data only when channel is switched on
//     {
//       continue;
//     }
//
//     sprintf(str, ":WAV:SOUR CHAN%i", i + 1);
//
//     tmcdev_write(device, str);
//
//     tmcdev_write(device, ":WAV:FORM BYTE");
//
//     sleep(1);
//
//     tmcdev_write(device, ":WAV:MODE RAW");
//
// tmcdev_write(device, ":WAV:POIN?");
//
// n = tmcdev_read(device);
//
// printf("n is: %i  points response is: ->%s<-\n", n, device->buf);
//
// tmcdev_write(device, ":WAV:STAR?");
//
// n = tmcdev_read(device);
//
// printf("n is: %i  start position response is: ->%s<-\n", n, device->buf);
//
// tmcdev_write(device, ":WAV:STOP?");
//
// n = tmcdev_read(device);
//
// printf("n is: %i  stop position response is: ->%s<-\n", n, device->buf);
//
// tmcdev_write(device, ":WAV:FORM?");
//
// n = tmcdev_read(device);
//
// printf("n is: %i  wav format response is: ->%s<-\n", n, device->buf);
//
// tmcdev_write(device, ":WAV:MODE?");
//
// n = tmcdev_read(device);
//
// printf("n is: %i  wav mode response is: ->%s<-\n", n, device->buf);
//
//     sleep(1);
//
//     tmcdev_write(device, ":WAV RES");
//
//     sleep(1);
//
//     tmcdev_write(device, ":WAV BEG");
//
//     sleep(1);
//
//     tmcdev_write(device, ":SING");
//
//     sleep(1);
//
//     bytes_rcvd = 0;
//
//     ready = 0;
//
//     while(1)
//     {
//       tmcdev_write(device, ":WAV:STAT?");
//
//       n = tmcdev_read(device);
//
//       printf("n is: %i   response is: ->%s<-\n", n, device->buf);
//
//       if(n < 4)
//       {
//         strcpy(str, "Error, could not read waveform status.");
//         goto OUT_ERROR;
//       }
//
//       if(strncmp(device->buf, "READ", 4))
//       {
//         ready = 1;
//       }
//
//       tmcdev_write(device, ":WAV:DATA?");
//
//       QApplication::setOverrideCursor(Qt::WaitCursor);
//
//       n = tmcdev_read(device);
//
//       QApplication::restoreOverrideCursor();
//
//       if(n < 0)
//       {
//         strcpy(str, "Can not read from device.\n"
//                     "Have you already patched the usbtmc driver?");
//         goto OUT_ERROR;
//       }
//
//       printf("received %i bytes\n", n);
//
//       if(n > devparms.memdepth)
//       {
//         strcpy(str, "Datablock too big for buffer.");
//         goto OUT_ERROR;
//       }
//
//       if(ready)
//       {
//         tmcdev_write(device, ":WAV END");
//
//         break;
//       }
//     }
//   }
//
//
//   for(i=0; i<MAX_CHNS; i++)
//   {
//     free(wavbuf[i]);
//   }
//
//   stat_timer->start(STAT_TIMER_IVAL);
//
//   scrn_timer->start(SCRN_TIMER_IVAL);
//
//   return;
//
// OUT_ERROR:
//
//   QMessageBox msgBox;
//   msgBox.setIcon(QMessageBox::Critical);
//   msgBox.setText(str);
//   msgBox.exec();
//
//   if(hdl >= 0)
//   {
//     edfclose_file(hdl);
//   }
//
//   for(i=0; i<MAX_CHNS; i++)
//   {
//     free(wavbuf[i]);
//   }
//
//   stat_timer->start(STAT_TIMER_IVAL);
//
//   scrn_timer->start(SCRN_TIMER_IVAL);
}


//     tmcdev_write(device, ":WAV:PRE?");
//
//     n = tmcdev_read(device);
//
//     if(n < 0)
//     {
//       strcpy(str, "Can not read from device.");
//       goto OUT_ERROR;
//     }

//     printf("waveform preamble: %s\n", device->buf);

//     if(parse_preamble(device->buf, device->sz, &wfp, i))
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
//            wfp.format, wfp.type, wfp.points, wfp.count,
//            wfp.xincrement[i], wfp.xorigin[i], wfp.xreference[i],
//            wfp.yincrement[i], wfp.yorigin[i], wfp.yreference[i]);

//     rec_len = wfp.xincrement[i] * wfp.points;





void UI_Mainwindow::save_screen_waveform()
{
  int i, j, n=0, chns=0, hdl=-1, yoffset[MAX_CHNS];

  char str[128],
       opath[MAX_PATHLEN];

  short *wavbuf[4];

  double rec_len = 0;

  if(device == NULL)
  {
    return;
  }

  wavbuf[0] = NULL;
  wavbuf[1] = NULL;
  wavbuf[2] = NULL;
  wavbuf[3] = NULL;

  if(devparms.timebasedelayenable)
  {
    rec_len = devparms.timebasedelayscale * 14;
  }
  else
  {
    rec_len = devparms.timebasescale * 14;
  }

  if(rec_len < 1e-6)
  {
    strcpy(str, "Can not save waveforms shorter than 1 uSec.");
    goto OUT_ERROR;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    wavbuf[i] = (short *)malloc(WAVFRM_MAX_BUFSZ * sizeof(short));
    if(wavbuf[i] == NULL)
    {
      strcpy(str, "Malloc error.");
      goto OUT_ERROR;
    }

    chns++;
  }

  if(!chns)
  {
    strcpy(str, "No active channels.");
    goto OUT_ERROR;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    sprintf(str, ":WAV:SOUR CHAN%i", i + 1);

    tmcdev_write(device, str);

    tmcdev_write(device, ":WAV:FORM BYTE");

    tmcdev_write(device, ":WAV:MODE NORM");

    tmcdev_write(device, ":WAV:DATA?");

    QApplication::setOverrideCursor(Qt::WaitCursor);

    n = tmcdev_read(device);

    QApplication::restoreOverrideCursor();

    if(n < 0)
    {
      strcpy(str, "Can not read from device.\n"
                  "Have you already patched the usbtmc driver?");
      goto OUT_ERROR;
    }

    if(n > WAVFRM_MAX_BUFSZ)
    {
      strcpy(str, "Datablock too big for buffer.");
      goto OUT_ERROR;
    }

    if(n < 16)
    {
      strcpy(str, "Not enough data in buffer.");
      goto OUT_ERROR;
    }

    yoffset[i] = ((devparms.chanoffset[i] / devparms.chanscale[i]) * 32.0);

    for(j=0; j<n; j++)
    {
      wavbuf[i][j] = (char)(device->buf[j] + 128);

      wavbuf[i][j] -= yoffset[i];
    }
  }

  opath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(opath, recent_savedir);
    strcat(opath, "/");
  }
  strcat(opath, "waveform.edf");

  strcpy(opath, QFileDialog::getSaveFileName(this, "Save file", opath, "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(opath, ""))
  {
    goto OUT_NORMAL;
  }

  get_directory_from_path(recent_savedir, opath, MAX_PATHLEN);

  hdl = edfopen_file_writeonly(opath, EDFLIB_FILETYPE_EDFPLUS, chns);
  if(hdl < 0)
  {
    strcpy(str, "Can not create EDF file.");
    goto OUT_ERROR;
  }

  if(edf_set_double_datarecord_duration(hdl, rec_len))
  {
    strcpy(str, "Can not set datarecord duration of EDF file.");
    goto OUT_ERROR;
  }

  j = 0;

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])
    {
      continue;
    }

    edf_set_samplefrequency(hdl, j, n);
    edf_set_digital_maximum(hdl, j, 32767);
    edf_set_digital_minimum(hdl, j, -32768);
    if(devparms.chanscale[i] > 2)
    {
      edf_set_physical_maximum(hdl, j, devparms.chanscale[i] * 4 * 256);
      edf_set_physical_minimum(hdl, j, devparms.chanscale[i] * -4 * 256);
      edf_set_physical_dimension(hdl, j, "V");
    }
    else
    {
      edf_set_physical_maximum(hdl, j, 1000 * devparms.chanscale[i] * 4 * 256);
      edf_set_physical_minimum(hdl, j, 1000 * devparms.chanscale[i] * -4 * 256);
      edf_set_physical_dimension(hdl, j, "mV");
    }
    sprintf(str, "CHAN%i", i + 1);
    edf_set_label(hdl, j, str);

    j++;
  }

  edf_set_equipment(hdl, devparms.modelname);

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])
    {
      continue;
    }

    if(edfwrite_digital_short_samples(hdl, wavbuf[i]))
    {
      strcpy(str, "A write error occurred.");
      goto OUT_ERROR;
    }
  }

OUT_NORMAL:

  if(hdl >= 0)
  {
    edfclose_file(hdl);
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    free(wavbuf[i]);
  }

  return;

OUT_ERROR:

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();

  if(hdl >= 0)
  {
    edfclose_file(hdl);
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    free(wavbuf[i]);
  }
}

















