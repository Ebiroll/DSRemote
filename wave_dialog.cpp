/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016 Teunis van Beelen
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
  int i, samples_per_div;

  mainwindow = (UI_Mainwindow *)parnt;

  setMinimumSize(840, 635);
  setWindowTitle("Wave Memory");

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
    samples_per_div = devparms->samplerate * devparms->timebasedelayscale;
  }
  else
  {
    samples_per_div = devparms->samplerate * devparms->timebasescale;
  }

  devparms->timebasedelayenable = 0;

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
  wavslider->setRange(0, devparms->wavebufsz - (devparms->hordivisions * samples_per_div));
  wavslider->setValue((devparms->wavebufsz - (devparms->hordivisions * samples_per_div)) / 2);

  devparms->wave_mem_view_sample_start = wavslider->value();

  g_layout = new QGridLayout(this);
  g_layout->addWidget(wavcurve, 0, 0);
  g_layout->addWidget(wavslider, 1, 0);

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


void UI_wave_window::wavslider_value_changed(int val)
{
  devparms->wave_mem_view_sample_start = val;

  wavcurve->update();
}





























