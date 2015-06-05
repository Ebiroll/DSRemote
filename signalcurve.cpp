/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015 Teunis van Beelen
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



#include "signalcurve.h"


SignalCurve::SignalCurve(QWidget *w_parent) : QWidget(w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  setAttribute(Qt::WA_OpaquePaintEvent);

  SignalColor[0] = Qt::blue;
  SignalColor[1] = Qt::blue;
  SignalColor[2] = Qt::blue;
  SignalColor[3] = Qt::blue;
  tracewidth = 0;
  BackgroundColor = Qt::gray;
  RasterColor = Qt::darkGray;
  TextColor = Qt::black;

  smallfont.setFamily("Arial");
  smallfont.setPixelSize(8);

  trig_line_timer = new QTimer(this);
  trig_line_timer->setSingleShot(true);

  trig_stat_timer = new QTimer(this);

  bufsize = 0;
  bordersize = 60;

  v_sense = 1;

  mouse_x = 0;
  mouse_y = 0;
  mouse_old_x = 0;
  mouse_old_y = 0;

  for(i=0; i<MAX_CHNS; i++)
  {
    chan_arrow_moving[i] = 0;

    chan_arrow_pos[i] = 127;
  }

  trig_level_arrow_moving = 0;

  trig_pos_arrow_moving = 0;

  trig_line_visible = 0;

  trig_stat_flash = 0;

  trig_level_arrow_pos = 127;

  trig_pos_arrow_pos = 100;

  use_move_events = 0;

  updates_enabled = true;

  old_w = 10000;

  devparms = NULL;

  device = NULL;

  connect(trig_line_timer, SIGNAL(timeout()), this, SLOT(trig_line_timer_handler()));
  connect(trig_stat_timer, SIGNAL(timeout()), this, SLOT(trig_stat_timer_handler()));
}


void SignalCurve::clear()
{
  bufsize = 0;

  update();
}


void SignalCurve::resizeEvent(QResizeEvent *resize_event)
{
  QWidget::resizeEvent(resize_event);
}


void SignalCurve::setUpdatesEnabled(bool enabled)
{
  updates_enabled = enabled;
}


void SignalCurve::paintEvent(QPaintEvent *)
{
  if(updates_enabled == true)
  {
    QPainter paint(this);

    drawWidget(&paint, width(), height());

    old_w = width();
  }
}


void SignalCurve::drawWidget(QPainter *painter, int curve_w, int curve_h)
{
  int i, chn, tmp, rot=1;

  double offset=0.0,
         h_step=0.0,
         step,
         step2;

//  clk_start = clock();

  painter->fillRect(0, 0, curve_w, curve_h, BackgroundColor);

  if((curve_w < ((bordersize * 2) + 5)) || (curve_h < ((bordersize * 2) + 5)))
  {
    return;
  }

  painter->fillRect(0, 0, curve_w, 30, QColor(32, 32, 32));

  drawTopLabels(painter);

  tmp = 407 - ((devparms->timebaseoffset / (devparms->timebasescale * 14.0)) * 233);

  if(tmp < 291)
  {
    tmp = 286;

    rot = 2;
  }
  else if(tmp > 523)
    {
      tmp = 528;

      rot = 0;
    }

  if((rot == 0) || (rot == 2))
  {
    drawSmallTriggerArrow(painter, tmp, 11, rot, QColor(255, 128, 0));
  }
  else
  {
    drawSmallTriggerArrow(painter, tmp, 16, rot, QColor(255, 128, 0));
  }

  painter->fillRect(0, curve_h - 30, curve_w, curve_h, QColor(32, 32, 32));

  for(i=0; i<devparms->channel_cnt; i++)
  {
    drawChanLabel(painter, 8 + (i * 125), curve_h - 25, i);
  }

/////////////////////////////////// translate coordinates, draw and fill a rectangle ///////////////////////////////////////////

  painter->translate(QPoint(bordersize, bordersize));

  curve_w -= (bordersize * 2);

  curve_h -= (bordersize * 2);

/////////////////////////////////// draw the rasters ///////////////////////////////////////////

  painter->setPen(RasterColor);

  painter->drawRect (0, 0, curve_w - 1, curve_h - 1);

  if(devparms->displaygrid)
  {
    painter->setPen(QPen(QBrush(RasterColor, Qt::SolidPattern), tracewidth, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));

    if(devparms->displaygrid == 2)
    {
      step = curve_w / 14.0;

      for(i=1; i<14; i++)
      {
        painter->drawLine(step * i, curve_h - 1, step * i, 0);
      }

      step = curve_h / 8.0;

      for(i=1; i<8; i++)
      {
        painter->drawLine(0, step * i, curve_w - 1, step * i);
      }
    }
    else
    {
      painter->drawLine(curve_w / 2, curve_h - 1, curve_w / 2, 0);

      painter->drawLine(0, curve_h / 2, curve_w - 1, curve_h / 2);
    }
  }

  painter->setPen(RasterColor);

  step = curve_w / 70.0;

  for(i=1; i<70; i++)
  {
    step2 = step * i;

    if(devparms->displaygrid)
    {
      painter->drawLine(step2, curve_h / 2 + 2, step2, curve_h / 2 - 2);
    }

    if(i % 5)
    {
      painter->drawLine(step2, curve_h - 1, step2, curve_h - 5);

      painter->drawLine(step2, 0, step2, 4);
    }
    else
    {
      painter->drawLine(step2, curve_h - 1, step2, curve_h - 9);

      painter->drawLine(step2, 0, step2, 8);
    }
  }

  step = curve_h / 40.0;

  for(i=1; i<40; i++)
  {
    step2 = step * i;

    if(devparms->displaygrid)
    {
      painter->drawLine(curve_w / 2 + 2, step2, curve_w / 2  - 2, step2);
    }

    if(i % 5)
    {
      painter->drawLine(curve_w - 1, step2, curve_w - 5, step2);

      painter->drawLine(0, step2, 4, step2);
    }
    else
    {
      painter->drawLine(curve_w - 1, step2, curve_w - 9, step2);

      painter->drawLine(0, step2, 8, step2);
    }
  }

  if(devparms == NULL)
  {
    return;
  }

  offset = -127;

  v_sense = -((double)curve_h / 256.0);

  h_sense = -((double)curve_w / 256.0);

/////////////////////////////////// draw the arrows ///////////////////////////////////////////

  drawTrigCenterArrow(painter, curve_w / 2, 0);

  for(chn=0; chn<devparms->channel_cnt; chn++)
  {
    if(!devparms->chandisplay[chn])
    {
      continue;
    }

    if(chan_arrow_moving[chn])
    {
      drawArrow(painter, 0, chan_arrow_pos[chn], 0, SignalColor[chn], '1' + chn);
    }
    else
    {
      chan_arrow_pos[chn] = (-128.0 + ((devparms->chanoffset[chn] / devparms->chanscale[chn]) * 32.0)) * v_sense;

      if(chan_arrow_pos[chn] < 0)
      {
        chan_arrow_pos[chn] = -1;

        drawArrow(painter, -6, chan_arrow_pos[chn], 3, SignalColor[chn], '1' + chn);
      }
      else if(chan_arrow_pos[chn] > curve_h)
        {
          chan_arrow_pos[chn] = curve_h + 1;

          drawArrow(painter, -6, chan_arrow_pos[chn], 1, SignalColor[chn], '1' + chn);
        }
        else
        {
          drawArrow(painter, 0, chan_arrow_pos[chn], 0, SignalColor[chn], '1' + chn);
        }
    }
  }

/////////////////////////////////// draw the curve ///////////////////////////////////////////

  if(bufsize < 2)  return;

  painter->setClipping(true);
  painter->setClipRegion(QRegion(0, 0, curve_w, curve_h), Qt::ReplaceClip);

  h_step = (double)curve_w / (double)bufsize;

  for(chn=0; chn<devparms->channel_cnt; chn++)
  {
    if(!devparms->chandisplay[chn])
    {
      continue;
    }

    painter->setPen(QPen(QBrush(SignalColor[chn], Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    for(i=0; i<bufsize; i++)
    {
      if(bufsize < (curve_w / 2))
      {
        painter->drawLine(i * h_step, ((double)(devparms->wavebuf[chn][i]) + offset) * v_sense, (i + 1) * h_step, ((double)(devparms->wavebuf[chn][i]) + offset) * v_sense);
        if(i)
        {
          painter->drawLine(i * h_step, ((double)(devparms->wavebuf[chn][i - 1]) + offset) * v_sense, i * h_step, ((double)(devparms->wavebuf[chn][i]) + offset) * v_sense);
        }
      }
      else
      {
        if(i < (bufsize - 1))
        {
          if(devparms->displaytype)
          {
            painter->drawPoint(i * h_step, ((double)(devparms->wavebuf[chn][i]) + offset) * v_sense);
          }
          else
          {
            painter->drawLine(i * h_step, ((double)(devparms->wavebuf[chn][i]) + offset) * v_sense, (i + 1) * h_step, ((double)(devparms->wavebuf[chn][i + 1]) + offset) * v_sense);
          }
        }
      }
    }
  }

  painter->setClipping(false);

/////////////////////////////////// draw the trigger arrows ///////////////////////////////////////////

  if(trig_level_arrow_moving)
  {
    drawArrow(painter, curve_w, trig_level_arrow_pos, 2, QColor(255, 128, 0), 'T');

    painter->setPen(QPen(QBrush(QColor(255, 128, 0), Qt::SolidPattern), tracewidth, Qt::DashDotLine, Qt::SquareCap, Qt::BevelJoin));

    painter->drawLine(1, trig_level_arrow_pos, curve_w - 2, trig_level_arrow_pos);
  }
  else
  {
    if(devparms->triggeredgesource < 4)
    {
      trig_level_arrow_pos = (-128.0 + (((devparms->triggeredgelevel[devparms->triggeredgesource] + devparms->chanoffset[devparms->triggeredgesource]) / devparms->chanscale[devparms->triggeredgesource]) * 32.0)) * v_sense;

      if(trig_level_arrow_pos < 0)
      {
        trig_level_arrow_pos = -1;

        drawArrow(painter, curve_w + 6, trig_level_arrow_pos, 3, QColor(255, 128, 0), 'T');
      }
      else if(trig_level_arrow_pos > curve_h)
        {
          trig_level_arrow_pos = curve_h + 1;

          drawArrow(painter, curve_w + 6, trig_level_arrow_pos, 1, QColor(255, 128, 0), 'T');
        }
        else
        {
          drawArrow(painter, curve_w, trig_level_arrow_pos, 2, QColor(255, 128, 0), 'T');
        }
    }

    if(trig_line_visible)
    {
      painter->setPen(QPen(QBrush(QColor(255, 128, 0), Qt::SolidPattern), tracewidth, Qt::DashDotLine, Qt::SquareCap, Qt::BevelJoin));

      painter->drawLine(1, trig_level_arrow_pos, curve_w - 2, trig_level_arrow_pos);
    }
  }

  if(trig_pos_arrow_moving)
  {
    drawArrow(painter, trig_pos_arrow_pos, 27, 1, QColor(255, 128, 0), 'T');
  }
  else
  {
    if(devparms->timebasedelayenable)
    {
      trig_pos_arrow_pos = (curve_w / 2) - ((devparms->timebasedelayoffset / (devparms->timebasedelayscale * 14.0)) * curve_w);
    }
    else
    {
      trig_pos_arrow_pos = (curve_w / 2) - ((devparms->timebaseoffset / (devparms->timebasescale * 14.0)) * curve_w);
    }

    if(trig_pos_arrow_pos < 0)
    {
      trig_pos_arrow_pos = -1;

      drawArrow(painter, trig_pos_arrow_pos, 18, 2, QColor(255, 128, 0), 'T');
    }
    else if(trig_pos_arrow_pos > curve_w)
      {
        trig_pos_arrow_pos = curve_w + 1;

        drawArrow(painter, trig_pos_arrow_pos, 18, 0, QColor(255, 128, 0), 'T');
      }
      else
      {
        drawArrow(painter, trig_pos_arrow_pos, 27, 1, QColor(255, 128, 0), 'T');
      }
  }

  if(devparms->countersrc)
  {
    paintCounterLabel(painter, curve_w - 150, 6);
  }

  char str[128];

  if(mainwindow->adjDialFunc == NAV_DIAL_FUNC_HOLDOFF)
  {
    convert_to_metric_suffix(str, devparms->triggerholdoff, 2);

    strcat(str, "S");

    paintLabel(painter, curve_w - 110, 5, 100, 20, str);
  }
  else if(mainwindow->adjDialFunc == ADJ_DIAL_FUNC_ACQ_AVG)
    {
      sprintf(str, "%i", devparms->acquireaverages);

      paintLabel(painter, curve_w - 110, 5, 100, 20, str);
    }

//   clk_end = clock();
//
//   cpu_time_used += ((double) (clk_end - clk_start)) / CLOCKS_PER_SEC;
//
//   scr_update_cntr++;
//
//   if(!(scr_update_cntr % 50))
//   {
//     printf("CPU time used: %f\n", cpu_time_used / 50);
//
//     cpu_time_used = 0;
//   }
}


void SignalCurve::drawCurve(struct device_settings *devp, struct tmcdev *dev, int bsize)
{
  devparms = devp;

  device = dev;

  bufsize = bsize;

  update();
}


void SignalCurve::setDeviceParameters(struct device_settings *devp)
{
  devparms = devp;
}


void SignalCurve::drawTopLabels(QPainter *painter)
{
  int i, x1;

  char str[128];

  double dtmp1, dtmp2;

  QPainterPath path;

  if(devparms == NULL)
  {
    return;
  }

  path.addRoundedRect(5, 5, 70, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::white);

  painter->drawText(5, 5, 70, 20, Qt::AlignCenter, devparms->modelname);

//////////////// triggerstatus ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(80, 5, 35, 20, 3, 3);

  if((devparms->triggerstatus == 1) || (devparms->triggerstatus == 3))
  {
    if(!trig_stat_flash)
    {
      trig_stat_flash = 1;

      trig_stat_timer->start(1000);
    }
  }
  else
  {
    if(trig_stat_flash)
    {
      trig_stat_flash = 0;

      trig_stat_timer->stop();
    }
  }

  if(trig_stat_flash == 2)
  {
    painter->fillPath(path, Qt::green);

    painter->setPen(Qt::black);
  }
  else
  {
    painter->fillPath(path, Qt::black);

    painter->setPen(Qt::green);
  }

  switch(devparms->triggerstatus)
  {
    case 0 : painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "T'D");
             break;
    case 1 : painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "WAIT");
             break;
    case 2 : painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "RUN");
             break;
    case 3 : painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "AUTO");
             break;
    case 4 : painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "FIN");
             break;
    case 5 : painter->setPen(Qt::red);
             painter->drawText(80, 5, 35, 20, Qt::AlignCenter, "STOP");
             break;
  }

//////////////// horizontal ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(140, 5, 70, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::white);

  painter->drawText(125, 20, "H");

  if(devparms->timebasedelayenable)
  {
    convert_to_metric_suffix(str, devparms->timebasedelayscale, 1);
  }
  else
  {
    convert_to_metric_suffix(str, devparms->timebasescale, 1);
  }

  strcat(str, "s");

  painter->drawText(140, 5, 70, 20, Qt::AlignCenter, str);

//////////////// samplerate ///////////////////////////////

  painter->setPen(Qt::gray);

  convert_to_metric_suffix(str, devparms->samplerate, 3);

  strcat(str, "Sa/s");

  painter->drawText(200, -1, 85, 20, Qt::AlignCenter, str);

  convert_to_metric_suffix(str, devparms->memdepth, 1);

  strcat(str, "pts");

  painter->drawText(200, 14, 85, 20, Qt::AlignCenter, str);

//////////////// memory position ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(285, 5, 240, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::gray);

  if(devparms->timebasedelayenable)
  {
    dtmp1 = devparms->timebasedelayscale / devparms->timebasescale;

    dtmp2 = (devparms->timebaseoffset - devparms->timebasedelayoffset) / (7 * devparms->timebasescale);

    painter->fillRect(288, 16, (116 - (dtmp1 * 116)) - (dtmp2 * 116), 8, QColor(64, 160, 255));

    x1 = (116 - (dtmp1 * 116)) + (dtmp2 * 116);

    painter->fillRect(288 + 233 - x1, 16, x1, 8, QColor(64, 160, 255));
  }

  painter->drawRect(288, 16, 233, 8);

  painter->setPen(Qt::white);

  painter->drawLine(289, 20, 291, 22);

  for(i=0; i<19; i++)
  {
    painter->drawLine((i * 12) + 291, 22, (i * 12) + 293, 22);

    painter->drawLine((i * 12) + 297, 18, (i * 12) + 299, 18);

    painter->drawLine((i * 12) + 294, 21, (i * 12) + 296, 19);

    painter->drawLine((i * 12) + 300, 19, (i * 12) + 302, 21);
  }

  painter->drawLine(519, 22, 520, 22);

//////////////// delay ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(570, 5, 85, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(QColor(255, 128, 0));

  painter->drawText(555, 20, "D");

  if(devparms->timebasedelayenable)
  {
    convert_to_metric_suffix(str, devparms->timebasedelayoffset, 4);
  }
  else
  {
    convert_to_metric_suffix(str, devparms->timebaseoffset, 4);
  }

  strcat(str, "s");

  painter->drawText(570, 5, 85, 20, Qt::AlignCenter, str);

//////////////// trigger ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(685, 5, 125, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::gray);

  painter->drawText(670, 20, "T");

  convert_to_metric_suffix(str, devparms->triggeredgelevel[devparms->triggeredgesource], 2);

  strcat(str, "V");

  if(devparms->triggeredgesource < 4)
  {
    painter->setPen(SignalColor[devparms->triggeredgesource]);
  }
  else
  {
    switch(devparms->triggeredgesource)
    {
      case 4:
      case 5: painter->setPen(Qt::green);
              break;
      case 6: painter->setPen(QColor(255, 64, 0));
              break;
    }
  }

  if(devparms->triggeredgesource != 6)
  {
    painter->drawText(735, 5, 85, 20, Qt::AlignCenter, str);
  }

  path = QPainterPath();

  path.addRoundedRect(725, 7, 15, 15, 3, 3);

  if(devparms->triggeredgesource < 4)
  {
    painter->fillPath(path, SignalColor[devparms->triggeredgesource]);

    sprintf(str, "%i", devparms->triggeredgesource + 1);
  }
  else
  {
    switch(devparms->triggeredgesource)
    {
      case 4:
      case 5: painter->fillPath(path, Qt::green);
              strcpy(str, "E");
              break;
      case 6: painter->fillPath(path, QColor(255, 64, 0));
              strcpy(str, "AC");
              break;
    }
  }

  if(devparms->triggeredgeslope == 0)
  {
    painter->drawLine(705, 8, 710, 8);
    painter->drawLine(705, 8, 705, 20);
    painter->drawLine(700, 20, 705, 20);
    painter->drawLine(701, 15, 705, 11);
    painter->drawLine(709, 15, 705, 11);
  }

  if(devparms->triggeredgeslope == 1)
  {
    painter->drawLine(700, 8, 705, 8);
    painter->drawLine(705, 8, 705, 20);
    painter->drawLine(705, 20, 710, 20);
    painter->drawLine(701, 12, 705, 16);
    painter->drawLine(709, 12, 705, 16);
  }

  if(devparms->triggeredgeslope == 2)
  {
    painter->drawLine(702, 8, 702, 18);
    painter->drawLine(700, 10, 702, 8);
    painter->drawLine(704, 10, 702, 8);
    painter->drawLine(708, 8, 708, 18);
    painter->drawLine(706, 16, 708, 18);
    painter->drawLine(710, 16, 708, 18);
  }

  painter->setPen(Qt::black);

  painter->drawText(725, 8, 15, 15, Qt::AlignCenter, str);
}


void SignalCurve::drawChanLabel(QPainter *painter, int xpos, int ypos, int chn)
{
  QPainterPath path;

  char str1[4],
       str2[128];

  if(devparms == NULL)
  {
    return;
  }

  str1[0] = '1' + chn;
  str1[1] = 0;

  convert_to_metric_suffix(str2, devparms->chanscale[chn], 2);

  strcat(str2, "V");

  if(devparms->chandisplay[chn])
  {
    if(chn == devparms->activechannel)
    {
      path.addRoundedRect(xpos, ypos, 20, 20, 3, 3);

      painter->fillPath(path, SignalColor[chn]);

      painter->setPen(Qt::black);

      painter->drawText(xpos + 6, ypos + 15, str1);

      path = QPainterPath();

      path.addRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

      painter->fillPath(path, Qt::black);

      painter->setPen(SignalColor[chn]);

      painter->drawRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

      painter->drawText(xpos + 25, ypos + 1, 85, 20, Qt::AlignCenter, str2);

      if(devparms->chanbwlimit[chn])
      {
        painter->drawText(xpos + 90, ypos + 1, 20, 20, Qt::AlignCenter, "B");
      }

      if(devparms->chancoupling[chn] == 0)
      {
        painter->drawLine(xpos + 33, ypos + 6, xpos + 33, ypos + 10);

        painter->drawLine(xpos + 28, ypos + 10, xpos + 38, ypos + 10);

        painter->drawLine(xpos + 30, ypos + 12, xpos + 36, ypos + 12);

        painter->drawLine(xpos + 32, ypos + 14, xpos + 34, ypos + 14);
      }
      else if(devparms->chancoupling[chn] == 1)
        {
          painter->drawLine(xpos + 28, ypos + 8, xpos + 38, ypos + 8);

          painter->drawLine(xpos + 28, ypos + 12, xpos + 30, ypos + 12);

          painter->drawLine(xpos + 32, ypos + 12, xpos + 34, ypos + 12);

          painter->drawLine(xpos + 36, ypos + 12, xpos + 38, ypos + 12);
        }
        else if(devparms->chancoupling[chn] == 2)
          {
            painter->drawArc(xpos + 30, ypos + 8, 5, 5, 10 * 16, 160 * 16);

            painter->drawArc(xpos + 35, ypos + 8, 5, 5, -10 * 16, -160 * 16);
          }
    }
    else
    {
      path.addRoundedRect(xpos, ypos, 20, 20, 3, 3);

      path.addRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

      painter->fillPath(path, Qt::black);

      painter->setPen(SignalColor[chn]);

      painter->drawText(xpos + 6, ypos + 15, str1);

      painter->drawText(xpos + 25, ypos + 1, 85, 20, Qt::AlignCenter, str2);

      if(devparms->chanbwlimit[chn])
      {
        painter->drawText(xpos + 90, ypos + 1, 20, 20, Qt::AlignCenter, "B");
      }

      if(devparms->chancoupling[chn] == 0)
      {
        painter->drawLine(xpos + 33, ypos + 6, xpos + 33, ypos + 10);

        painter->drawLine(xpos + 28, ypos + 10, xpos + 38, ypos + 10);

        painter->drawLine(xpos + 30, ypos + 12, xpos + 36, ypos + 12);

        painter->drawLine(xpos + 32, ypos + 14, xpos + 34, ypos + 14);
      }
      else if(devparms->chancoupling[chn] == 1)
        {
          painter->drawLine(xpos + 28, ypos + 8, xpos + 38, ypos + 8);

          painter->drawLine(xpos + 28, ypos + 12, xpos + 30, ypos + 12);

          painter->drawLine(xpos + 32, ypos + 12, xpos + 34, ypos + 12);

          painter->drawLine(xpos + 36, ypos + 12, xpos + 38, ypos + 12);
        }
        else if(devparms->chancoupling[chn] == 2)
          {
            painter->drawArc(xpos + 30, ypos + 8, 5, 5, 10 * 16, 160 * 16);

            painter->drawArc(xpos + 35, ypos + 8, 5, 5, -10 * 16, -160 * 16);
          }
    }
  }
  else
  {
    path.addRoundedRect(xpos, ypos, 20, 20, 3, 3);

    path.addRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

    painter->fillPath(path, Qt::black);

    painter->setPen(QColor(48, 48, 48));

    painter->drawText(xpos + 6, ypos + 15, str1);

    painter->drawText(xpos + 25, ypos + 1, 85, 20, Qt::AlignCenter, str2);

    if(devparms->chanbwlimit[chn])
    {
      painter->drawText(xpos + 90, ypos + 1, 20, 20, Qt::AlignCenter, "B");
    }

    if(devparms->chancoupling[chn] == 0)
    {
      painter->drawLine(xpos + 33, ypos + 6, xpos + 33, ypos + 10);

      painter->drawLine(xpos + 28, ypos + 10, xpos + 38, ypos + 10);

      painter->drawLine(xpos + 30, ypos + 12, xpos + 36, ypos + 12);

      painter->drawLine(xpos + 32, ypos + 14, xpos + 34, ypos + 14);
    }
    else if(devparms->chancoupling[chn] == 1)
      {
        painter->drawLine(xpos + 28, ypos + 8, xpos + 38, ypos + 8);

        painter->drawLine(xpos + 28, ypos + 12, xpos + 30, ypos + 12);

        painter->drawLine(xpos + 32, ypos + 12, xpos + 34, ypos + 12);

        painter->drawLine(xpos + 36, ypos + 12, xpos + 38, ypos + 12);
      }
      else if(devparms->chancoupling[chn] == 2)
        {
          painter->drawArc(xpos + 30, ypos + 8, 5, 5, 10 * 16, 160 * 16);

          painter->drawArc(xpos + 35, ypos + 8, 5, 5, -10 * 16, -160 * 16);
        }
  }
}


void SignalCurve::drawArrow(QPainter *painter, int xpos, int ypos, int rot, QColor color, char ch)
{
  QPainterPath path;

  char str[4];

  str[0] = ch;
  str[1] = 0;

  if(rot == 0)
  {
    path.moveTo(xpos - 20, ypos + 6);
    path.lineTo(xpos - 7,  ypos + 6);
    path.lineTo(xpos,     ypos);
    path.lineTo(xpos - 7,  ypos - 6);
    path.lineTo(xpos - 20, ypos - 6);
    path.lineTo(xpos - 20, ypos + 6);
    painter->fillPath(path, color);

    painter->setPen(Qt::black);

    painter->drawText(xpos - 17, ypos + 4, str);
  }
  else if(rot == 1)
    {
      path.moveTo(xpos + 6, ypos - 20);
      path.lineTo(xpos + 6,  ypos - 7);
      path.lineTo(xpos,     ypos);
      path.lineTo(xpos - 6,  ypos - 7);
      path.lineTo(xpos - 6, ypos - 20);
      path.lineTo(xpos + 6, ypos - 20);
      painter->fillPath(path, color);

      painter->setPen(Qt::black);

      painter->drawText(xpos - 3, ypos - 7, str);
    }
    else if(rot == 2)
      {
        path.moveTo(xpos + 20, ypos + 6);
        path.lineTo(xpos + 7,  ypos + 6);
        path.lineTo(xpos,     ypos);
        path.lineTo(xpos + 7,  ypos - 6);
        path.lineTo(xpos + 20, ypos - 6);
        path.lineTo(xpos + 20, ypos + 6);
        painter->fillPath(path, color);

        painter->setPen(Qt::black);

        painter->drawText(xpos + 9, ypos + 4, str);
      }
      else if(rot == 3)
        {
          path.moveTo(xpos + 6, ypos + 20);
          path.lineTo(xpos + 6,  ypos + 7);
          path.lineTo(xpos,     ypos);
          path.lineTo(xpos - 6,  ypos + 7);
          path.lineTo(xpos - 6, ypos + 20);
          path.lineTo(xpos + 6, ypos + 20);
          painter->fillPath(path, color);

          painter->setPen(Qt::black);

          painter->drawText(xpos - 3, ypos + 16, str);
        }
}


void SignalCurve::drawSmallTriggerArrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
{
  QPainterPath path;

  if(rot == 0)
  {
    path.moveTo(xpos - 13, ypos - 5);
    path.lineTo(xpos - 5,  ypos - 5);
    path.lineTo(xpos,     ypos);
    path.lineTo(xpos - 5,  ypos + 5);
    path.lineTo(xpos - 13, ypos + 5);
    path.lineTo(xpos - 13, ypos - 5);

    painter->fillPath(path, color);

    painter->setPen(Qt::black);

    painter->drawLine(xpos - 10, ypos - 4, xpos - 6, ypos - 4);

    painter->drawLine(xpos - 8, ypos - 4, xpos - 8, ypos + 4);
  }
  else if(rot == 1)
    {
      path.moveTo(xpos + 5, ypos - 10);
      path.lineTo(xpos + 5,  ypos - 5);
      path.lineTo(xpos,     ypos);
      path.lineTo(xpos - 4,  ypos - 5);
      path.lineTo(xpos - 4, ypos - 10);
      path.lineTo(xpos + 5, ypos - 10);

      painter->fillPath(path, color);

      painter->setPen(Qt::black);

      painter->drawLine(xpos - 2, ypos - 8, xpos + 2, ypos - 8);

      painter->drawLine(xpos, ypos - 8, xpos, ypos - 3);
    }
    else if(rot == 2)
      {
        path.moveTo(xpos + 12, ypos - 5);
        path.lineTo(xpos + 5,  ypos - 5);
        path.lineTo(xpos,     ypos);
        path.lineTo(xpos + 5,  ypos + 5);
        path.lineTo(xpos + 12, ypos + 5);
        path.lineTo(xpos + 12, ypos - 5);

        painter->fillPath(path, color);

        painter->setPen(Qt::black);

        painter->drawLine(xpos + 9, ypos - 4, xpos + 5, ypos - 4);

        painter->drawLine(xpos + 7, ypos - 4, xpos + 7, ypos + 4);
      }
}


void SignalCurve::drawTrigCenterArrow(QPainter *painter, int xpos, int ypos)
{
  QPainterPath path;

  path.moveTo(xpos + 7, ypos);
  path.lineTo(xpos - 6, ypos);
  path.lineTo(xpos, ypos + 7);
  path.lineTo(xpos + 7, ypos);
  painter->fillPath(path, QColor(255, 128, 0));
}


void SignalCurve::setSignalColor1(QColor newColor)
{
  SignalColor[0] = newColor;
  update();
}


void SignalCurve::setSignalColor2(QColor newColor)
{
  SignalColor[1] = newColor;
  update();
}


void SignalCurve::setSignalColor3(QColor newColor)
{
  SignalColor[2] = newColor;
  update();
}


void SignalCurve::setSignalColor4(QColor newColor)
{
  SignalColor[3] = newColor;
  update();
}


void SignalCurve::setTraceWidth(int tr_width)
{
  tracewidth = tr_width;
  if(tracewidth < 0)  tracewidth = 0;
  update();
}


void SignalCurve::setBackgroundColor(QColor newColor)
{
  BackgroundColor = newColor;
  update();
}


void SignalCurve::setRasterColor(QColor newColor)
{
  RasterColor = newColor;
  update();
}


void SignalCurve::setBorderSize(int newsize)
{
  bordersize = newsize;
  if(bordersize < 0)  bordersize = 0;
  update();
}


void SignalCurve::mousePressEvent(QMouseEvent *press_event)
{
  int chn,
      m_x,
      m_y;

  setFocus(Qt::MouseFocusReason);

  w = width() - (2 * bordersize);
  h = height() - (2 * bordersize);

  m_x = press_event->x() - bordersize;
  m_y = press_event->y() - bordersize;

  if((devparms == NULL) || (!devparms->connected))
  {
    return;
  }

//  printf("m_x: %i   m_y: %i   trig_pos_arrow_pos: %i\n",m_x, m_y, trig_pos_arrow_pos);

  if(press_event->button()==Qt::LeftButton)
  {
    if((m_x > (trig_pos_arrow_pos - 8)) && (m_x < (trig_pos_arrow_pos + 8)) && (m_y > 5) && (m_y < 24))
    {
      trig_pos_arrow_moving = 1;
      use_move_events = 1;
      setMouseTracking(true);
      mouse_old_x = m_x;
      mouse_old_y = m_y;
    }
    else if((m_x > w) && (m_x < (w + 26)) && (m_y > (trig_level_arrow_pos - 7)) && (m_y < (trig_level_arrow_pos + 7)))
      {
        trig_level_arrow_moving = 1;
        use_move_events = 1;
        trig_line_visible = 1;
        setMouseTracking(true);
        mouse_old_x = m_x;
        mouse_old_y = m_y;
      }
      else
      {
        for(chn=0; chn<devparms->channel_cnt; chn++)
        {
          if(!devparms->chandisplay[chn])
          {
            continue;
          }

          if(m_x > -26 && (m_x < 0) && (m_y > (chan_arrow_pos[chn] - 7)) && (m_y < (chan_arrow_pos[chn] + 7)))
          {
            chan_arrow_moving[chn] = 1;
            devparms->activechannel = chn;
            use_move_events = 1;
            setMouseTracking(true);
            mouse_old_x = m_x;
            mouse_old_y = m_y;

            break;
          }
        }
      }
  }
}


void SignalCurve::mouseReleaseEvent(QMouseEvent *release_event)
{
  int chn, tmp;

  char str[512];

  double lefttime, righttime, delayrange;

  w = width() - (2 * bordersize);
  h = height() - (2 * bordersize);

  mouse_x = release_event->x() - bordersize;
  mouse_y = release_event->y() - bordersize;

  if((devparms == NULL) || (!devparms->connected))
  {
    return;
  }

  if(trig_pos_arrow_moving)
  {
    trig_pos_arrow_pos = mouse_x;

    if(trig_pos_arrow_pos < 0)
    {
      trig_pos_arrow_pos = 0;
    }

    if(trig_pos_arrow_pos > w)
    {
      trig_pos_arrow_pos = w;
    }

//    printf("w is %i   trig_pos_arrow_pos is %i\n", w, trig_pos_arrow_pos);

    if(devparms->timebasedelayenable)
    {
      devparms->timebasedelayoffset = ((devparms->timebasedelayscale * 14.0) / w) * ((w / 2) - trig_pos_arrow_pos);

      tmp = devparms->timebasedelayoffset / (devparms->timebasedelayscale / 50);

      devparms->timebasedelayoffset = (devparms->timebasedelayscale / 50) * tmp;

      lefttime = (7 * devparms->timebasescale) - devparms->timebaseoffset;

      righttime = (7 * devparms->timebasescale) + devparms->timebaseoffset;

      delayrange = 7 * devparms->timebasedelayscale;

      if(devparms->timebasedelayoffset < -(lefttime - delayrange))
      {
        devparms->timebasedelayoffset = -(lefttime - delayrange);
      }

      if(devparms->timebasedelayoffset > (righttime - delayrange))
      {
        devparms->timebasedelayoffset = (righttime - delayrange);
      }

      strcpy(str, "Delayed timebase position: ");

      convert_to_metric_suffix(str + strlen(str), devparms->timebasedelayoffset, 2);

      strcat(str, "s");

      mainwindow->statusLabel->setText(str);

      sprintf(str, ":TIM:DEL:OFFS %e", devparms->timebasedelayoffset);

      tmcdev_write(device, str);
    }
    else
    {
      devparms->timebaseoffset = ((devparms->timebasescale * 14.0) / w) * ((w / 2) - trig_pos_arrow_pos);

      tmp = devparms->timebaseoffset / (devparms->timebasescale / 50);

      devparms->timebaseoffset = (devparms->timebasescale / 50) * tmp;

      strcpy(str, "Horizontal position: ");

      convert_to_metric_suffix(str + strlen(str), devparms->timebaseoffset, 2);

      strcat(str, "s");

      mainwindow->statusLabel->setText(str);

      sprintf(str, ":TIM:OFFS %e", devparms->timebaseoffset);

      tmcdev_write(device, str);
    }
  }
  else if(trig_level_arrow_moving)
    {
      if(devparms->triggeredgesource > 3)
      {
        return;
      }

      trig_level_arrow_pos = mouse_y;

      if(trig_level_arrow_pos < 0)
      {
        trig_level_arrow_pos = 0;
      }

      if(trig_level_arrow_pos > h)
      {
        trig_level_arrow_pos = h;
      }

  //       printf("chanoffset[chn] is: %e   chanscale[chn] is %e   trig_level_arrow_pos is: %i   v_sense is: %e\n",
  //              devparms->chanoffset[chn], devparms->chanscale[chn], trig_level_arrow_pos, v_sense);

      devparms->triggeredgelevel[devparms->triggeredgesource] = ((devparms->chanscale[devparms->triggeredgesource] / 32.0) * (128 + (trig_level_arrow_pos / v_sense)))
                                                                - devparms->chanoffset[devparms->triggeredgesource];

      tmp = devparms->triggeredgelevel[devparms->triggeredgesource] / (devparms->chanscale[devparms->triggeredgesource] / 50);

      devparms->triggeredgelevel[devparms->triggeredgesource] = (devparms->chanscale[devparms->triggeredgesource] / 50) * tmp;

      sprintf(str, "Trigger level: ");

      convert_to_metric_suffix(str + strlen(str), devparms->triggeredgelevel[devparms->triggeredgesource], 2);

      strcat(str, "V");

      mainwindow->statusLabel->setText(str);

      sprintf(str, ":TRIG:EDG:LEV %e", devparms->triggeredgelevel[devparms->triggeredgesource]);

      if(device != NULL)
      {
        tmcdev_write(device, str);
      }

      trig_line_timer->start(1000);
    }
    else
    {
      for(chn=0; chn<devparms->channel_cnt; chn++)
      {
        if(!devparms->chandisplay[chn])
        {
          continue;
        }

        if(chan_arrow_moving[chn])
        {
          chan_arrow_pos[chn] = mouse_y;

          if(chan_arrow_pos[chn] < 0)
          {
            chan_arrow_pos[chn] = 0;
          }

          if(chan_arrow_pos[chn] > h)
          {
            chan_arrow_pos[chn] = h;
          }

    //       printf("chanoffset[chn] is: %e   chanscale[chn] is %e   chan_arrow_pos[chn] is: %i   v_sense is: %e\n",
    //              devparms->chanoffset[chn], devparms->chanscale[chn], chan_arrow_pos[chn], v_sense);

          devparms->chanoffset[chn] = (devparms->chanscale[chn] / 32.0) * (128 + (chan_arrow_pos[chn] / v_sense));

          tmp = devparms->chanoffset[chn] / (devparms->chanscale[chn] / 50);

          devparms->chanoffset[chn] = (devparms->chanscale[chn] / 50) * tmp;

          sprintf(str, "Channel %i offset: ", chn + 1);

          convert_to_metric_suffix(str + strlen(str), devparms->chanoffset[chn], 2);

          strcat(str, "V");

          mainwindow->statusLabel->setText(str);

          sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms->chanoffset[chn]);

          if(device != NULL)
          {
            tmcdev_write(device, str);
          }

          devparms->activechannel = chn;

          break;
        }
      }
    }

  for(chn=0; chn<MAX_CHNS; chn++)
  {
    chan_arrow_moving[chn] = 0;
  }
  trig_level_arrow_moving = 0;
  trig_pos_arrow_moving = 0;
  use_move_events = 0;
  setMouseTracking(false);
}


void SignalCurve::mouseMoveEvent(QMouseEvent *move_event)
{
  int chn;

  if(!use_move_events)
  {
    return;
  }

  mouse_x = move_event->x() - bordersize;
  mouse_y = move_event->y() - bordersize;

  if((devparms == NULL) || (!devparms->connected))
  {
    return;
  }

  if(trig_pos_arrow_moving)
  {
    trig_pos_arrow_pos = mouse_x;

    if(trig_pos_arrow_pos < 0)
    {
      trig_pos_arrow_pos = 0;
    }

    if(trig_pos_arrow_pos > w)
    {
      trig_pos_arrow_pos = w;
    }
  }
  else if(trig_level_arrow_moving)
    {
      trig_level_arrow_pos = mouse_y;

      if(trig_level_arrow_pos < 0)
      {
        trig_level_arrow_pos = 0;
      }

      if(trig_level_arrow_pos > h)
      {
        trig_level_arrow_pos = h;
      }
    }
    else
    {
      for(chn=0; chn<devparms->channel_cnt; chn++)
      {
        if(!devparms->chandisplay[chn])
        {
          continue;
        }

        if(chan_arrow_moving[chn])
        {
          chan_arrow_pos[chn] = mouse_y;

          if(chan_arrow_pos[chn] < 0)
          {
            chan_arrow_pos[chn] = 0;
          }

          if(chan_arrow_pos[chn] > h)
          {
            chan_arrow_pos[chn] = h;
          }

          break;
        }
      }
    }

  update();
}


void SignalCurve::trig_line_timer_handler()
{
  trig_line_visible = 0;
}


void SignalCurve::setTrigLineVisible(void)
{
  trig_line_visible = 1;

  trig_line_timer->start(1000);
}


void SignalCurve::trig_stat_timer_handler()
{
  if(!trig_stat_flash)
  {
    trig_stat_timer->stop();

    return;
  }

  if(trig_stat_flash == 1)
  {
    trig_stat_flash = 2;
  }
  else
  {
    trig_stat_flash = 1;
  }
}


void SignalCurve::paintLabel(QPainter *painter, int xpos, int ypos, int xw, int yh, const char *str)
{
  QPainterPath path;

  path.addRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::white);

  painter->drawRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->drawText(xpos, ypos, xw, yh, Qt::AlignCenter, str);
}


void SignalCurve::paintCounterLabel(QPainter *painter, int xpos, int ypos)
{
  char str[128];

  QPainterPath path;

  path.addRoundedRect(xpos, ypos, 122, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::darkGray);

  painter->drawRoundedRect(xpos, ypos, 122, 20, 3, 3);

  path = QPainterPath();

  path.addRoundedRect(xpos + 4, ypos + 3, 14, 14, 3, 3);

  painter->fillPath(path, SignalColor[devparms->countersrc - 1]);

  painter->setPen(Qt::black);

  painter->drawLine(xpos + 7, ypos + 6, xpos + 15, ypos + 6);

  painter->drawLine(xpos + 11, ypos + 6, xpos + 11, ypos + 14);

  painter->setPen(Qt::white);

  if((devparms->counterfreq < 15) || (devparms->counterfreq > 1.1e9))
  {
    strcpy(str, "< 15 Hz");
  }
  else
  {
    convert_to_metric_suffix(str, devparms->counterfreq, 4);

    strcat(str, "Hz");
  }

  painter->drawText(xpos + 22, ypos, 100, 20, Qt::AlignCenter, str);
}





