/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017, 2018, 2019 Teunis van Beelen
*
* Email: teuniz@protonmail.com
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



#ifndef WAVECURVE_H
#define WAVECURVE_H


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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "utils.h"
#include "wave_dialog.h"


class UI_wave_window;


class WaveCurve: public QWidget
{
  Q_OBJECT

public:
  WaveCurve(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(30,10); }

  void setSignalColor1(QColor);
  void setSignalColor2(QColor);
  void setSignalColor3(QColor);
  void setSignalColor4(QColor);
  void setTraceWidth(int);
  void setBackgroundColor(QColor);
  void setRasterColor(QColor);
  void setTextColor(QColor);
  void setBorderSize(int);
  void setDeviceParameters(struct device_settings *);


private slots:

private:

  QColor SignalColor[MAX_CHNS],
         BackgroundColor,
         RasterColor,
         TextColor;

  QFont smallfont;

  double v_sense;

  int bufsize,
      bordersize,
      tracewidth,
      w,
      h,
      old_w,
      chan_arrow_pos[MAX_CHNS],
      trig_level_arrow_pos,
      trig_pos_arrow_pos,
      use_move_events,
      mouse_x,
      mouse_y,
      mouse_old_x,
      mouse_old_y;

  void drawArrow(QPainter *, int, int, int, QColor, char);
  void drawSmallTriggerArrow(QPainter *, int, int, int, QColor);
  void drawTrigCenterArrow(QPainter *, int, int);
  void drawTopLabels(QPainter *);
  void drawChanLabel(QPainter *, int, int, int);
  void paintLabel(QPainter *, int, int, int, int, const char *, QColor);
  void draw_decoder(QPainter *, int, int);
  int ascii_decode_control_char(char, char *);

  struct device_settings *devparms;

  UI_wave_window *wavedialog;

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);

};


#endif


