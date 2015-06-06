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


#include "mainwindow.h"



UI_Mainwindow::UI_Mainwindow()
{
  int i;

  setMinimumSize(1170, 630);
  setWindowTitle(PROGRAM_NAME " " PROGRAM_VERSION);
  setWindowIcon(QIcon(":/images/r.png"));

  appfont = new QFont;

  monofont = new QFont;

  appfont->setFamily("Arial");
  appfont->setPixelSize(12);

  monofont->setFamily("andale mono");
  monofont->setPixelSize(12);

  QApplication::setFont(*appfont);

  setlocale(LC_NUMERIC, "C");

  QCoreApplication::setOrganizationName("TvB");
  QCoreApplication::setOrganizationDomain("teuniz.net");
  QCoreApplication::setApplicationName(PROGRAM_NAME);

  memset(&devparms, 0, sizeof(struct device_settings));

  devparms.screenshot_buf = (char *)malloc(1024 * 1024 * 2);

  for(i=0; i< MAX_CHNS; i++)
  {
    devparms.wavebuf[i] = (short *)malloc(WAVFRM_MAX_BUFSZ);
  }

  devparms.displaygrid = 2;

  devparms.channel_cnt = 4;

  devparms.timebasescale = 1;

  devparms.hordivisions = 14;

  devparms.status_timer_ival = 200;

  devparms.screen_timer_ival = 500;

  menubar = menuBar();

  devicemenu = new QMenu;
  devicemenu->setTitle("Device");
  devicemenu->addAction("Connect",    this, SLOT(open_connection()));
  devicemenu->addAction("Disconnect", this, SLOT(close_connection()));
  devicemenu->addAction("Exit",       this, SLOT(close()), QKeySequence::Quit);
  menubar->addMenu(devicemenu);

  menubar->addAction("Settings", this, SLOT(open_settings_dialog()));

  helpmenu = new QMenu;
  helpmenu->setTitle("Help");
  helpmenu->addAction("About", this, SLOT(show_about_dialog()));
  menubar->addMenu(helpmenu);

  statusLabel = new QLabel;

  mainLabel = new QLabel();

  vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(mainLabel);

  waveForm = new SignalCurve(this);
  waveForm->setBackgroundColor(Qt::black);
  waveForm->setSignalColor1(Qt::yellow);
  waveForm->setSignalColor2(Qt::cyan);
  waveForm->setSignalColor3(Qt::magenta);
  waveForm->setSignalColor4(QColor(0, 128, 255));
  waveForm->setRasterColor(Qt::darkGray);
  waveForm->setBorderSize(40);
  waveForm->setDeviceParameters(&devparms);

  setCentralWidget(waveForm);

  statusBar = new QStatusBar;
  setStatusBar(statusBar);
  statusBar->addPermanentWidget(statusLabel, 100);
  statusLabel->setText("Disconnected");

  DPRwidget = new QWidget;

  adjDialLabel = new QLabel(DPRwidget);
  adjDialLabel->setGeometry(20, 0, 40, 18);
  adjDialLabel->setStyleSheet("font: 7pt;");
  adjDial = new QDial(DPRwidget);
  adjDial->setWrapping(true);
  adjDial->setNotchesVisible(true);
  adjDial->setGeometry(15, 15, 40, 40);
  adjDial->setSingleStep(1);
  adjDial->setMaximum(100);
  adjDial->setMinimum(0);
  adjDial->setContextMenuPolicy(Qt::CustomContextMenu);

  navDial = new QDial(DPRwidget);
  navDial->setWrapping(false);
  navDial->setNotchesVisible(true);
  navDial->setGeometry(70, 80, 80, 80);
  navDial->setSingleStep(1);
  navDial->setMaximum(100);
  navDial->setMinimum(0);
  navDial->setValue(50);
  navDial->setContextMenuPolicy(Qt::CustomContextMenu);

  clearButton = new QPushButton(DPRwidget);
  def_stylesh = clearButton->styleSheet();
  clearButton->setGeometry(70, 15, 40, 18);
  clearButton->setText("Clear");
  autoButton = new QPushButton(DPRwidget);
  autoButton->setGeometry(125, 15, 40, 18);
  autoButton->setText("Auto");
  autoButton->setStyleSheet("background: #66FF99;");
  runButton = new QPushButton(DPRwidget);
  runButton->setGeometry(180, 15, 70, 18);
  runButton->setText("Run/Stop");
  singleButton = new QPushButton(DPRwidget);
  singleButton->setGeometry(265, 15, 40, 18);
  singleButton->setText("Single");

  playpauseButton = new QPushButton(DPRwidget);
  playpauseButton->setGeometry(20, 75, 25, 25);
  playpauseButton->setStyleSheet("background-image: url(:/images/playpause.png);");
  stopButton = new QPushButton(DPRwidget);
  stopButton->setGeometry(20, 110, 25, 25);
  stopButton->setStyleSheet("background-image: url(:/images/stop.png);");
  recordButton = new QPushButton(DPRwidget);
  recordButton->setGeometry(20, 145, 25, 25);
  recordButton->setStyleSheet("background-image: url(:/images/record.png);");

  menuGrpBox = new QGroupBox("Menu", DPRwidget);
  menuGrpBox->setGeometry(180, 60, 125, 120);

  acqButton = new QPushButton(menuGrpBox);
  acqButton->setGeometry(15, 20, 40, 18);
  acqButton->setText("acq");
  cursButton = new QPushButton(menuGrpBox);
  cursButton->setGeometry(70, 20, 40, 18);
  cursButton->setText("curs");
  saveButton = new QPushButton(menuGrpBox);
  saveButton->setGeometry(15, 55, 40, 18);
  saveButton->setText("save");
  dispButton = new QPushButton(menuGrpBox);
  dispButton->setGeometry(70, 55, 40, 18);
  dispButton->setText("disp");
  utilButton = new QPushButton(menuGrpBox);
  utilButton->setGeometry(15, 90, 40, 18);
  utilButton->setText("util");
  helpButton = new QPushButton(menuGrpBox);
  helpButton->setGeometry(70, 90, 40, 18);
  helpButton->setText("help");

  horizontalGrpBox = new QGroupBox("Horizontal", DPRwidget);
  horizontalGrpBox->setGeometry(5, 190, 180, 90);

  horScaleLabel = new QLabel(horizontalGrpBox);
  horScaleLabel->setGeometry(20, 18, 40, 18);
  horScaleLabel->setText("Scale");
  horScaleLabel->setStyleSheet("font: 7pt;");
  horScaleDial = new QDial(horizontalGrpBox);
  horScaleDial->setGeometry(10, 35, 50, 50);
  horScaleDial->setWrapping(true);
  horScaleDial->setNotchesVisible(true);
  horScaleDial->setSingleStep(1);
  horScaleDial->setMaximum(100);
  horScaleDial->setMinimum(0);
  horScaleDial->setContextMenuPolicy(Qt::CustomContextMenu);
  horMenuButton = new QPushButton(horizontalGrpBox);
  horMenuButton->setGeometry(70, 51, 40, 18);
  horMenuButton->setText("Menu");
  horPosLabel = new QLabel(horizontalGrpBox);
  horPosLabel->setGeometry(128, 21, 40, 18);
  horPosLabel->setText("Position");
  horPosLabel->setStyleSheet("font: 7pt;");
  horPosDial = new QDial(horizontalGrpBox);
  horPosDial->setGeometry(125, 40, 40, 40);
  horPosDial->setWrapping(true);
  horPosDial->setNotchesVisible(true);
  horPosDial->setSingleStep(1);
  horPosDial->setMaximum(100);
  horPosDial->setMinimum(0);
  horPosDial->setContextMenuPolicy(Qt::CustomContextMenu);

  quickGrpBox = new QGroupBox("Quick", DPRwidget);
  quickGrpBox->setGeometry(195, 190, 70, 90);

  measureButton = new QPushButton(quickGrpBox);
  measureButton->setGeometry(15, 30, 40, 18);
  measureButton->setText("Meas");

  verticalGrpBox = new QGroupBox("Vertical", DPRwidget);
  verticalGrpBox->setGeometry(5, 290, 140, 255);

  ch1InputLabel = new QLabel(verticalGrpBox);
  ch1InputLabel->setGeometry(12, 15, 50, 18);
  ch1InputLabel->setStyleSheet("color: #C0C000; font: 7pt;");
  ch1Button = new QPushButton(verticalGrpBox);
  ch1Button->setGeometry(15, 30, 40, 18);
  ch1Button->setText("CH1");
  ch2InputLabel = new QLabel(verticalGrpBox);
  ch2InputLabel->setGeometry(12, 55, 50, 18);
  ch2InputLabel->setStyleSheet("color: #C0C000; font: 7pt;");
  ch2Button = new QPushButton(verticalGrpBox);
  ch2Button->setGeometry(15, 70, 40, 18);
  ch2Button->setText("CH2");
  ch3InputLabel = new QLabel(verticalGrpBox);
  ch3InputLabel->setGeometry(12, 95, 50, 18);
  ch3InputLabel->setStyleSheet("color: #C0C000; font: 7pt;");
  ch3Button = new QPushButton(verticalGrpBox);
  ch3Button->setGeometry(15, 110, 40, 18);
  ch3Button->setText("CH3");
  ch4InputLabel = new QLabel(verticalGrpBox);
  ch4InputLabel->setGeometry(12, 135, 50, 18);
  ch4InputLabel->setStyleSheet("color: #C0C000; font: 7pt;");
  ch4Button = new QPushButton(verticalGrpBox);
  ch4Button->setGeometry(15, 150, 40, 18);
  ch4Button->setText("CH4");
  chanMenuButton = new QPushButton(verticalGrpBox);
  chanMenuButton->setGeometry(15, 190, 40, 18);
  chanMenuButton->setText("Menu");
  vertOffsetLabel = new QLabel(verticalGrpBox);
  vertOffsetLabel->setGeometry(80, 30, 40, 18);
  vertOffsetLabel->setStyleSheet("font: 7pt;");
  vertOffsetLabel->setText("Position");
  vertOffsetDial = new QDial(verticalGrpBox);
  vertOffsetDial->setGeometry(75, 50, 40, 40);
  vertOffsetDial->setWrapping(true);
  vertOffsetDial->setNotchesVisible(true);
  vertOffsetDial->setSingleStep(1);
  vertOffsetDial->setMaximum(100);
  vertOffsetDial->setMinimum(0);
  vertOffsetDial->setContextMenuPolicy(Qt::CustomContextMenu);
  vertScaleLabel = new QLabel(verticalGrpBox);
  vertScaleLabel->setGeometry(80, 103, 40, 18);
  vertScaleLabel->setStyleSheet("font: 7pt;");
  vertScaleLabel->setText("Scale");
  vertScaleDial = new QDial(verticalGrpBox);
  vertScaleDial->setGeometry(70, 120, 50, 50);
  vertScaleDial->setWrapping(true);
  vertScaleDial->setNotchesVisible(true);
  vertScaleDial->setSingleStep(1);
  vertScaleDial->setMaximum(100);
  vertScaleDial->setMinimum(0);
  vertScaleDial->setContextMenuPolicy(Qt::CustomContextMenu);

  triggerGrpBox = new QGroupBox("Trigger", DPRwidget);
  triggerGrpBox->setGeometry(155, 290, 100, 255);

  trigModeLabel = new QLabel(triggerGrpBox);
  trigModeLabel->setGeometry(10, 20, 100, 18);
  trigModeLabel->setText("Auto Normal Single");
  trigModeLabel->setStyleSheet("font: 7pt;");
  trigModeAutoLed = new TLed(triggerGrpBox);
  trigModeAutoLed->setGeometry(20, 45, 10, 10);
  trigModeAutoLed->setOnColor(Qt::yellow);
  trigModeAutoLed->setOffColor(Qt::lightGray);
  trigModeNormLed = new TLed(triggerGrpBox);
  trigModeNormLed->setGeometry(45, 45, 10, 10);
  trigModeNormLed->setOnColor(Qt::yellow);
  trigModeNormLed->setOffColor(Qt::lightGray);
  trigModeSingLed = new TLed(triggerGrpBox);
  trigModeSingLed->setGeometry(70, 45, 10, 10);
  trigModeSingLed->setOnColor(Qt::yellow);
  trigModeSingLed->setOffColor(Qt::lightGray);
  trigModeButton = new QPushButton(triggerGrpBox);
  trigModeButton->setGeometry(30, 65, 40, 18);
  trigModeButton->setText("Mode");
  trigLevelLabel = new QLabel(triggerGrpBox);
  trigLevelLabel->setGeometry(34, 92, 40, 18);
  trigLevelLabel->setText("Level");
  trigAdjustDial = new QDial(triggerGrpBox);
  trigAdjustDial->setGeometry(30, 110, 40, 40);
  trigAdjustDial->setWrapping(true);
  trigAdjustDial->setNotchesVisible(true);
  trigAdjustDial->setSingleStep(1);
  trigAdjustDial->setMaximum(100);
  trigAdjustDial->setMinimum(0);
  trigAdjustDial->setContextMenuPolicy(Qt::CustomContextMenu);
  trigMenuButton = new QPushButton(triggerGrpBox);
  trigMenuButton->setGeometry(30, 160, 40, 18);
  trigMenuButton->setText("Menu");
  trigForceButton = new QPushButton(triggerGrpBox);
  trigForceButton->setGeometry(30, 190, 40, 18);
  trigForceButton->setText("Force");
  trig50pctButton = new QPushButton(triggerGrpBox);
  trig50pctButton->setGeometry(30, 220, 40, 18);
  trig50pctButton->setText("50\%");

  dockPanelRight = new QDockWidget(this);
  dockPanelRight->setFeatures(QDockWidget::DockWidgetFloatable);
  dockPanelRight->setAllowedAreas(Qt::RightDockWidgetArea);
  dockPanelRight->setWidget(DPRwidget);
  dockPanelRight->setMinimumWidth(350);
  dockPanelRight->setMinimumHeight(400);
  addDockWidget(Qt::RightDockWidgetArea, dockPanelRight);

  DPRwidget->setEnabled(false);

  recent_dir[0] = 0;

  recent_savedir[0] = 0;

  device = NULL;

  QSettings settings;

  strcpy(recent_savedir, settings.value("path/savedir").toString().toLocal8Bit().data());

  adjDialFunc = ADJ_DIAL_FUNC_NONE;
  navDialFunc = NAV_DIAL_FUNC_NONE;

  scrn_timer = new QTimer(this);
  stat_timer = new QTimer(this);
  adjdial_timer = new QTimer(this);
  navDial_timer = new QTimer(this);
  navDial_timer->setSingleShot(true);
  test_timer = new QTimer(this);

  connect(scrn_timer,    SIGNAL(timeout()),        this, SLOT(scrn_timer_handler()));
  connect(stat_timer,    SIGNAL(timeout()),        this, SLOT(stat_timer_handler()));
  connect(adjdial_timer, SIGNAL(timeout()),        this, SLOT(adjdial_timer_handler()));
  connect(navDial,       SIGNAL(sliderReleased()), this, SLOT(navDialReleased()));
  connect(navDial_timer, SIGNAL(timeout()),        this, SLOT(navDial_timer_handler()));
  connect(test_timer,    SIGNAL(timeout()),        this, SLOT(test_timer_handler()));

///// TEST /////////////////////////////////////
//   DPRwidget->setEnabled(true);
//
//   connect(adjDial,          SIGNAL(valueChanged(int)), this, SLOT(adjDialChanged(int)));
//   connect(trigAdjustDial,   SIGNAL(valueChanged(int)), this, SLOT(trigAdjustDialChanged(int)));
//   connect(horScaleDial,     SIGNAL(valueChanged(int)), this, SLOT(horScaleDialChanged(int)));
//   connect(horPosDial,       SIGNAL(valueChanged(int)), this, SLOT(horPosDialChanged(int)));
//   connect(vertOffsetDial,   SIGNAL(valueChanged(int)), this, SLOT(vertOffsetDialChanged(int)));
//   connect(vertScaleDial,    SIGNAL(valueChanged(int)), this, SLOT(vertScaleDialChanged(int)));
//   connect(navDial,          SIGNAL(valueChanged(int)), this, SLOT(navDialChanged(int)));
//
//   connect(ch1Button,        SIGNAL(clicked()),     this, SLOT(ch1ButtonClicked()));
//   connect(ch2Button,        SIGNAL(clicked()),     this, SLOT(ch2ButtonClicked()));
//   connect(ch3Button,        SIGNAL(clicked()),     this, SLOT(ch3ButtonClicked()));
//   connect(ch4Button,        SIGNAL(clicked()),     this, SLOT(ch4ButtonClicked()));
//   connect(clearButton,      SIGNAL(clicked()),     this, SLOT(clearButtonClicked()));
//   connect(autoButton,       SIGNAL(clicked()),     this, SLOT(autoButtonClicked()));
//   connect(runButton,        SIGNAL(clicked()),     this, SLOT(runButtonClicked()));
//   connect(singleButton,     SIGNAL(clicked()),     this, SLOT(singleButtonClicked()));
//   connect(horMenuButton,    SIGNAL(clicked()),     this, SLOT(horMenuButtonClicked()));
//   connect(trigModeButton,   SIGNAL(clicked()),     this, SLOT(trigModeButtonClicked()));
//   connect(trigMenuButton,   SIGNAL(clicked()),     this, SLOT(trigMenuButtonClicked()));
//   connect(trigForceButton,  SIGNAL(clicked()),     this, SLOT(trigForceButtonClicked()));
//   connect(trig50pctButton,  SIGNAL(clicked()),     this, SLOT(trig50pctButtonClicked()));
//   connect(acqButton,        SIGNAL(clicked()),     this, SLOT(acqButtonClicked()));
//   connect(cursButton,       SIGNAL(clicked()),     this, SLOT(cursButtonClicked()));
//   connect(saveButton,       SIGNAL(clicked()),     this, SLOT(saveButtonClicked()));
//   connect(dispButton,       SIGNAL(clicked()),     this, SLOT(dispButtonClicked()));
//   connect(utilButton,       SIGNAL(clicked()),     this, SLOT(utilButtonClicked()));
//   connect(helpButton,       SIGNAL(clicked()),     this, SLOT(helpButtonClicked()));
//
//   connect(horPosDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horPosDialClicked(QPoint)));
//   connect(vertOffsetDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertOffsetDialClicked(QPoint)));
//   connect(horScaleDial,   SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horScaleDialClicked(QPoint)));
//   connect(vertScaleDial,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertScaleDialClicked(QPoint)));
//   connect(trigAdjustDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(trigAdjustDialClicked(QPoint)));
//   connect(adjDial,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(adjustDialClicked(QPoint)));
///// TEST /////////////////////////////////////


  show();
}


UI_Mainwindow::~UI_Mainwindow()
{
  QSettings settings;

  settings.setValue("path/savedir", recent_savedir);

  delete appfont;
  delete monofont;

  free(devparms.screenshot_buf);

  for(int i=0; i<MAX_CHNS; i++)
  {
    free(devparms.wavebuf[i]);
  }
}












