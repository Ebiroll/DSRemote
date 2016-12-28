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


#define SAV_MEM_BSZ    (250000)



void UI_Mainwindow::save_screenshot()
{
  int n;

  char str[128],
       opath[MAX_PATHLEN];

  QPainter painter;
#if QT_VERSION >= 0x050000
  painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif

  QPainterPath path;

  if(device == NULL)
  {
    return;
  }

  scrn_timer->stop();

  scrn_thread->wait();

  tmc_write(":DISP:DATA?");

  save_data_thread get_data_thrd(0);

  QMessageBox w_msg_box;
  w_msg_box.setIcon(QMessageBox::NoIcon);
  w_msg_box.setText("Downloading data...");
  w_msg_box.setStandardButtons(QMessageBox::Abort);

  connect(&get_data_thrd, SIGNAL(finished()), &w_msg_box, SLOT(accept()));

  get_data_thrd.start();

  if(w_msg_box.exec() != QDialog::Accepted)
  {
    disconnect(&get_data_thrd, 0, 0, 0);
    strcpy(str, "Aborted by user.");
    goto OUT_ERROR;
  }

  disconnect(&get_data_thrd, 0, 0, 0);

  n = get_data_thrd.get_num_bytes_rcvd();
  if(n < 0)
  {
    strcpy(str, "Can not read from device.");
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

  if(devparms.modelserie == 1)
  {
    painter.begin(&screenXpm);

    painter.fillRect(0, 0, 80, 29, Qt::black);

    painter.setPen(Qt::white);

    painter.drawText(5, 8, 65, 20, Qt::AlignCenter, devparms.modelname);

    painter.end();
  }
  else if(devparms.modelserie == 6)
    {
      painter.begin(&screenXpm);

      painter.fillRect(0, 0, 95, 29, QColor(48, 48, 48));

      path.addRoundedRect(5, 5, 85, 20, 3, 3);

      painter.fillPath(path, Qt::black);

      painter.setPen(Qt::white);

      painter.drawText(5, 5, 85, 20, Qt::AlignCenter, devparms.modelname);

      painter.end();
    }

  if(devparms.screenshot_inv)
  {
    screenXpm.invertPixels(QImage::InvertRgb);
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
    scrn_timer->start(devparms.screentimerival);

    return;
  }

  get_directory_from_path(recent_savedir, opath, MAX_PATHLEN);

  if(screenXpm.save(QString::fromLocal8Bit(opath), "PNG", 50) == false)
  {
    strcpy(str, "Could not save file (unknown error)");
    goto OUT_ERROR;
  }

  scrn_timer->start(devparms.screentimerival);

  return;

OUT_ERROR:

  if(get_data_thrd.isFinished() != true)
  {
    connect(&get_data_thrd, SIGNAL(finished()), &w_msg_box, SLOT(accept()));
    w_msg_box.setText("Waiting for thread to finish, please wait...");
    w_msg_box.exec();
  }

  scrn_timer->start(devparms.screentimerival);

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();
}


void UI_Mainwindow::save_memory_waveform(int job)
{
  int i, j, k,
      n=0,
      chn,
      chns=0,
      hdl=-1,
      bytes_rcvd=0,
      mempnts,
      yref[MAX_CHNS],
      yor[MAX_CHNS],
      smps_per_record,
      datrecs=1,
      empty_buf,
      ret_stat;

  char str[256],
       opath[MAX_PATHLEN];

  short *wavbuf[MAX_CHNS];

  long long rec_len=0LL;

  double yinc[MAX_CHNS];

  QEventLoop ev_loop;

  QMessageBox wi_msg_box;

  save_data_thread get_data_thrd(0);

  save_data_thread sav_data_thrd(1);

  if(device == NULL)
  {
    return;
  }

  scrn_timer->stop();

  scrn_thread->wait();

  for(i=0; i<MAX_CHNS; i++)
  {
    wavbuf[i] = NULL;
  }

  mempnts = devparms.acquirememdepth;

  smps_per_record = mempnts;

  QProgressDialog progress("Downloading data...", "Abort", 0, mempnts, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(0);

  connect(&get_data_thrd, SIGNAL(finished()), &ev_loop, SLOT(quit()));

  statusLabel->setText("Downloading data...");

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])
    {
      continue;
    }

    chns++;
  }

  if(!chns)
  {
    strcpy(str, "No active channels.");
    goto OUT_ERROR;
  }

  while(smps_per_record >= (5000000 / chns))
  {
    smps_per_record /= 2;

    datrecs *= 2;
  }

  if(mempnts < 1)
  {
    strcpy(str, "Can not download waveform when memory depth is set to \"Auto\".");
    goto OUT_ERROR;
  }

  rec_len = (EDFLIB_TIME_DIMENSION * (long long)mempnts) / devparms.samplerate;

  if((job == 1) && (rec_len < 100))
  {
    strcpy(str, "Can not save waveforms shorter than 10 uSec.\n"
                "Set the horizontal timebase to 1 uSec or higher.");
    goto OUT_ERROR;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    wavbuf[i] = (short *)malloc(mempnts * sizeof(short));
    if(wavbuf[i] == NULL)
    {
      sprintf(str, "Malloc error.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }
  }

  tmc_write(":STOP");

  usleep(20000);

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])  // Download data only when channel is switched on
    {
      continue;
    }

    sprintf(str, ":WAV:SOUR CHAN%i", chn + 1);

    tmc_write(str);

    tmc_write(":WAV:FORM BYTE");

    usleep(20000);

    tmc_write(":WAV:MODE RAW");

    usleep(20000);

    tmc_write(":WAV:YINC?");

    usleep(20000);

    tmc_read();

    yinc[chn] = atof(device->buf);

    if(yinc[chn] < 1e-6)
    {
      sprintf(str, "Error, parameter \"YINC\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    usleep(20000);

    tmc_write(":WAV:YREF?");

    usleep(20000);

    tmc_read();

    yref[chn] = atoi(device->buf);

    if((yref[chn] < 1) || (yref[chn] > 255))
    {
      sprintf(str, "Error, parameter \"YREF\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    usleep(20000);

    tmc_write(":WAV:YOR?");

    usleep(20000);

    tmc_read();

    yor[chn] = atoi(device->buf);

    if((yor[chn] < -255) || (yor[chn] > 255))
    {
      sprintf(str, "Error, parameter \"YOR\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    empty_buf = 0;

    for(bytes_rcvd=0; bytes_rcvd<mempnts ;)
    {
      progress.setValue(bytes_rcvd);

      if(progress.wasCanceled())
      {
        strcpy(str, "Canceled");
        goto OUT_ERROR;
      }

      sprintf(str, ":WAV:STAR %i",  bytes_rcvd + 1);

      usleep(20000);

      tmc_write(str);

      if((bytes_rcvd + SAV_MEM_BSZ) > mempnts)
      {
        sprintf(str, ":WAV:STOP %i", mempnts);
      }
      else
      {
        sprintf(str, ":WAV:STOP %i", bytes_rcvd + SAV_MEM_BSZ);
      }

      usleep(20000);

      tmc_write(str);

      usleep(20000);

      tmc_write(":WAV:DATA?");

      get_data_thrd.start();

      ev_loop.exec();

      n = get_data_thrd.get_num_bytes_rcvd();
      if(n < 0)
      {
        sprintf(str, "Can not read from device.  line %i file %s", __LINE__, __FILE__);
        goto OUT_ERROR;
      }

      if(n == 0)
      {
        sprintf(str, "No waveform data available.");
        goto OUT_ERROR;
      }

      printf("received %i bytes, total %i bytes\n", n, n + bytes_rcvd);

      if(n > SAV_MEM_BSZ)
      {
        sprintf(str, "Datablock too big for buffer.  line %i file %s", __LINE__, __FILE__);
        goto OUT_ERROR;
      }

      if(n < 1)
      {
        if(empty_buf++ > 100)
        {
          break;
        }
      }
      else
      {
        empty_buf = 0;
      }

      if(job == 1)
      {
        for(k=0; k<n; k++)
        {
          if((bytes_rcvd + k) >= mempnts)
          {
            break;
          }

          wavbuf[chn][bytes_rcvd + k] = ((int)(((unsigned char *)device->buf)[k]) - yref[chn] - yor[chn]) << 5;
        }
      }
      else
      {
        for(k=0; k<n; k++)
        {
          if((bytes_rcvd + k) >= mempnts)
          {
            break;
          }

          wavbuf[chn][bytes_rcvd + k] = (int)(((unsigned char *)device->buf)[k]) - yref[chn] - yor[chn];
        }
      }

      bytes_rcvd += n;

      if(bytes_rcvd >= mempnts)
      {
        break;
      }
    }

    if(bytes_rcvd < mempnts)
    {
      sprintf(str, "Download error.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }
  }

  progress.reset();

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])
    {
      continue;
    }

    sprintf(str, ":WAV:SOUR CHAN%i", chn + 1);

    usleep(20000);

    tmc_write(str);

    usleep(20000);

    tmc_write(":WAV:MODE NORM");

    usleep(20000);

    tmc_write(":WAV:STAR 1");

    if(devparms.modelserie == 1)
    {
      usleep(20000);

      tmc_write(":WAV:STOP 1200");
    }
    else
    {
      usleep(20000);

      tmc_write(":WAV:STOP 1400");

      usleep(20000);

      tmc_write(":WAV:POIN 1400");
    }
  }

  if(bytes_rcvd < mempnts)
  {
    sprintf(str, "Download error.  line %i file %s", __LINE__, __FILE__);
    goto OUT_ERROR;
  }
  else
  {
    statusLabel->setText("Downloading finished");
  }

  if(job == 0)
  {
    new UI_wave_window(&devparms, wavbuf, this);

    goto OUT_NORMAL;
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

  statusLabel->setText("Saving EDF file...");

  if(edf_set_datarecord_us_duration(hdl, (rec_len / 10LL) / datrecs))
  {
    strcpy(str, "Can not set datarecord duration of EDF file.");
    goto OUT_ERROR;
  }

  j = 0;

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])
    {
      continue;
    }

    edf_set_samplefrequency(hdl, j, smps_per_record);
    edf_set_digital_maximum(hdl, j, 32767);
    edf_set_digital_minimum(hdl, j, -32768);
    if(devparms.chanscale[chn] > 2)
    {
      edf_set_physical_maximum(hdl, j, yinc[chn] * 32767.0 / 32.0);
      edf_set_physical_minimum(hdl, j, yinc[chn] * -32768.0 / 32.0);
      edf_set_physical_dimension(hdl, j, "V");
    }
    else
    {
      edf_set_physical_maximum(hdl, j, 1000.0 * yinc[chn] * 32767.0 / 32.0);
      edf_set_physical_minimum(hdl, j, 1000.0 * yinc[chn] * -32768.0 / 32.0);
      edf_set_physical_dimension(hdl, j, "mV");
    }
    sprintf(str, "CHAN%i", chn + 1);
    edf_set_label(hdl, j, str);

    j++;
  }

  edf_set_equipment(hdl, devparms.modelname);

  sav_data_thrd.init_save_memory_edf_file(&devparms, hdl, datrecs, smps_per_record, wavbuf);

  connect(&sav_data_thrd, SIGNAL(finished()), &ev_loop, SLOT(quit()));

  wi_msg_box.setIcon(QMessageBox::NoIcon);
  wi_msg_box.setText("Saving EDF file ...");
  wi_msg_box.setStandardButtons(QMessageBox::Abort);

  connect(&sav_data_thrd, SIGNAL(finished()), &wi_msg_box, SLOT(accept()));

  sav_data_thrd.start();

  ret_stat = wi_msg_box.exec();

  if(ret_stat != QDialog::Accepted)
  {
    strcpy(str, "Saving EDF file aborted.");
    goto OUT_ERROR;
  }

  if(sav_data_thrd.get_error_num())
  {
    sav_data_thrd.get_error_str(str);
    goto OUT_ERROR;
  }

OUT_NORMAL:

  disconnect(&get_data_thrd, 0, 0, 0);
  disconnect(&sav_data_thrd, 0, 0, 0);

  if(job == 1)
  {
    if(hdl >= 0)
    {
      edfclose_file(hdl);
    }

    for(chn=0; chn<MAX_CHNS; chn++)
    {
      free(wavbuf[chn]);
      wavbuf[chn] = NULL;
    }
  }

  scrn_timer->start(devparms.screentimerival);

  return;

OUT_ERROR:

  disconnect(&get_data_thrd, 0, 0, 0);
  disconnect(&sav_data_thrd, 0, 0, 0);

  progress.reset();

  statusLabel->setText("Downloading aborted");

  if(get_data_thrd.isRunning() == true)
  {
    QMessageBox w_msg_box;
    w_msg_box.setIcon(QMessageBox::NoIcon);
    w_msg_box.setText("Waiting for thread to finish, please wait...");
    w_msg_box.setStandardButtons(QMessageBox::Abort);

    connect(&get_data_thrd, SIGNAL(finished()), &w_msg_box, SLOT(accept()));

    w_msg_box.exec();
  }

  if(hdl >= 0)
  {
    edfclose_file(hdl);
  }

  if(progress.wasCanceled() == false)
  {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(str);
    msgBox.exec();
  }

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])
    {
      continue;
    }

    sprintf(str, ":WAV:SOUR CHAN%i", chn + 1);

    tmc_write(str);

    tmc_write(":WAV:MODE NORM");

    tmc_write(":WAV:STAR 1");

    if(devparms.modelserie == 1)
    {
      tmc_write(":WAV:STOP 1200");
    }
    else
    {
      tmc_write(":WAV:STOP 1400");

      tmc_write(":WAV:POIN 1400");
    }
  }

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    free(wavbuf[chn]);
    wavbuf[chn] = NULL;
  }

  scrn_timer->start(devparms.screentimerival);
}


//     tmc_write(":WAV:PRE?");
//
//     n = tmc_read();
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
  int i, j,
      n=0,
      chn,
      chns=0,
      hdl=-1,
      yref[MAX_CHNS],
      yor[MAX_CHNS];

  char str[128],
       opath[MAX_PATHLEN];

  short *wavbuf[MAX_CHNS];

  long long rec_len=0LL;

  double yinc[MAX_CHNS];

  if(device == NULL)
  {
    return;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    wavbuf[i] = NULL;
  }

  save_data_thread get_data_thrd(0);

  QMessageBox w_msg_box;
  w_msg_box.setIcon(QMessageBox::NoIcon);
  w_msg_box.setText("Downloading data...");
  w_msg_box.setStandardButtons(QMessageBox::Abort);

  scrn_timer->stop();

  scrn_thread->wait();

  if(devparms.timebasedelayenable)
  {
    rec_len = EDFLIB_TIME_DIMENSION * devparms.timebasedelayscale * devparms.hordivisions;
  }
  else
  {
    rec_len = EDFLIB_TIME_DIMENSION * devparms.timebasescale * devparms.hordivisions;
  }

  if(rec_len < 10LL)
  {
    strcpy(str, "Can not save waveforms when timebase < 1uSec.");
    goto OUT_ERROR;
  }

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])  // Download data only when channel is switched on
    {
      continue;
    }

    wavbuf[chn] = (short *)malloc(WAVFRM_MAX_BUFSZ * sizeof(short));
    if(wavbuf[chn] == NULL)
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

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])  // Download data only when channel is switched on
    {
      continue;
    }

    usleep(20000);

    sprintf(str, ":WAV:SOUR CHAN%i", chn + 1);

    tmc_write(str);

    usleep(20000);

    tmc_write(":WAV:FORM BYTE");

    usleep(20000);

    tmc_write(":WAV:MODE NORM");

    usleep(20000);

    tmc_write(":WAV:YINC?");

    usleep(20000);

    tmc_read();

    yinc[chn] = atof(device->buf);

    if(yinc[chn] < 1e-6)
    {
      sprintf(str, "Error, parameter \"YINC\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    usleep(20000);

    tmc_write(":WAV:YREF?");

    usleep(20000);

    tmc_read();

    yref[chn] = atoi(device->buf);

    if((yref[chn] < 1) || (yref[chn] > 255))
    {
      sprintf(str, "Error, parameter \"YREF\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    usleep(20000);

    tmc_write(":WAV:YOR?");

    usleep(20000);

    tmc_read();

    yor[chn] = atoi(device->buf);

    if((yor[chn] < -255) || (yor[chn] > 255))
    {
      sprintf(str, "Error, parameter \"YOR\" out of range.  line %i file %s", __LINE__, __FILE__);
      goto OUT_ERROR;
    }

    usleep(20000);

    tmc_write(":WAV:DATA?");

    connect(&get_data_thrd, SIGNAL(finished()), &w_msg_box, SLOT(accept()));

    get_data_thrd.start();

    if(w_msg_box.exec() != QDialog::Accepted)
    {
      disconnect(&get_data_thrd, 0, 0, 0);
      strcpy(str, "Aborted by user.");
      goto OUT_ERROR;
    }

    disconnect(&get_data_thrd, 0, 0, 0);

    n = get_data_thrd.get_num_bytes_rcvd();
    if(n < 0)
    {
      strcpy(str, "Can not read from device.");
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

    for(i=0; i<n; i++)
    {
      wavbuf[chn][i] = ((int)(((unsigned char *)device->buf)[i]) - yref[chn] - yor[chn]) << 5;
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

  if(edf_set_datarecord_duration(hdl, rec_len / 100LL))
  {
    strcpy(str, "Can not set datarecord duration of EDF file.");
    goto OUT_ERROR;
  }

  j = 0;

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])
    {
      continue;
    }

    edf_set_samplefrequency(hdl, j, n);
    edf_set_digital_maximum(hdl, j, 32767);
    edf_set_digital_minimum(hdl, j, -32768);
    if(devparms.chanscale[chn] > 2)
    {
      edf_set_physical_maximum(hdl, j, yinc[chn] * 32767.0 / 32.0);
      edf_set_physical_minimum(hdl, j, yinc[chn] * -32768.0 / 32.0);
      edf_set_physical_dimension(hdl, j, "V");
    }
    else
    {
      edf_set_physical_maximum(hdl, j, 1000.0 * yinc[chn] * 32767.0 / 32.0);
      edf_set_physical_minimum(hdl, j, 1000.0 * yinc[chn] * -32768.0 / 32.0);
      edf_set_physical_dimension(hdl, j, "mV");
    }
    sprintf(str, "CHAN%i", chn + 1);
    edf_set_label(hdl, j, str);

    j++;
  }

  edf_set_equipment(hdl, devparms.modelname);

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    if(!devparms.chandisplay[chn])
    {
      continue;
    }

    if(edfwrite_digital_short_samples(hdl, wavbuf[chn]))
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

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    free(wavbuf[chn]);
    wavbuf[chn] = NULL;
  }

  scrn_timer->start(devparms.screentimerival);

  return;

OUT_ERROR:

  if(get_data_thrd.isFinished() != true)
  {
    connect(&get_data_thrd, SIGNAL(finished()), &w_msg_box, SLOT(accept()));
    w_msg_box.setText("Waiting for thread to finish, please wait...");
    w_msg_box.exec();
  }

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();

  if(hdl >= 0)
  {
    edfclose_file(hdl);
  }

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    free(wavbuf[chn]);
    wavbuf[chn] = NULL;
  }

  scrn_timer->start(devparms.screentimerival);
}

















