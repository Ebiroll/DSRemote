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




#include "select_device_dialog.h"



UI_select_device_window::UI_select_device_window(QDialog *parnt)
{
  QSettings settings;

  mainwindow = (UI_Mainwindow *)parnt;

  char dev_str[128];

  setMinimumSize(QSize(400, 200));
  setMaximumSize(QSize(400, 200));
  setWindowTitle("Device settings");
  setModal(true);

  comboBox1 = new QComboBox(this);
  comboBox1->setGeometry(20, 20, 110, 26);
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

  applyButton = new QPushButton(this);
  applyButton->setGeometry(20, 155, 100, 25);
  applyButton->setText("Apply");

  cancelButton = new QPushButton(this);
  cancelButton->setGeometry(280, 155, 100, 25);
  cancelButton->setText("Cancel");

  strcpy(dev_str, settings.value("connection/device").toString().toLocal8Bit().data());

  if(!strcmp(dev_str, ""))
  {
    strcpy(dev_str, "/dev/usbtmc0");
  }

  comboBox1->setCurrentIndex(dev_str[11] - '0');

  QObject::connect(applyButton,  SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
  QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

  exec();
}



void UI_select_device_window::applyButtonClicked()
{
  char dev_str[128];

  QSettings settings;

  strcpy(dev_str, "/dev/usbtmc0");

  dev_str[11] = '0' + comboBox1->currentIndex();

  settings.setValue("connection/device", dev_str);

  close();
}



