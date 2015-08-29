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


#ifndef DEF_SCREEN_THREAD_H
#define DEF_SCREEN_THREAD_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <QObject>
#include <QThread>

#include "global.h"
#include "utils.h"
#include "connection.h"
#include "tmc_dev.h"



class screenThread : public QThread
{
  Q_OBJECT

public:

  screenThread();
  ~screenThread();

  int h_busy;

  void set_device(struct tmcdev *);

  void set_params(struct device_settings *);
  void get_params(struct device_settings *);

private:

  struct {
    int connected;
    int chandisplay[MAX_CHNS];
    int triggerstatus;
    int triggersweep;
    double samplerate;
    int memdepth;
    int countersrc;
    double counterfreq;
    int wavebufsz;
    short *wavebuf[MAX_CHNS];
    int error_stat;
    int error_line;
    int cmd_cue_idx_in;
    int cmd_cue_idx_out;
    int result;
    int job;

    double triggeredgelevel;
    double timebasedelayoffset;
    double timebasedelayscale;

    char debug_str[1024];
  } params;

  struct tmcdev *device;

  struct device_settings *deviceparms;

  void run();

  int get_devicestatus();

};



#endif


