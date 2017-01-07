/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017 Teunis van Beelen
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




#include "wave_dialog.h"



UI_wave_window::UI_wave_window(struct device_settings *p_devparms, short *wbuf[MAX_CHNS], QWidget *parnt)
{
  int i;

  mainwindow = (UI_Mainwindow *)parnt;

  setMinimumSize(840, 655);
  setWindowTitle("Wave Inspector");

  devparms = (struct device_settings *)calloc(1, sizeof(struct device_settings));
  if(devparms == NULL)
  {
    printf("Malloc error! file: %s  line: %i", __FILE__, __LINE__);
  }
  else
  {
    *devparms = *p_devparms;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    devparms->wavebuf[i] = wbuf[i];
  }

  devparms->wavebufsz = devparms->acquirememdepth;

  if(devparms->timebasedelayenable)
  {
    devparms->timebasescale = devparms->timebasedelayscale;
  }

  devparms->timebasedelayenable = 0;

  devparms->viewer_center_position = 0;

  devparms->wave_mem_view_enabled = 1;

  if(devparms->math_decode_display)
  {
    mainwindow->serial_decoder(devparms);
  }

  wavcurve = new WaveCurve;
  wavcurve->setBackgroundColor(Qt::black);
  wavcurve->setSignalColor1(Qt::yellow);
  wavcurve->setSignalColor2(Qt::cyan);
  wavcurve->setSignalColor3(Qt::magenta);
  wavcurve->setSignalColor4(QColor(0, 128, 255));
  wavcurve->setRasterColor(Qt::darkGray);
  wavcurve->setBorderSize(40);
  wavcurve->setDeviceParameters(devparms);

  wavslider = new QSlider;
  wavslider->setOrientation(Qt::Horizontal);
  set_wavslider();

  devparms->wave_mem_view_sample_start = wavslider->value();

  menubar = new QMenuBar(this);

  savemenu = new QMenu(this);
  savemenu->setTitle("Save");
  savemenu->addAction("Save to EDF file", this, SLOT(save_wi_buffer_to_edf()));
  menubar->addMenu(savemenu);

  helpmenu = new QMenu(this);
  helpmenu->setTitle("Help");
  helpmenu->addAction("How to operate", mainwindow, SLOT(helpButtonClicked()));
  helpmenu->addAction("About", mainwindow, SLOT(show_about_dialog()));
  menubar->addMenu(helpmenu);

  g_layout = new QGridLayout(this);
  g_layout->setMenuBar(menubar);
  g_layout->addWidget(wavcurve, 0, 0);
  g_layout->addWidget(wavslider, 1, 0);

  former_page_act = new QAction(this);
  former_page_act->setShortcut(QKeySequence::MoveToPreviousPage);
  connect(former_page_act, SIGNAL(triggered()), this, SLOT(former_page()));
  addAction(former_page_act);

  shift_page_left_act = new QAction(this);
  shift_page_left_act->setShortcut(QKeySequence::MoveToPreviousChar);
  connect(shift_page_left_act, SIGNAL(triggered()), this, SLOT(shift_page_left()));
  addAction(shift_page_left_act);

  center_position_act = new QAction(this);
  center_position_act->setShortcut(QKeySequence("c"));
  connect(center_position_act, SIGNAL(triggered()), this, SLOT(center_position()));
  addAction(center_position_act);

  center_trigger_act = new QAction(this);
  center_trigger_act->setShortcut(QKeySequence("t"));
  connect(center_trigger_act, SIGNAL(triggered()), this, SLOT(center_trigger()));
  addAction(center_trigger_act);

  shift_page_right_act = new QAction(this);
  shift_page_right_act->setShortcut(QKeySequence::MoveToNextChar);
  connect(shift_page_right_act, SIGNAL(triggered()), this, SLOT(shift_page_right()));
  addAction(shift_page_right_act);

  next_page_act = new QAction(this);
  next_page_act->setShortcut(QKeySequence::MoveToNextPage);
  connect(next_page_act, SIGNAL(triggered()), this, SLOT(next_page()));
  addAction(next_page_act);

  zoom_in_act = new QAction(this);
  zoom_in_act->setShortcut(QKeySequence::ZoomIn);
  connect(zoom_in_act, SIGNAL(triggered()), this, SLOT(zoom_in()));
  addAction(zoom_in_act);

  zoom_out_act = new QAction(this);
  zoom_out_act->setShortcut(QKeySequence::ZoomOut);
  connect(zoom_out_act, SIGNAL(triggered()), this, SLOT(zoom_out()));
  addAction(zoom_out_act);

  connect(wavslider, SIGNAL(sliderMoved(int)), this, SLOT(wavslider_value_changed(int)));

  show();
}


UI_wave_window::~UI_wave_window()
{
  int i;

  for(i=0; i<MAX_CHNS; i++)
  {
    free(devparms->wavebuf[i]);
  }

  free(devparms);
}


void UI_wave_window::save_wi_buffer_to_edf()
{
  mainwindow->save_wave_inspector_buffer_to_edf(devparms);
}


void UI_wave_window::wavslider_value_changed(int val)
{
  devparms->wave_mem_view_sample_start = val;

  int samples_per_div = devparms->samplerate * devparms->timebasescale;

  devparms->viewer_center_position = (double)(((devparms->wavebufsz - (devparms->hordivisions * samples_per_div)) / 2) - devparms->wave_mem_view_sample_start) /
                              devparms->samplerate * -1.0;


  devparms->viewer_center_position = round_to_3digits(devparms->viewer_center_position);

  wavcurve->update();
}


void UI_wave_window::set_wavslider(void)
{
  int samples_per_div = devparms->samplerate * devparms->timebasescale;

  wavslider->setRange(0, devparms->wavebufsz - (devparms->hordivisions * samples_per_div));

  devparms->wave_mem_view_sample_start = ((devparms->wavebufsz - (devparms->hordivisions * samples_per_div)) / 2) +
                                         devparms->samplerate * devparms->viewer_center_position;

  wavslider->setValue(devparms->wave_mem_view_sample_start);
}


void UI_wave_window::former_page()
{
  devparms->viewer_center_position -= devparms->timebasescale * devparms->hordivisions;

  if(devparms->viewer_center_position <= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / -2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / -2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::next_page()
{
  devparms->viewer_center_position += devparms->timebasescale * devparms->hordivisions;

  if(devparms->viewer_center_position >= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / 2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / 2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::shift_page_left()
{
  devparms->viewer_center_position -= devparms->timebasescale;

  if(devparms->viewer_center_position <= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / -2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / -2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::shift_page_right()
{
  devparms->viewer_center_position += devparms->timebasescale;

  if(devparms->viewer_center_position >= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / 2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / 2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::center_position()
{
  devparms->viewer_center_position = 0;

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::center_trigger()
{
  devparms->viewer_center_position = -devparms->timebaseoffset;

  if(devparms->viewer_center_position <= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / -2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / -2;
  }

  if(devparms->viewer_center_position >= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / 2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / 2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::zoom_in()
{
  devparms->timebasescale = round_down_step125(devparms->timebasescale, NULL);

  if(devparms->timebasescale <= 1.001e-9)
  {
    devparms->timebasescale = 1e-9;
  }

  if(devparms->viewer_center_position <= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / -2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / -2;
  }

  if(devparms->viewer_center_position >= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / 2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / 2;
  }

  set_wavslider();

  wavcurve->update();
}


void UI_wave_window::zoom_out()
{
  double dtmp = round_up_step125(devparms->timebasescale, NULL);

  if(dtmp >= ((double)devparms->acquirememdepth / devparms->samplerate) / (double)devparms->hordivisions)
  {
    return;
  }

  devparms->timebasescale = round_up_step125(devparms->timebasescale, NULL);

  if(devparms->viewer_center_position <= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / -2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / -2;
  }

  if(devparms->viewer_center_position >= ((((double)devparms->acquirememdepth / devparms->samplerate) -
                                  (devparms->timebasescale * devparms->hordivisions)) / 2))
  {
    devparms->viewer_center_position = (((double)devparms->acquirememdepth / devparms->samplerate) -
                                (devparms->timebasescale * devparms->hordivisions)) / 2;
  }

  set_wavslider();

  wavcurve->update();
}





























