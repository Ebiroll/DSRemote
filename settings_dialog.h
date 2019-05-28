/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016, 2017, 2018, 2019 Teunis van Beelen
*
* Email: teuniz@protonmail.com
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




#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H



#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSettings>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
             *lanIPRadioButton;

QComboBox    *comboBox1;

QSpinBox     *refreshSpinbox,
             *ipSpinbox1,
             *ipSpinbox2,
             *ipSpinbox3,
             *ipSpinbox4;

QLabel       *refreshLabel,
             *invScrShtLabel,
             *showfpsLabel,
             *extendvertdivLabel,
             *hostnameLabel;

QCheckBox    *invScrShtCheckbox,
             *showfpsCheckbox,
             *extendvertdivCheckbox;

QLineEdit     *HostLineEdit;

UI_Mainwindow *mainwindow;

private slots:

void applyButtonClicked();
void refreshSpinboxChanged(int);
void invScrShtCheckboxChanged(int);
void showfpsCheckboxChanged(int);
void extendvertdivCheckboxChanged(int);
void hostnamechanged(QString);

};



#endif


