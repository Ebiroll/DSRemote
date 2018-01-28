/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017, 2018 Teunis van Beelen
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



#include "tdial.h"

#include <math.h>


TDial::TDial(QWidget *w_parent) : QWidget(w_parent)
{
  fg_color = QColor(100, 100, 100);
  bg_color = Qt::lightGray;
  notch_visible = false;
  wrap = false;
  dmin = 0;
  dmax = 2;
  step = 1;
  slider_down = false;
  dval = 1;
  dval_old = 1;
  enabled = true;
  mouse_grad_old=180;
  dial_grad = 180;
  degr_per_step = 1;
}


void TDial::paintEvent(QPaintEvent *)
{
  int w, h, r0, hor_a, hor_b, vert_a, vert_b, gr;

  double r1, r2, r3, r4, dtmp;

  QPainter p(this);
#if QT_VERSION >= 0x050000
  p.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif

  p.save();

  w = width();
  h = height();

  r0 = w;
  if(r0 > h)  r0 = h;

  r0 /= 2;

  p.setRenderHint(QPainter::Antialiasing);

  if(enabled == true)
  {
    QRadialGradient gradient((w / 2) - (r0 * 0.5), (h / 2) - (r0 * 0.5), r0);
    gradient.setColorAt(0.001, QColor(250, 250, 250));
    gradient.setColorAt(0.9, QColor(228, 228, 228));
    gradient.setColorAt(1, QColor(215, 215, 215));
    p.setBrush(gradient);
    draw_fill_circ(w / 2, h / 2, r0 * 0.8, &p);

    p.setPen(fg_color);
  }
  else
  {
    p.setPen(fg_color);

    draw_circ(w / 2, h / 2, r0 * 0.8, &p);
  }

  if(notch_visible == true)
  {
    r1 = r0;
    r2 = r0 * 0.85;
    r3 = r0 * 0.98;
    r4 = r0 * 0.9;

    if(r0 < 30)
    {
      step = 20;
    }
    else
    {
      step = 12;
    }

    for(gr=0; gr<360; gr+=step)
    {
      if(wrap == false)
      {
        if((gr > 240) && (gr < 300))  continue;
      }
      if(gr % 60)
      {
        vert_a = r3 * sin(gr * M_PI / 180.0);
        hor_a = r3 * cos(gr * M_PI / 180.0);

        vert_b = r4 * sin(gr * M_PI / 180.0);
        hor_b = r4 * cos(gr * M_PI / 180.0);
      }
      else
      {
        vert_a = r1 * sin(gr * M_PI / 180.0);
        hor_a = r1 * cos(gr * M_PI / 180.0);

        vert_b = r2 * sin(gr * M_PI / 180.0);
        hor_b = r2 * cos(gr * M_PI / 180.0);
      }

      p.drawLine((w / 2) - hor_b, (h / 2) - vert_b, (w / 2) - hor_a, (h / 2) - vert_a);
    }
  }

  dtmp = dial_grad - 90;
  if(dtmp < 0)  dtmp += 360;

  vert_a = r0 * 0.55 * sin(dtmp * M_PI / 180.0);
  hor_a = r0 * 0.55 * cos(dtmp * M_PI / 180.0);

  if(enabled == true)
  {
    p.setBrush(QBrush(QColor(210, 210, 210)));
  }
  else
  {
    p.setBrush(QBrush(QColor(220, 220, 220)));
  }

  draw_fill_circ(w / 2 - hor_a, h / 2 - vert_a, r0 * 0.15, &p);

  p.restore();
}


void TDial::mousePressEvent(QMouseEvent *press_event)
{
  if(press_event->button()==Qt::LeftButton)
  {
    slider_down = true;
    setMouseTracking(true);

    mouse_grad_old = polar_to_degr(press_event->x() - (width() / 2), press_event->y() - (height() / 2));
  }

  press_event->accept();
}


void TDial::mouseReleaseEvent(QMouseEvent *release_event)
{
  if(release_event->button()==Qt::LeftButton)
  {
    setMouseTracking(false);
    slider_down = false;

    emit sliderReleased();
  }

  release_event->accept();
}


void TDial::mouseMoveEvent(QMouseEvent *move_event)
{
  double gr_new,
         gr_diff;

  if(slider_down == false)  return;

  gr_new = polar_to_degr(move_event->x() - (width() / 2), move_event->y() - (height() / 2));

  gr_diff = gr_new - mouse_grad_old;

  mouse_grad_old = gr_new;

  dial_grad += gr_diff;

  process_rotation();

  move_event->accept();
}


bool TDial::isSliderDown(void)
{
  return slider_down;
}


void TDial::setWrapping(bool wr)
{
  wrap = wr;
  update();
}


void TDial::setNotchesVisible(bool vis)
{
  notch_visible = vis;
  update();
}


void TDial::setMaximum(int m)
{
  dmax = m;
  if(dmax < 2)  dmax = 2;
  if(dmin >= dmax) dmin = dmax - 1;
  if(dmin < 0)  dmin = 0;
  if(wrap == true)
  {
    degr_per_step = 360.0 / ((double)(dmax - dmin) / (double)step);
  }
  else
  {
    degr_per_step = 300.0 / ((double)(dmax - dmin) / (double)step);
  }

  dval = (dmax + dmin) / 2;

  dval_old = dval;

  update();
}


void TDial::setMinimum(int m)
{
  dmin = m;
  if(dmin >= dmax) dmax = dmin + 1;
  if(dmax < 2)  dmax = 2;
  if(dmin >= dmax) dmin = dmax - 1;
  if(dmin < 0)  dmin = 0;
  if(wrap == true)
  {
    degr_per_step = 360.0 / ((double)(dmax - dmin) / (double)step);
  }
  else
  {
    degr_per_step = 300.0 / ((double)(dmax - dmin) / (double)step);
  }

  dval = (dmax + dmin) / 2;

  dval_old = dval;

  update();
}


void TDial::setValue(int v)
{
  dval = v;

  if(dval > dmax)  dval = dmax;
  if(dval < dmin)  dval = dmin;

  if(wrap == true)
  {
    dial_grad = ((double)(dval - dmin) / (double)(dmax - dmin)) * 360.0;
  }
  else
  {
    dial_grad = (((double)(dval - dmin) / (double)(dmax - dmin)) * 300.0) + 30.0;
  }

  dval_old = dval;

  update();
}


void TDial::setSliderPosition(int v)
{
  dval = v;

  if(dval > dmax)  dval = dmax;
  if(dval < dmin)  dval = dmin;

  if(wrap == true)
  {
    dial_grad = ((double)(dval - dmin) / (double)(dmax - dmin)) * 360.0;
  }
  else
  {
    dial_grad = (((double)(dval - dmin) / (double)(dmax - dmin)) * 300.0) + 30.0;
  }

  dval_old = dval;

  update();
}


int TDial::value(void)
{
  return dval;
}


void TDial::setSingleStep(int s)
{
  step = s;
  if(step > dmax)  step = dmax;
  if(step < 1)  step = 1;
  if(wrap == true)
  {
    degr_per_step = 360.0 / ((double)(dmax - dmin) / (double)step);
  }
  else
  {
    degr_per_step = 300.0 / ((double)(dmax - dmin) / (double)step);
  }
  update();
}


void TDial::set_fg_color(QColor new_color)
{
  fg_color = new_color;
  update();
}


void TDial::set_bg_color(QColor new_color)
{
  bg_color = new_color;
  update();
}


QColor TDial::get_fg_color(void)
{
  return fg_color;
}


QColor TDial::get_bg_color(void)
{
  return bg_color;
}


void TDial::draw_circ(int px, int py, int r, QPainter *p)
{
  p->drawArc(px - r, py - r, r * 2, r * 2, 0, 5760);
}


void TDial::draw_fill_circ(int px, int py, int r, QPainter *p)
{
  p->drawEllipse(px - r, py - r, r * 2, r * 2);
}


void TDial::setEnabled(bool enab)
{
  enabled = enab;
  update();
}


double TDial::polar_to_degr(double px, double py)
{
  int quad=0;

  double gr;

  if(px < 0)
  {
    quad += 2;

    px *= -1;
  }

  if(py < 0)
  {
    quad += 1;

    py *= -1;
  }

  if(px < 0.01) px = 0.01;
  if(py < 0.01) py = 0.01;

  gr = atan(py / px) * 180.0 / M_PI;

  switch(quad)
  {
    case 0: gr += 270;
            break;
    case 1: gr = 270 - gr;
            break;
    case 2: gr = 90 - gr;
            break;
    case 3: gr += 90;
            break;
  }

  return gr;
}


void TDial::wheelEvent(QWheelEvent *wheel_event)
{
  dial_grad += (wheel_event->delta() / 8);

  process_rotation();

  wheel_event->accept();
}


void TDial::process_rotation(void)
{
  if(dial_grad > 360)  dial_grad -= 360;

  if(dial_grad < 0)  dial_grad += 360;

  if(wrap == false)
  {
    if(dial_grad > 330)
    {
      dial_grad = 330;
    }
    else if(dial_grad < 30)
    {
      dial_grad = 30;
    }
  }

  if(wrap == false)
  {
    dval = (dial_grad - 30) * ((double)(dmax - dmin) / 300.0);
  }
  else
  {
    dval = dial_grad * ((double)(dmax - dmin) / 360.0);
  }

  if(dval == dval_old)  return;

  dval_old = dval;

  emit valueChanged(dval);

  update();
}








