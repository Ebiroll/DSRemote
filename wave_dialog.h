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




#ifndef WAVE_DIALOG_H
#define WAVE_DIALOG_H



#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QGridLayout>
#include <QSlider>
#include <QMenuBar>
#include <QMenu>

#include "mainwindow.h"
#include "global.h"
#include "wave_view.h"


class UI_Mainwindow;

class WaveCurve;


class UI_wave_window : public QDialog
{
  Q_OBJECT

public:

  UI_wave_window(struct device_settings *, short *wbuf[MAX_CHNS], QWidget *parent=0);
  ~UI_wave_window();

  void set_wavslider(void);

private:

struct device_settings *devparms;

UI_Mainwindow *mainwindow;

QMenuBar     *menubar;

QMenu        *helpmenu;

QGridLayout *g_layout;

WaveCurve *wavcurve;

QSlider *wavslider;

QAction *former_page_act,
        *shift_page_left_act,
        *shift_page_right_act,
        *next_page_act,
        *zoom_in_act,
        *zoom_out_act,
        *center_position_act,
        *center_trigger_act;

private slots:

void wavslider_value_changed(int);

void former_page();
void shift_page_left();
void shift_page_right();
void next_page();
void zoom_in();
void zoom_out();
void center_position();
void center_trigger();

};



#endif


