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
         *uart_tx_src_label,
         *uart_rx_src_label,
         *uart_polarity_label,
         *uart_endian_label,
         *uart_baud_label,
         *uart_width_label,
         *uart_stop_label,
         *uart_parity_label,
         *threshold_1_label,
         *threshold_2_label,
         *threshold_3_label,
         *threshold_4_label,
         *trace_pos_label,
         *format_label;

  QComboBox *spi_clk_src_combobox,
            *spi_mosi_src_combobox,
            *spi_miso_src_combobox,
            *spi_cs_src_combobox,
            *spi_select_combobox,
            *spi_mode_combobox,
            *spi_polarity_combobox,
            *spi_edge_combobox,
            *spi_endian_combobox,
            *uart_tx_src_combobox,
            *uart_rx_src_combobox,
            *uart_polarity_combobox,
            *uart_endian_combobox,
            *uart_width_combobox,
            *uart_stop_combobox,
            *uart_parity_combobox,
            *threshold_auto_combobox,
            *format_combobox;

  QSpinBox *spi_width_spinbox,
           *uart_baud_spinbox,
           *trace_pos_spinbox;

  QDoubleSpinBox *spi_timeout_dspinbox,
                 *threshold_1_dspinbox,
                 *threshold_2_dspinbox,
                 *threshold_3_dspinbox,
                 *threshold_4_dspinbox;

  QPushButton *close_button,
              *toggle_decode_button;

  struct device_settings *devparms;

private slots:

  void threshold_auto_clicked(int);
  void threshold_1_dspinbox_changed();
  void threshold_2_dspinbox_changed();
  void threshold_3_dspinbox_changed();
  void threshold_4_dspinbox_changed();
  void tabholder_index_changed(int);
  void src_combobox_clicked(int);
  void spi_select_combobox_clicked(int);
  void spi_mode_combobox_clicked(int);
  void spi_polarity_combobox_clicked(int);
  void spi_edge_combobox_clicked(int);
  void spi_endian_combobox_clicked(int);
  void spi_width_spinbox_changed();
  void spi_timeout_dspinbox_changed();
  void uart_baud_spinbox_changed();
  void uart_polarity_combobox_clicked(int);
  void uart_endian_combobox_clicked(int);
  void uart_width_combobox_clicked(int);
  void uart_stop_combobox_clicked(int);
  void uart_parity_combobox_clicked(int);
  void trace_pos_spinbox_changed();
  void toggle_decode();
  void format_combobox_clicked(int);

};


#endif






