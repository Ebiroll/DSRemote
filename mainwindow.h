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


#ifndef UI_VIEW_MAINFORM_H
#define UI_VIEW_MAINFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
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
#include <QImage>
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
#include <QProgressDialog>
#include <QKeySequence>
#include <QEventLoop>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global.h"
#include "about_dialog.h"
#include "utils.h"
#include "connection.h"
#include "tmc_dev.h"
#include "tled.h"
#include "edflib.h"
#include "signalcurve.h"
#include "settings_dialog.h"
#include "screen_thread.h"
#include "lan_connect_thread.h"
#include "read_settings_thread.h"
#include "save_data_thread.h"
#include "decode_dialog.h"

#include "third_party/kiss_fft/kiss_fftr.h"


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

  void set_cue_cmd(const char *);

  struct device_settings devparms;

  QLabel       *statusLabel;

  QFont        *appfont,
               *monofont;

  int adjDialFunc,
      navDialFunc;

  QTimer       *scrn_timer,
               *label_timer;

  screen_thread *scrn_thread;

private:

  QMenuBar     *menubar;

  QMenu        *devicemenu,
               *settingsmenu,
               *helpmenu;

  QTimer       *adjdial_timer,
               *navDial_timer,
               *horPosDial_timer,
               *trigAdjDial_timer,
               *vertOffsDial_timer,
               *horScaleDial_timer,
               *vertScaleDial_timer,
               *test_timer;

  QStatusBar   *statusBar;

  QLabel       *adjDialLabel,
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
              *menuGrpBox,
              *quickGrpBox;

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
              *recordButton,
              *measureButton,
              *mathMenuButton;

  QDial       *adjDial,
              *horScaleDial,
              *horPosDial,
              *vertScaleDial,
              *vertOffsetDial,
              *trigAdjustDial,
              *navDial;

  QString def_stylesh;

  QImage screenXpm;

  QAction *former_page_act,
          *shift_page_left_act,
          *shift_page_right_act,
          *next_page_act,
          *zoom_in_act,
          *zoom_out_act,
          *chan_scale_plus_act,
          *chan_scale_minus_act,
          *shift_trace_up_act,
          *shift_trace_down_act,
          *select_chan1_act,
          *select_chan2_act,
          *select_chan3_act,
          *select_chan4_act,
          *toggle_fft_act;

  struct tmcdev *device;

  TLed *trigModeAutoLed,
       *trigModeNormLed,
       *trigModeSingLed;

  SignalCurve *waveForm;

  int parse_preamble(char *, int, struct waveform_preamble *, int);
  int get_metric_factor(double);
  void get_device_model(const char *);
  double get_stepsize_divide_by_1000(double);
  void serial_decoder(void);

private slots:

  void scrn_timer_handler();
  void screenUpdate();
  void adjdial_timer_handler();
  void label_timer_handler();
  void test_timer_handler();
  void horPosDial_timer_handler();
  void trigAdjDial_timer_handler();
  void vertOffsDial_timer_handler();
  void horScaleDial_timer_handler();
  void vertScaleDial_timer_handler();

  void show_about_dialog();
  void show_howto_operate();
  void open_connection();
  void close_connection();
  void open_settings_dialog();
  int get_device_settings();
  void save_screen_waveform();
  void save_memory_waveform();
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
  void measureButtonClicked();

  void horizontal_delayed_toggle();
  void horizontal_delayed_on();
  void horizontal_delayed_off();

  void counter_off();
  void counter_ch1();
  void counter_ch2();
  void counter_ch3();
  void counter_ch4();

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

  void set_grid_type_vectors();
  void set_grid_type_dots();

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
  void math_menu();

  void set_acq_normal();
  void set_acq_average();
  void set_acq_peak();
  void set_acq_hres();

  void set_memdepth(int);
  void set_memdepth_auto();
  void set_memdepth_12k();
  void set_memdepth_120k();
  void set_memdepth_1200k();
  void set_memdepth_12m();
  void set_memdepth_24m();
  void set_memdepth_6k();
  void set_memdepth_60k();
  void set_memdepth_600k();
  void set_memdepth_6m();
  void set_memdepth_3k();
  void set_memdepth_30k();
  void set_memdepth_300k();
  void set_memdepth_3m();
  void set_memdepth_7k();
  void set_memdepth_70k();
  void set_memdepth_700k();
  void set_memdepth_7m();
  void set_memdepth_70m();
  void set_memdepth_14k();
  void set_memdepth_140k();
  void set_memdepth_1400k();
  void set_memdepth_14m();
  void set_memdepth_140m();
  void set_memdepth_28m();
  void set_memdepth_56m();

  void former_page();
  void shift_page_left();
  void shift_page_right();
  void next_page();
  void zoom_in();
  void zoom_out();
  void chan_scale_plus();
  void chan_scale_minus();
  void shift_trace_up();
  void shift_trace_down();

  void set_to_factory();

  void toggle_fft();
  void toggle_fft_split();
  void toggle_fft_unit();
  void select_fft_ch1();
  void select_fft_ch2();
  void select_fft_ch3();
  void select_fft_ch4();
  void select_fft_hzdiv_20();
  void select_fft_hzdiv_40();
  void select_fft_hzdiv_80();
  void select_fft_hzdiv_100();
  void select_fft_hzdiv_200();
  void set_fft_hzdiv(double);
  void select_fft_ctr_5();
  void select_fft_ctr_6();
  void select_fft_ctr_7();
  void select_fft_ctr_8();
  void select_fft_ctr_9();
  void select_fft_ctr_10();
  void select_fft_ctr_11();
  void select_fft_ctr_12();
  void select_fft_vscale1();
  void select_fft_vscale2();
  void select_fft_vscale5();
  void select_fft_vscale10();
  void select_fft_vscale20();
  void set_fft_vscale();
  void select_fft_voffsetp4();
  void select_fft_voffsetp3();
  void select_fft_voffsetp2();
  void select_fft_voffsetp1();
  void select_fft_voffset0();
  void select_fft_voffsetm1();
  void select_fft_voffsetm2();
  void select_fft_voffsetm3();
  void select_fft_voffsetm4();
  void set_fft_voffset();

  void show_decode_window();

  void updateLabels();

protected:
  void closeEvent(QCloseEvent *);

};


#endif



