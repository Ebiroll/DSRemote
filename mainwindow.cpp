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


#include "mainwindow.h"
#include "mainwindow_constr.cpp"
#include "timer_handlers.cpp"
#include "save_data.cpp"
#include "interface.cpp"



void UI_Mainwindow::open_settings_dialog()
{
  UI_settings_window settings(this);
}


void UI_Mainwindow::open_connection()
{
  int i, j, n, len;

  char str[1024] = {""},
       dev_str[256] = {""},
       resp_str[1024] = {""},
       *ptr;

  QSettings settings;

  if(device != NULL)
  {
    return;
  }

  if(devparms.connected)
  {
    return;
  }

  strcpy(str, settings.value("connection/type").toString().toLatin1().data());

  if(!strcmp(str, "LAN"))
  {
    devparms.connectiontype = 1;
  }
  else
  {
    devparms.connectiontype = 0;
  }

  if(devparms.connectiontype == 0)  // USB
  {
    strcpy(dev_str, settings.value("connection/device").toString().toLatin1().data());

    if(!strcmp(dev_str, ""))
    {
      strcpy(dev_str, "/dev/usbtmc0");

      settings.setValue("connection/device", dev_str);
    }

    device = tmc_open_usb(dev_str);
    if(device == NULL)
    {
      sprintf(str, "Can not open device %s", dev_str);
      goto OUT_ERROR;
    }
  }

  if(devparms.connectiontype == 1)  // LAN
  {
    strcpy(dev_str, settings.value("connection/ip").toString().toLatin1().data());

    if(!strcmp(dev_str, ""))
    {
      sprintf(str, "No IP address set");
      goto OUT_ERROR;
    }

    len = strlen(dev_str);

    if(len < 7)
    {
      sprintf(str, "No IP address set");
      goto OUT_ERROR;
    }

    for(i=0; i<len; i++)
    {
      if(dev_str[i] == '0')
      {
        if((dev_str[i+1] != 0) && (dev_str[i+1] != '.'))
        {
          for(j=i; j<len; j++)
          {
            dev_str[j] = dev_str[j+1];
          }

          i--;

          len--;
        }
      }
    }

    device = tmc_open_lan(dev_str);
    if(device == NULL)
    {
      sprintf(str, "Can not open connection to %s", dev_str);
      goto OUT_ERROR;
    }
  }

  if(tmc_write("*IDN?") != 5)
  {
    sprintf(str, "Can not write to device %s", dev_str);
    goto OUT_ERROR;
  }

  n = tmc_read();

  if(n < 0)
  {
    sprintf(str, "Can not read from device %s", dev_str);
    goto OUT_ERROR;
  }

  devparms.channel_cnt = 0;

  devparms.bandwidth = 0;

  devparms.modelname[0] = 0;

  strncpy(resp_str, device->buf, 1024);

  device->buf[1023] = 0;

  ptr = strtok(resp_str, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  if(strcmp(ptr, "RIGOL TECHNOLOGIES"))
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  get_device_model(ptr);

  if((!devparms.channel_cnt) || (!devparms.bandwidth))
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  strcpy(devparms.serialnr, ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  strcpy(devparms.softwvers, ptr);

  if((devparms.modelserie != 6) &&
     (devparms.modelserie != 1))
  {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Unsupported device detected.");
    msgBox.setInformativeText("This software has not been tested with your device.\n"
      "It has been tested with the DS6000 and DS1000Z series only.\n"
      "If you continue, it's likely that the program will not work correctly at some points.\n"
      "\nDo you want to continue?\n");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if(ret == QMessageBox::No)
    {
      statusLabel->setText("Disconnected");

      devparms.connected = 0;

      close_connection();

      return;
    }
  }

  statusLabel->setText("Reading settings from device...");

  if(get_device_settings())
  {
    strcpy(str, "Can not read settings from device");
    goto OUT_ERROR;
  }

  if(devparms.modelserie == 1)
  {
    trig50pctButton->setEnabled(false);
  }

  if(devparms.channel_cnt < 4)
  {
    ch4Button->setEnabled(false);

    ch4Button->setVisible(false);
  }

  if(devparms.channel_cnt < 3)
  {
    ch3Button->setEnabled(false);

    ch3Button->setVisible(false);
  }

  if(devparms.channel_cnt < 2)
  {
    ch2Button->setEnabled(false);

    ch2Button->setVisible(false);
  }

  devparms.cmd_cue_idx_in = 0;
  devparms.cmd_cue_idx_out = 0;

  connect(adjDial,          SIGNAL(valueChanged(int)), this, SLOT(adjDialChanged(int)));
  connect(trigAdjustDial,   SIGNAL(valueChanged(int)), this, SLOT(trigAdjustDialChanged(int)));
  connect(horScaleDial,     SIGNAL(valueChanged(int)), this, SLOT(horScaleDialChanged(int)));
  connect(horPosDial,       SIGNAL(valueChanged(int)), this, SLOT(horPosDialChanged(int)));
  connect(vertOffsetDial,   SIGNAL(valueChanged(int)), this, SLOT(vertOffsetDialChanged(int)));
  connect(vertScaleDial,    SIGNAL(valueChanged(int)), this, SLOT(vertScaleDialChanged(int)));
  connect(navDial,          SIGNAL(valueChanged(int)), this, SLOT(navDialChanged(int)));

  connect(ch1Button,        SIGNAL(clicked()),      this, SLOT(ch1ButtonClicked()));
  connect(ch2Button,        SIGNAL(clicked()),      this, SLOT(ch2ButtonClicked()));
  connect(ch3Button,        SIGNAL(clicked()),      this, SLOT(ch3ButtonClicked()));
  connect(ch4Button,        SIGNAL(clicked()),      this, SLOT(ch4ButtonClicked()));
  connect(chanMenuButton,   SIGNAL(clicked()),      this, SLOT(chan_menu()));
  connect(waveForm,         SIGNAL(chan1Clicked()), this, SLOT(ch1ButtonClicked()));
  connect(waveForm,         SIGNAL(chan2Clicked()), this, SLOT(ch2ButtonClicked()));
  connect(waveForm,         SIGNAL(chan3Clicked()), this, SLOT(ch3ButtonClicked()));
  connect(waveForm,         SIGNAL(chan4Clicked()), this, SLOT(ch4ButtonClicked()));
  connect(clearButton,      SIGNAL(clicked()),      this, SLOT(clearButtonClicked()));
  connect(autoButton,       SIGNAL(clicked()),      this, SLOT(autoButtonClicked()));
  connect(runButton,        SIGNAL(clicked()),      this, SLOT(runButtonClicked()));
  connect(singleButton,     SIGNAL(clicked()),      this, SLOT(singleButtonClicked()));
  connect(horMenuButton,    SIGNAL(clicked()),      this, SLOT(horMenuButtonClicked()));
  connect(trigModeButton,   SIGNAL(clicked()),      this, SLOT(trigModeButtonClicked()));
  connect(trigMenuButton,   SIGNAL(clicked()),      this, SLOT(trigMenuButtonClicked()));
  connect(trigForceButton,  SIGNAL(clicked()),      this, SLOT(trigForceButtonClicked()));
  connect(trig50pctButton,  SIGNAL(clicked()),      this, SLOT(trig50pctButtonClicked()));
  connect(acqButton,        SIGNAL(clicked()),      this, SLOT(acqButtonClicked()));
  connect(cursButton,       SIGNAL(clicked()),      this, SLOT(cursButtonClicked()));
  connect(saveButton,       SIGNAL(clicked()),      this, SLOT(saveButtonClicked()));
  connect(dispButton,       SIGNAL(clicked()),      this, SLOT(dispButtonClicked()));
  connect(utilButton,       SIGNAL(clicked()),      this, SLOT(utilButtonClicked()));
  connect(helpButton,       SIGNAL(clicked()),      this, SLOT(helpButtonClicked()));
  connect(measureButton,    SIGNAL(clicked()),      this, SLOT(measureButtonClicked()));

  connect(horPosDial,     SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horPosDialClicked(QPoint)));
  connect(vertOffsetDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertOffsetDialClicked(QPoint)));
  connect(horScaleDial,   SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horScaleDialClicked(QPoint)));
  connect(vertScaleDial,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertScaleDialClicked(QPoint)));
  connect(trigAdjustDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(trigAdjustDialClicked(QPoint)));
  connect(adjDial,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(adjustDialClicked(QPoint)));

  sprintf(str, PROGRAM_NAME " " PROGRAM_VERSION "   %s   %s", devparms.serialnr, devparms.softwvers);

  setWindowTitle(str);

  statusLabel->setText("Connected");

  scrn_thread->set_device(device);

  devparms.connected = 1;

//  test_timer->start(2000);

  DPRwidget->setEnabled(true);

  devparms.screenupdates_on = 1;

  scrn_thread->h_busy = 0;

  scrn_timer->start(devparms.screentimerival);

  return;

OUT_ERROR:

  statusLabel->setText("Disconnected");

  devparms.connected = 0;

  QMessageBox mesgbox;
  mesgbox.setIcon(QMessageBox::Critical);
  mesgbox.setText(str);
  mesgbox.exec();

  close_connection();
}


void UI_Mainwindow::close_connection()
{
  DPRwidget->setEnabled(false);

  test_timer->stop();

  scrn_timer->stop();

  adjdial_timer->stop();

  devparms.connected = 0;

  if(scrn_thread->wait(5000) == false)
  {
    scrn_thread->terminate();

    scrn_thread->wait(5000);

    devparms.mutexx->unlock();

    scrn_thread->h_busy = 0;
  }

  devparms.screenupdates_on = 0;

  setWindowTitle(PROGRAM_NAME " " PROGRAM_VERSION);

  strcpy(devparms.modelname, "-----");

  adjDialFunc = ADJ_DIAL_FUNC_NONE;
  navDialFunc = NAV_DIAL_FUNC_NONE;

  disconnect(adjDial,         SIGNAL(valueChanged(int)), this, SLOT(adjDialChanged(int)));
  disconnect(trigAdjustDial,  SIGNAL(valueChanged(int)), this, SLOT(trigAdjustDialChanged(int)));
  disconnect(horScaleDial,    SIGNAL(valueChanged(int)), this, SLOT(horScaleDialChanged(int)));
  disconnect(horPosDial,      SIGNAL(valueChanged(int)), this, SLOT(horPosDialChanged(int)));
  disconnect(vertOffsetDial,  SIGNAL(valueChanged(int)), this, SLOT(vertOffsetDialChanged(int)));
  disconnect(vertScaleDial,   SIGNAL(valueChanged(int)), this, SLOT(vertScaleDialChanged(int)));
  disconnect(navDial,         SIGNAL(valueChanged(int)), this, SLOT(navDialChanged(int)));

  disconnect(ch1Button,        SIGNAL(clicked()),     this, SLOT(ch1ButtonClicked()));
  disconnect(ch2Button,        SIGNAL(clicked()),     this, SLOT(ch2ButtonClicked()));
  disconnect(ch3Button,        SIGNAL(clicked()),     this, SLOT(ch3ButtonClicked()));
  disconnect(ch4Button,        SIGNAL(clicked()),     this, SLOT(ch4ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan1Clicked()), this, SLOT(ch1ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan2Clicked()), this, SLOT(ch2ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan3Clicked()), this, SLOT(ch3ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan4Clicked()), this, SLOT(ch4ButtonClicked()));
  disconnect(clearButton,      SIGNAL(clicked()),     this, SLOT(clearButtonClicked()));
  disconnect(autoButton,       SIGNAL(clicked()),     this, SLOT(autoButtonClicked()));
  disconnect(runButton,        SIGNAL(clicked()),     this, SLOT(runButtonClicked()));
  disconnect(singleButton,     SIGNAL(clicked()),     this, SLOT(singleButtonClicked()));
  disconnect(horMenuButton,    SIGNAL(clicked()),     this, SLOT(horMenuButtonClicked()));
  disconnect(trigModeButton,   SIGNAL(clicked()),     this, SLOT(trigModeButtonClicked()));
  disconnect(trigMenuButton,   SIGNAL(clicked()),     this, SLOT(trigMenuButtonClicked()));
  disconnect(trigForceButton,  SIGNAL(clicked()),     this, SLOT(trigForceButtonClicked()));
  disconnect(trig50pctButton,  SIGNAL(clicked()),     this, SLOT(trig50pctButtonClicked()));
  disconnect(acqButton,        SIGNAL(clicked()),     this, SLOT(acqButtonClicked()));
  disconnect(cursButton,       SIGNAL(clicked()),     this, SLOT(cursButtonClicked()));
  disconnect(saveButton,       SIGNAL(clicked()),     this, SLOT(saveButtonClicked()));
  disconnect(dispButton,       SIGNAL(clicked()),     this, SLOT(dispButtonClicked()));
  disconnect(utilButton,       SIGNAL(clicked()),     this, SLOT(utilButtonClicked()));
  disconnect(helpButton,       SIGNAL(clicked()),     this, SLOT(helpButtonClicked()));
  disconnect(measureButton,    SIGNAL(clicked()),     this, SLOT(measureButtonClicked()));

  disconnect(horPosDial,     SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horPosDialClicked(QPoint)));
  disconnect(vertOffsetDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertOffsetDialClicked(QPoint)));
  disconnect(horScaleDial,   SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horScaleDialClicked(QPoint)));
  disconnect(vertScaleDial,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertScaleDialClicked(QPoint)));
  disconnect(trigAdjustDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(trigAdjustDialClicked(QPoint)));
  disconnect(adjDial,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(adjustDialClicked(QPoint)));

  scrn_thread->set_device(NULL);

  waveForm->clear();

  tmc_close();

  device = NULL;

  statusLabel->setText("Disconnected");
}


void UI_Mainwindow::closeEvent(QCloseEvent *cl_event)
{
  devparms.connected = 0;

  test_timer->stop();

  scrn_timer->stop();

  adjdial_timer->stop();

  scrn_thread->wait(5000);

  scrn_thread->terminate();

  scrn_thread->wait(5000);

  devparms.screenupdates_on = 0;

  scrn_thread->set_device(NULL);

  tmc_close();

  device = NULL;

  cl_event->accept();
}


int UI_Mainwindow::get_device_settings()
{
  int chn, line;

  char str[512];

  devparms.activechannel = -1;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    sprintf(str, ":CHAN%i:BWL?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 11)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "20M"))
    {
      devparms.chanbwlimit[chn] = 20;
    }
    else if(!strcmp(device->buf, "250M"))
      {
        devparms.chanbwlimit[chn] = 250;
      }
      else if(!strcmp(device->buf, "OFF"))
        {
          devparms.chanbwlimit[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:COUP?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "AC"))
    {
      devparms.chancoupling[chn] = 2;
    }
    else if(!strcmp(device->buf, "DC"))
      {
        devparms.chancoupling[chn] = 1;
      }
      else if(!strcmp(device->buf, "GND"))
        {
          devparms.chancoupling[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:DISP?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      switch(chn)
      {
        case 0: ch1Button->setStyleSheet(def_stylesh);
                break;
        case 1: ch2Button->setStyleSheet(def_stylesh);
                break;
        case 2: ch3Button->setStyleSheet(def_stylesh);
                break;
        case 3: ch4Button->setStyleSheet(def_stylesh);
                break;
      }

      devparms.chandisplay[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        switch(chn)
        {
          case 0: ch1Button->setStyleSheet("background: #FFFF33;");
                  break;
          case 1: ch2Button->setStyleSheet("background: #33FFFF;");
                  break;
          case 2: ch3Button->setStyleSheet("background: #FF33FF;");
                  break;
          case 3: ch4Button->setStyleSheet("background: #0080FF;");
                  break;
        }

        devparms.chandisplay[chn] = 1;

        if(devparms.activechannel == -1)
        {
          devparms.activechannel = chn;
        }
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    if(devparms.modelserie != 1)
    {
      sprintf(str, ":CHAN%i:IMP?", chn + 1);

      usleep(TMC_GDS_DELAY);

      if(tmc_write(str) != 11)
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_read() < 1)
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(!strcmp(device->buf, "OMEG"))
      {
        devparms.chanimpedance[chn] = 0;
      }
      else if(!strcmp(device->buf, "FIFT"))
        {
          devparms.chanimpedance[chn] = 1;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }
    }

    sprintf(str, ":CHAN%i:INV?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 11)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.chaninvert[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.chaninvert[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    sprintf(str, ":CHAN%i:OFFS?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanoffset[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:PROB?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanprobe[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:SCAL?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanscale[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:VERN?", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.chanvernier[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.chanvernier[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:OFFS?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebaseoffset = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:SCAL?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasescale = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:ENAB?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms.timebasedelayenable = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms.timebasedelayenable = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:OFFS?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasedelayoffset = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:DEL:SCAL?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasedelayscale = atof(device->buf);

  if(devparms.modelserie != 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:HREF:MODE?") != 15)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "CENT"))
    {
      devparms.timebasehrefmode = 0;
    }
    else if(!strcmp(device->buf, "TPOS"))
      {
        devparms.timebasehrefmode = 1;
      }
      else if(!strcmp(device->buf, "USER"))
        {
          devparms.timebasehrefmode = 2;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:HREF:POS?") != 14)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  devparms.timebasehrefpos = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TIM:MODE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "MAIN"))
  {
    devparms.timebasemode = 0;
  }
  else if(!strcmp(device->buf, "XY"))
    {
      devparms.timebasemode = 1;
    }
    else if(!strcmp(device->buf, "ROLL"))
      {
        devparms.timebasemode = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(devparms.modelserie != 1)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:VERN?") != 10)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.timebasevernier = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.timebasevernier = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }
  }

  if((devparms.modelserie != 1) && (devparms.modelserie != 2))
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:XY1:DISP?") != 14)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.timebasexy1display = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.timebasexy1display = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TIM:XY2:DISP?") != 14)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.timebasexy2display = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.timebasexy2display = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:COUP?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "AC"))
  {
    devparms.triggercoupling = 0;
  }
  else if(!strcmp(device->buf, "DC"))
    {
      devparms.triggercoupling = 1;
    }
    else if(!strcmp(device->buf, "LFR"))
      {
        devparms.triggercoupling = 2;
      }
      else if(!strcmp(device->buf, "HFR"))
        {
          devparms.triggercoupling = 3;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

  usleep(TMC_GDS_DELAY);

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

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:MODE?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "EDGE"))
  {
    devparms.triggermode = 0;
  }
  else if(!strcmp(device->buf, "PULS"))
    {
      devparms.triggermode = 1;
    }
    else if(!strcmp(device->buf, "SLOP"))
      {
        devparms.triggermode = 2;
      }
      else if(!strcmp(device->buf, "VID"))
        {
          devparms.triggermode = 3;
        }
        else if(!strcmp(device->buf, "PATT"))
          {
            devparms.triggermode = 4;
          }
          else if(!strcmp(device->buf, "RS232"))
            {
              devparms.triggermode = 5;
            }
            else if(!strcmp(device->buf, "IIC"))
              {
                devparms.triggermode = 6;
              }
              else if(!strcmp(device->buf, "SPI"))
                {
                  devparms.triggermode = 7;
                }
                else if(!strcmp(device->buf, "CAN"))
                  {
                    devparms.triggermode = 8;
                  }
                  else if(!strcmp(device->buf, "USB"))
                    {
                      devparms.triggermode = 9;
                    }
                    else
                    {
                      line = __LINE__;
                      goto OUT_ERROR;
                    }

  usleep(TMC_GDS_DELAY);

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

  if(!strcmp(device->buf, "TD"))
  {
    devparms.triggerstatus = 0;
  }
  else if(!strcmp(device->buf, "WAIT"))
    {
      devparms.triggerstatus = 1;
    }
    else if(!strcmp(device->buf, "RUN"))
      {
        devparms.triggerstatus = 2;
      }
      else if(!strcmp(device->buf, "AUTO"))
        {
          devparms.triggerstatus = 3;
        }
        else if(!strcmp(device->buf, "FIN"))
          {
            devparms.triggerstatus = 4;
          }
          else if(!strcmp(device->buf, "STOP"))
            {
              devparms.triggerstatus = 5;
            }
            else
            {
              line = __LINE__;
              goto OUT_ERROR;
            }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:EDG:SLOP?") != 15)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "POS"))
  {
    devparms.triggeredgeslope = 0;
  }
  else if(!strcmp(device->buf, "NEG"))
    {
      devparms.triggeredgeslope = 1;
    }
    else if(!strcmp(device->buf, "RFAL"))
      {
        devparms.triggeredgeslope = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:EDG:SOUR?") != 15)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms.triggeredgesource = 0;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms.triggeredgesource = 1;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms.triggeredgesource = 2;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms.triggeredgesource = 3;
        }
        else if(!strcmp(device->buf, "EXT"))
          {
            devparms.triggeredgesource = 4;
          }
          else if(!strcmp(device->buf, "EXT5"))
            {
              devparms.triggeredgesource = 5;
            }
            else if(!strcmp(device->buf, "ACL"))
              {
                devparms.triggeredgesource = 6;
              }
              else
              {
                line = __LINE__;
                goto OUT_ERROR;
              }

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", chn + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 20)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:LEV?") != 14)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.triggeredgelevel[chn] = atof(device->buf);
  }

  if(devparms.triggeredgesource < 4)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", devparms.triggeredgesource + 1);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(str) != 20)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 4)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR EXT") != 18)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 5)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR EXT5") != 19)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 6)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":TRIG:EDG:SOUR AC") != 17)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:HOLD?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.triggerholdoff = atof(device->buf);

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

  usleep(TMC_GDS_DELAY);

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

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:GRID?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "NONE"))
  {
    devparms.displaygrid = 0;
  }
  else if(!strcmp(device->buf, "HALF"))
    {
      devparms.displaygrid = 1;
    }
    else if(!strcmp(device->buf, "FULL"))
      {
        devparms.displaygrid = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":MEAS:COUN:SOUR?") != 16)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "OFF"))
  {
    devparms.countersrc = 0;
  }
  else if(!strcmp(device->buf, "CHAN1"))
    {
      devparms.countersrc = 1;
    }
    else if(!strcmp(device->buf, "CHAN2"))
      {
        devparms.countersrc = 2;
      }
      else if(!strcmp(device->buf, "CHAN3"))
        {
          devparms.countersrc = 3;
        }
        else if(!strcmp(device->buf, "CHAN4"))
          {
            devparms.countersrc = 4;
          }
          else
          {
            line = __LINE__;
            goto OUT_ERROR;
          }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:TYPE?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "VECT"))
  {
    devparms.displaytype = 0;
  }
  else if(!strcmp(device->buf, "DOTS"))
    {
      devparms.displaytype = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:TYPE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "NORM"))
  {
    devparms.acquiretype = 0;
  }
  else if(!strcmp(device->buf, "AVER"))
    {
      devparms.acquiretype = 1;
    }
    else if(!strcmp(device->buf, "PEAK"))
      {
        devparms.acquiretype = 2;
      }
      else if(!strcmp(device->buf, "HRES"))
        {
          devparms.acquiretype = 3;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:AVER?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.acquireaverages = atoi(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":DISP:GRAD:TIME?") != 16)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "MIN"))
  {
    devparms.displaygrading = 0;
  }
  else if(!strcmp(device->buf, "0.1"))
    {
      devparms.displaygrading = 1;
    }
    else if(!strcmp(device->buf, "0.2"))
      {
        devparms.displaygrading = 2;
      }
      else if(!strcmp(device->buf, "0.5"))
        {
          devparms.displaygrading = 5;
        }
        else if(!strcmp(device->buf, "1"))
          {
            devparms.displaygrading = 10;
          }
          else if(!strcmp(device->buf, "2"))
            {
              devparms.displaygrading = 20;
            }
            else if(!strcmp(device->buf, "5"))
              {
                devparms.displaygrading = 50;
              }
              else if(!strcmp(device->buf, "INF"))
                {
                  devparms.displaygrading = 10000;
                }
                else
                {
                  line = __LINE__;
                  goto OUT_ERROR;
                }

  QApplication::restoreOverrideCursor();

  return 0;

OUT_ERROR:

  QApplication::restoreOverrideCursor();

  sprintf(str, "An error occurred while reading settings from device.\n"
               "File %s line %i", __FILE__, line);

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();

  return -1;
}


int UI_Mainwindow::parse_preamble(char *str, int sz, struct waveform_preamble *wfp, int chn)
{
  char *ptr;

  if(sz < 19)
  {
    return -1;
  }

  ptr = strtok(str, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->format = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->type = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->points = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->count = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xincrement[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xorigin[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xreference[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yincrement[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yorigin[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yreference[chn] = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr != NULL)
  {
    return -1;
  }

  return 0;
}


int UI_Mainwindow::get_metric_factor(double value)
{
  int suffix=0;

  if(value < 0)
  {
      value *= -1;
  }

  if(value >= 1e12 && value < 1e15)
  {
      suffix = 12;
  }
  else if(value >= 1e9 && value < 1e12)
    {
        suffix = 9;
    }
    else if(value >= 1e6 && value < 1e9)
      {
          suffix = 6;
      }
      else if(value >= 1e3 && value < 1e6)
        {
          suffix = 3;
        }
        else if(value >= 1e-3 && value < 1)
          {
            suffix = -3;
          }
          else if( value >= 1e-6 && value < 1e-3)
            {
              suffix = -6;
            }
            else if(value >= 1e-9 && value < 1e-6)
              {
                suffix = -9;
              }
              else if(value >= 1e-12 && value < 1e-9)
                {
                  suffix = -12;
                }

  return suffix;
}


double UI_Mainwindow::get_stepsize_divide_by_1000(double val)
{
  int exp=0;

  if(val < 1e-9)
  {
    return 1e-9;
  }

  while(val < 1)
  {
    val *= 10;

    exp--;
  }

  while(val >= 10)
  {
    val /= 10;

    exp++;
  }

  val = exp10(exp - 2);

  if((val < 1e-13) && (val > -1e-13))
  {
    return 0;
  }

  return val;
}


void UI_Mainwindow::get_device_model(const char *str)
{
  devparms.channel_cnt = 0;

  devparms.bandwidth = 0;

  devparms.hordivisions = 14;

  if(!strcmp(str, "DS6104"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 1000;

    devparms.modelserie = 6;
  }

  if(!strcmp(str, "DS6064"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 600;

    devparms.modelserie = 6;
  }

  if(!strcmp(str, "DS6102"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 1000;

    devparms.modelserie = 6;
  }

  if(!strcmp(str, "DS6062"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 600;

    devparms.modelserie = 6;
  }

  if(!strcmp(str, "DS4012"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4014"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4022"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4024"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 200;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4032"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 350;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4034"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 350;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4052"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 500;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS4054"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 500;

    devparms.modelserie = 4;
  }

  if(!strcmp(str, "DS2072A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 70;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2072A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 70;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2102A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2102A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2202A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2202A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2302A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 300;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS2302A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 300;

    devparms.modelserie = 2;
  }

  if(!strcmp(str, "DS1054Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 50;

    devparms.modelserie = 1;

    devparms.hordivisions = 12;
  }

  if(!strcmp(str, "DS1074Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 70;

    devparms.modelserie = 1;

    devparms.hordivisions = 12;
  }

  if(!strcmp(str, "DS1074Z-S"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 70;

    devparms.modelserie = 1;

    devparms.hordivisions = 12;
  }

  if(!strcmp(str, "DS1104Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;

    devparms.modelserie = 1;

    devparms.hordivisions = 12;
  }

  if(!strcmp(str, "DS1104Z-S"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;

    devparms.modelserie = 1;

    devparms.hordivisions = 12;
  }

  if(devparms.channel_cnt && devparms.bandwidth && devparms.modelserie)
  {
    strcpy(devparms.modelname, str);
  }
}


void UI_Mainwindow::former_page()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.timebasedelayoffset <= -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      return;
    }

    devparms.timebasedelayoffset -= devparms.timebasedelayscale * devparms.hordivisions;

    if(devparms.timebasedelayoffset <= -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      devparms.timebasedelayoffset = -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale));
    }

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }
  else
  {
    devparms.timebaseoffset -= devparms.timebasescale * devparms.hordivisions;

    if(devparms.acquirememdepth > 10)
    {
      if(devparms.timebaseoffset <= -(((double)devparms.acquirememdepth / devparms.samplerate) / 2))
      {
        devparms.timebaseoffset = -(((double)devparms.acquirememdepth / devparms.samplerate) / 2);
      }
    }
    else
    {
      if(devparms.timebaseoffset <= -((devparms.hordivisions / 2) * devparms.timebasescale))
      {
        devparms.timebaseoffset = -((devparms.hordivisions / 2) * devparms.timebasescale);
      }
    }

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }

  waveForm->update();
}


void UI_Mainwindow::next_page()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.timebasedelayoffset >= (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      return;
    }

    devparms.timebasedelayoffset += devparms.timebasedelayscale * devparms.hordivisions;

    if(devparms.timebasedelayoffset >= (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      devparms.timebasedelayoffset = (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale));
    }

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }
  else
  {
    devparms.timebaseoffset += devparms.timebasescale * devparms.hordivisions;

    if(devparms.acquirememdepth > 10)
    {
      if(devparms.timebaseoffset >= (((double)devparms.acquirememdepth / devparms.samplerate) / 2))
      {
        devparms.timebaseoffset = (((double)devparms.acquirememdepth / devparms.samplerate) / 2);
      }
    }
    else
    {
      if(devparms.timebaseoffset >= (devparms.hordivisions / 2) * devparms.timebasescale)
      {
        devparms.timebaseoffset = (devparms.hordivisions / 2) * devparms.timebasescale;
      }
    }

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }

  waveForm->update();
}


void UI_Mainwindow::shift_page_left()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.timebasedelayoffset <= -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      return;
    }

    devparms.timebasedelayoffset -= devparms.timebasedelayscale;

    if(devparms.timebasedelayoffset <= -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      devparms.timebasedelayoffset = -(((devparms.hordivisions / 2) * devparms.timebasescale) - devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale));
    }

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }
  else
  {
    devparms.timebaseoffset -= devparms.timebasescale;

    if(devparms.acquirememdepth > 10)
    {
      if(devparms.timebaseoffset <= -(((double)devparms.acquirememdepth / devparms.samplerate) / 2))
      {
        devparms.timebaseoffset = -(((double)devparms.acquirememdepth / devparms.samplerate) / 2);
      }
    }
    else
    {
      if(devparms.timebaseoffset <= -((devparms.hordivisions / 2) * devparms.timebasescale))
      {
        devparms.timebaseoffset = -((devparms.hordivisions / 2) * devparms.timebasescale);
      }
    }

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }

  waveForm->update();
}


void UI_Mainwindow::shift_page_right()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.timebasedelayoffset >= (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      return;
    }

    devparms.timebasedelayoffset += devparms.timebasedelayscale;

    if(devparms.timebasedelayoffset >= (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale)))
    {
      devparms.timebasedelayoffset = (((devparms.hordivisions / 2) * devparms.timebasescale) + devparms.timebaseoffset - ((devparms.hordivisions / 2) * devparms.timebasedelayscale));
    }

    strcpy(str, "Delayed timebase position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }
  else
  {
    devparms.timebaseoffset += devparms.timebasescale;

    if(devparms.acquirememdepth > 10)
    {
      if(devparms.timebaseoffset >= (((double)devparms.acquirememdepth / devparms.samplerate) / 2))
      {
        devparms.timebaseoffset = (((double)devparms.acquirememdepth / devparms.samplerate) / 2);
      }
    }
    else
    {
      if(devparms.timebaseoffset >= (devparms.hordivisions / 2) * devparms.timebasescale)
      {
        devparms.timebaseoffset = (devparms.hordivisions / 2) * devparms.timebasescale;
      }
    }

    strcpy(str, "Horizontal position: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebaseoffset, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    horPosDial_timer->start(TMC_DIAL_TIMER_DELAY);
  }

  waveForm->update();
}


void UI_Mainwindow::zoom_in()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.modelserie == 1)
    {
      if(devparms.timebasescale <= 5.001e-9)
      {
        devparms.timebasescale = 5e-9;

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

          return;
        }
      }
      else
      {
        if(devparms.timebasedelayscale <= 1.001e-9)
        {
          devparms.timebasedelayscale = 1e-9;

          return;
        }
      }
    }

    devparms.timebasedelayscale = round_down_step125(devparms.timebasedelayscale);

    strcpy(str, "Delayed timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayscale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:SCAL %e", devparms.timebasedelayscale);

    set_cue_cmd(str);
  }
  else
  {
    if(devparms.modelserie == 1)
    {
      if(devparms.timebasescale <= 5.001e-9)
      {
        devparms.timebasescale = 5e-9;

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

          return;
        }
      }
      else
      {
        if(devparms.timebasescale <= 1.001e-9)
        {
          devparms.timebasescale = 1e-9;

          return;
        }
      }
    }

    devparms.timebasescale = round_down_step125(devparms.timebasescale);

    strcpy(str, "Timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasescale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:SCAL %e", devparms.timebasescale);

    set_cue_cmd(str);
  }

  waveForm->update();
}


void UI_Mainwindow::zoom_out()
{
  char str[256];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  if(devparms.timebasedelayenable)
  {
    if(devparms.timebasedelayscale >= devparms.timebasescale / 2)
    {
      devparms.timebasedelayscale = devparms.timebasescale / 2;

      return;
    }

    if(devparms.timebasedelayscale >= 0.1)
    {
      devparms.timebasedelayscale = 0.1;

      return;
    }

    devparms.timebasedelayscale = round_up_step125(devparms.timebasedelayscale);

    strcpy(str, "Delayed timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasedelayscale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:DEL:SCAL %e", devparms.timebasedelayscale);

    set_cue_cmd(str);
  }
  else
  {
    if(devparms.timebasescale >= 10)
    {
      devparms.timebasescale = 10;

      return;
    }

    devparms.timebasescale = round_up_step125(devparms.timebasescale);

    strcpy(str, "Timebase: ");

    convert_to_metric_suffix(str + strlen(str), devparms.timebasescale, 2);

    strcat(str, "s");

    statusLabel->setText(str);

    sprintf(str, ":TIM:SCAL %e", devparms.timebasescale);

    set_cue_cmd(str);
  }

  waveForm->update();
}


void UI_Mainwindow::chan_scale_plus()
{
  int chn;

  double val, ltmp;

  char str[512];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  chn = devparms.activechannel;

  if(devparms.chanscale[chn] >= 20)
  {
    devparms.chanscale[chn] = 20;

    return;
  }

  ltmp = devparms.chanscale[chn];

  val = round_up_step125(devparms.chanscale[chn]);

  if(devparms.chanvernier[chn])
  {
    val /= 100;

    devparms.chanscale[chn] += val;
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

  set_cue_cmd(str);

  waveForm->update();
}


void UI_Mainwindow::shift_trace_up()
{
  int chn;

  char str[512];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  chn = devparms.activechannel;

  if(devparms.chanoffset[chn] >= 20)
  {
    devparms.chanoffset[chn] = 20;

    return;
  }

  devparms.chanoffset[chn] += devparms.chanscale[chn];

  sprintf(str, "Channel %i offset: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  waveForm->label_active = chn + 1;

  label_timer->start(LABEL_TIMER_IVAL);

  vertOffsDial_timer->start(TMC_DIAL_TIMER_DELAY);

  waveForm->update();
}


void UI_Mainwindow::shift_trace_down()
{
  int chn;

  char str[512];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  chn = devparms.activechannel;

  if(devparms.chanoffset[chn] <= -20)
  {
    devparms.chanoffset[chn] = -20;

    return;
  }

  devparms.chanoffset[chn] -= devparms.chanscale[chn];

  sprintf(str, "Channel %i offset: ", chn + 1);

  convert_to_metric_suffix(str + strlen(str), devparms.chanoffset[chn], 2);

  strcat(str, "V");

  statusLabel->setText(str);

  waveForm->label_active = chn + 1;

  label_timer->start(LABEL_TIMER_IVAL);

  vertOffsDial_timer->start(TMC_DIAL_TIMER_DELAY);

  waveForm->update();
}


void UI_Mainwindow::chan_scale_minus()
{
  int chn;

  double val, ltmp;

  char str[512];

  if((device == NULL) || (!devparms.connected) || (devparms.activechannel < 0))
  {
    return;
  }

  chn = devparms.activechannel;

  if(devparms.chanscale[chn] <= 1e-2)
  {
    devparms.chanscale[chn] = 1e-2;

    return;
  }

  ltmp = devparms.chanscale[chn];

  if(devparms.chanvernier[chn])
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

    devparms.chanscale[chn] -= val;
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

  set_cue_cmd(str);

  waveForm->update();
}


void UI_Mainwindow::set_to_factory()
{
  int i;

  char str[256];

  if((device == NULL) || (!devparms.connected))
  {
    return;
  }

  scrn_timer->stop();

  scrn_thread->wait();

  qApp->processEvents();

  tmc_write("*RST");

  qApp->processEvents();

  devparms.timebasescale = 1e-6;

  devparms.timebaseoffset = 0;

  devparms.timebasedelayenable = 0;

  devparms.timebasedelayoffset = 0;

  for(i=0; i<MAX_CHNS; i++)
  {
    devparms.chanscale[i] = 1;

    devparms.chanoffset[i] = 0;

    devparms.chandisplay[i] = 0;

    devparms.chancoupling[i] = 1;

    devparms.chanbwlimit[i] = 0;

    devparms.chanprobe[i] = 10;

    devparms.chaninvert[i] = 0;

    devparms.chanvernier[i] = 0;

    devparms.triggeredgelevel[i] = 0;
  }

  devparms.chandisplay[0] = 1;

  devparms.activechannel = 0;

  devparms.acquiretype = 0;

  devparms.acquirememdepth = 0;

  devparms.triggermode = 0;

  devparms.triggeredgesource = 0;

  devparms.triggeredgeslope = 0;

  devparms.triggerstatus = 3;

  devparms.triggercoupling = 1;

  if(devparms.modelserie == 1)
  {
    devparms.triggerholdoff = 1.6e-8;
  }
  else
  {
    devparms.triggerholdoff = 1e-7;
  }

  devparms.displaytype = 0;

  devparms.displaygrid = 2;

  devparms.countersrc = 0;

  statusLabel->setText("Reset to factory settings");

  waveForm->update();

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  for(i=0; i<50; i++)
  {
    qApp->processEvents();

    usleep(500000);

    tmc_write("*OPC?");

    tmc_read();

    if(device->buf[0] == '1')
    {
      break;
    }
  }

  if(devparms.modelserie == 6)
  {
    for(i=0; i<MAX_CHNS; i++)
    {
      sprintf(str, ":CHAN%i:SCAL 1", i + 1);

      tmc_write(str);

      usleep(20000);
    }
  }

  QApplication::restoreOverrideCursor();

  scrn_timer->start(devparms.screentimerival);
}


// this function is called when screen_thread has finished
void UI_Mainwindow::screenUpdate()
{
  int i, chns=0;

  char str[512];

  if(device == NULL)
  {
    devparms.mutexx->unlock();

    return;
  }

  if(!devparms.connected)
  {
    devparms.mutexx->unlock();

    return;
  }

  if(!devparms.screenupdates_on)
  {
    devparms.mutexx->unlock();

    return;
  }

  scrn_thread->get_params(&devparms);

  if(devparms.thread_error_stat)
  {
    scrn_timer->stop();

    sprintf(str, "An error occurred while reading screen data from device.\n"
                "File screen_thread.cpp line %i", devparms.thread_error_line);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(str);
    msgBox.exec();

    devparms.mutexx->unlock();

    close_connection();

    return;
  }

  if(devparms.thread_result == TMC_THRD_RESULT_NONE)
  {
    devparms.mutexx->unlock();

    return;
  }

  if(devparms.thread_result == TMC_THRD_RESULT_CMD)
  {
    if(devparms.thread_job == TMC_THRD_JOB_TRIGEDGELEV)
    {
      devparms.triggeredgelevel[devparms.triggeredgesource] = devparms.thread_value;

//      waveForm->setTrigLineVisible();
    }

    devparms.mutexx->unlock();

    return;
  }

  if(scrn_timer->isActive() == false)
  {
    devparms.mutexx->unlock();

    return;
  }

  runButton->setStyleSheet(def_stylesh);

  singleButton->setStyleSheet(def_stylesh);

  if(devparms.triggerstatus == 0)
  {
    runButton->setStyleSheet("background: #66FF99;");
  }
  else if(devparms.triggerstatus == 1)
    {
      singleButton->setStyleSheet("background: #FF9966;");
    }
    else if(devparms.triggerstatus == 2)
      {
        runButton->setStyleSheet("background: #66FF99;");
      }
      else if(devparms.triggerstatus == 3)
        {
          runButton->setStyleSheet("background: #66FF99;");
        }
        else if(devparms.triggerstatus == 5)
          {
            runButton->setStyleSheet("background: #FF0066;");
          }

  if(devparms.triggersweep == 0)
  {
    trigModeAutoLed->setValue(true);
    trigModeNormLed->setValue(false);
    trigModeSingLed->setValue(false);
  }
  else if(devparms.triggersweep == 1)
    {
      trigModeAutoLed->setValue(false);
      trigModeNormLed->setValue(true);
      trigModeSingLed->setValue(false);
    }
    else if(devparms.triggersweep == 2)
      {
        trigModeAutoLed->setValue(false);
        trigModeNormLed->setValue(false);
        trigModeSingLed->setValue(true);
      }

  if(waveForm->hasMoveEvent() == true)
  {
    devparms.mutexx->unlock();

    return;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!devparms.chandisplay[i])  // Display data only when channel is switched on
    {
      continue;
    }

    chns++;
  }

  if(!chns)
  {
    waveForm->clear();

    devparms.mutexx->unlock();

    return;
  }

  if(devparms.triggerstatus != 1)  // Don't plot waveform data when triggerstatus is "wait"
  {
    waveForm->drawCurve(&devparms, device, devparms.wavebufsz);
  }
  else
  {
    waveForm->update();
  }

  devparms.mutexx->unlock();
}


void UI_Mainwindow::set_cue_cmd(const char *str)
{
  strncpy(devparms.cmd_cue[devparms.cmd_cue_idx_in], str, 128);

  devparms.cmd_cue[devparms.cmd_cue_idx_in][127] = 0;

  devparms.cmd_cue_idx_in++;

  devparms.cmd_cue_idx_in %= TMC_CMD_CUE_SZ;

  scrn_timer_handler();
}














