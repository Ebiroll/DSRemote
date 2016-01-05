/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016 Teunis van Beelen
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

#if QT_VERSION >= 0x050000
  trig_line_timer->setTimerType(Qt::PreciseTimer);
  trig_stat_timer->setTimerType(Qt::PreciseTimer);
#endif

  bufsize = 0;
  bordersize = 60;

  v_sense = 1;

  mouse_x = 0;
  mouse_y = 0;
  mouse_old_x = 0;
  mouse_old_y = 0;

  label_active = LABEL_ACTIVE_NONE;

  for(i=0; i<MAX_CHNS; i++)
  {
    chan_arrow_moving[i] = 0;

    chan_arrow_pos[i] = 127;
  }

  trig_level_arrow_moving = 0;

  trig_pos_arrow_moving = 0;

  fft_arrow_pos = 0;

  fft_arrow_moving = 0;

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
#if QT_VERSION >= 0x050000
    paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif

    drawWidget(&paint, width(), height());

    old_w = width();
  }
}


void SignalCurve::drawWidget(QPainter *painter, int curve_w, int curve_h)
{
  int i, chn, tmp, rot=1, small_rulers, curve_w_backup, curve_h_backup;

  char str[1024];

  double h_step=0.0,
         step,
         step2;

//  clk_start = clock();

  if(devparms == NULL)
  {
    return;
  }

  curve_w_backup = curve_w;

  curve_h_backup = curve_h;

  small_rulers = 5 * devparms->hordivisions;

  painter->fillRect(0, 0, curve_w, curve_h, BackgroundColor);

  if((curve_w < ((bordersize * 2) + 5)) || (curve_h < ((bordersize * 2) + 5)))
  {
    return;
  }

  painter->fillRect(0, 0, curve_w, 30, QColor(32, 32, 32));

  drawTopLabels(painter);

  if((devparms->acquirememdepth > 1000) && !devparms->timebasedelayenable)
  {
    tmp = 405 - ((devparms->timebaseoffset / (devparms->acquirememdepth / devparms->samplerate)) * 233);
  }
  else
  {
    tmp = 405 - ((devparms->timebaseoffset / ((double)devparms->timebasescale * (double)devparms->hordivisions)) * 233);
  }

  if(tmp < 289)
  {
    tmp = 284;

    rot = 2;
  }
  else if(tmp > 521)
    {
      tmp = 526;

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

  painter->translate(bordersize, bordersize);

  curve_w -= (bordersize * 2);

  curve_h -= (bordersize * 2);

  if(devparms->math_fft && devparms->math_fft_split)
  {
    curve_h /= 3;
  }

/////////////////////////////////// draw the rasters ///////////////////////////////////////////

  painter->setPen(RasterColor);

  painter->drawRect (0, 0, curve_w - 1, curve_h - 1);

  if((devparms->math_fft == 0) || (devparms->math_fft_split == 0))
  {
    if(devparms->displaygrid)
    {
      painter->setPen(QPen(QBrush(RasterColor, Qt::SolidPattern), tracewidth, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));

      if(devparms->displaygrid == 2)
      {
        step = (double)curve_w / (double)devparms->hordivisions;

        for(i=1; i<devparms->hordivisions; i++)
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

    step = (double)curve_w / (double)small_rulers;

    for(i=1; i<small_rulers; i++)
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
  }  // if((devparms->math_fft == 0) || (devparms->math_fft_split == 0))
  else
  {
    painter->drawLine(curve_w / 2, curve_h - 1, curve_w / 2, 0);

    painter->drawLine(0, curve_h / 2, curve_w - 1, curve_h / 2);
  }

/////////////////////////////////// draw the arrows ///////////////////////////////////////////

  if(devparms->modelserie == 6)
  {
    v_sense = -((double)curve_h / 256.0);
  }
  else
  {
    v_sense = -((double)curve_h / 200.0);
  }

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
      chan_arrow_pos[chn] =  (curve_h / 2) - (devparms->chanoffset[chn] / ((devparms->chanscale[chn] * 8) / curve_h));

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

/////////////////////////////////// FFT: draw the curve ///////////////////////////////////////////

  if((devparms->math_fft == 1) && (devparms->math_fft_split == 0))
  {
    drawFFT(painter, curve_h_backup, curve_w_backup);
  }

/////////////////////////////////// draw the curve ///////////////////////////////////////////

  if(bufsize > 32)
  {
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
          painter->drawLine(i * h_step, (devparms->wavebuf[chn][i] * v_sense) + (curve_h / 2), (i + 1) * h_step, (devparms->wavebuf[chn][i] * v_sense) + (curve_h / 2));
          if(i)
          {
            painter->drawLine(i * h_step, (devparms->wavebuf[chn][i - 1] * v_sense) + (curve_h / 2), i * h_step, (devparms->wavebuf[chn][i] * v_sense) + (curve_h / 2));
          }
        }
        else
        {
          if(i < (bufsize - 1))
          {
            if(devparms->displaytype)
            {
              painter->drawPoint(i * h_step, (devparms->wavebuf[chn][i] * v_sense) + (curve_h / 2));
            }
            else
            {
              painter->drawLine(i * h_step, (devparms->wavebuf[chn][i] * v_sense) + (curve_h / 2), (i + 1) * h_step, (devparms->wavebuf[chn][i + 1] * v_sense) + (curve_h / 2));
            }
          }
        }
      }
    }

    painter->setClipping(false);
  }

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
      trig_level_arrow_pos = (curve_h / 2) - ((devparms->triggeredgelevel[devparms->triggeredgesource] + devparms->chanoffset[devparms->triggeredgesource]) / ((devparms->chanscale[devparms->triggeredgesource] * 8) / curve_h));

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
      trig_pos_arrow_pos = (curve_w / 2) - ((devparms->timebasedelayoffset / ((double)devparms->timebasedelayscale * (double)devparms->hordivisions)) * curve_w);
    }
    else
    {
      trig_pos_arrow_pos = (curve_w / 2) - ((devparms->timebaseoffset / ((double)devparms->timebasescale * (double)devparms->hordivisions)) * curve_w);
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
    paintCounterLabel(painter, curve_w - 180, 6);
  }

  if((mainwindow->adjDialFunc == ADJ_DIAL_FUNC_HOLDOFF) || (mainwindow->navDialFunc == NAV_DIAL_FUNC_HOLDOFF))
  {
    convert_to_metric_suffix(str, devparms->triggerholdoff, 2);

    strcat(str, "S");

    paintLabel(painter, curve_w - 110, 5, 100, 20, str, Qt::white);
  }
  else if(mainwindow->adjDialFunc == ADJ_DIAL_FUNC_ACQ_AVG)
    {
      sprintf(str, "%i", devparms->acquireaverages);

      paintLabel(painter, curve_w - 110, 5, 100, 20, str, Qt::white);
    }

  if(label_active == LABEL_ACTIVE_TRIG)
  {
    convert_to_metric_suffix(str, devparms->triggeredgelevel[devparms->triggeredgesource], 2);

    strcat(str, "V");

    paintLabel(painter, curve_w - 120, curve_h - 50, 100, 20, str, QColor(255, 128, 0));
  }
  else if(label_active == LABEL_ACTIVE_CHAN1)
    {
      convert_to_metric_suffix(str, devparms->chanoffset[0], 2);

      strcat(str, "V");

      paintLabel(painter, 20, curve_h - 50, 100, 20, str, SignalColor[0]);
    }
    else if(label_active == LABEL_ACTIVE_CHAN2)
      {
        convert_to_metric_suffix(str, devparms->chanoffset[1], 2);

        strcat(str, "V");

        paintLabel(painter, 20, curve_h - 50, 100, 20, str, SignalColor[1]);
      }
      else if(label_active == LABEL_ACTIVE_CHAN3)
        {
          convert_to_metric_suffix(str, devparms->chanoffset[2], 2);

          strcat(str, "V");

          paintLabel(painter, 20, curve_h - 50, 100, 20, str, SignalColor[2]);
        }
        else if(label_active == LABEL_ACTIVE_CHAN4)
          {
            convert_to_metric_suffix(str, devparms->chanoffset[3], 2);

            strcat(str, "V");

            paintLabel(painter, 20, curve_h - 50, 100, 20, str, SignalColor[3]);
          }

  if((devparms->math_fft == 1) && (devparms->math_fft_split == 1))
  {
    drawFFT(painter, curve_h_backup, curve_w_backup);

    if(label_active == LABEL_ACTIVE_FFT)
    {
      sprintf(str, "%.1fdB", devparms->fft_voffset);

      paintLabel(painter, 20, curve_h * 1.85 - 50.0, 100, 20, str, QColor(128, 0, 255));
    }
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


void SignalCurve::drawFFT(QPainter *painter, int curve_h_b, int curve_w_b)
{
  int i, small_rulers, curve_w, curve_h;

  char str[1024];

  double h_step=0.0,
         step,
         step2,
         fft_h_offset=0.0;

  small_rulers = 5 * devparms->hordivisions;

/////////////////////////////////// FFT: translate coordinates, draw and fill a rectangle ///////////////////////////////////////////

  if(devparms->math_fft_split == 0)
  {
    curve_w = curve_w_b - (bordersize * 2);

    curve_h = curve_h_b - (bordersize * 2);
  }
  else
  {
    curve_h = curve_h_b - (bordersize * 2);

    curve_h /= 3;

    painter->resetTransform();

    painter->translate(bordersize, bordersize + curve_h + 15);

    curve_w = curve_w_b - (bordersize * 2);

    curve_h = curve_h_b - (bordersize * 2);

    curve_h *= 0.64;

/////////////////////////////////// FFT: draw the rasters ///////////////////////////////////////////

    painter->setPen(RasterColor);

    painter->drawRect (0, 0, curve_w - 1, curve_h - 1);

    if(devparms->displaygrid)
    {
      painter->setPen(QPen(QBrush(RasterColor, Qt::SolidPattern), tracewidth, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));

      if(devparms->displaygrid == 2)
      {
        step = (double)curve_w / (double)devparms->hordivisions;

        for(i=1; i<devparms->hordivisions; i++)
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

    step = (double)curve_w / (double)small_rulers;

    for(i=1; i<small_rulers; i++)
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

/////////////////////////////////// FFT: draw the arrow ///////////////////////////////////////////

    fft_arrow_pos = (curve_h / 2.0) - ((curve_h / 80.0) * devparms->fft_voffset);

    drawArrow(painter, 0, fft_arrow_pos, 0, QColor(128, 0, 255), 'M');
  }

/////////////////////////////////// FFT: draw the curve ///////////////////////////////////////////

  if((devparms->fftbufsz > 32) && devparms->chandisplay[devparms->math_fft_src])
  {
    painter->setClipping(true);
    painter->setClipRegion(QRegion(0, 0, curve_w, curve_h), Qt::ReplaceClip);

    h_step = (double)curve_w / (double)devparms->fftbufsz;

    if(devparms->timebasedelayenable)
    {
      h_step *= (100.0 / devparms->timebasedelayscale) / devparms->math_fft_hscale;
    }
    else
    {
      h_step *= (100.0 / devparms->timebasescale) / devparms->math_fft_hscale;
    }

    h_step /= 24.0;

    fft_v_sense = curve_h / (-0.8 * devparms->fft_vscale);

    fft_v_offset = (curve_h / 2.0) - ((curve_h / 80.0) * devparms->fft_voffset);

    fft_h_offset = (curve_w / 2) - ((devparms->math_fft_hcenter / devparms->math_fft_hscale) * curve_w / devparms->hordivisions);

    painter->setPen(QPen(QBrush(QColor(128, 0, 255), Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    for(i=0; i<devparms->fftbufsz; i++)
    {
      if(devparms->fftbufsz < (curve_w / 2))
      {
        painter->drawLine(i * h_step + fft_h_offset, (devparms->fftbuf_out[i] * fft_v_sense) + fft_v_offset, (i + 1) * h_step + fft_h_offset, (devparms->fftbuf_out[i] * fft_v_sense) + fft_v_offset);
        if(i)
        {
          painter->drawLine(i * h_step + fft_h_offset, (devparms->fftbuf_out[i - 1] * fft_v_sense) + fft_v_offset, i * h_step + fft_h_offset, (devparms->fftbuf_out[i] * fft_v_sense) + fft_v_offset);
        }
      }
      else
      {
        if(i < (devparms->fftbufsz - 1))
        {
          if(devparms->displaytype)
          {
            painter->drawPoint(i * h_step + fft_h_offset, (devparms->fftbuf_out[i] * fft_v_sense) + fft_v_offset);
          }
          else
          {
            painter->drawLine(i * h_step + fft_h_offset, (devparms->fftbuf_out[i] * fft_v_sense) + fft_v_offset, (i + 1) * h_step + fft_h_offset, (devparms->fftbuf_out[i + 1] * fft_v_sense) + fft_v_offset);
          }
        }
      }
    }

    sprintf(str, "FFT: CH%i  ", devparms->math_fft_src + 1);

    convert_to_metric_suffix(str + strlen(str), devparms->fft_vscale, 2);

    strcat(str, "dBV/Div  Center ");

    convert_to_metric_suffix(str + strlen(str), devparms->math_fft_hcenter, 1);

    strcat(str, "Hz  ");

    convert_to_metric_suffix(str + strlen(str), devparms->math_fft_hscale, 2);

    strcat(str, "Hz/Div  ");

    if(devparms->timebasedelayenable)
    {
      convert_to_metric_suffix(str + strlen(str), 100.0 / devparms->timebasedelayscale, 0);
    }
    else
    {
      convert_to_metric_suffix(str + strlen(str), 100.0 / devparms->timebasescale, 0);
    }

    strcat(str, "Sa/s");

    painter->drawText(15, 30, str);

    painter->setClipping(false);
  }
  else
  {
    painter->setPen(QPen(QBrush(QColor(128, 0, 255), Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    sprintf(str, "FFT: CH%i Data Invalid!", devparms->math_fft_src + 1);

    painter->drawText(15, 30, str);
  }
}


void SignalCurve::drawCurve(struct device_settings *devp, struct tmcdev *dev)
{
  devparms = devp;

  device = dev;

  bufsize = devparms->wavebufsz;

  update();
}


void SignalCurve::setDeviceParameters(struct device_settings *devp)
{
  devparms = devp;
}


void SignalCurve::drawTopLabels(QPainter *painter)
{
  int i, x1, tmp;

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

  remove_trailing_zeros(str);

  strcat(str, "s");

  painter->drawText(140, 5, 70, 20, Qt::AlignCenter, str);

//////////////// samplerate ///////////////////////////////

  painter->setPen(Qt::gray);

  convert_to_metric_suffix(str, devparms->samplerate, 0);

  strcat(str, "Sa/s");

  painter->drawText(200, -1, 85, 20, Qt::AlignCenter, str);

  if(devparms->acquirememdepth)
  {
    convert_to_metric_suffix(str, devparms->acquirememdepth, 1);

    remove_trailing_zeros(str);

    strcat(str, "pts");

    painter->drawText(200, 14, 85, 20, Qt::AlignCenter, str);
  }
  else
  {
    painter->drawText(200, 14, 85, 20, Qt::AlignCenter, "AUTO");
  }

//////////////// memory position ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(285, 5, 240, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::gray);

  if(devparms->timebasedelayenable)
  {
    dtmp1 = devparms->timebasedelayscale / devparms->timebasescale;

    dtmp2 = (devparms->timebaseoffset - devparms->timebasedelayoffset) / ((devparms->hordivisions / 2) * devparms->timebasescale);

    tmp = (116 - (dtmp1 * 116)) - (dtmp2 * 116);

    if(tmp > 0)
    {
      painter->fillRect(288, 16, tmp, 8, QColor(64, 160, 255));
    }

    x1 = (116 - (dtmp1 * 116)) + (dtmp2 * 116);

    if(x1 > 0)
    {
      painter->fillRect(288 + 233 - x1, 16, x1, 8, QColor(64, 160, 255));
    }
  }
  else if(devparms->acquirememdepth > 1000)
  {
    dtmp1 = (devparms->hordivisions * devparms->timebasescale) / (devparms->acquirememdepth / devparms->samplerate);

    dtmp2 = devparms->timebaseoffset / dtmp1;

    tmp = (116 - (dtmp1 * 116)) - (dtmp2 * 116);

    if(tmp > 0)
    {
      painter->fillRect(288, 16, tmp, 8, QColor(64, 160, 255));
    }

    x1 = (116 - (dtmp1 * 116)) + (dtmp2 * 116);

    if(x1 > 0)
    {
      painter->fillRect(288 + 233 - x1, 16, x1, 8, QColor(64, 160, 255));
    }
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

      if(devparms->chaninvert[chn])
      {
        painter->drawLine(xpos + 6, ypos + 3, xpos + 14, ypos + 3);
      }

      path = QPainterPath();

      path.addRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

      painter->fillPath(path, Qt::black);

      painter->setPen(SignalColor[chn]);

      painter->drawRoundedRect(xpos + 25, ypos, 85, 20, 3, 3);

      painter->drawText(xpos + 30, ypos + 1, 85, 20, Qt::AlignCenter, str2);

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

      if(devparms->chaninvert[chn])
      {
        painter->drawLine(xpos + 6, ypos + 3, xpos + 14, ypos + 3);
      }

      painter->drawText(xpos + 30, ypos + 1, 85, 20, Qt::AlignCenter, str2);

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

    painter->drawText(xpos + 30, ypos + 1, 85, 20, Qt::AlignCenter, str2);

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

  if(devparms == NULL)
  {
    return;
  }

  if(!devparms->connected)
  {
    return;
  }

  if(devparms->math_fft && devparms->math_fft_split)
  {
    m_y -= ((h / 3) + 15);

    if((m_x > -26) && (m_x < 0) && (m_y > (fft_arrow_pos - 7)) && (m_y < (fft_arrow_pos + 7)))
    {
      fft_arrow_moving = 1;
      use_move_events = 1;
      setMouseTracking(true);
      mouse_old_x = m_x;
      mouse_old_y = m_y;
      mainwindow->scrn_timer->stop();
    }

    return;
  }

//  printf("m_x: %i   m_y: %i   trig_pos_arrow_pos: %i\n",m_x, m_y, trig_pos_arrow_pos);

  if(press_event->button() == Qt::LeftButton)
  {
    if(m_y > (h + 12))
    {
//      printf("m_x is: %i   m_y is: %i\n", m_x, m_y);

      m_x += bordersize;

      if((m_x > 8) && (m_x < 118))
      {
        emit chan1Clicked();
      }
      else if((m_x > 133) && (m_x < 243))
        {
          emit chan2Clicked();
        }
        else if((m_x > 258) && (m_x < 368))
          {
            if(devparms->channel_cnt > 2)
            {
              emit chan3Clicked();
            }
          }
          else if((m_x > 383) && (m_x < 493))
            {
              if(devparms->channel_cnt > 3)
              {
                emit chan4Clicked();
              }
            }

      return;
    }

    if(((m_x > (trig_pos_arrow_pos - 8)) && (m_x < (trig_pos_arrow_pos + 8)) && (m_y > 5) && (m_y < 24)) ||
       ((trig_pos_arrow_pos > w) && (m_x > (trig_pos_arrow_pos - 24)) && (m_x <= trig_pos_arrow_pos) && (m_y > 9) && (m_y < 26)) ||
       ((trig_pos_arrow_pos < 0) && (m_x < 24) && (m_x >= 0) && (m_y > 9) && (m_y < 26)))
    {
      trig_pos_arrow_moving = 1;
      use_move_events = 1;
      setMouseTracking(true);
      mouse_old_x = m_x;
      mouse_old_y = m_y;
    }
    else if(((m_x > w) && (m_x < (w + 26)) && (m_y > (trig_level_arrow_pos - 7)) && (m_y < (trig_level_arrow_pos + 7))) ||
            ((trig_level_arrow_pos < 0) && (m_x >= w) && (m_x < (w + 18)) && (m_y >= 0) && (m_y < 22)) ||
            ((trig_level_arrow_pos > h) && (m_x >= w) && (m_x < (w + 18)) && (m_y <= h) && (m_y > (h - 22))))
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

          if((m_x > -26 && (m_x < 0) && (m_y > (chan_arrow_pos[chn] - 7)) && (m_y < (chan_arrow_pos[chn] + 7))) ||
             ((chan_arrow_pos[chn] < 0) && (m_x > -18) && (m_x <= 0) && (m_y >= 0) && (m_y < 22)) ||
             ((chan_arrow_pos[chn] > h) && (m_x > -18) && (m_x <= 0) && (m_y <= h) && (m_y > (h - 22))))
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

  if(use_move_events)
  {
    mainwindow->scrn_timer->stop();
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

  if(devparms == NULL)
  {
    return;
  }

  if(!devparms->connected)
  {
    return;
  }

  if(devparms->math_fft && devparms->math_fft_split)
  {
    fft_arrow_moving = 0;
    use_move_events = 0;
    setMouseTracking(false);

    if(devparms->screenupdates_on == 1)
    {
      mainwindow->scrn_timer->start(devparms->screentimerival);
    }

    update();

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
      devparms->timebasedelayoffset = ((devparms->timebasedelayscale * (double)devparms->hordivisions) / w) * ((w / 2) - trig_pos_arrow_pos);

      tmp = devparms->timebasedelayoffset / (devparms->timebasedelayscale / 50);

      devparms->timebasedelayoffset = (devparms->timebasedelayscale / 50) * tmp;

      lefttime = ((devparms->hordivisions / 2) * devparms->timebasescale) - devparms->timebaseoffset;

      righttime = ((devparms->hordivisions / 2) * devparms->timebasescale) + devparms->timebaseoffset;

      delayrange = (devparms->hordivisions / 2) * devparms->timebasedelayscale;

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

      mainwindow->set_cue_cmd(str);
    }
    else
    {
      devparms->timebaseoffset = ((devparms->timebasescale * (double)devparms->hordivisions) / w) * ((w / 2) - trig_pos_arrow_pos);

      tmp = devparms->timebaseoffset / (devparms->timebasescale / 50);

      devparms->timebaseoffset = (devparms->timebasescale / 50) * tmp;

      strcpy(str, "Horizontal position: ");

      convert_to_metric_suffix(str + strlen(str), devparms->timebaseoffset, 2);

      strcat(str, "s");

      mainwindow->statusLabel->setText(str);

      sprintf(str, ":TIM:OFFS %e", devparms->timebaseoffset);

      mainwindow->set_cue_cmd(str);
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

      devparms->triggeredgelevel[devparms->triggeredgesource] = (((h / 2) - trig_level_arrow_pos) * ((devparms->chanscale[devparms->triggeredgesource] * 8) / h))
                                                                - devparms->chanoffset[devparms->triggeredgesource];

      tmp = devparms->triggeredgelevel[devparms->triggeredgesource] / (devparms->chanscale[devparms->triggeredgesource] / 50);

      devparms->triggeredgelevel[devparms->triggeredgesource] = (devparms->chanscale[devparms->triggeredgesource] / 50) * tmp;

      sprintf(str, "Trigger level: ");

      convert_to_metric_suffix(str + strlen(str), devparms->triggeredgelevel[devparms->triggeredgesource], 2);

      strcat(str, "V");

      mainwindow->statusLabel->setText(str);

      sprintf(str, ":TRIG:EDG:LEV %e", devparms->triggeredgelevel[devparms->triggeredgesource]);

      mainwindow->set_cue_cmd(str);

      trig_line_timer->start(1300);
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

          devparms->chanoffset[chn] = ((h / 2) - chan_arrow_pos[chn]) * ((devparms->chanscale[chn] * 8) / h);

          tmp = devparms->chanoffset[chn] / (devparms->chanscale[chn] / 50);

          devparms->chanoffset[chn] = (devparms->chanscale[chn] / 50) * tmp;

          sprintf(str, "Channel %i offset: ", chn + 1);

          convert_to_metric_suffix(str + strlen(str), devparms->chanoffset[chn], 3);

          strcat(str, "V");

          mainwindow->statusLabel->setText(str);

          sprintf(str, ":CHAN%i:OFFS %e", chn + 1, devparms->chanoffset[chn]);

          mainwindow->set_cue_cmd(str);

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

  if(devparms->screenupdates_on == 1)
  {
    mainwindow->scrn_timer->start(devparms->screentimerival);
  }

  update();
}


void SignalCurve::mouseMoveEvent(QMouseEvent *move_event)
{
  int chn, h_fft, a_pos;

  double dtmp;

  if(!use_move_events)
  {
    return;
  }

  mouse_x = move_event->x() - bordersize;
  mouse_y = move_event->y() - bordersize;

  if(devparms == NULL)
  {
    return;
  }

  if(!devparms->connected)
  {
    return;
  }

  if(devparms->math_fft && devparms->math_fft_split)
  {
    mouse_y -= ((h / 3) + 15);

    h_fft = h * 0.64;

    if(fft_arrow_moving)
    {
      a_pos = mouse_y;

      if(a_pos < 0)
      {
        a_pos = 0;
      }

      if(a_pos > (h * 0.64))
      {
        a_pos = (h * 0.64);
      }

      devparms->fft_voffset = ((h_fft / 2) - a_pos) * (devparms->fft_vscale * 8.0 / h_fft);

      label_active = LABEL_ACTIVE_FFT;

      mainwindow->label_timer->start(LABEL_TIMER_IVAL);
    }

    update();

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

      devparms->triggeredgelevel[devparms->triggeredgesource] = (((h / 2) - trig_level_arrow_pos) * ((devparms->chanscale[devparms->triggeredgesource] * 8) / h))
                                                                - devparms->chanoffset[devparms->triggeredgesource];

      dtmp = devparms->triggeredgelevel[devparms->triggeredgesource] / (devparms->chanscale[devparms->triggeredgesource] / 50);

      devparms->triggeredgelevel[devparms->triggeredgesource] = (devparms->chanscale[devparms->triggeredgesource] / 50) * dtmp;

      label_active = LABEL_ACTIVE_TRIG;

      mainwindow->label_timer->start(LABEL_TIMER_IVAL);
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

          devparms->chanoffset[chn] = ((h / 2) - chan_arrow_pos[chn]) * ((devparms->chanscale[chn] * 8) / h);

          dtmp = devparms->chanoffset[chn] / (devparms->chanscale[chn] / 50);

          devparms->chanoffset[chn] = (devparms->chanscale[chn] / 50) * dtmp;

          label_active = chn + 1;

          mainwindow->label_timer->start(LABEL_TIMER_IVAL);

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

  trig_line_timer->start(1300);
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


void SignalCurve::paintLabel(QPainter *painter, int xpos, int ypos, int xw, int yh, const char *str, QColor color)
{
  QPainterPath path;

  path.addRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(color);

  painter->drawRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->drawText(xpos, ypos, xw, yh, Qt::AlignCenter, str);
}


void SignalCurve::paintCounterLabel(QPainter *painter, int xpos, int ypos)
{
  int i;

  char str[128];

  QPainterPath path;

  path.addRoundedRect(xpos, ypos, 175, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::darkGray);

  painter->drawRoundedRect(xpos, ypos, 175, 20, 3, 3);

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
    convert_to_metric_suffix(str, devparms->counterfreq, 5);

    strcat(str, "Hz");
  }

  for(i=0; i<3; i++)
  {
    painter->drawLine(xpos + 22 + (i * 14), ypos + 14, xpos + 29 + (i * 14), ypos + 14);
    painter->drawLine(xpos + 29 + (i * 14), ypos + 14, xpos + 29 + (i * 14), ypos + 7);
    painter->drawLine(xpos + 29 + (i * 14), ypos + 7, xpos + 36 + (i * 14), ypos + 7);
    painter->drawLine(xpos + 36 + (i * 14), ypos + 7, xpos + 36 + (i * 14), ypos + 14);
  }
  painter->drawLine(xpos + 22 + (i * 14), ypos + 14, xpos + 29 + (i * 14), ypos + 14);

  painter->drawText(xpos + 75, ypos, 100, 20, Qt::AlignCenter, str);
}


bool SignalCurve::hasMoveEvent(void)
{
  if(use_move_events)
  {
    return true;
  }

  return false;
}















