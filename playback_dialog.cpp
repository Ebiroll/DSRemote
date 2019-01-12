/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017, 2018, 2019 Teunis van Beelen
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


#include "playback_dialog.h"



UI_playback_window::UI_playback_window(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  devparms = &mainwindow->devparms;

  mainwindow->set_cue_cmd(":FUNC:WREC:FMAX?", rec_fmax_resp);
  mainwindow->set_cue_cmd(":FUNC:WREC:FEND?", rec_fend_resp);
  if(devparms->modelserie == 1)
  {
    mainwindow->set_cue_cmd(":FUNC:WREC:FINT?", rec_fint_resp);
    mainwindow->set_cue_cmd(":FUNC:WREP:FINT?", rep_fint_resp);
  }
  else
  {
    mainwindow->set_cue_cmd(":FUNC:WREC:INT?", rec_fint_resp);
    mainwindow->set_cue_cmd(":FUNC:WREP:INT?", rep_fint_resp);
  }
  mainwindow->set_cue_cmd(":FUNC:WREP:FST?", rep_fstart_resp);
  mainwindow->set_cue_cmd(":FUNC:WREP:FEND?", rep_fend_resp);
  mainwindow->set_cue_cmd(":FUNC:WREP:FMAX?", rep_fmax_resp);

  setWindowTitle("Record/Playback");

  setMinimumSize(420, 300);
  setMaximumSize(420, 300);

  rec_fend_label = new QLabel(this);
  rec_fend_label->setGeometry(20, 20, 150, 25);
  rec_fend_label->setText("Recording Length");

  rec_fend_spinbox = new QSpinBox(this);
  rec_fend_spinbox->setGeometry(200, 20, 140, 25);
  rec_fend_spinbox->setSuffix(" frames");
  if(!devparms->func_wrec_enable)
  {
    rec_fend_spinbox->setEnabled(false);
  }

  rec_fint_label = new QLabel(this);
  rec_fint_label->setGeometry(20, 65, 150, 25);
  rec_fint_label->setText("Recording Interval");

  rec_fint_spinbox = new QDoubleSpinBox(this);
  rec_fint_spinbox->setGeometry(200, 65, 140, 25);
  rec_fint_spinbox->setDecimals(7);
  rec_fint_spinbox->setRange(1e-7, 10);
  rec_fint_spinbox->setSuffix(" Sec.");
  if(!devparms->func_wrec_enable)
  {
    rec_fint_spinbox->setEnabled(false);
  }

  rep_fstart_label = new QLabel(this);
  rep_fstart_label->setGeometry(20, 110, 150, 25);
  rep_fstart_label->setText("Playback Start Frame");

  rep_fstart_spinbox = new QSpinBox(this);
  rep_fstart_spinbox->setGeometry(200, 110, 140, 25);
  if(!devparms->func_has_record || !devparms->func_wrec_enable)
  {
    rep_fstart_spinbox->setEnabled(false);
  }

  rep_fend_label = new QLabel(this);
  rep_fend_label->setGeometry(20, 155, 150, 25);
  rep_fend_label->setText("Playback End Frame");

  rep_fend_spinbox = new QSpinBox(this);
  rep_fend_spinbox->setGeometry(200, 155, 140, 25);
  if(!devparms->func_has_record || !devparms->func_wrec_enable)
  {
    rep_fend_spinbox->setEnabled(false);
  }

  rep_fint_label = new QLabel(this);
  rep_fint_label->setGeometry(20, 200, 150, 25);
  rep_fint_label->setText("Playback Interval");

  rep_fint_spinbox = new QDoubleSpinBox(this);
  rep_fint_spinbox->setGeometry(200, 200, 140, 25);
  rep_fint_spinbox->setDecimals(7);
  rep_fint_spinbox->setRange(1e-7, 10);
  rep_fint_spinbox->setSuffix(" Sec.");
  if(!devparms->func_has_record || !devparms->func_wrec_enable)
  {
    rep_fint_spinbox->setEnabled(false);
  }

  toggle_playback_button = new QPushButton(this);
  toggle_playback_button->setGeometry(20, 255, 100, 25);
  if(!devparms->func_wrec_enable)
  {
    toggle_playback_button->setText("Enable");
  }
  else
  {
    toggle_playback_button->setText("Disable");
  }
  toggle_playback_button->setAutoDefault(false);
  toggle_playback_button->setDefault(false);
  toggle_playback_button->setEnabled(false);

  close_button = new QPushButton(this);
  close_button->setGeometry(300, 255, 100, 25);
  close_button->setText("Close");
  close_button->setAutoDefault(false);
  close_button->setDefault(false);

  t1 = new QTimer(this);

  connect(close_button,           SIGNAL(clicked()), this, SLOT(close()));
  connect(toggle_playback_button, SIGNAL(clicked()), this, SLOT(toggle_playback()));
  connect(t1,                     SIGNAL(timeout()), this, SLOT(t1_func()));

  t1->start(100);

  exec();
}


void UI_playback_window::t1_func()
{
  if((rep_fint_resp[0] == 0) ||
     (rep_fmax_resp[0] == 0) ||
     (rep_fend_resp[0] == 0) ||
     (rep_fstart_resp[0] == 0) ||
     (rec_fend_resp[0] == 0) ||
     (rec_fmax_resp[0] == 0) ||
     (rec_fint_resp[0] == 0))
  {
    return;
  }

  t1->stop();

  devparms->func_wrec_fmax = atoi(rec_fmax_resp);

  devparms->func_wrec_fend = atoi(rec_fend_resp);

  devparms->func_wrec_fintval = atof(rec_fint_resp);

  devparms->func_wplay_fstart = atoi(rep_fstart_resp);

  devparms->func_wplay_fend = atoi(rep_fend_resp);

  devparms->func_wplay_fmax = atoi(rep_fmax_resp);

  devparms->func_wplay_fintval = atof(rep_fint_resp);

  rec_fend_spinbox->setRange(1, devparms->func_wrec_fmax);
  rec_fend_spinbox->setValue(devparms->func_wrec_fend);
  rec_fint_spinbox->setValue(devparms->func_wrec_fintval);
  rep_fstart_spinbox->setValue(devparms->func_wplay_fstart);
  rep_fstart_spinbox->setRange(1, devparms->func_wrec_fend);
  rep_fend_spinbox->setRange(1, devparms->func_wrec_fend);
  rep_fend_spinbox->setValue(devparms->func_wplay_fend);
  rep_fint_spinbox->setValue(devparms->func_wplay_fintval);

  connect(rec_fend_spinbox,   SIGNAL(valueChanged(int)),    this, SLOT(rec_fend_spinbox_changed(int)));
  connect(rec_fint_spinbox,   SIGNAL(valueChanged(double)), this, SLOT(rec_fint_spinbox_changed(double)));
  connect(rep_fstart_spinbox, SIGNAL(valueChanged(int)),    this, SLOT(rep_fstart_spinbox_changed(int)));
  connect(rep_fend_spinbox,   SIGNAL(valueChanged(int)),    this, SLOT(rep_fend_spinbox_changed(int)));
  connect(rep_fint_spinbox,   SIGNAL(valueChanged(double)), this, SLOT(rep_fint_spinbox_changed(double)));

  toggle_playback_button->setEnabled(true);
}


void UI_playback_window::toggle_playback()
{
  QMessageBox msgBox;
  msgBox.setText("Timebase scale must be <= 100mS.");

  if(!devparms->func_wrec_enable)
  {
    if(devparms->timebasedelayenable)
    {
      if(devparms->timebasedelayscale > 0.1000001)
      {
        msgBox.exec();
        return;
      }
    }
    else
    {
      if(devparms->timebasescale > 0.1000001)
      {
        msgBox.exec();
        return;
      }
    }

    devparms->func_wrec_enable = 1;

    toggle_playback_button->setText("Disable");

    rec_fend_spinbox->setEnabled(true);

    rec_fint_spinbox->setEnabled(true);

    mainwindow->statusLabel->setText("Recording enabled");

    if(devparms->modelserie == 1)
    {
      mainwindow->set_cue_cmd(":FUNC:WREC:ENAB ON");
    }
    else
    {
      mainwindow->set_cue_cmd(":FUNC:WRM REC");
    }
  }
  else
  {
    devparms->func_wrec_enable = 0;

    devparms->func_has_record = 0;

    rec_fend_spinbox->setEnabled(false);

    rec_fint_spinbox->setEnabled(false);

    rep_fstart_spinbox->setEnabled(false);

    rep_fend_spinbox->setEnabled(false);

    rep_fint_spinbox->setEnabled(false);

    mainwindow->statusLabel->setText("Recording disabled");

    toggle_playback_button->setText("Enable");

    if(devparms->modelserie == 1)
    {
      mainwindow->set_cue_cmd(":FUNC:WREC:ENAB OFF");
    }
    else
    {
      mainwindow->set_cue_cmd(":FUNC:WRM OFF");
    }
  }
}


void UI_playback_window::rec_fend_spinbox_changed(int fend)
{
  char str[128];

  sprintf(str, "Recording frame end: %i", fend);

  mainwindow->statusLabel->setText(str);

  sprintf(str, ":FUNC:WREC:FEND %i", fend);

  mainwindow->set_cue_cmd(str);
}


void UI_playback_window::rec_fint_spinbox_changed(double fint)
{
  char str[128];

  strcpy(str, "Recording frame interval: ");

  convert_to_metric_suffix(str + strlen(str), 3, fint);

  strcat(str, "S");

  if(devparms->modelserie == 1)
  {
    sprintf(str, ":FUNC:WREC:FINT %e", fint);
  }
  else
  {
    sprintf(str, ":FUNC:WREC:INT %e", fint);
  }

  mainwindow->set_cue_cmd(str);
}


void UI_playback_window::rep_fstart_spinbox_changed(int fstart)
{
  char str[128];

  sprintf(str, "Playback frame start: %i", fstart);

  mainwindow->statusLabel->setText(str);

  sprintf(str, ":FUNC:WREP:FST %i", fstart);

  mainwindow->set_cue_cmd(str);
}


void UI_playback_window::rep_fend_spinbox_changed(int fend)
{
  char str[128];

  sprintf(str, "Playback frame end: %i", fend);

  mainwindow->statusLabel->setText(str);

  sprintf(str, ":FUNC:WREP:FEND %i", fend);

  mainwindow->set_cue_cmd(str);
}


void UI_playback_window::rep_fint_spinbox_changed(double fint)
{
  char str[128];

  strcpy(str, "Playback frame interval: ");

  convert_to_metric_suffix(str + strlen(str), 3, fint);

  strcat(str, "S");

  mainwindow->statusLabel->setText(str);

  if(devparms->modelserie == 1)
  {
    sprintf(str, ":FUNC:WREP:FINT %e", fint);
  }
  else
  {
    sprintf(str, ":FUNC:WREP:INT %e", fint);
  }

  mainwindow->set_cue_cmd(str);
}



























