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



#include "wave_view.h"



WaveCurve::WaveCurve(QWidget *w_parent) : QWidget(w_parent)
{
  wavedialog = (UI_wave_window *)w_parent;

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

  bufsize = 0;
  bordersize = 60;

  v_sense = 1;

  mouse_x = 0;
  mouse_y = 0;
  mouse_old_x = 0;
  mouse_old_y = 0;

  use_move_events = 0;

  old_w = 10000;

  devparms = NULL;
}


void WaveCurve::paintEvent(QPaintEvent *)
{
  int i, chn,
      small_rulers,
      w_trace_offset,
      curve_w,
      curve_h,
      samples_per_div,
      sample_range,
      sample_start,
      sample_end,
      t_pos;

  double h_step=0.0,
         step,
         step2;

  if(devparms == NULL)
  {
    return;
  }

  QPainter paint(this);
#if QT_VERSION >= 0x050000
  paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif

  QPainter *painter = &paint;

  curve_w = width();

  curve_h = height();

  bufsize = devparms->wavebufsz;

  small_rulers = 5 * devparms->hordivisions;

  painter->fillRect(0, 0, curve_w, curve_h, BackgroundColor);

  if((curve_w < ((bordersize * 2) + 5)) || (curve_h < ((bordersize * 2) + 5)))
  {
    return;
  }

  painter->fillRect(0, 0, curve_w, 30, QColor(32, 32, 32));

  samples_per_div = devparms->samplerate * devparms->timebasescale;

  drawTopLabels(painter);

  t_pos = 408 - ((devparms->timebaseoffset / ((double)devparms->acquirememdepth / devparms->samplerate)) * 233);

  drawSmallTriggerArrow(painter, t_pos, 16, 1, QColor(255, 128, 0));

  painter->fillRect(0, curve_h - 30, curve_w, curve_h, QColor(32, 32, 32));

  for(i=0; i<devparms->channel_cnt; i++)
  {
    drawChanLabel(painter, 8 + (i * 130), curve_h - 25, i);
  }

/////////////////////////////////// translate coordinates, draw and fill a rectangle ///////////////////////////////////////////

  painter->translate(bordersize, bordersize);

  curve_w -= (bordersize * 2);

  curve_h -= (bordersize * 2);

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

        step = curve_h / (double)devparms->vertdivisions;

        for(i=1; i<devparms->vertdivisions; i++)
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

    step = curve_h / (5.0 * devparms->vertdivisions);

    for(i=1; i<(5 * devparms->vertdivisions); i++)
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
  }
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
    v_sense = -((double)curve_h / (25.0 * devparms->vertdivisions));
  }

  drawTrigCenterArrow(painter, curve_w / 2, 0);

  for(chn=0; chn<devparms->channel_cnt; chn++)
  {
    if(!devparms->chandisplay[chn])
    {
      continue;
    }

    chan_arrow_pos[chn] =  (curve_h / 2) - (devparms->chanoffset[chn] / ((devparms->chanscale[chn] * devparms->vertdivisions) / curve_h));

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

/////////////////////////////////// draw the curve ///////////////////////////////////////////

  if(bufsize > 32)
  {
    painter->setClipping(true);
    painter->setClipRegion(QRegion(0, 0, curve_w, curve_h), Qt::ReplaceClip);

    h_step = (double)curve_w / (devparms->hordivisions * samples_per_div);

    sample_start = devparms->wave_mem_view_sample_start;

    sample_end = devparms->hordivisions * samples_per_div + sample_start;

    if(sample_end > bufsize)
    {
      sample_end = bufsize;
    }

    sample_range = sample_end - sample_start;

    w_trace_offset = 0;

//     if(bufsize != (devparms->hordivisions * samples_per_div))
//     {
//       if(devparms->timebaseoffset < 0)
//       {
//         w_trace_offset = curve_w - ((double)curve_w * ((double)bufsize / (double)(devparms->hordivisions * samples_per_div)));
//       }
//     }

    for(chn=0; chn<devparms->channel_cnt; chn++)
    {
      if(!devparms->chandisplay[chn])
      {
        continue;
      }

      painter->setPen(QPen(QBrush(SignalColor[chn], Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

      for(i=0; i<sample_range; i++)
      {
        if(sample_range < (curve_w / 2))
        {
          if(devparms->displaytype)
          {
            painter->drawPoint(i * h_step + w_trace_offset,
                               (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2));
          }
          else
          {
            painter->drawLine(i * h_step + w_trace_offset,
                              (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2),
                              (i + 1) * h_step + w_trace_offset,
                              (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2));
            if(i)
            {
              painter->drawLine(i * h_step + w_trace_offset,
                                (devparms->wavebuf[chn][i - 1 + sample_start] * v_sense) + (curve_h / 2),
                                i * h_step + w_trace_offset,
                                (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2));
            }
          }
        }
        else
        {
          if(i < (bufsize - 1))
          {
            if(devparms->displaytype)
            {
              painter->drawPoint(i * h_step + w_trace_offset,
                                 (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2));
            }
            else
            {
              painter->drawLine(i * h_step + w_trace_offset,
                                (devparms->wavebuf[chn][i + sample_start] * v_sense) + (curve_h / 2),
                                (i + 1) * h_step + w_trace_offset,
                                (devparms->wavebuf[chn][i + 1 + sample_start] * v_sense) + (curve_h / 2));
            }
          }
        }
      }
    }

    painter->setClipping(false);
  }

/////////////////////////////////// draw the decoder ///////////////////////////////////////////

  if(devparms->math_decode_display)  draw_decoder(painter, curve_w, curve_h);

/////////////////////////////////// draw the trigger arrows ///////////////////////////////////////////

  if(devparms->triggeredgesource < 4)
  {
    trig_level_arrow_pos = (curve_h / 2) - ((devparms->triggeredgelevel[devparms->triggeredgesource] + devparms->chanoffset[devparms->triggeredgesource]) / ((devparms->chanscale[devparms->triggeredgesource] * devparms->vertdivisions) / curve_h));

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

  trig_pos_arrow_pos = (curve_w / 2) - (((devparms->timebaseoffset + devparms->viewer_center_position) / (devparms->timebasescale * (double)devparms->hordivisions)) * curve_w);

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


void WaveCurve::setDeviceParameters(struct device_settings *devp)
{
  devparms = devp;
}


void WaveCurve::drawTopLabels(QPainter *painter)
{
  int i;

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

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::green);

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

  convert_to_metric_suffix(str, devparms->timebasescale, 1);

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

  dtmp1 = (devparms->hordivisions * devparms->timebasescale) / (devparms->acquirememdepth / devparms->samplerate);

  dtmp2 = devparms->viewer_center_position / (devparms->acquirememdepth / devparms->samplerate);

  painter->fillRect(288, 16, 233, 8, QColor(64, 160, 255));

  painter->fillRect(288 + 119 + ((233.0 * dtmp2) - (116.0 * dtmp1)), 16, 233 * dtmp1, 8, Qt::black);

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

  convert_to_metric_suffix(str, devparms->timebaseoffset + devparms->viewer_center_position, 4);

  strcat(str, "s");

  painter->drawText(570, 5, 85, 20, Qt::AlignCenter, str);

//////////////// trigger ///////////////////////////////

  path = QPainterPath();

  path.addRoundedRect(685, 5, 125, 20, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(Qt::gray);

  painter->drawText(670, 20, "T");

  convert_to_metric_suffix(str, devparms->triggeredgelevel[devparms->triggeredgesource], 2);

  strcat(str, devparms->chanunitstr[devparms->chanunit[devparms->triggeredgesource]]);

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


void WaveCurve::drawArrow(QPainter *painter, int xpos, int ypos, int rot, QColor color, char ch)
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


void WaveCurve::drawSmallTriggerArrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
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


void WaveCurve::drawTrigCenterArrow(QPainter *painter, int xpos, int ypos)
{
  QPainterPath path;

  path.moveTo(xpos + 7, ypos);
  path.lineTo(xpos - 6, ypos);
  path.lineTo(xpos, ypos + 7);
  path.lineTo(xpos + 7, ypos);
  painter->fillPath(path, QColor(255, 128, 0));
}


void WaveCurve::setSignalColor1(QColor newColor)
{
  SignalColor[0] = newColor;
  update();
}


void WaveCurve::setSignalColor2(QColor newColor)
{
  SignalColor[1] = newColor;
  update();
}


void WaveCurve::setSignalColor3(QColor newColor)
{
  SignalColor[2] = newColor;
  update();
}


void WaveCurve::setSignalColor4(QColor newColor)
{
  SignalColor[3] = newColor;
  update();
}


void WaveCurve::setTraceWidth(int tr_width)
{
  tracewidth = tr_width;
  if(tracewidth < 0)  tracewidth = 0;
  update();
}


void WaveCurve::setBackgroundColor(QColor newColor)
{
  BackgroundColor = newColor;
  update();
}


void WaveCurve::setRasterColor(QColor newColor)
{
  RasterColor = newColor;
  update();
}


void WaveCurve::setBorderSize(int newsize)
{
  bordersize = newsize;
  if(bordersize < 0)  bordersize = 0;
  update();
}


void WaveCurve::mousePressEvent(QMouseEvent *) //press_event)
{
//   int m_x,
//       m_y;

  setFocus(Qt::MouseFocusReason);

  w = width() - (2 * bordersize);
  h = height() - (2 * bordersize);

//   m_x = press_event->x() - bordersize;
//   m_y = press_event->y() - bordersize;

  if(devparms == NULL)
  {
    return;
  }
}


void WaveCurve::mouseReleaseEvent(QMouseEvent *release_event)
{
  w = width() - (2 * bordersize);
  h = height() - (2 * bordersize);

  mouse_x = release_event->x() - bordersize;
  mouse_y = release_event->y() - bordersize;

  if(devparms == NULL)
  {
    return;
  }

  use_move_events = 0;
  setMouseTracking(false);

  update();
}


void WaveCurve::mouseMoveEvent(QMouseEvent *move_event)
{
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

  update();
}


void WaveCurve::paintLabel(QPainter *painter, int xpos, int ypos, int xw, int yh, const char *str, QColor color)
{
  QPainterPath path;

  path.addRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->fillPath(path, Qt::black);

  painter->setPen(color);

  painter->drawRoundedRect(xpos, ypos, xw, yh, 3, 3);

  painter->drawText(xpos, ypos, xw, yh, Qt::AlignCenter, str);
}


void WaveCurve::draw_decoder(QPainter *painter, int dw, int dh)
{
  int i, j, k,
      cell_width,
      base_line,
      line_h_uart_tx=0,
      line_h_uart_rx=0,
      line_h_spi_mosi=0,
      line_h_spi_miso=0,
      spi_chars=1,
      pixel_per_bit=1,
      samples_per_div,
      sample_start,
      sample_end;

  double pix_per_smpl;

  char str[256];


  painter->setClipping(true);
  painter->setClipRegion(QRegion(0, 0, dw, dh), Qt::ReplaceClip);

  samples_per_div = devparms->samplerate * devparms->timebasescale;

  sample_start = devparms->wave_mem_view_sample_start;

  sample_end = devparms->hordivisions * samples_per_div + sample_start;

  if(sample_end > bufsize)
  {
    sample_end = bufsize;
  }

  if(devparms->modelserie == 6)
  {
    base_line = (dh / 2) - (((double)dh / 400.0) * devparms->math_decode_pos);
  }
  else
  {
    base_line = ((double)dh / 400.0) * devparms->math_decode_pos;
  }

  pix_per_smpl = (double)dw / (devparms->hordivisions * samples_per_div);

  switch(devparms->math_decode_format)
  {
    case 0:  cell_width = 40;  // hex
             break;
    case 1:  cell_width = 30;  // ASCII
             break;
    case 2:  cell_width = 30;  // decimal;
             break;
    case 3:  cell_width = 70;  // binary
             break;
    default: cell_width = 70;  // line
             break;
  }

  if(devparms->math_decode_mode == DECODE_MODE_UART)
  {
    pixel_per_bit = ((double)dw / devparms->hordivisions / devparms->timebasescale) / (double)devparms->math_decode_uart_baud;

    cell_width = pixel_per_bit * devparms->math_decode_uart_width;

    painter->setPen(Qt::green);

    if(devparms->math_decode_uart_tx && devparms->math_decode_uart_rx)
    {
      line_h_uart_tx = base_line - 5;

      line_h_uart_rx = base_line + 45;

      painter->drawLine(0, line_h_uart_tx, dw, line_h_uart_tx);

      painter->drawLine(0, line_h_uart_rx, dw, line_h_uart_rx);
    }
    else if(devparms->math_decode_uart_tx)
      {
        line_h_uart_tx = base_line;

        painter->drawLine(0, line_h_uart_tx, dw, line_h_uart_tx);
      }
      else if(devparms->math_decode_uart_rx)
        {
          line_h_uart_rx = base_line;

          painter->drawLine(0, line_h_uart_rx, dw, line_h_uart_rx);
        }

    if(devparms->math_decode_uart_tx)
    {
      for(i=0; i<devparms->math_decode_uart_tx_nval; i++)
      {
        if((devparms->math_decode_uart_tx_val_pos[i] >= sample_start) && (devparms->math_decode_uart_tx_val_pos[i] < sample_end))
        {
          painter->fillRect((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 26, Qt::black);

          painter->drawRect((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 26);
        }
      }
    }

    if(devparms->math_decode_uart_rx)
    {
      for(i=0; i<devparms->math_decode_uart_rx_nval; i++)
      {
        if((devparms->math_decode_uart_rx_val_pos[i] >= sample_start) && (devparms->math_decode_uart_rx_val_pos[i] < sample_end))
        {
          painter->fillRect((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 26, Qt::black);

          painter->drawRect((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 26);
        }
      }
    }

    painter->setPen(Qt::white);

    if(devparms->math_decode_uart_tx)
    {
      switch(devparms->math_decode_format)
      {
        case 0: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[HEX]");
                break;
        case 1: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[ASC]");
                break;
        case 2: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[DEC]");
                break;
        case 3: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[BIN]");
                break;
        case 4: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[LINE]");
                break;
        default: painter->drawText(5, line_h_uart_tx - 35, 65, 30, Qt::AlignCenter, "Tx[\?\?\?]");
                break;
      }

      for(i=0; i<devparms->math_decode_uart_tx_nval; i++)
      {
        if((devparms->math_decode_uart_tx_val_pos[i] >= sample_start) && (devparms->math_decode_uart_tx_val_pos[i] < sample_end))
        {
          if(devparms->math_decode_format == 0)  // hex
          {
            sprintf(str, "%02X", devparms->math_decode_uart_tx_val[i]);

            painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 30, Qt::AlignCenter, str);
          }
          else if(devparms->math_decode_format == 1)  // ASCII
            {
              ascii_decode_control_char(devparms->math_decode_uart_tx_val[i], str);

              painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 30, Qt::AlignCenter, str);
            }
            else if(devparms->math_decode_format == 2)  // decimal
              {
                sprintf(str, "%u", (unsigned int)devparms->math_decode_uart_tx_val[i]);

                painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 30, Qt::AlignCenter, str);
              }
              else if(devparms->math_decode_format == 3)  // binary
                {
                  for(j=0; j<devparms->math_decode_uart_width; j++)
                  {
                    str[devparms->math_decode_uart_width - 1 - j] = ((devparms->math_decode_uart_tx_val[i] >> j) & 1) + '0';
                  }

                  str[j] = 0;

                  painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 30, Qt::AlignCenter, str);
                }
                else if(devparms->math_decode_format == 4)  // line
                  {
                    for(j=0; j<devparms->math_decode_uart_width; j++)
                    {
                      str[j] = ((devparms->math_decode_uart_tx_val[i] >> j) & 1) + '0';
                    }

                    str[j] = 0;

                    painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_tx - 13, cell_width, 30, Qt::AlignCenter, str);
                  }

            if(devparms->math_decode_uart_tx_err[i])
            {
              painter->setPen(Qt::red);

              painter->drawText((devparms->math_decode_uart_tx_val_pos[i] - sample_start) * pix_per_smpl + cell_width, line_h_uart_tx - 13, 25, 25, Qt::AlignCenter, "?");

              painter->setPen(Qt::white);
            }
        }
      }
    }

    if(devparms->math_decode_uart_rx)
    {
      switch(devparms->math_decode_format)
      {
        case 0: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[HEX]");
                break;
        case 1: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[ASC]");
                break;
        case 2: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[DEC]");
                break;
        case 3: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[BIN]");
                break;
        case 4: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[LINE]");
                break;
        default: painter->drawText(5, line_h_uart_rx - 35, 65, 30, Qt::AlignCenter, "Rx[\?\?\?]");
                break;
      }

      for(i=0; i<devparms->math_decode_uart_rx_nval; i++)
      {
        if((devparms->math_decode_uart_rx_val_pos[i] >= sample_start) && (devparms->math_decode_uart_rx_val_pos[i] < sample_end))
        {
          if(devparms->math_decode_format == 0)  // hex
          {
            sprintf(str, "%02X", devparms->math_decode_uart_rx_val[i]);

            painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 30, Qt::AlignCenter, str);
          }
          else if(devparms->math_decode_format == 1)  // ASCII
            {
              ascii_decode_control_char(devparms->math_decode_uart_rx_val[i], str);

              painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 30, Qt::AlignCenter, str);
            }
            else if(devparms->math_decode_format == 2)  // decimal
              {
                sprintf(str, "%u", (unsigned int)devparms->math_decode_uart_rx_val[i]);

                painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 30, Qt::AlignCenter, str);
              }
              else if(devparms->math_decode_format == 3)  // binary
                {
                  for(j=0; j<devparms->math_decode_uart_width; j++)
                  {
                    str[devparms->math_decode_uart_width - 1 - j] = ((devparms->math_decode_uart_rx_val[i] >> j) & 1) + '0';
                  }

                  str[j] = 0;

                  painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 30, Qt::AlignCenter, str);
                }
                else if(devparms->math_decode_format == 4)  // line
                  {
                    for(j=0; j<devparms->math_decode_uart_width; j++)
                    {
                      str[j] = ((devparms->math_decode_uart_rx_val[i] >> j) & 1) + '0';
                    }

                    str[j] = 0;

                    painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl, line_h_uart_rx - 13, cell_width, 30, Qt::AlignCenter, str);
                  }

            if(devparms->math_decode_uart_rx_err[i])
            {
              painter->setPen(Qt::red);

              painter->drawText((devparms->math_decode_uart_rx_val_pos[i] - sample_start) * pix_per_smpl + cell_width, line_h_uart_rx - 13, 25, 25, Qt::AlignCenter, "?");

              painter->setPen(Qt::white);
            }
        }
      }
    }
  }

  if(devparms->math_decode_mode == DECODE_MODE_SPI)
  {
    painter->setPen(Qt::green);

    if(devparms->math_decode_spi_width > 24)
    {
      spi_chars = 4;
    }
    else if(devparms->math_decode_spi_width > 16)
      {
        spi_chars = 3;
      }
      else if(devparms->math_decode_spi_width > 8)
        {
          spi_chars = 2;
        }
        else
        {
          spi_chars = 1;
        }

    cell_width *= spi_chars;

    if(devparms->math_decode_spi_mosi && devparms->math_decode_spi_miso)
    {
      line_h_spi_mosi = base_line - 5;

      line_h_spi_miso = base_line + 45;

      painter->drawLine(0, line_h_spi_mosi, dw, line_h_spi_mosi);

      painter->drawLine(0, line_h_spi_miso, dw, line_h_spi_miso);
    }
    else if(devparms->math_decode_spi_mosi)
      {
        line_h_spi_mosi = base_line;

        painter->drawLine(0, line_h_spi_mosi, dw, line_h_spi_mosi);
      }
      else if(devparms->math_decode_spi_miso)
        {
          line_h_spi_miso = base_line;

          painter->drawLine(0, line_h_spi_miso, dw, line_h_spi_miso);
        }

    if(devparms->math_decode_spi_mosi)
    {
      for(i=0; i<devparms->math_decode_spi_mosi_nval; i++)
      {
        cell_width = (devparms->math_decode_spi_mosi_val_pos_end[i] - devparms->math_decode_spi_mosi_val_pos[i]) *
                      pix_per_smpl;

        painter->fillRect((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 26, Qt::black);

        painter->drawRect((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 26);
      }
    }

    if(devparms->math_decode_spi_miso)
    {
      for(i=0; i<devparms->math_decode_spi_miso_nval; i++)
      {
        cell_width = (devparms->math_decode_spi_miso_val_pos_end[i] - devparms->math_decode_spi_miso_val_pos[i]) *
                      pix_per_smpl;

        painter->fillRect((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 26, Qt::black);

        painter->drawRect((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 26);
      }
    }

    painter->setPen(Qt::white);

    if(devparms->math_decode_spi_mosi)
    {
      switch(devparms->math_decode_format)
      {
        case 0: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[HEX]");
                break;
        case 1: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[ASC]");
                break;
        case 2: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[DEC]");
                break;
        case 3: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[BIN]");
                break;
        case 4: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[LINE]");
                break;
        default: painter->drawText(5, line_h_spi_mosi - 35, 80, 30, Qt::AlignCenter, "Mosi[\?\?\?]");
                break;
      }

      for(i=0; i<devparms->math_decode_spi_mosi_nval; i++)
      {
        if(devparms->math_decode_format == 0)  // hex
        {
          switch(spi_chars)
          {
            case 1: sprintf(str, "%02X", devparms->math_decode_spi_mosi_val[i]);
                    break;
            case 2: sprintf(str, "%04X", devparms->math_decode_spi_mosi_val[i]);
                    break;
            case 3: sprintf(str, "%06X", devparms->math_decode_spi_mosi_val[i]);
                    break;
            case 4: sprintf(str, "%08X", devparms->math_decode_spi_mosi_val[i]);
                    break;
          }

          painter->drawText((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 30, Qt::AlignCenter, str);
        }
        else if(devparms->math_decode_format == 1)  // ASCII
          {
            for(k=0, j=0; k<spi_chars; k++)
            {
              j += ascii_decode_control_char(devparms->math_decode_spi_mosi_val[i] >> (k * 8), str + j);
            }

            painter->drawText((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 30, Qt::AlignCenter, str);
          }
          else if(devparms->math_decode_format == 2)  // decimal
            {
              sprintf(str, "%u", devparms->math_decode_spi_mosi_val[i]);

              painter->drawText((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 30, Qt::AlignCenter, str);
            }
            else if(devparms->math_decode_format == 3)  // binary
              {
                for(j=0; j<devparms->math_decode_spi_width; j++)
                {
                  str[devparms->math_decode_spi_width - 1 - j] = ((devparms->math_decode_spi_mosi_val[i] >> j) & 1) + '0';
                }

                str[j] = 0;

                painter->drawText((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 30, Qt::AlignCenter, str);
              }
              else if(devparms->math_decode_format == 4)  // line
                {
                  for(j=0; j<devparms->math_decode_spi_width; j++)
                  {
                    str[j] = ((devparms->math_decode_spi_mosi_val[i] >> j) & 1) + '0';
                  }

                  str[devparms->math_decode_spi_width] = 0;

                  painter->drawText((devparms->math_decode_spi_mosi_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_mosi - 13, cell_width, 30, Qt::AlignCenter, str);
                }
      }
    }

    if(devparms->math_decode_spi_miso)
    {
      switch(devparms->math_decode_format)
      {
        case 0: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[HEX]");
                break;
        case 1: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[HEX]");
                break;
        case 2: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[DEC]");
                break;
        case 3: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[BIN]");
                break;
        case 4: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[LINE]");
                break;
        default: painter->drawText(5, line_h_spi_miso - 35, 80, 30, Qt::AlignCenter, "Miso[\?\?\?]");
                break;
      }

      for(i=0; i<devparms->math_decode_spi_miso_nval; i++)
      {
        if(devparms->math_decode_format == 0)  // hex
        {
          switch(spi_chars)
          {
            case 1: sprintf(str, "%02X", devparms->math_decode_spi_miso_val[i]);
                    break;
            case 2: sprintf(str, "%04X", devparms->math_decode_spi_miso_val[i]);
                    break;
            case 3: sprintf(str, "%06X", devparms->math_decode_spi_miso_val[i]);
                    break;
            case 4: sprintf(str, "%08X", devparms->math_decode_spi_miso_val[i]);
                    break;
          }

          painter->drawText((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 30, Qt::AlignCenter, str);
        }
        else if(devparms->math_decode_format == 1)  // ASCII
          {
            for(k=0, j=0; k<spi_chars; k++)
            {
              j += ascii_decode_control_char(devparms->math_decode_spi_miso_val[i] >> (k * 8), str + j);
            }

            painter->drawText((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 30, Qt::AlignCenter, str);
          }
          else if(devparms->math_decode_format == 2)  // decimal
            {
              sprintf(str, "%u", devparms->math_decode_spi_miso_val[i]);

              painter->drawText((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 30, Qt::AlignCenter, str);
            }
            else if(devparms->math_decode_format == 3)  // binary
              {
                for(j=0; j<devparms->math_decode_spi_width; j++)
                {
                  str[devparms->math_decode_spi_width - 1 - j] = ((devparms->math_decode_spi_miso_val[i] >> j) & 1) + '0';
                }

                str[j] = 0;

                painter->drawText((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 30, Qt::AlignCenter, str);
              }
              else if(devparms->math_decode_format == 4)  // line
                {
                  for(j=0; j<devparms->math_decode_spi_width; j++)
                  {
                    str[j] = ((devparms->math_decode_spi_miso_val[i] >> j) & 1) + '0';
                  }

                  str[devparms->math_decode_spi_width] = 0;

                  painter->drawText((devparms->math_decode_spi_miso_val_pos[i] - sample_start) * pix_per_smpl, line_h_spi_miso - 13, cell_width, 30, Qt::AlignCenter, str);
                }
      }
    }
  }

  painter->setClipping(false);
}


int WaveCurve::ascii_decode_control_char(char ch, char *str)
{
  if((ch > 32) && (ch < 127))
  {
    str[0] = ch;

    str[1] = 0;

    return 1;
  }

  switch(ch)
  {
    case  0: strcpy(str, "NULL");
             break;
    case  1: strcpy(str, "SOH");
             break;
    case  2: strcpy(str, "STX");
             break;
    case  3: strcpy(str, "ETX");
             break;
    case  4: strcpy(str, "EOT");
             break;
    case  5: strcpy(str, "ENQ");
             break;
    case  6: strcpy(str, "ACK");
             break;
    case  7: strcpy(str, "BEL");
             break;
    case  8: strcpy(str, "BS");
             break;
    case  9: strcpy(str, "HT");
             break;
    case 10: strcpy(str, "LF");
             break;
    case 11: strcpy(str, "VT");
             break;
    case 12: strcpy(str, "FF");
             break;
    case 13: strcpy(str, "CR");
             break;
    case 14: strcpy(str, "SO");
             break;
    case 15: strcpy(str, "SI");
             break;
    case 16: strcpy(str, "DLE");
             break;
    case 17: strcpy(str, "DC1");
             break;
    case 18: strcpy(str, "DC2");
             break;
    case 19: strcpy(str, "DC3");
             break;
    case 20: strcpy(str, "DC4");
             break;
    case 21: strcpy(str, "NAK");
             break;
    case 22: strcpy(str, "SYN");
             break;
    case 23: strcpy(str, "ETB");
             break;
    case 24: strcpy(str, "CAN");
             break;
    case 25: strcpy(str, "EM");
             break;
    case 26: strcpy(str, "SUB");
             break;
    case 27: strcpy(str, "ESC");
             break;
    case 28: strcpy(str, "FS");
             break;
    case 29: strcpy(str, "GS");
             break;
    case 30: strcpy(str, "RS");
             break;
    case 31: strcpy(str, "US");
             break;
    case 32: strcpy(str, "SP");
             break;
    case 127: strcpy(str, "DEL");
             break;
    default: strcpy(str, ".");
             break;
  }

  return strlen(str);
}


void WaveCurve::drawChanLabel(QPainter *painter, int xpos, int ypos, int chn)
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

  strcat(str2, devparms->chanunitstr[devparms->chanunit[chn]]);

  if(devparms->chanbwlimit[chn])
  {
    strcat(str2, " B");
  }

  if(devparms->chandisplay[chn])
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

    painter->drawText(xpos + 35, ypos + 1, 90, 20, Qt::AlignCenter, str2);

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












