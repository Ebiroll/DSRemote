/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015, 2016, 2017, 2018, 2019 Teunis van Beelen
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





#include "tled.h"


TLed::TLed(QWidget *p) : QWidget(p)
{
  OnColor = QColor(Qt::green);
  OffColor = QColor(Qt::darkGreen);
  value = false;
}


void TLed::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
#if QT_VERSION >= 0x050000
  paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  drawLed(&paint);
}


void TLed::drawLed(QPainter *painter)
{
  int w, h, m, n, o;

  QColor color;

  painter->save();

  w = this->width();
  h = this->height();
  m = qMin(w,h);
  n = m / 2;
  o = m / 3;
  if(value)  color = OnColor;
  else  color = OffColor;

  painter->setRenderHint(QPainter::Antialiasing);
  QRadialGradient gradient(n, n, n, o, o);
  if(value)  gradient.setColorAt(0.001, Qt::white);
  gradient.setColorAt(0.9, color);
  gradient.setColorAt(1, Qt::black);
  painter->setBrush(gradient);
  painter->drawEllipse(0, 0, m, m);

  painter->restore();
}


void TLed::setOnColor(QColor newColor)
{
  OnColor = newColor;
  update();
}


void TLed::setOffColor(QColor newColor)
{
  OffColor = newColor;
  update();
}


void TLed::setValue(bool newValue)
{
  value = newValue;
  update();
}



