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




#include "settings_dialog.h"



UI_settings_window::UI_settings_window(QWidget *parnt)
{
  int err;

  unsigned int ip_addr;

  char dev_str[128];

  QSettings settings;

  mainwindow = (UI_Mainwindow *)parnt;

  setMinimumSize(QSize(490, 200));
  setMaximumSize(QSize(490, 200));
  setWindowTitle("Settings");
  setModal(true);

  usbRadioButton = new QRadioButton("USB", this);
  usbRadioButton->setAutoExclusive(true);
  usbRadioButton->setGeometry(40, 20, 110, 25);
  if(mainwindow->devparms.connectiontype == 0)
  {
    usbRadioButton->setChecked(true);
  }

  lanRadioButton = new QRadioButton("LAN", this);
  lanRadioButton->setAutoExclusive(true);
  lanRadioButton->setGeometry(220, 20, 110, 25);
  if(mainwindow->devparms.connectiontype == 1)
  {
    lanRadioButton->setChecked(true);
  }

  comboBox1 = new QComboBox(this);
  comboBox1->setGeometry(20, 65, 110, 25);
  comboBox1->addItem("/dev/usbtmc0");
  comboBox1->addItem("/dev/usbtmc1");
  comboBox1->addItem("/dev/usbtmc2");
  comboBox1->addItem("/dev/usbtmc3");
  comboBox1->addItem("/dev/usbtmc4");
  comboBox1->addItem("/dev/usbtmc5");
  comboBox1->addItem("/dev/usbtmc6");
  comboBox1->addItem("/dev/usbtmc7");
  comboBox1->addItem("/dev/usbtmc8");
  comboBox1->addItem("/dev/usbtmc9");

  ipSpinbox1 = new QSpinBox(this);
  ipSpinbox1->setGeometry(180, 65, 35, 25);
  ipSpinbox1->setRange(0, 255);
  ipSpinbox1->setSingleStep(1);
  ipSpinbox1->setButtonSymbols(QAbstractSpinBox::NoButtons);
  ipSpinbox1->setAlignment(Qt::AlignHCenter);

  ipSpinbox2 = new QSpinBox(this);
  ipSpinbox2->setGeometry(220, 65, 35, 25);
  ipSpinbox2->setRange(0, 255);
  ipSpinbox2->setSingleStep(1);
  ipSpinbox2->setButtonSymbols(QAbstractSpinBox::NoButtons);
  ipSpinbox2->setAlignment(Qt::AlignHCenter);

  ipSpinbox3 = new QSpinBox(this);
  ipSpinbox3->setGeometry(260, 65, 35, 25);
  ipSpinbox3->setRange(0, 255);
  ipSpinbox3->setSingleStep(1);
  ipSpinbox3->setButtonSymbols(QAbstractSpinBox::NoButtons);
  ipSpinbox3->setAlignment(Qt::AlignHCenter);

  ipSpinbox4 = new QSpinBox(this);
  ipSpinbox4->setGeometry(300, 65, 35, 25);
  ipSpinbox4->setRange(0, 255);
  ipSpinbox4->setSingleStep(1);
  ipSpinbox4->setButtonSymbols(QAbstractSpinBox::NoButtons);
  ipSpinbox4->setAlignment(Qt::AlignHCenter);

  err = 1;

  if(settings.contains("connection/ip"))
  {
    if(strtoipaddr(&ip_addr, settings.value("connection/ip").toString().toLatin1().data()) == 0)
    {
      ipSpinbox1->setValue((ip_addr >> 24) & 0xff);
      ipSpinbox2->setValue((ip_addr >> 16) & 0xff);
      ipSpinbox3->setValue((ip_addr >> 8) & 0xff);
      ipSpinbox4->setValue(ip_addr & 0xff);

      err = 0;
    }
  }

  if(err)
  {
    ipSpinbox1->setValue(192);
    ipSpinbox2->setValue(168);
    ipSpinbox3->setValue(1);
    ipSpinbox4->setValue(88);
  }

  refreshLabel = new QLabel(this);
  refreshLabel->setGeometry(370, 20, 120, 35);
  refreshLabel->setText("Screen update\n interval");

  refreshSpinbox = new QSpinBox(this);
  refreshSpinbox->setGeometry(370, 65, 100, 25);
  refreshSpinbox->setSuffix(" mS");
  refreshSpinbox->setRange(50, 2000);
  refreshSpinbox->setSingleStep(10);
  refreshSpinbox->setValue(mainwindow->devparms.screentimerival);

  applyButton = new QPushButton(this);
  applyButton->setGeometry(20, 155, 100, 25);
  applyButton->setText("Apply");

  cancelButton = new QPushButton(this);
  cancelButton->setGeometry(200, 155, 100, 25);
  cancelButton->setText("Cancel");

  strncpy(dev_str, settings.value("connection/device").toString().toLocal8Bit().data(), 128);

  dev_str[127] = 0;

  if(!strcmp(dev_str, ""))
  {
    strcpy(dev_str, "/dev/usbtmc0");
  }

  comboBox1->setCurrentIndex(dev_str[11] - '0');

  if(mainwindow->devparms.connected)
  {
    usbRadioButton->setEnabled(false);
    lanRadioButton->setEnabled(false);
    ipSpinbox1->setEnabled(false);
    ipSpinbox2->setEnabled(false);
    ipSpinbox3->setEnabled(false);
    ipSpinbox4->setEnabled(false);
    comboBox1->setEnabled(false);
    applyButton->setEnabled(false);
  }
  else
  {
    QObject::connect(applyButton, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
  }

  QObject::connect(cancelButton,   SIGNAL(clicked()),         this, SLOT(close()));
  QObject::connect(refreshSpinbox, SIGNAL(valueChanged(int)), this, SLOT(refreshSpinboxChanged(int)));

  exec();
}


void UI_settings_window::applyButtonClicked()
{
  char dev_str[128];

  QSettings settings;

  if(mainwindow->devparms.connected)
  {
    close();
  }

  strcpy(dev_str, "/dev/usbtmc0");

  dev_str[11] = '0' + comboBox1->currentIndex();

  if(usbRadioButton->isChecked() == true)
  {
    settings.setValue("connection/type", "USB");

    mainwindow->devparms.connectiontype = 0;
  }
  else
  {
    settings.setValue("connection/type", "LAN");

    mainwindow->devparms.connectiontype = 1;
  }

  settings.setValue("connection/device", dev_str);

  sprintf(dev_str, "%i.%i.%i.%i",
          ipSpinbox1->value(), ipSpinbox2->value(), ipSpinbox3->value(), ipSpinbox4->value());

  settings.setValue("connection/ip", dev_str);

  close();
}


void UI_settings_window::refreshSpinboxChanged(int value)
{
  QSettings settings;

  mainwindow->devparms.screentimerival = value;

  settings.setValue("gui/refresh", value);

  if(mainwindow->scrn_timer->isActive())
  {
    mainwindow->scrn_timer->start(value);
  }
}




