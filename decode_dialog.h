/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016 Teunis van Beelen
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


#ifndef UI_DECODE_DIALOG_H
#define UI_DECODE_DIALOG_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Mainwindow;



class UI_decoder_window : public QDialog
{
  Q_OBJECT

public:
  UI_decoder_window(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QWidget *tab_par,
          *tab_uart,
          *tab_spi,
          *tab_iic;

  QTabWidget *tabholder;

  QLabel *spi_clk_src_label,
         *spi_mosi_src_label,
         *spi_miso_src_label,
         *spi_cs_src_label,
         *spi_select_label,
         *spi_mode_label,
         *spi_timeout_label,
         *spi_polarity_label,
         *spi_edge_label,
         *spi_endian_label,
         *spi_width_label,
         *spi_clk_threshold_label,
         *spi_mosi_threshold_label,
         *spi_miso_threshold_label,
         *spi_cs_threshold_label,
         *spi_trace_pos_label;

  QComboBox *spi_clk_src_combobox,
            *spi_mosi_src_combobox,
            *spi_miso_src_combobox,
            *spi_cs_src_combobox,
            *spi_select_combobox,
            *spi_mode_combobox,
            *spi_polarity_combobox,
            *spi_edge_combobox,
            *spi_endian_combobox,
            *spi_threshold_auto_combobox;

  QSpinBox *spi_width_spinbox,
           *spi_trace_pos_spinbox;

  QDoubleSpinBox *spi_timeout_dspinbox,
                 *spi_clk_threshold_dspinbox,
                 *spi_mosi_threshold_dspinbox,
                 *spi_miso_threshold_dspinbox,
                 *spi_cs_threshold_dspinbox;

  QPushButton *close_button,
              *toggle_decode_button;

  struct device_settings *devparms;

private slots:

  void spi_threshold_auto_clicked(int);
  void spi_src_combobox_clicked(int);
  void spi_mosi_threshold_dspinbox_changed();
  void spi_miso_threshold_dspinbox_changed();
  void spi_cs_threshold_dspinbox_changed();
  void spi_clk_threshold_dspinbox_changed();
  void tabholder_index_changed(int);
  void spi_select_combobox_clicked(int);
  void spi_mode_combobox_clicked(int);
  void spi_polarity_combobox_clicked(int);
  void spi_edge_combobox_clicked(int);
  void spi_endian_combobox_clicked(int);
  void spi_width_spinbox_changed();
  void toggle_decode();

};


#endif






