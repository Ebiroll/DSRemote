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



#ifndef SIGNALCURVE_H
#define SIGNALCURVE_H


#include <QtGlobal>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QPen>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QTimer>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mainwindow.h"
#include "global.h"
#include "connection.h"
#include "tmc_dev.h"
#include "utils.h"



class UI_Mainwindow;


class SignalCurve: public QWidget
{
  Q_OBJECT

public:
  SignalCurve(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(30,10); }

  int label_active,
      trig_line_visible;

  unsigned int scr_update_cntr;

  void setSignalColor1(QColor);
  void setSignalColor2(QColor);
  void setSignalColor3(QColor);
  void setSignalColor4(QColor);
  void setTraceWidth(int);
  void setBackgroundColor(QColor);
  void setRasterColor(QColor);
  void setTextColor(QColor);
  void setBorderSize(int);
  void drawCurve(struct device_settings *, struct tmcdev *);
  void clear();
  void setUpdatesEnabled(bool);
  void setTrigLineVisible(void);
  void setDeviceParameters(struct device_settings *);
  bool hasMoveEvent(void);

signals: void chan1Clicked();
         void chan2Clicked();
         void chan3Clicked();
         void chan4Clicked();

private slots:

  void trig_line_timer_handler();
  void trig_stat_timer_handler();

private:

  QColor SignalColor[MAX_CHNS],
         BackgroundColor,
         RasterColor,
         TextColor;

  QTimer *trig_line_timer,
         *trig_stat_timer;

  QFont smallfont;

  double v_sense,
         fft_v_sense,
         fft_v_offset;

  int bufsize,
      bordersize,
      tracewidth,
      w,
      h,
      old_w,
      updates_enabled,
      chan_arrow_moving[MAX_CHNS],
      trig_level_arrow_moving,
      trig_pos_arrow_moving,
      use_move_events,
      chan_arrow_pos[MAX_CHNS],
      trig_level_arrow_pos,
      trig_pos_arrow_pos,
      trig_stat_flash,
      fft_arrow_pos,
      fft_arrow_moving,
      mouse_x,
      mouse_y,
      mouse_old_x,
      mouse_old_y;

  clock_t clk_start,
          clk_end;

  double cpu_time_used;

  void drawWidget(QPainter *, int, int);
  void drawArrow(QPainter *, int, int, int, QColor, char);
  void drawSmallTriggerArrow(QPainter *, int, int, int, QColor);
  void drawTrigCenterArrow(QPainter *, int, int);
  void drawChanLabel(QPainter *, int, int, int);
  void drawTopLabels(QPainter *);
  void paintLabel(QPainter *, int, int, int, int, const char *, QColor);
  void paintCounterLabel(QPainter *, int, int);
  void drawFFT(QPainter *, int, int);
  void drawfpsLabel(QPainter *, int, int);

  struct device_settings *devparms;

  struct tmcdev *device;

  UI_Mainwindow *mainwindow;

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void resizeEvent(QResizeEvent *);

};


#endif


