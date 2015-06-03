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
#include "timer_handlers.cpp"
#include "save_data.cpp"
#include "interface.cpp"



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
    devparms.wavebuf[i] = (char *)malloc(WAVFRM_MAX_BUFSZ);
  }

  devparms.displaygrid = 2;

  devparms.channel_cnt = 4;

  devparms.timebasescale = 1;

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


void UI_Mainwindow::open_settings_dialog()
{
  UI_select_device_window sel_device;
}


void UI_Mainwindow::open_connection()
{
  int n;

  char str[1024],
       dev_str[128] = {""},
       resp_str[1024],
       *ptr;

  QSettings settings;

  if(device != NULL)
  {
    return;
  }

  strcpy(dev_str, settings.value("connection/device").toString().toLocal8Bit().data());

  if(!strcmp(dev_str, ""))
  {
    strcpy(dev_str, "/dev/usbtmc0");

    settings.setValue("connection/device", dev_str);
  }

  device = tmcdev_open(dev_str);
  if(device == NULL)
  {
    sprintf(str, "Can not open device %s", dev_str);
    goto OUT_ERROR;
  }

  if(tmcdev_write(device, "*IDN?") != 5)
  {
    sprintf(str, "Can not write to device %s", dev_str);
    goto OUT_ERROR;
  }

  n = tmcdev_read(device);

  if(n < 0)
  {
    sprintf(str, "Can not read from device %s", dev_str);
    goto OUT_ERROR;
  }

  devparms.channel_cnt = 0;

  devparms.bandwidth = 0;

  devparms.modelname[0] = 0;

  strncpy(resp_str, device->buf, 1024);

  device->buf[1023] = 0;

  ptr = strtok(resp_str, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  if(strcmp(ptr, "RIGOL TECHNOLOGIES"))
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  get_device_model(ptr);

  if((!devparms.channel_cnt) || (!devparms.bandwidth))
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  strcpy(devparms.serialnr, ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    snprintf(str, 1024, "Received an unknown identification string from device:\n\n%s\n ", device->buf);
    str[1023] = 0;
    goto OUT_ERROR;
  }

  strcpy(devparms.softwvers, ptr);

  if(strncmp(devparms.modelname, "DS6", 3))
  {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Unsupported device detected.");
    msgBox.setInformativeText("This software has not been tested with your device.\n"
      "It has been tested with the DS6000 series only.\n"
      "If you continue, it's likely that the program will not work correctly at some points.\n"
      "\nDo you want to continue?\n");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if(ret == QMessageBox::No)
    {
      statusLabel->setText("Disconnected");

      devparms.connected = 0;

      close_connection();

      return;
    }
  }

  statusLabel->setText("Reading settings from device...");

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  if(get_device_settings())
  {
    QApplication::restoreOverrideCursor();

    strcpy(str, "Can not read settings from device");
    goto OUT_ERROR;
  }

  QApplication::restoreOverrideCursor();

  if(devparms.channel_cnt < 4)
  {
    ch4Button->setEnabled(false);

    ch4Button->setVisible(false);
  }

  if(devparms.channel_cnt < 3)
  {
    ch3Button->setEnabled(false);

    ch3Button->setVisible(false);
  }

  if(devparms.channel_cnt < 2)
  {
    ch2Button->setEnabled(false);

    ch2Button->setVisible(false);
  }

  connect(adjDial,          SIGNAL(valueChanged(int)), this, SLOT(adjDialChanged(int)));
  connect(trigAdjustDial,   SIGNAL(valueChanged(int)), this, SLOT(trigAdjustDialChanged(int)));
  connect(horScaleDial,     SIGNAL(valueChanged(int)), this, SLOT(horScaleDialChanged(int)));
  connect(horPosDial,       SIGNAL(valueChanged(int)), this, SLOT(horPosDialChanged(int)));
  connect(vertOffsetDial,   SIGNAL(valueChanged(int)), this, SLOT(vertOffsetDialChanged(int)));
  connect(vertScaleDial,    SIGNAL(valueChanged(int)), this, SLOT(vertScaleDialChanged(int)));
  connect(navDial,          SIGNAL(valueChanged(int)), this, SLOT(navDialChanged(int)));

  connect(ch1Button,        SIGNAL(clicked()),      this, SLOT(ch1ButtonClicked()));
  connect(ch2Button,        SIGNAL(clicked()),      this, SLOT(ch2ButtonClicked()));
  connect(ch3Button,        SIGNAL(clicked()),      this, SLOT(ch3ButtonClicked()));
  connect(ch4Button,        SIGNAL(clicked()),      this, SLOT(ch4ButtonClicked()));
  connect(chanMenuButton,   SIGNAL(clicked()),      this, SLOT(chan_menu()));
  connect(waveForm,         SIGNAL(chan1Clicked()), this, SLOT(ch1ButtonClicked()));
  connect(waveForm,         SIGNAL(chan2Clicked()), this, SLOT(ch2ButtonClicked()));
  connect(waveForm,         SIGNAL(chan3Clicked()), this, SLOT(ch3ButtonClicked()));
  connect(waveForm,         SIGNAL(chan4Clicked()), this, SLOT(ch4ButtonClicked()));
  connect(clearButton,      SIGNAL(clicked()),      this, SLOT(clearButtonClicked()));
  connect(autoButton,       SIGNAL(clicked()),      this, SLOT(autoButtonClicked()));
  connect(runButton,        SIGNAL(clicked()),      this, SLOT(runButtonClicked()));
  connect(singleButton,     SIGNAL(clicked()),      this, SLOT(singleButtonClicked()));
  connect(horMenuButton,    SIGNAL(clicked()),      this, SLOT(horMenuButtonClicked()));
  connect(trigModeButton,   SIGNAL(clicked()),      this, SLOT(trigModeButtonClicked()));
  connect(trigMenuButton,   SIGNAL(clicked()),      this, SLOT(trigMenuButtonClicked()));
  connect(trigForceButton,  SIGNAL(clicked()),      this, SLOT(trigForceButtonClicked()));
  connect(trig50pctButton,  SIGNAL(clicked()),      this, SLOT(trig50pctButtonClicked()));
  connect(acqButton,        SIGNAL(clicked()),      this, SLOT(acqButtonClicked()));
  connect(cursButton,       SIGNAL(clicked()),      this, SLOT(cursButtonClicked()));
  connect(saveButton,       SIGNAL(clicked()),      this, SLOT(saveButtonClicked()));
  connect(dispButton,       SIGNAL(clicked()),      this, SLOT(dispButtonClicked()));
  connect(utilButton,       SIGNAL(clicked()),      this, SLOT(utilButtonClicked()));
  connect(helpButton,       SIGNAL(clicked()),      this, SLOT(helpButtonClicked()));

  connect(horPosDial,     SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horPosDialClicked(QPoint)));
  connect(vertOffsetDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertOffsetDialClicked(QPoint)));
  connect(horScaleDial,   SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horScaleDialClicked(QPoint)));
  connect(vertScaleDial,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertScaleDialClicked(QPoint)));
  connect(trigAdjustDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(trigAdjustDialClicked(QPoint)));
  connect(adjDial,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(adjustDialClicked(QPoint)));

  sprintf(str, PROGRAM_NAME " " PROGRAM_VERSION "   %s   %s", devparms.serialnr, devparms.softwvers);

  setWindowTitle(str);

  statusLabel->setText("Connected");

  devparms.connected = 1;

  stat_timer->start(STAT_TIMER_IVAL);

//  test_timer->start(2000);

  DPRwidget->setEnabled(true);

  scrn_timer->start(SCRN_TIMER_IVAL);

  return;

OUT_ERROR:

  statusLabel->setText("Disconnected");

  devparms.connected = 0;

  QMessageBox mesgbox;
  mesgbox.setIcon(QMessageBox::Critical);
  mesgbox.setText(str);
  mesgbox.exec();

  close_connection();
}


void UI_Mainwindow::close_connection()
{
  DPRwidget->setEnabled(false);

  test_timer->stop();

  scrn_timer->stop();

  stat_timer->stop();

  adjdial_timer->stop();

  setWindowTitle(PROGRAM_NAME " " PROGRAM_VERSION);

  disconnect(adjDial,         SIGNAL(valueChanged(int)), this, SLOT(adjDialChanged(int)));
  disconnect(trigAdjustDial,  SIGNAL(valueChanged(int)), this, SLOT(trigAdjustDialChanged(int)));
  disconnect(horScaleDial,    SIGNAL(valueChanged(int)), this, SLOT(horScaleDialChanged(int)));
  disconnect(horPosDial,      SIGNAL(valueChanged(int)), this, SLOT(horPosDialChanged(int)));
  disconnect(vertOffsetDial,  SIGNAL(valueChanged(int)), this, SLOT(vertOffsetDialChanged(int)));
  disconnect(vertScaleDial,   SIGNAL(valueChanged(int)), this, SLOT(vertScaleDialChanged(int)));
  disconnect(navDial,          SIGNAL(valueChanged(int)), this, SLOT(navDialChanged(int)));

  disconnect(ch1Button,        SIGNAL(clicked()),     this, SLOT(ch1ButtonClicked()));
  disconnect(ch2Button,        SIGNAL(clicked()),     this, SLOT(ch2ButtonClicked()));
  disconnect(ch3Button,        SIGNAL(clicked()),     this, SLOT(ch3ButtonClicked()));
  disconnect(ch4Button,        SIGNAL(clicked()),     this, SLOT(ch4ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan1Clicked()), this, SLOT(ch1ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan2Clicked()), this, SLOT(ch2ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan3Clicked()), this, SLOT(ch3ButtonClicked()));
  disconnect(waveForm,         SIGNAL(chan4Clicked()), this, SLOT(ch4ButtonClicked()));
  disconnect(clearButton,      SIGNAL(clicked()),     this, SLOT(clearButtonClicked()));
  disconnect(autoButton,       SIGNAL(clicked()),     this, SLOT(autoButtonClicked()));
  disconnect(runButton,        SIGNAL(clicked()),     this, SLOT(runButtonClicked()));
  disconnect(singleButton,     SIGNAL(clicked()),     this, SLOT(singleButtonClicked()));
  disconnect(horMenuButton,    SIGNAL(clicked()),     this, SLOT(horMenuButtonClicked()));
  disconnect(trigModeButton,   SIGNAL(clicked()),     this, SLOT(trigModeButtonClicked()));
  disconnect(trigMenuButton,   SIGNAL(clicked()),     this, SLOT(trigMenuButtonClicked()));
  disconnect(trigForceButton,  SIGNAL(clicked()),     this, SLOT(trigForceButtonClicked()));
  disconnect(trig50pctButton,  SIGNAL(clicked()),     this, SLOT(trig50pctButtonClicked()));
  disconnect(acqButton,        SIGNAL(clicked()),     this, SLOT(acqButtonClicked()));
  disconnect(cursButton,       SIGNAL(clicked()),     this, SLOT(cursButtonClicked()));
  disconnect(saveButton,       SIGNAL(clicked()),     this, SLOT(saveButtonClicked()));
  disconnect(dispButton,       SIGNAL(clicked()),     this, SLOT(dispButtonClicked()));
  disconnect(utilButton,       SIGNAL(clicked()),     this, SLOT(utilButtonClicked()));
  disconnect(helpButton,       SIGNAL(clicked()),     this, SLOT(helpButtonClicked()));

  disconnect(horPosDial,     SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horPosDialClicked(QPoint)));
  disconnect(vertOffsetDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertOffsetDialClicked(QPoint)));
  disconnect(horScaleDial,   SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(horScaleDialClicked(QPoint)));
  disconnect(vertScaleDial,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(vertScaleDialClicked(QPoint)));
  disconnect(trigAdjustDial, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(trigAdjustDialClicked(QPoint)));
  disconnect(adjDial,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(adjustDialClicked(QPoint)));

  devparms.connected = 0;

  waveForm->clear();

  tmcdev_close(device);

  device = NULL;

  statusLabel->setText("Disconnected");
}


void UI_Mainwindow::closeEvent(QCloseEvent *cl_event)
{
  test_timer->stop();

  scrn_timer->stop();

  stat_timer->stop();

  adjdial_timer->stop();

  tmcdev_close(device);

  device = NULL;

  cl_event->accept();
}


int UI_Mainwindow::get_device_settings()
{
  int chn, line;

  char str[512];

  devparms.activechannel = -1;

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    sprintf(str, ":CHAN%i:BWL?", chn + 1);

    if(tmcdev_write(device, str) != 11)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "20M"))
    {
      devparms.chanbwlimit[chn] = 20;
    }
    else if(!strcmp(device->buf, "250M"))
      {
        devparms.chanbwlimit[chn] = 250;
      }
      else if(!strcmp(device->buf, "OFF"))
        {
          devparms.chanbwlimit[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:COUP?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "AC"))
    {
      devparms.chancoupling[chn] = 2;
    }
    else if(!strcmp(device->buf, "DC"))
      {
        devparms.chancoupling[chn] = 1;
      }
      else if(!strcmp(device->buf, "GND"))
        {
          devparms.chancoupling[chn] = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    sprintf(str, ":CHAN%i:DISP?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      switch(chn)
      {
        case 0: ch1Button->setStyleSheet(def_stylesh);
                break;
        case 1: ch2Button->setStyleSheet(def_stylesh);
                break;
        case 2: ch3Button->setStyleSheet(def_stylesh);
                break;
        case 3: ch4Button->setStyleSheet(def_stylesh);
                break;
      }

      devparms.chandisplay[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        switch(chn)
        {
          case 0: ch1Button->setStyleSheet("background: #FFFF33;");
                  break;
          case 1: ch2Button->setStyleSheet("background: #33FFFF;");
                  break;
          case 2: ch3Button->setStyleSheet("background: #FF33FF;");
                  break;
          case 3: ch4Button->setStyleSheet("background: #0080FF;");
                  break;
        }

        devparms.chandisplay[chn] = 1;

        if(devparms.activechannel == -1)
        {
          devparms.activechannel = chn;
        }
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    sprintf(str, ":CHAN%i:IMP?", chn + 1);

    if(tmcdev_write(device, str) != 11)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "OMEG"))
    {
      devparms.chanimpedance[chn] = 0;
    }
    else if(!strcmp(device->buf, "FIFT"))
      {
        devparms.chanimpedance[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    sprintf(str, ":CHAN%i:INV?", chn + 1);

    if(tmcdev_write(device, str) != 11)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.chaninvert[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.chaninvert[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

    sprintf(str, ":CHAN%i:OFFS?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanoffset[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:PROB?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanprobe[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:SCAL?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.chanscale[chn] = atof(device->buf);

    sprintf(str, ":CHAN%i:VERN?", chn + 1);

    if(tmcdev_write(device, str) != 12)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "0"))
    {
      devparms.chanvernier[chn] = 0;
    }
    else if(!strcmp(device->buf, "1"))
      {
        devparms.chanvernier[chn] = 1;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }
  }

  if(tmcdev_write(device, ":TIM:OFFS?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebaseoffset = atof(device->buf);

  if(tmcdev_write(device, ":TIM:SCAL?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasescale = atof(device->buf);

  if(tmcdev_write(device, ":TIM:DEL:ENAB?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms.timebasedelayenable = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms.timebasedelayenable = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  if(tmcdev_write(device, ":TIM:DEL:OFFS?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasedelayoffset = atof(device->buf);

  if(tmcdev_write(device, ":TIM:DEL:SCAL?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasedelayscale = atof(device->buf);

  if(tmcdev_write(device, ":TIM:HREF:MODE?") != 15)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "CENT"))
  {
    devparms.timebasehrefmode = 0;
  }
  else if(!strcmp(device->buf, "TPOS"))
    {
      devparms.timebasehrefmode = 1;
    }
    else if(!strcmp(device->buf, "USER"))
      {
        devparms.timebasehrefmode = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(tmcdev_write(device, ":TIM:HREF:POS?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.timebasehrefpos = atoi(device->buf);

  if(tmcdev_write(device, ":TIM:MODE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "MAIN"))
  {
    devparms.timebasemode = 0;
  }
  else if(!strcmp(device->buf, "XY"))
    {
      devparms.timebasemode = 1;
    }
    else if(!strcmp(device->buf, "ROLL"))
      {
        devparms.timebasemode = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(tmcdev_write(device, ":TIM:VERN?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms.timebasevernier = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms.timebasevernier = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  if(tmcdev_write(device, ":TIM:XY1:DISP?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms.timebasexy1display = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms.timebasexy1display = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  if(tmcdev_write(device, ":TIM:XY2:DISP?") != 14)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "0"))
  {
    devparms.timebasexy2display = 0;
  }
  else if(!strcmp(device->buf, "1"))
    {
      devparms.timebasexy2display = 1;
    }
    else
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

  if(tmcdev_write(device, ":TRIG:COUP?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "AC"))
  {
    devparms.triggercoupling = 0;
  }
  else if(!strcmp(device->buf, "DC"))
    {
      devparms.triggercoupling = 1;
    }
    else if(!strcmp(device->buf, "LFR"))
      {
        devparms.triggercoupling = 2;
      }
      else if(!strcmp(device->buf, "HFR"))
        {
          devparms.triggercoupling = 3;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

  if(tmcdev_write(device, ":TRIG:SWE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "AUTO"))
  {
    devparms.triggersweep = 0;

    trigModeAutoLed->setValue(true);
    trigModeNormLed->setValue(false);
    trigModeSingLed->setValue(false);
  }
  else if(!strcmp(device->buf, "NORM"))
    {
      devparms.triggersweep = 1;

      trigModeAutoLed->setValue(false);
      trigModeNormLed->setValue(true);
      trigModeSingLed->setValue(false);
    }
    else if(!strcmp(device->buf, "SING"))
      {
        devparms.triggersweep = 2;

        trigModeAutoLed->setValue(false);
        trigModeNormLed->setValue(false);
        trigModeSingLed->setValue(true);
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(tmcdev_write(device, ":TRIG:MODE?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "EDGE"))
  {
    devparms.triggermode = 0;
  }
  else if(!strcmp(device->buf, "PULS"))
    {
      devparms.triggermode = 1;
    }
    else if(!strcmp(device->buf, "SLOP"))
      {
        devparms.triggermode = 2;
      }
      else if(!strcmp(device->buf, "VID"))
        {
          devparms.triggermode = 3;
        }
        else if(!strcmp(device->buf, "PATT"))
          {
            devparms.triggermode = 4;
          }
          else if(!strcmp(device->buf, "RS232"))
            {
              devparms.triggermode = 5;
            }
            else if(!strcmp(device->buf, "IIC"))
              {
                devparms.triggermode = 6;
              }
              else if(!strcmp(device->buf, "SPI"))
                {
                  devparms.triggermode = 7;
                }
                else if(!strcmp(device->buf, "CAN"))
                  {
                    devparms.triggermode = 8;
                  }
                  else if(!strcmp(device->buf, "USB"))
                    {
                      devparms.triggermode = 9;
                    }
                    else
                    {
                      line = __LINE__;
                      goto OUT_ERROR;
                    }

  if(tmcdev_write(device, ":TRIG:STAT?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "TD"))
  {
    devparms.triggerstatus = 0;
  }
  else if(!strcmp(device->buf, "WAIT"))
    {
      devparms.triggerstatus = 1;
    }
    else if(!strcmp(device->buf, "RUN"))
      {
        devparms.triggerstatus = 2;
      }
      else if(!strcmp(device->buf, "AUTO"))
        {
          devparms.triggerstatus = 3;
        }
        else if(!strcmp(device->buf, "FIN"))
          {
            devparms.triggerstatus = 4;
          }
          else if(!strcmp(device->buf, "STOP"))
            {
              devparms.triggerstatus = 5;
            }
            else
            {
              line = __LINE__;
              goto OUT_ERROR;
            }

  if(tmcdev_write(device, ":TRIG:EDG:SLOP?") != 15)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "POS"))
  {
    devparms.triggeredgeslope = 0;
  }
  else if(!strcmp(device->buf, "NEG"))
    {
      devparms.triggeredgeslope = 1;
    }
    else if(!strcmp(device->buf, "RFAL"))
      {
        devparms.triggeredgeslope = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  if(tmcdev_write(device, ":TRIG:EDG:SOUR?") != 15)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "CHAN1"))
  {
    devparms.triggeredgesource = 0;
  }
  else if(!strcmp(device->buf, "CHAN2"))
    {
      devparms.triggeredgesource = 1;
    }
    else if(!strcmp(device->buf, "CHAN3"))
      {
        devparms.triggeredgesource = 2;
      }
      else if(!strcmp(device->buf, "CHAN4"))
        {
          devparms.triggeredgesource = 3;
        }
        else if(!strcmp(device->buf, "EXT"))
          {
            devparms.triggeredgesource = 4;
          }
          else if(!strcmp(device->buf, "EXT5"))
            {
              devparms.triggeredgesource = 5;
            }
            else if(!strcmp(device->buf, "ACL"))
              {
                devparms.triggeredgesource = 6;
              }
              else
              {
                line = __LINE__;
                goto OUT_ERROR;
              }

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", chn + 1);

    if(tmcdev_write(device, str) != 20)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_write(device, ":TRIG:EDG:LEV?") != 14)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmcdev_read(device) < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    devparms.triggeredgelevel[chn] = atof(device->buf);
  }

  if(devparms.triggeredgesource < 4)
  {
    sprintf(str, ":TRIG:EDG:SOUR CHAN%i", devparms.triggeredgesource + 1);

    if(tmcdev_write(device, str) != 20)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 4)
  {
    if(tmcdev_write(device, ":TRIG:EDG:SOUR EXT") != 18)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 5)
  {
    if(tmcdev_write(device, ":TRIG:EDG:SOUR EXT5") != 19)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(devparms.triggeredgesource== 6)
  {
    if(tmcdev_write(device, ":TRIG:EDG:SOUR AC") != 17)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }
  }

  if(tmcdev_write(device, ":TRIG:HOLD?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.triggerholdoff = atof(device->buf);

  for(chn=0; chn<devparms.channel_cnt; chn++)
  {
    str[0] = 0;

    if(devparms.chancoupling[chn] == 2)
    {
      strcat(str, "AC");
    }

    if(devparms.chanimpedance[chn])
    {
      strcat(str, " 50");
    }

    if(devparms.chanbwlimit[chn])
    {
      strcat(str, " BW");
    }

    switch(chn)
    {
      case 0: ch1InputLabel->setText(str);
              break;
      case 1: ch2InputLabel->setText(str);
              break;
      case 2: ch3InputLabel->setText(str);
              break;
      case 3: ch4InputLabel->setText(str);
              break;
    }
  }

  if(tmcdev_write(device, ":ACQ:SRAT?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  devparms.samplerate = atof(device->buf);

  if(tmcdev_write(device, ":DISP:GRID?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmcdev_read(device) < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "NONE"))
  {
    devparms.displaygrid = 0;
  }
  else if(!strcmp(device->buf, "HALF"))
    {
      devparms.displaygrid = 1;
    }
    else if(!strcmp(device->buf, "FULL"))
      {
        devparms.displaygrid = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  return 0;

OUT_ERROR:

  sprintf(str, "An error occurred while reading settings from device.\n"
               "File %s line %i", __FILE__, line);

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(str);
  msgBox.exec();

  return -1;
}


int UI_Mainwindow::parse_preamble(char *str, int sz, struct waveform_preamble *wfp, int chn)
{
  char *ptr;

  if(sz < 19)
  {
    return -1;
  }

  ptr = strtok(str, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->format = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->type = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->points = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->count = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xincrement[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xorigin[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->xreference[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yincrement[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yorigin[chn] = atof(ptr);

  ptr = strtok(NULL, ",");
  if(ptr == NULL)
  {
    return -1;
  }

  wfp->yreference[chn] = atoi(ptr);

  ptr = strtok(NULL, ",");
  if(ptr != NULL)
  {
    return -1;
  }

  return 0;
}


int UI_Mainwindow::get_metric_factor(double value)
{
  int suffix=0;

  if(value < 0)
  {
      value *= -1;
  }

  if(value >= 1e12 && value < 1e15)
  {
      suffix = 12;
  }
  else if(value >= 1e9 && value < 1e12)
    {
        suffix = 9;
    }
    else if(value >= 1e6 && value < 1e9)
      {
          suffix = 6;
      }
      else if(value >= 1e3 && value < 1e6)
        {
          suffix = 3;
        }
        else if(value >= 1e-3 && value < 1)
          {
            suffix = -3;
          }
          else if( value >= 1e-6 && value < 1e-3)
            {
              suffix = -6;
            }
            else if(value >= 1e-9 && value < 1e-6)
              {
                suffix = -9;
              }
              else if(value >= 1e-12 && value < 1e-9)
                {
                  suffix = -12;
                }

  return suffix;
}


double UI_Mainwindow::get_stepsize_divide_by_1000(double val)
{
  int exp=0;

  while(val < 1)
  {
    val *= 10;

    exp--;
  }

  while(val >= 10)
  {
    val /= 10;

    exp++;
  }

  return(exp10(exp - 2));
}


void UI_Mainwindow::get_device_model(const char *str)
{
  devparms.channel_cnt = 0;

  devparms.bandwidth = 0;

  if(!strcmp(str, "DS6104"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 1000;
  }

  if(!strcmp(str, "DS6064"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 600;
  }

  if(!strcmp(str, "DS6102"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 1000;
  }

  if(!strcmp(str, "DS6062"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 600;
  }

  if(!strcmp(str, "DS4012"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;
  }

  if(!strcmp(str, "DS4014"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;
  }

  if(!strcmp(str, "DS4022"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;
  }

  if(!strcmp(str, "DS4024"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 200;
  }

  if(!strcmp(str, "DS4032"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 350;
  }

  if(!strcmp(str, "DS4034"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 350;
  }

  if(!strcmp(str, "DS4052"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 500;
  }

  if(!strcmp(str, "DS4054"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 500;
  }

  if(!strcmp(str, "DS2072A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 70;
  }

  if(!strcmp(str, "DS2072A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 70;
  }

  if(!strcmp(str, "DS2102A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;
  }

  if(!strcmp(str, "DS2102A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 100;
  }

  if(!strcmp(str, "DS2202A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;
  }

  if(!strcmp(str, "DS2202A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 200;
  }

  if(!strcmp(str, "DS2302A"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 300;
  }

  if(!strcmp(str, "DS2302A-S"))
  {
    devparms.channel_cnt = 2;

    devparms.bandwidth = 300;
  }

  if(!strcmp(str, "DS1054Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 50;
  }

  if(!strcmp(str, "DS1074Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 70;
  }

  if(!strcmp(str, "DS1074Z-S"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 70;
  }

  if(!strcmp(str, "DS1104Z"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;
  }

  if(!strcmp(str, "DS1104Z-S"))
  {
    devparms.channel_cnt = 4;

    devparms.bandwidth = 100;
  }

  if(devparms.channel_cnt && devparms.bandwidth)
  {
    strcpy(devparms.modelname, str);
  }
}

















