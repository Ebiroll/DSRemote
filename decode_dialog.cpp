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


#include "decode_dialog.h"



UI_decoder_window::UI_decoder_window(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  devparms = &mainwindow->devparms;

  setWindowTitle("Decode");

  setMinimumSize(690, 525);
  setMaximumSize(690, 525);

  tabholder = new QTabWidget(this);
  tabholder->setGeometry(0, 0, 250, 455);

  tab_par = new QWidget;
  tab_uart = new QWidget;
  tab_spi = new QWidget;
  tab_iic = new QWidget;

  spi_clk_src_label = new QLabel(tab_spi);
  spi_clk_src_label->setGeometry(10, 20, 100, 25);
  spi_clk_src_label->setText("Clock");

  spi_clk_src_combobox = new QComboBox(tab_spi);
  spi_clk_src_combobox->setGeometry(130, 20, 100, 25);
  spi_clk_src_combobox->addItem("Ch. 1");
  spi_clk_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    spi_clk_src_combobox->addItem("Ch. 3");
    spi_clk_src_combobox->addItem("Ch. 4");
  }
  spi_clk_src_combobox->setCurrentIndex(devparms->math_decode_spi_clk);

  threshold_1_label = new QLabel(this);
  threshold_1_label->setGeometry(270, 42, 100, 25);
  threshold_1_label->setText("Threshold");

  threshold_1_dspinbox = new QDoubleSpinBox(this);
  threshold_1_dspinbox->setGeometry(370, 42, 100, 25);
  threshold_1_dspinbox->setDecimals(3);
  threshold_1_dspinbox->setRange(-20.0, 20.0);

  threshold_auto_combobox = new QComboBox(this);
  threshold_auto_combobox->setGeometry(510, 42, 140, 25);
  threshold_auto_combobox->addItem("Manual threshold");
  threshold_auto_combobox->addItem("Auto threshold");
  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold_auto = 0;
    threshold_auto_combobox->setCurrentIndex(0);
    threshold_auto_combobox->setEnabled(false);
  }
  else
  {
    threshold_auto_combobox->setCurrentIndex(devparms->math_decode_threshold_auto);
  }

  spi_mosi_src_label = new QLabel(tab_spi);
  spi_mosi_src_label->setGeometry(10, 55, 100, 25);
  spi_mosi_src_label->setText("MOSI");

  spi_mosi_src_combobox = new QComboBox(tab_spi);
  spi_mosi_src_combobox->setGeometry(130, 55, 100, 25);
  spi_mosi_src_combobox->addItem("Off");
  spi_mosi_src_combobox->addItem("Ch. 1");
  spi_mosi_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    spi_mosi_src_combobox->addItem("Ch. 3");
    spi_mosi_src_combobox->addItem("Ch. 4");
  }
  spi_mosi_src_combobox->setCurrentIndex(devparms->math_decode_spi_mosi);

  threshold_2_label = new QLabel(this);
  threshold_2_label->setGeometry(270, 77, 100, 25);
  threshold_2_label->setText("Threshold");

  threshold_2_dspinbox = new QDoubleSpinBox(this);
  threshold_2_dspinbox->setGeometry(370, 77, 100, 25);
  threshold_2_dspinbox->setDecimals(3);
  threshold_2_dspinbox->setRange(-20.0, 20.0);

  spi_miso_src_label = new QLabel(tab_spi);
  spi_miso_src_label->setGeometry(10, 90, 100, 25);
  spi_miso_src_label->setText("MISO");

  spi_miso_src_combobox = new QComboBox(tab_spi);
  spi_miso_src_combobox->setGeometry(130, 90, 100, 25);
  spi_miso_src_combobox->addItem("Off");
  spi_miso_src_combobox->addItem("Ch. 1");
  spi_miso_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    spi_miso_src_combobox->addItem("Ch. 3");
    spi_miso_src_combobox->addItem("Ch. 4");
  }
  spi_miso_src_combobox->setCurrentIndex(devparms->math_decode_spi_miso);

  threshold_3_label = new QLabel(this);
  threshold_3_label->setGeometry(270, 112, 100, 25);
  threshold_3_label->setText("Threshold");

  threshold_3_dspinbox = new QDoubleSpinBox(this);
  threshold_3_dspinbox->setGeometry(370, 112, 100, 25);
  threshold_3_dspinbox->setDecimals(3);
  threshold_3_dspinbox->setRange(-20.0, 20.0);

  spi_cs_src_label = new QLabel(tab_spi);
  spi_cs_src_label->setGeometry(10, 125, 100, 25);
  spi_cs_src_label->setText("CS");

  spi_cs_src_combobox = new QComboBox(tab_spi);
  spi_cs_src_combobox->setGeometry(130, 125, 100, 25);
  spi_cs_src_combobox->addItem("Off");
  spi_cs_src_combobox->addItem("Ch. 1");
  spi_cs_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    spi_cs_src_combobox->addItem("Ch. 3");
    spi_cs_src_combobox->addItem("Ch. 4");
  }
  if(devparms->math_decode_spi_mode)
  {
    spi_cs_src_combobox->setCurrentIndex(devparms->math_decode_spi_cs);
  }

  threshold_4_label = new QLabel(this);
  threshold_4_label->setGeometry(270, 147, 100, 25);
  threshold_4_label->setText("Threshold");

  threshold_4_dspinbox = new QDoubleSpinBox(this);
  threshold_4_dspinbox->setGeometry(370, 147, 100, 25);
  threshold_4_dspinbox->setDecimals(3);
  threshold_4_dspinbox->setRange(-20.0, 20.0);

  spi_select_label = new QLabel(tab_spi);
  spi_select_label->setGeometry(10, 160, 100, 25);
  spi_select_label->setText("CS active level");

  spi_select_combobox = new QComboBox(tab_spi);
  spi_select_combobox->setGeometry(130, 160, 100, 25);
  spi_select_combobox->addItem("Low");
  spi_select_combobox->addItem("High");
  spi_select_combobox->setCurrentIndex(devparms->math_decode_spi_select);

  format_label = new QLabel(this);
  format_label->setGeometry(270, 182, 100, 25);
  format_label->setText("Format");

  format_combobox = new QComboBox(this);
  format_combobox->setGeometry(370, 182, 100, 25);
  format_combobox->addItem("Hexadecimal");
  format_combobox->addItem("ASCII");
  format_combobox->addItem("Decimal");
  format_combobox->addItem("Binary");
  if(devparms->modelserie != 6)
  {
    format_combobox->addItem("Line");
  }
  format_combobox->setCurrentIndex(devparms->math_decode_format);

  spi_mode_label = new QLabel(tab_spi);
  spi_mode_label->setGeometry(10, 195, 100, 25);
  spi_mode_label->setText("Mode");

  spi_mode_combobox = new QComboBox(tab_spi);
  spi_mode_combobox->setGeometry(130, 195, 100, 25);
  spi_mode_combobox->addItem("Timeout");
  spi_mode_combobox->addItem("CS");
  spi_mode_combobox->setCurrentIndex(devparms->math_decode_spi_mode);

  spi_timeout_label = new QLabel(tab_spi);
  spi_timeout_label->setGeometry(10, 230, 100, 25);
  spi_timeout_label->setText("Timeout");

  spi_timeout_dspinbox = new QDoubleSpinBox(tab_spi);
  spi_timeout_dspinbox->setGeometry(110, 230, 120, 25);
  spi_timeout_dspinbox->setSuffix(" Sec.");
  spi_timeout_dspinbox->setDecimals(7);
  if(devparms->modelserie == 6)
  {
    spi_timeout_dspinbox->setValue(0.0);
    spi_timeout_dspinbox->setEnabled(false);
  }
  else
  {
    spi_timeout_dspinbox->setRange(1e-7, 1.0);
    spi_timeout_dspinbox->setValue(devparms->math_decode_spi_timeout);
  }

  spi_polarity_label = new QLabel(tab_spi);
  spi_polarity_label->setGeometry(10, 265, 100, 25);
  spi_polarity_label->setText("Polarity");

  spi_polarity_combobox = new QComboBox(tab_spi);
  spi_polarity_combobox->setGeometry(130, 265, 100, 25);
  spi_polarity_combobox->addItem("Negative");
  spi_polarity_combobox->addItem("Positive");
  spi_polarity_combobox->setCurrentIndex(devparms->math_decode_spi_pol);

  spi_edge_label = new QLabel(tab_spi);
  spi_edge_label->setGeometry(10, 300, 100, 25);
  spi_edge_label->setText("Edge");

  spi_edge_combobox = new QComboBox(tab_spi);
  spi_edge_combobox->setGeometry(130, 300, 100, 25);
  spi_edge_combobox->addItem("Falling");
  spi_edge_combobox->addItem("Rising");
  spi_edge_combobox->setCurrentIndex(devparms->math_decode_spi_edge);

  spi_endian_label = new QLabel(tab_spi);
  spi_endian_label->setGeometry(10, 335, 100, 25);
  spi_endian_label->setText("Endian");

  spi_endian_combobox = new QComboBox(tab_spi);
  spi_endian_combobox->setGeometry(130, 335, 100, 25);
  spi_endian_combobox->addItem("LSB");
  spi_endian_combobox->addItem("MSB");
  spi_endian_combobox->setCurrentIndex(devparms->math_decode_spi_end);

  spi_width_label = new QLabel(tab_spi);
  spi_width_label->setGeometry(10, 370, 100, 25);
  spi_width_label->setText("Width");

  spi_width_spinbox = new QSpinBox(tab_spi);
  spi_width_spinbox->setGeometry(130, 370, 100, 25);
  spi_width_spinbox->setSuffix(" bits");
  spi_width_spinbox->setRange(8, 32);
  spi_width_spinbox->setValue(devparms->math_decode_spi_width);

  trace_pos_label = new QLabel(this);
  trace_pos_label->setGeometry(270, 392, 100, 25);
  trace_pos_label->setText("Vertical position");

  trace_pos_spinbox = new QSpinBox(this);
  trace_pos_spinbox->setGeometry(370, 392, 100, 25);
  if(devparms->modelserie == 6)
  {
    trace_pos_spinbox->setRange(-163, 143);
  }
  else
  {
    trace_pos_spinbox->setRange(50, 350);
  }
  trace_pos_spinbox->setValue(devparms->math_decode_pos);

  uart_tx_src_label = new QLabel(tab_uart);
  uart_tx_src_label->setGeometry(10, 20, 100, 25);
  uart_tx_src_label->setText("TX");

  uart_tx_src_combobox = new QComboBox(tab_uart);
  uart_tx_src_combobox->setGeometry(130, 20, 100, 25);
  uart_tx_src_combobox->addItem("Off");
  uart_tx_src_combobox->addItem("Ch. 1");
  uart_tx_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    uart_tx_src_combobox->addItem("Ch. 3");
    uart_tx_src_combobox->addItem("Ch. 4");
  }
  uart_tx_src_combobox->setCurrentIndex(devparms->math_decode_uart_tx);

  uart_rx_src_label = new QLabel(tab_uart);
  uart_rx_src_label->setGeometry(10, 55, 100, 25);
  uart_rx_src_label->setText("RX");

  uart_rx_src_combobox = new QComboBox(tab_uart);
  uart_rx_src_combobox->setGeometry(130, 55, 100, 25);
  uart_rx_src_combobox->addItem("Off");
  uart_rx_src_combobox->addItem("Ch. 1");
  uart_rx_src_combobox->addItem("Ch. 2");
  if(devparms->channel_cnt == 4)
  {
    uart_rx_src_combobox->addItem("Ch. 3");
    uart_rx_src_combobox->addItem("Ch. 4");
  }
  uart_rx_src_combobox->setCurrentIndex(devparms->math_decode_uart_rx);

  uart_polarity_label = new QLabel(tab_uart);
  uart_polarity_label->setGeometry(10, 90, 100, 25);
  uart_polarity_label->setText("Polarity");

  uart_polarity_combobox = new QComboBox(tab_uart);
  uart_polarity_combobox->setGeometry(130, 90, 100, 25);
  uart_polarity_combobox->addItem("Negative");
  uart_polarity_combobox->addItem("Positive");
  uart_polarity_combobox->setCurrentIndex(devparms->math_decode_uart_pol);

  uart_endian_label = new QLabel(tab_uart);
  uart_endian_label->setGeometry(10, 125, 100, 25);
  uart_endian_label->setText("Endian");

  uart_endian_combobox = new QComboBox(tab_uart);
  uart_endian_combobox->setGeometry(130, 125, 100, 25);
  uart_endian_combobox->addItem("LSB");
  uart_endian_combobox->addItem("MSB");
  uart_endian_combobox->setCurrentIndex(devparms->math_decode_uart_end);

  uart_baud_label = new QLabel(tab_uart);
  uart_baud_label->setGeometry(10, 160, 100, 25);
  uart_baud_label->setText("Baudrate");

  uart_baud_spinbox = new QSpinBox(tab_uart);
  uart_baud_spinbox->setGeometry(130, 160, 100, 25);
  uart_baud_spinbox->setRange(110, 20000000);
  uart_baud_spinbox->setValue(devparms->math_decode_uart_baud);

  uart_width_label = new QLabel(tab_uart);
  uart_width_label->setGeometry(10, 195, 100, 25);
  uart_width_label->setText("Data bits");

  uart_width_combobox = new QComboBox(tab_uart);
  uart_width_combobox->setGeometry(130, 195, 100, 25);
  uart_width_combobox->addItem("5");
  uart_width_combobox->addItem("6");
  uart_width_combobox->addItem("7");
  uart_width_combobox->addItem("8");
  if((devparms->math_decode_uart_width >= 5) && (devparms->math_decode_uart_width <= 8))
  {
    uart_width_combobox->setCurrentIndex(devparms->math_decode_uart_width - 5);
  }

  uart_stop_label = new QLabel(tab_uart);
  uart_stop_label->setGeometry(10, 230, 100, 25);
  uart_stop_label->setText("Stop bits");

  uart_stop_combobox = new QComboBox(tab_uart);
  uart_stop_combobox->setGeometry(130, 230, 100, 25);
  uart_stop_combobox->addItem("1");
  uart_stop_combobox->addItem("1.5");
  uart_stop_combobox->addItem("2");
  uart_stop_combobox->setCurrentIndex(devparms->math_decode_uart_stop);

  uart_parity_label = new QLabel(tab_uart);
  uart_parity_label->setGeometry(10, 265, 100, 25);
  uart_parity_label->setText("Parity");

  uart_parity_combobox = new QComboBox(tab_uart);
  uart_parity_combobox->setGeometry(130, 265, 100, 25);
  uart_parity_combobox->addItem("None");
  uart_parity_combobox->addItem("Odd");
  uart_parity_combobox->addItem("Even");
  uart_parity_combobox->setCurrentIndex(devparms->math_decode_uart_par);

  threshold_auto_clicked(devparms->math_decode_threshold_auto);

  toggle_decode_button = new QPushButton(this);
  toggle_decode_button->setGeometry(20, 475, 100, 25);
  if(devparms->math_decode_display == 1)
  {
    toggle_decode_button->setText("Stop Decoding");
  }
  else
  {
    toggle_decode_button->setText("Start Decoding");
  }
  toggle_decode_button->setAutoDefault(false);
  toggle_decode_button->setDefault(false);

  close_button = new QPushButton(this);
  close_button->setGeometry(570, 475, 100, 25);
  close_button->setText("Close");
  close_button->setAutoDefault(false);
  close_button->setDefault(false);

  tabholder->addTab(tab_par, "Parallel");
  tabholder->addTab(tab_uart, "UART");
  tabholder->addTab(tab_spi, "SPI");
  tabholder->addTab(tab_iic, "I2C");

  tabholder->setCurrentIndex(devparms->math_decode_mode);
  tabholder_index_changed(devparms->math_decode_mode);

  connect(threshold_auto_combobox,     SIGNAL(currentIndexChanged(int)), this, SLOT(threshold_auto_clicked(int)));

  connect(uart_tx_src_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));
  connect(uart_rx_src_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));
  connect(uart_polarity_combobox,      SIGNAL(currentIndexChanged(int)), this, SLOT(uart_polarity_combobox_clicked(int)));
  connect(uart_endian_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(uart_endian_combobox_clicked(int)));
  connect(uart_width_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(uart_width_combobox_clicked(int)));
  connect(uart_stop_combobox,          SIGNAL(currentIndexChanged(int)), this, SLOT(uart_stop_combobox_clicked(int)));
  connect(uart_parity_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(uart_parity_combobox_clicked(int)));

  connect(spi_clk_src_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));
  connect(spi_mosi_src_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));
  connect(spi_miso_src_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));
  connect(spi_cs_src_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(src_combobox_clicked(int)));

  connect(threshold_1_dspinbox,        SIGNAL(editingFinished()),        this, SLOT(threshold_1_dspinbox_changed()));
  connect(threshold_2_dspinbox,        SIGNAL(editingFinished()),        this, SLOT(threshold_2_dspinbox_changed()));
  connect(threshold_3_dspinbox,        SIGNAL(editingFinished()),        this, SLOT(threshold_3_dspinbox_changed()));
  connect(threshold_4_dspinbox,        SIGNAL(editingFinished()),        this, SLOT(threshold_4_dspinbox_changed()));

  connect(uart_baud_spinbox,           SIGNAL(editingFinished()),        this, SLOT(uart_baud_spinbox_changed()));
  connect(spi_width_spinbox,           SIGNAL(editingFinished()),        this, SLOT(spi_width_spinbox_changed()));
  connect(spi_timeout_dspinbox,        SIGNAL(editingFinished()),        this, SLOT(spi_timeout_dspinbox_changed()));
  connect(trace_pos_spinbox,           SIGNAL(editingFinished()),        this, SLOT(trace_pos_spinbox_changed()));

  connect(tabholder,                   SIGNAL(currentChanged(int)),      this, SLOT(tabholder_index_changed(int)));

  connect(spi_select_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(spi_select_combobox_clicked(int)));
  connect(spi_mode_combobox,           SIGNAL(currentIndexChanged(int)), this, SLOT(spi_mode_combobox_clicked(int)));
  connect(spi_polarity_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(spi_polarity_combobox_clicked(int)));
  connect(spi_edge_combobox,           SIGNAL(currentIndexChanged(int)), this, SLOT(spi_edge_combobox_clicked(int)));
  connect(spi_endian_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(spi_endian_combobox_clicked(int)));
  connect(format_combobox,             SIGNAL(currentIndexChanged(int)), this, SLOT(format_combobox_clicked(int)));

  connect(close_button,                SIGNAL(clicked()),                this, SLOT(close()));
  connect(toggle_decode_button,        SIGNAL(clicked()),                this, SLOT(toggle_decode()));

  exec();
}


void UI_decoder_window::uart_polarity_combobox_clicked(int idx)
{
  devparms->math_decode_uart_pol = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:POL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:POL POS");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:UART:POL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:UART:POL POS");
    }
  }
}


void UI_decoder_window::uart_endian_combobox_clicked(int idx)
{
  devparms->math_decode_uart_end = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:END LSB");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:END MSB");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:UART:END LSB");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:UART:END MSB");
    }
  }
}


void UI_decoder_window::uart_baud_spinbox_changed()
{
  char str[256];

  devparms->math_decode_uart_baud = uart_baud_spinbox->value();

  if(devparms->modelserie == 6)
  {
    sprintf(str, ":BUS1:RS232:BAUD %i", devparms->math_decode_uart_baud);
  }
  else
  {
    sprintf(str, ":DEC1:UART:BAUD %i", devparms->math_decode_uart_baud);
  }

  mainwindow->set_cue_cmd(str);
}


void UI_decoder_window::uart_width_combobox_clicked(int idx)
{
  char str[256];

  idx += 5;

  devparms->math_decode_uart_width = idx;

  if(devparms->modelserie == 6)
  {
    sprintf(str, ":BUS1:RS232:DBIT %i", idx);
  }
  else
  {
    sprintf(str, ":DEC1:UART:WIDT %i", idx);
  }

  mainwindow->set_cue_cmd(str);
}


void UI_decoder_window::uart_stop_combobox_clicked(int idx)
{
  devparms->math_decode_uart_stop = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:SBIT 1");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":BUS1:RS232:SBIT 1.5");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":BUS1:RS232:SBIT 2");
        }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:UART:STOP 1");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":DEC1:UART:STOP 1.5");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":DEC1:UART:STOP 2");
        }
  }
}


void UI_decoder_window::uart_parity_combobox_clicked(int idx)
{
  devparms->math_decode_uart_par = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:RS232:PAR NONE");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":BUS1:RS232:PAR ODD");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":BUS1:RS232:PAR EVEN");
        }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:UART:PAR NONE");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":DEC1:UART:PAR ODD");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":DEC1:UART:PAR EVEN");
        }
  }
}


void UI_decoder_window::spi_width_spinbox_changed()
{
  char str[256];

  devparms->math_decode_spi_width = spi_width_spinbox->value();

  if(devparms->modelserie == 6)
  {
    sprintf(str, ":BUS1:SPI:DBIT %i", devparms->math_decode_spi_width);
  }
  else
  {
    sprintf(str, ":DEC1:SPI:WIDT %i", devparms->math_decode_spi_width);
  }

  mainwindow->set_cue_cmd(str);
}


void UI_decoder_window::spi_timeout_dspinbox_changed()
{
  char str[256];

  devparms->math_decode_spi_timeout = spi_timeout_dspinbox->value();

  if(devparms->modelserie != 6)
  {
    sprintf(str, ":DEC1:SPI:TIM %e", devparms->math_decode_spi_timeout);

    mainwindow->set_cue_cmd(str);
  }
}


void UI_decoder_window::trace_pos_spinbox_changed()
{
  char str[256];

  devparms->math_decode_pos = trace_pos_spinbox->value();

  if(devparms->modelserie == 6)
  {
    if(devparms->math_decode_mode == DECODE_MODE_SPI)
    {
      sprintf(str, ":BUS1:SPI:OFFS %i", devparms->math_decode_pos);
    }
    else if(devparms->math_decode_mode == DECODE_MODE_UART)
      {
        sprintf(str, ":BUS1:RS232:OFFS %i", devparms->math_decode_pos);
      }
      else if(devparms->math_decode_mode == DECODE_MODE_I2C)
        {
          sprintf(str, ":BUS1:IIC:OFFS %i", devparms->math_decode_pos);
        }
        else if(devparms->math_decode_mode == DECODE_MODE_PAR)
          {
            sprintf(str, ":BUS1:PARA:OFFS %i", devparms->math_decode_pos);
          }
  }
  else
  {
    sprintf(str, ":DEC1:POS %i", devparms->math_decode_pos);
  }

  mainwindow->set_cue_cmd(str);
}


void UI_decoder_window::spi_select_combobox_clicked(int idx)
{
  devparms->math_decode_spi_select = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL POS");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:SEL NCS");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:SEL CS");
    }
  }
}


void UI_decoder_window::spi_mode_combobox_clicked(int idx)
{
  char str[256];

  devparms->math_decode_spi_mode = idx;

  if(devparms->modelserie != 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:MODE TIM");

      spi_cs_src_combobox->setCurrentIndex(0);

      devparms->math_decode_spi_cs = 0;
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:MODE CS");

      if(spi_cs_src_combobox->currentIndex() > 0)
      {
        sprintf(str, ":DEC1:SPI:CS CHAN%i", spi_cs_src_combobox->currentIndex());

        mainwindow->set_cue_cmd(str);
      }
    }
  }
}


void UI_decoder_window::spi_polarity_combobox_clicked(int idx)
{
  devparms->math_decode_spi_pol = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:MISO:POL NEG");
      mainwindow->set_cue_cmd(":BUS1:SPI:MOSI:POL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:MISO:POL POS");
      mainwindow->set_cue_cmd(":BUS1:SPI:MOSI:POL POS");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:POL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:POL POS");
    }
  }
}


void UI_decoder_window::spi_edge_combobox_clicked(int idx)
{
  devparms->math_decode_spi_edge = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SCLK:SLOP NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SCLK:SLOP POS");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE FALL");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE RISE");
    }
  }
}


void UI_decoder_window::spi_endian_combobox_clicked(int idx)
{
  devparms->math_decode_spi_end = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:END LSB");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:END MSB");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:END LSB");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:END MSB");
    }
  }
}


void UI_decoder_window::format_combobox_clicked(int idx)
{
  devparms->math_decode_format = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:FORM HEX");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":BUS1:FORM ASC");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":BUS1:FORM DEC");
        }
        else if(idx == 3)
          {
            mainwindow->set_cue_cmd(":BUS1:FORM BIN");
          }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:FORM HEX");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":DEC1:FORM ASC");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":DEC1:FORM DEC");
        }
        else if(idx == 3)
          {
            mainwindow->set_cue_cmd(":DEC1:FORM BIN");
          }
          else if(idx == 4)
            {
              mainwindow->set_cue_cmd(":DEC1:FORM LINE");
            }
  }
}


void UI_decoder_window::tabholder_index_changed(int idx)
{
  devparms->math_decode_mode = idx;

  threshold_auto_clicked(devparms->math_decode_threshold_auto);

  if(devparms->modelserie == 6)
  {
    if(idx == DECODE_MODE_TAB_PAR)
    {
      mainwindow->set_cue_cmd(":BUS1:MODE PAR");
    }
    else if(idx == DECODE_MODE_TAB_UART)
      {
        mainwindow->set_cue_cmd(":BUS1:MODE RS232");
      }
      else if(idx == DECODE_MODE_TAB_SPI)
        {
          mainwindow->set_cue_cmd(":BUS1:MODE SPI");
        }
        else if(idx == DECODE_MODE_TAB_I2C)
          {
            mainwindow->set_cue_cmd(":BUS1:MODE IIC");
          }
  }
  else
  {
    if(idx == DECODE_MODE_TAB_PAR)
    {
      mainwindow->set_cue_cmd(":DEC1:MODE PAR");
    }
    else if(idx == DECODE_MODE_TAB_UART)
      {
        mainwindow->set_cue_cmd(":DEC1:MODE UART");
      }
      else if(idx == DECODE_MODE_TAB_SPI)
        {
          mainwindow->set_cue_cmd(":DEC1:MODE SPI");
        }
        else if(idx == DECODE_MODE_TAB_I2C)
          {
            mainwindow->set_cue_cmd(":DEC1:MODE IIC");
          }
  }
}


void UI_decoder_window::threshold_1_dspinbox_changed()
{
  char str[256];

  if(tabholder->currentIndex() == DECODE_MODE_TAB_UART)
  {
    if(devparms->modelserie == 6)
    {
      devparms->math_decode_threshold_uart_tx = threshold_1_dspinbox->value();

      sprintf(str, ":BUS1:RS232:TTHR %e", devparms->math_decode_threshold_uart_tx);

      mainwindow->set_cue_cmd(str);
    }
    else if(uart_tx_src_combobox->currentIndex() > 0)
      {
        devparms->math_decode_threshold[uart_tx_src_combobox->currentIndex() - 1] = threshold_1_dspinbox->value();

        sprintf(str, ":DEC1:THRE:CHAN%i %e", uart_tx_src_combobox->currentIndex(),
                devparms->math_decode_threshold[uart_tx_src_combobox->currentIndex() - 1]);

        mainwindow->set_cue_cmd(str);
      }
  }
  else if(tabholder->currentIndex() == DECODE_MODE_TAB_SPI)
    {
      if(devparms->modelserie == 6)
      {
        devparms->math_decode_threshold[2] = threshold_1_dspinbox->value();

        sprintf(str, ":BUS1:SPI:SCLK:THR %e", devparms->math_decode_threshold[2]);

        mainwindow->set_cue_cmd(str);
      }
      else
      {
        devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex()] = threshold_1_dspinbox->value();

        sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_clk_src_combobox->currentIndex() + 1,
                devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex()]);

        mainwindow->set_cue_cmd(str);
      }
    }

  threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::threshold_2_dspinbox_changed()
{
  char str[256];

  if(tabholder->currentIndex() == DECODE_MODE_TAB_UART)
  {
    if(devparms->modelserie == 6)
    {
      devparms->math_decode_threshold_uart_rx = threshold_2_dspinbox->value();

      sprintf(str, ":BUS1:RS232:RTHR %e", devparms->math_decode_threshold_uart_rx);

      mainwindow->set_cue_cmd(str);
    }
    else if(uart_rx_src_combobox->currentIndex() > 0)
      {
        devparms->math_decode_threshold[uart_rx_src_combobox->currentIndex() - 1] = threshold_2_dspinbox->value();

        sprintf(str, ":DEC1:THRE:CHAN%i %e", uart_rx_src_combobox->currentIndex(),
                devparms->math_decode_threshold[uart_rx_src_combobox->currentIndex() - 1]);

        mainwindow->set_cue_cmd(str);
      }
  }
  else if(tabholder->currentIndex() == DECODE_MODE_TAB_SPI)
    {
      if(devparms->modelserie == 6)
      {
        devparms->math_decode_threshold[1] = threshold_2_dspinbox->value();

        sprintf(str, ":BUS1:SPI:MOSI:THR %e", devparms->math_decode_threshold[1]);

        mainwindow->set_cue_cmd(str);
      }
      else if(spi_mosi_src_combobox->currentIndex() > 0)
        {
          devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1] = threshold_2_dspinbox->value();

          sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_mosi_src_combobox->currentIndex(),
                  devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1]);

          mainwindow->set_cue_cmd(str);
        }
    }

  threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::threshold_3_dspinbox_changed()
{
  char str[256];

  if(tabholder->currentIndex() == DECODE_MODE_TAB_UART)
  {
  }
  else if(tabholder->currentIndex() == DECODE_MODE_TAB_SPI)
    {
      if(devparms->modelserie == 6)
      {
        devparms->math_decode_threshold[0] = threshold_3_dspinbox->value();

        sprintf(str, ":BUS1:SPI:MISO:THR %e", devparms->math_decode_threshold[0]);

        mainwindow->set_cue_cmd(str);
      }
      else if(spi_miso_src_combobox->currentIndex() > 0)
        {
          devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1] = threshold_3_dspinbox->value();

          sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_miso_src_combobox->currentIndex(),
                  devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1]);

          mainwindow->set_cue_cmd(str);
        }
    }

  threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::threshold_4_dspinbox_changed()
{
  char str[256];

  if(tabholder->currentIndex() == DECODE_MODE_TAB_UART)
  {
  }
  else if(tabholder->currentIndex() == DECODE_MODE_TAB_SPI)
    {
      if(devparms->modelserie == 6)
      {
        devparms->math_decode_threshold[3] = threshold_4_dspinbox->value();

        sprintf(str, ":BUS1:SPI:SS:THR %e", devparms->math_decode_threshold[3]);

        mainwindow->set_cue_cmd(str);
      }
      else if(spi_cs_src_combobox->currentIndex() > 0)
        {
          devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1] = threshold_4_dspinbox->value();

          sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_cs_src_combobox->currentIndex(),
                  devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1]);

          mainwindow->set_cue_cmd(str);
        }
    }

  threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::src_combobox_clicked(int)
{
  devparms->math_decode_spi_clk = spi_clk_src_combobox->currentIndex();
  devparms->math_decode_spi_mosi = spi_mosi_src_combobox->currentIndex();
  devparms->math_decode_spi_miso = spi_miso_src_combobox->currentIndex();
  devparms->math_decode_spi_cs = spi_cs_src_combobox->currentIndex();
  devparms->math_decode_uart_tx = uart_tx_src_combobox->currentIndex();
  devparms->math_decode_uart_rx = uart_rx_src_combobox->currentIndex();
  devparms->math_decode_spi_mode = spi_mode_combobox->currentIndex();
  devparms->math_decode_spi_select = spi_select_combobox->currentIndex();

  threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::threshold_auto_clicked(int thr_auto)
{
  char str[256];

  devparms->math_decode_threshold_auto = thr_auto;

  if(devparms->modelserie != 6)
  {
    if(thr_auto == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:THRE:AUTO 0");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:THRE:AUTO 1");
    }
  }

  if(tabholder->currentIndex() == DECODE_MODE_TAB_UART)
  {
    if(uart_tx_src_combobox->currentIndex() > 0)
    {
      if(devparms->modelserie == 6)
      {
        sprintf(str, ":BUS1:RS232:TX CHAN%i", uart_tx_src_combobox->currentIndex());

        mainwindow->set_cue_cmd(str);
      }
      else
      {
        sprintf(str, ":DEC1:UART:TX CHAN%i", uart_tx_src_combobox->currentIndex());

        mainwindow->set_cue_cmd(str);
      }
    }
    else
    {
      if(devparms->modelserie == 6)
      {
        mainwindow->set_cue_cmd(":BUS1:RS232:TX OFF");
      }
      else
      {
        mainwindow->set_cue_cmd(":DEC1:UART:TX OFF");
      }
    }

    if(uart_rx_src_combobox->currentIndex() > 0)
    {
      if(devparms->modelserie == 6)
      {
        sprintf(str, ":BUS1:RS232:RX CHAN%i", uart_rx_src_combobox->currentIndex());

        mainwindow->set_cue_cmd(str);
      }
      else
      {
        sprintf(str, ":DEC1:UART:RX CHAN%i", uart_rx_src_combobox->currentIndex());

        mainwindow->set_cue_cmd(str);
      }
    }
    else
    {
      if(devparms->modelserie == 6)
      {
        mainwindow->set_cue_cmd(":BUS1:RS232:RX OFF");
      }
      else
      {
        mainwindow->set_cue_cmd(":DEC1:UART:RX OFF");
      }
    }

    if(thr_auto == 0)
    {
      threshold_1_dspinbox->setEnabled(true);
      threshold_2_dspinbox->setEnabled(true);

      if(devparms->modelserie == 6)
      {
        threshold_1_dspinbox->setValue(devparms->math_decode_threshold_uart_tx);
        threshold_2_dspinbox->setValue(devparms->math_decode_threshold_uart_rx);
      }
      else
      {
        threshold_1_dspinbox->setValue(devparms->math_decode_threshold[uart_tx_src_combobox->currentIndex() - 1]);
        threshold_2_dspinbox->setValue(devparms->math_decode_threshold[uart_rx_src_combobox->currentIndex() - 1]);
      }
    }
    else
    {
      threshold_1_dspinbox->setValue(0.0);
      threshold_2_dspinbox->setValue(0.0);

      threshold_1_dspinbox->setEnabled(false);
      threshold_2_dspinbox->setEnabled(false);
    }

    threshold_1_label->setVisible(true);
    threshold_2_label->setVisible(true);
    threshold_3_label->setVisible(false);
    threshold_4_label->setVisible(false);

    threshold_1_dspinbox->setVisible(true);
    threshold_2_dspinbox->setVisible(true);
    threshold_3_dspinbox->setVisible(false);
    threshold_4_dspinbox->setVisible(false);
  }
  else if(tabholder->currentIndex() == DECODE_MODE_TAB_SPI)
    {
      if(devparms->modelserie == 6)
      {
        sprintf(str, ":BUS1:SPI:SCLK:SOUR CHAN%i", spi_clk_src_combobox->currentIndex() + 1);

        mainwindow->set_cue_cmd(str);
      }
      else
      {
        sprintf(str, ":DEC1:SPI:CLK CHAN%i", spi_clk_src_combobox->currentIndex() + 1);

        mainwindow->set_cue_cmd(str);
      }

      if(spi_mosi_src_combobox->currentIndex() > 0)
      {
        if(devparms->modelserie == 6)
        {
          sprintf(str, ":BUS1:SPI:MOSI:SOUR CHAN%i", spi_mosi_src_combobox->currentIndex());

          mainwindow->set_cue_cmd(str);
        }
        else
        {
          sprintf(str, ":DEC1:SPI:MOSI CHAN%i", spi_mosi_src_combobox->currentIndex());

          mainwindow->set_cue_cmd(str);
        }
      }
      else
      {
        if(devparms->modelserie == 6)
        {
          mainwindow->set_cue_cmd(":BUS1:SPI:MOSI:SOUR OFF");
        }
        else
        {
          mainwindow->set_cue_cmd(":DEC1:SPI:MOSI OFF");
        }
      }

      if(spi_miso_src_combobox->currentIndex() > 0)
      {
        if(devparms->modelserie == 6)
        {
          sprintf(str, ":BUS1:SPI:MISO:SOUR CHAN%i", spi_miso_src_combobox->currentIndex());

          mainwindow->set_cue_cmd(str);
        }
        else
        {
          sprintf(str, ":DEC1:SPI:MISO CHAN%i", spi_miso_src_combobox->currentIndex());

          mainwindow->set_cue_cmd(str);
        }
      }
      else
      {
        if(devparms->modelserie == 6)
        {
          mainwindow->set_cue_cmd(":BUS1:SPI:MISO:SOUR OFF");
        }
        else
        {
          mainwindow->set_cue_cmd(":DEC1:SPI:MISO OFF");
        }
      }

      if(devparms->modelserie != 6)
      {
        if(spi_mode_combobox->currentIndex() == 0)
        {
          mainwindow->set_cue_cmd(":DEC1:SPI:MODE TIM");
        }
        else
        {
          mainwindow->set_cue_cmd(":DEC1:SPI:MODE CS");
        }
      }

      if(spi_cs_src_combobox->currentIndex() > 0)
      {
        if(devparms->modelserie == 6)
        {
          sprintf(str, ":BUS1:SPI:SS:SOUR CHAN%i", spi_cs_src_combobox->currentIndex());

          mainwindow->set_cue_cmd(str);
        }
        else
        {
          if(devparms->math_decode_spi_mode)
          {
            sprintf(str, ":DEC1:SPI:CS CHAN%i", spi_cs_src_combobox->currentIndex());

            mainwindow->set_cue_cmd(str);
          }
        }
      }
      else
      {
        if(devparms->modelserie == 6)
        {
          mainwindow->set_cue_cmd(":BUS1:SPI:SS:SOUR OFF");
        }
      }

      if(spi_select_combobox->currentIndex() == 0)
      {
        if(devparms->modelserie == 6)
        {
          mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL NEG");
        }
        else
        {
          if(devparms->math_decode_spi_mode)
          {
            mainwindow->set_cue_cmd(":DEC1:SPI:SEL NCS");
          }
        }
      }
      else
      {
        if(devparms->modelserie == 6)
        {
          mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL POS");
        }
        else
        {
          mainwindow->set_cue_cmd(":DEC1:SPI:SEL CS");
        }
      }

      if(thr_auto == 0)
      {
        threshold_1_dspinbox->setEnabled(true);
        threshold_2_dspinbox->setEnabled(true);
        threshold_3_dspinbox->setEnabled(true);
        threshold_4_dspinbox->setEnabled(true);

        if(devparms->modelserie == 6)
        {
          threshold_1_dspinbox->setValue(devparms->math_decode_threshold[2]);
          threshold_2_dspinbox->setValue(devparms->math_decode_threshold[1]);
          threshold_3_dspinbox->setValue(devparms->math_decode_threshold[0]);
          threshold_4_dspinbox->setValue(devparms->math_decode_threshold[3]);
        }
        else
        {
          threshold_1_dspinbox->setValue(devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex()]);
          threshold_2_dspinbox->setValue(devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1]);
          threshold_3_dspinbox->setValue(devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1]);
          threshold_4_dspinbox->setValue(devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1]);
        }
      }
      else
      {
        threshold_1_dspinbox->setValue(0.0);
        threshold_2_dspinbox->setValue(0.0);
        threshold_3_dspinbox->setValue(0.0);
        threshold_4_dspinbox->setValue(0.0);

        threshold_1_dspinbox->setEnabled(false);
        threshold_2_dspinbox->setEnabled(false);
        threshold_3_dspinbox->setEnabled(false);
        threshold_4_dspinbox->setEnabled(false);
      }

      threshold_1_label->setVisible(true);
      threshold_2_label->setVisible(true);
      threshold_3_label->setVisible(true);
      threshold_4_label->setVisible(true);

      threshold_1_dspinbox->setVisible(true);
      threshold_2_dspinbox->setVisible(true);
      threshold_3_dspinbox->setVisible(true);
      threshold_4_dspinbox->setVisible(true);
    }
}


void UI_decoder_window::toggle_decode()
{
  if(devparms->math_decode_display == 1)
  {
    devparms->math_decode_display = 0;

    if(devparms->modelserie == 6)
    {
      mainwindow->set_cue_cmd(":BUS1:DISP OFF");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:DISP OFF");
    }

    toggle_decode_button->setText("Start Decoding");

    mainwindow->statusLabel->setText("Decode off");
  }
  else
  {
    devparms->math_decode_display = 1;

    if(devparms->modelserie == 6)
    {
      mainwindow->set_cue_cmd(":BUS1:DISP ON");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:DISP ON");
    }

    toggle_decode_button->setText("Stop Decoding");

    mainwindow->statusLabel->setText("Decode on");
  }
}




















