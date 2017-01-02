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



#ifndef T_DIAL_H
#define T_DIAL_H

#include <QtGlobal>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <QBrush>
#include <QRadialGradient>



class TDial: public QWidget
{
  Q_OBJECT

public:
  TDial(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(80,80); }

  void set_fg_color(QColor);
  void set_bg_color(QColor);
  QColor get_fg_color(void);
  QColor get_bg_color(void);
  void setWrapping(bool);
  void setEnabled(bool);
  void setNotchesVisible(bool);
  void setMaximum(int);
  void setMinimum(int);
  void setValue(int);
  int value(void);
  void setSingleStep(int);
  bool isSliderDown(void);
  void setSliderPosition(int);

private:

  int dval,
      dval_old,
      dmin,
      dmax,
      step;

  double dial_grad,
         mouse_grad_old,
         degr_per_step;

  bool wrap,
       notch_visible,
       slider_down,
       enabled;

  QColor fg_color,
         bg_color;

  void draw_circ(int, int, int, QPainter *);
  void draw_fill_circ(int, int, int, QPainter *);
  double polar_to_degr(double, double);
  void process_rotation(void);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);

signals:
   void valueChanged(int);
   void sliderReleased();
};


#endif


