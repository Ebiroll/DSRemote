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


#ifndef UI_VIEW_MAINFORM_H
#define UI_VIEW_MAINFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QtGui>
#include <QWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QFont>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileDialog>
#include <QAction>
#include <QActionGroup>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QLibrary>
#include <QString>
#include <QVBoxLayout>
#include <QList>
#include <QDockWidget>
#include <QPalette>
#include <QGroupBox>
#include <QMessageBox>
#include <QPoint>
#include <QDial>
#include <QSettings>
#include <QProcess>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "global.h"
#include "about_dialog.h"
#include "utils.h"
#include "tmc_dev.h"
#include "tled.h"
#include "edflib.h"
#include "signalcurve.h"
#include "settings_dialog.h"



class SignalCurve;


class UI_Mainwindow : public QMainWindow
{
  Q_OBJECT

public:
  UI_Mainwindow();
  ~UI_Mainwindow();

  char recent_dir[MAX_PATHLEN],
       recent_savedir[MAX_PATHLEN];

  void read_settings(void);
  void write_settings(void);

  struct device_settings devparms;

  QLabel       *statusLabel;

  QFont        *appfont,
               *monofont;

private:

  QMenuBar     *menubar;

  QMenu        *devicemenu,
               *helpmenu;

  QTimer       *scrn_timer,
               *stat_timer,
               *adjdial_timer,
               *navDial_timer,
               *test_timer;

  QStatusBar   *statusBar;

  QVBoxLayout *vlayout1;

  QLabel       *mainLabel,
               *adjDialLabel,
               *horScaleLabel,
               *horPosLabel,
               *vertScaleLabel,
               *vertOffsetLabel,
               *trigLevelLabel,
               *trigModeLabel,
               *ch1InputLabel,
               *ch2InputLabel,
               *ch3InputLabel,
               *ch4InputLabel;

  QDockWidget *dockPanelRight;

  QWidget     *DPRwidget;

  QGroupBox   *verticalGrpBox,
              *horizontalGrpBox,
              *triggerGrpBox,
              *menuGrpBox;

  QPushButton *ch1Button,
              *ch2Button,
              *ch3Button,
              *ch4Button,
              *chanMenuButton,
              *clearButton,
              *autoButton,
              *runButton,
              *singleButton,
              *horMenuButton,
              *trigModeButton,
              *trigMenuButton,
              *trigForceButton,
              *trig50pctButton,
              *acqButton,
              *cursButton,
              *saveButton,
              *dispButton,
              *utilButton,
              *helpButton,
              *playpauseButton,
              *stopButton,
              *recordButton;

  QDial       *adjDial,
              *horScaleDial,
              *horPosDial,
              *vertScaleDial,
              *vertOffsetDial,
              *trigAdjustDial,
              *navDial;

  QString def_stylesh;

  QPixmap screenXpm;

  struct tmcdev *device;

  TLed *trigModeAutoLed,
       *trigModeNormLed,
       *trigModeSingLed;

  SignalCurve *waveForm;

  int adjDialFunc,
      adjDialCnt,
      navDialFunc;

  int parse_preamble(char *, int, struct waveform_preamble *, int);
  int get_metric_factor(double);
  void get_device_model(const char *);
  double get_stepsize_divide_by_1000(double);

private slots:

  void scrn_timer_handler();
  void stat_timer_handler();
  void adjdial_timer_handler();
  void test_timer_handler();

  void show_about_dialog();
  void open_connection();
  void close_connection();
  void open_settings_dialog();
  int get_device_settings();
  void save_waveform();
  void save_screenshot();

  void adjDialChanged(int);
  void trigAdjustDialChanged(int);
  void horScaleDialChanged(int);
  void horPosDialChanged(int);
  void vertOffsetDialChanged(int);
  void vertScaleDialChanged(int);

  void ch1ButtonClicked();
  void ch2ButtonClicked();
  void ch3ButtonClicked();
  void ch4ButtonClicked();
  void clearButtonClicked();
  void autoButtonClicked();
  void runButtonClicked();
  void singleButtonClicked();
  void horMenuButtonClicked();
  void trigModeButtonClicked();
  void trigMenuButtonClicked();
  void trigForceButtonClicked();
  void trig50pctButtonClicked();
  void acqButtonClicked();
  void cursButtonClicked();
  void saveButtonClicked();
  void dispButtonClicked();
  void utilButtonClicked();
  void helpButtonClicked();

  void horizontal_delayed();

  void trigger_source_ch1();
  void trigger_source_ch2();
  void trigger_source_ch3();
  void trigger_source_ch4();
  void trigger_source_ext();
  void trigger_source_ext5();
  void trigger_source_acl();
  void trigger_coupling_dc();
  void trigger_coupling_ac();
  void trigger_coupling_lfreject();
  void trigger_coupling_hfreject();
  void trigger_slope_pos();
  void trigger_slope_neg();
  void trigger_slope_rfal();
  void trigger_setting_holdoff();

  void horPosDialClicked(QPoint);
  void vertOffsetDialClicked(QPoint);
  void horScaleDialClicked(QPoint);
  void vertScaleDialClicked(QPoint);
  void trigAdjustDialClicked(QPoint);
  void adjustDialClicked(QPoint);

  void navDialReleased();
  void navDial_timer_handler();
  void navDialChanged(int);

  void set_grid_full();
  void set_grid_half();
  void set_grid_none();

  void set_grading_min();
  void set_grading_005();
  void set_grading_01();
  void set_grading_02();
  void set_grading_05();
  void set_grading_1();
  void set_grading_2();
  void set_grading_5();
  void set_grading_10();
  void set_grading_20();
  void set_grading_inf();

  void chan_coupling_ac();
  void chan_coupling_dc();
  void chan_coupling_gnd();
  void chan_bwl_off();
  void chan_bwl_20();
  void chan_bwl_250();
  void chan_invert_on();
  void chan_invert_off();

  void chan_menu();

protected:
  void closeEvent(QCloseEvent *);

};


#endif



