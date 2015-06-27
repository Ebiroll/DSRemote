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




#ifndef SELECT_DEVICE_FORM1_H
#define SELECT_DEVICE_FORM1_H



#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSettings>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

#include "mainwindow.h"
#include "global.h"


class UI_Mainwindow;


class UI_settings_window : public QDialog
{
  Q_OBJECT

public:

  UI_settings_window(QWidget *parent=0);

private:

QPushButton  *cancelButton,
             *applyButton;

QRadioButton *usbRadioButton,
             *lanRadioButton;

QComboBox    *comboBox1;

QLineEdit    *ipLineEdit;

QSpinBox     *refreshSpinbox;

QLabel       *refreshLabel;

UI_Mainwindow *mainwindow;

private slots:

void applyButtonClicked();
void refreshSpinboxChanged(int);

};



#endif


