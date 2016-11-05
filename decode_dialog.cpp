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
  tabholder->setGeometry(0, 0, 690, 455);

  tab_par = new QWidget;
  tab_uart = new QWidget;
  tab_spi = new QWidget;
  tab_iic = new QWidget;

  spi_clk_src_label = new QLabel(tab_spi);
  spi_clk_src_label->setGeometry(10, 20, 100, 25);
  spi_clk_src_label->setText("Clock");

  spi_clk_src_combobox = new QComboBox(tab_spi);
  spi_clk_src_combobox->setGeometry(130, 20, 100, 25);
  spi_clk_src_combobox->addItem("Off");
  spi_clk_src_combobox->addItem("Ch. 1");
  spi_clk_src_combobox->addItem("Ch. 2");
  spi_clk_src_combobox->addItem("Ch. 3");
  spi_clk_src_combobox->addItem("Ch. 4");
  spi_clk_src_combobox->setCurrentIndex(devparms->math_decode_spi_clk);

  spi_clk_threshold_label = new QLabel(tab_spi);
  spi_clk_threshold_label->setGeometry(270, 20, 100, 25);
  spi_clk_threshold_label->setText("Threshold");

  spi_clk_threshold_dspinbox = new QDoubleSpinBox(tab_spi);
  spi_clk_threshold_dspinbox->setGeometry(370, 20, 100, 25);
  spi_clk_threshold_dspinbox->setDecimals(3);

  spi_threshold_auto_combobox = new QComboBox(tab_spi);
  spi_threshold_auto_combobox->setGeometry(510, 20, 140, 25);
  spi_threshold_auto_combobox->addItem("Manual threshold");
  spi_threshold_auto_combobox->addItem("Auto threshold");
  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold_auto = 0;
    spi_threshold_auto_combobox->setCurrentIndex(0);
    spi_threshold_auto_combobox->setEnabled(false);
  }
  else
  {
    spi_threshold_auto_combobox->setCurrentIndex(devparms->math_decode_threshold_auto);
  }

  spi_mosi_src_label = new QLabel(tab_spi);
  spi_mosi_src_label->setGeometry(10, 55, 100, 25);
  spi_mosi_src_label->setText("MOSI");

  spi_mosi_src_combobox = new QComboBox(tab_spi);
  spi_mosi_src_combobox->setGeometry(130, 55, 100, 25);
  spi_mosi_src_combobox->addItem("Off");
  spi_mosi_src_combobox->addItem("Ch. 1");
  spi_mosi_src_combobox->addItem("Ch. 2");
  spi_mosi_src_combobox->addItem("Ch. 3");
  spi_mosi_src_combobox->addItem("Ch. 4");
  spi_mosi_src_combobox->setCurrentIndex(devparms->math_decode_spi_mosi);

  spi_mosi_threshold_label = new QLabel(tab_spi);
  spi_mosi_threshold_label->setGeometry(270, 55, 100, 25);
  spi_mosi_threshold_label->setText("Threshold");

  spi_mosi_threshold_dspinbox = new QDoubleSpinBox(tab_spi);
  spi_mosi_threshold_dspinbox->setGeometry(370, 55, 100, 25);
  spi_mosi_threshold_dspinbox->setDecimals(3);

  spi_miso_src_label = new QLabel(tab_spi);
  spi_miso_src_label->setGeometry(10, 90, 100, 25);
  spi_miso_src_label->setText("MISO");

  spi_miso_src_combobox = new QComboBox(tab_spi);
  spi_miso_src_combobox->setGeometry(130, 90, 100, 25);
  spi_miso_src_combobox->addItem("Off");
  spi_miso_src_combobox->addItem("Ch. 1");
  spi_miso_src_combobox->addItem("Ch. 2");
  spi_miso_src_combobox->addItem("Ch. 3");
  spi_miso_src_combobox->addItem("Ch. 4");
  spi_miso_src_combobox->setCurrentIndex(devparms->math_decode_spi_miso);

  spi_miso_threshold_label = new QLabel(tab_spi);
  spi_miso_threshold_label->setGeometry(270, 90, 100, 25);
  spi_miso_threshold_label->setText("Threshold");

  spi_miso_threshold_dspinbox = new QDoubleSpinBox(tab_spi);
  spi_miso_threshold_dspinbox->setGeometry(370, 90, 100, 25);
  spi_miso_threshold_dspinbox->setDecimals(3);

  spi_cs_src_label = new QLabel(tab_spi);
  spi_cs_src_label->setGeometry(10, 125, 100, 25);
  spi_cs_src_label->setText("CS");

  spi_cs_src_combobox = new QComboBox(tab_spi);
  spi_cs_src_combobox->setGeometry(130, 125, 100, 25);
  spi_cs_src_combobox->addItem("Off");
  spi_cs_src_combobox->addItem("Ch. 1");
  spi_cs_src_combobox->addItem("Ch. 2");
  spi_cs_src_combobox->addItem("Ch. 3");
  spi_cs_src_combobox->addItem("Ch. 4");
  spi_cs_src_combobox->setCurrentIndex(devparms->math_decode_spi_cs);

  spi_cs_threshold_label = new QLabel(tab_spi);
  spi_cs_threshold_label->setGeometry(270, 125, 100, 25);
  spi_cs_threshold_label->setText("Threshold");

  spi_cs_threshold_dspinbox = new QDoubleSpinBox(tab_spi);
  spi_cs_threshold_dspinbox->setGeometry(370, 125, 100, 25);
  spi_cs_threshold_dspinbox->setDecimals(3);

  spi_select_label = new QLabel(tab_spi);
  spi_select_label->setGeometry(10, 160, 100, 25);
  spi_select_label->setText("CS active level");

  spi_select_combobox = new QComboBox(tab_spi);
  spi_select_combobox->setGeometry(130, 160, 100, 25);
  spi_select_combobox->addItem("Low");
  spi_select_combobox->addItem("High");
  spi_select_combobox->setCurrentIndex(devparms->math_decode_spi_select);

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
  spi_timeout_dspinbox->setRange(1e-7, 1.0);
  spi_timeout_dspinbox->setValue(devparms->math_decode_spi_timeout);

  spi_polarity_label = new QLabel(tab_spi);
  spi_polarity_label->setGeometry(10, 265, 100, 25);
  spi_polarity_label->setText("Polarity");

  spi_polarity_combobox = new QComboBox(tab_spi);
  spi_polarity_combobox->setGeometry(130, 265, 100, 25);
  spi_polarity_combobox->addItem("Negative");
  spi_polarity_combobox->addItem("Positive");

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

  spi_trace_pos_label = new QLabel(tab_spi);
  spi_trace_pos_label->setGeometry(270, 370, 100, 25);
  spi_trace_pos_label->setText("Vertical position");

  spi_trace_pos_spinbox = new QSpinBox(tab_spi);
  spi_trace_pos_spinbox->setGeometry(370, 370, 100, 25);
  spi_trace_pos_spinbox->setRange(50, 350);
  spi_trace_pos_spinbox->setValue(devparms->math_decode_pos);

  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);

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

  close_button = new QPushButton(this);
  close_button->setGeometry(570, 475, 100, 25);
  close_button->setText("Close");

  tabholder->addTab(tab_par, "Parallel");
  tabholder->addTab(tab_uart, "UART");
  tabholder->addTab(tab_spi, "SPI");
  tabholder->addTab(tab_iic, "I2C");

  tabholder->setCurrentIndex(devparms->math_decode_mode);

  connect(spi_threshold_auto_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(spi_threshold_auto_clicked(int)));

  connect(spi_clk_src_combobox,        SIGNAL(currentIndexChanged(int)), this, SLOT(spi_src_combobox_clicked(int)));
  connect(spi_mosi_src_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(spi_src_combobox_clicked(int)));
  connect(spi_miso_src_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(spi_src_combobox_clicked(int)));
  connect(spi_cs_src_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(spi_src_combobox_clicked(int)));

  connect(spi_mosi_threshold_dspinbox, SIGNAL(editingFinished()),        this, SLOT(spi_mosi_threshold_dspinbox_changed()));
  connect(spi_miso_threshold_dspinbox, SIGNAL(editingFinished()),        this, SLOT(spi_miso_threshold_dspinbox_changed()));
  connect(spi_clk_threshold_dspinbox,  SIGNAL(editingFinished()),        this, SLOT(spi_clk_threshold_dspinbox_changed()));
  connect(spi_cs_threshold_dspinbox,   SIGNAL(editingFinished()),        this, SLOT(spi_cs_threshold_dspinbox_changed()));

  connect(spi_width_spinbox,           SIGNAL(editingFinished()),        this, SLOT(spi_width_spinbox_changed()));

  connect(tabholder,                   SIGNAL(currentChanged(int)),      this, SLOT(tabholder_index_changed(int)));

  connect(spi_select_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(spi_select_combobox_clicked(int)));
  connect(spi_mode_combobox,           SIGNAL(currentIndexChanged(int)), this, SLOT(spi_mode_combobox_clicked(int)));
  connect(spi_polarity_combobox,       SIGNAL(currentIndexChanged(int)), this, SLOT(spi_polarity_combobox_clicked(int)));
  connect(spi_edge_combobox,           SIGNAL(currentIndexChanged(int)), this, SLOT(spi_edge_combobox_clicked(int)));
  connect(spi_endian_combobox,         SIGNAL(currentIndexChanged(int)), this, SLOT(spi_endian_combobox_clicked(int)));

  connect(close_button,                SIGNAL(clicked()),                this, SLOT(accept()));
  connect(toggle_decode_button,        SIGNAL(clicked()),                this, SLOT(toggle_decode()));

  exec();
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


void UI_decoder_window::spi_select_combobox_clicked(int idx)
{
  devparms->math_decode_spi_select = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL NCS");
    }
    else
    {
      mainwindow->set_cue_cmd(":BUS1:SPI:SS:POL CS");
    }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:SEL NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:SEL POS");
    }
  }
}


void UI_decoder_window::spi_mode_combobox_clicked(int idx)
{
  devparms->math_decode_spi_mode = idx;

  if(devparms->modelserie != 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:MODE TIM");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:MODE CS");
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
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE POS");
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
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE NEG");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:SPI:EDGE POS");
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


void UI_decoder_window::tabholder_index_changed(int idx)
{
  devparms->math_decode_mode = idx;

  if(devparms->modelserie == 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":BUS1:MODE PAR");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":BUS1:MODE RS232");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":BUS1:MODE SPI");
        }
        else if(idx == 3)
          {
            mainwindow->set_cue_cmd(":BUS1:MODE IIC");
          }
  }
  else
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:MODE PAR");
    }
    else if(idx == 1)
      {
        mainwindow->set_cue_cmd(":DEC1:MODE UART");
      }
      else if(idx == 2)
        {
          mainwindow->set_cue_cmd(":DEC1:MODE SPI");
        }
        else if(idx == 3)
          {
            mainwindow->set_cue_cmd(":DEC1:MODE IIC");
          }
  }
}


void UI_decoder_window::spi_miso_threshold_dspinbox_changed()
{
  char str[256];

  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold[0] = spi_miso_threshold_dspinbox->value();

    sprintf(str, ":BUS1:SPI:MISO:THR %e", devparms->math_decode_threshold[0]);

    mainwindow->set_cue_cmd(str);
  }
  else if(spi_miso_src_combobox->currentIndex() > 0)
    {
      devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1] = spi_miso_threshold_dspinbox->value();

      sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_miso_src_combobox->currentIndex(),
              devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1]);

      mainwindow->set_cue_cmd(str);
    }

  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::spi_mosi_threshold_dspinbox_changed()
{
  char str[256];

  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold[1] = spi_mosi_threshold_dspinbox->value();

    sprintf(str, ":BUS1:SPI:MOSI:THR %e", devparms->math_decode_threshold[1]);

    mainwindow->set_cue_cmd(str);
  }
  else if(spi_mosi_src_combobox->currentIndex() > 0)
    {
      devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1] = spi_mosi_threshold_dspinbox->value();

      sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_mosi_src_combobox->currentIndex(),
              devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1]);

      mainwindow->set_cue_cmd(str);
    }

  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::spi_clk_threshold_dspinbox_changed()
{
  char str[256];

  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold[2] = spi_clk_threshold_dspinbox->value();

    sprintf(str, ":BUS1:SPI:SCLK:THR %e", devparms->math_decode_threshold[2]);

    mainwindow->set_cue_cmd(str);
  }
  else if(spi_clk_src_combobox->currentIndex() > 0)
    {
      devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex() - 1] = spi_clk_threshold_dspinbox->value();

      sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_clk_src_combobox->currentIndex(),
              devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex() - 1]);

      mainwindow->set_cue_cmd(str);
    }

  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::spi_cs_threshold_dspinbox_changed()
{
  char str[256];

  if(devparms->modelserie == 6)
  {
    devparms->math_decode_threshold[3] = spi_cs_threshold_dspinbox->value();

    sprintf(str, ":BUS1:SPI:SS:THR %e", devparms->math_decode_threshold[3]);

    mainwindow->set_cue_cmd(str);
  }
  else if(spi_cs_src_combobox->currentIndex() > 0)
    {
      devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1] = spi_cs_threshold_dspinbox->value();

      sprintf(str, ":DEC1:THRE:CHAN%i %e", spi_cs_src_combobox->currentIndex(),
              devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1]);

      mainwindow->set_cue_cmd(str);
    }

  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::spi_src_combobox_clicked(int)
{
  spi_threshold_auto_clicked(devparms->math_decode_threshold_auto);
}


void UI_decoder_window::spi_threshold_auto_clicked(int idx)
{
  char str[256];

  devparms->math_decode_threshold_auto = idx;

  if(devparms->modelserie != 6)
  {
    if(idx == 0)
    {
      mainwindow->set_cue_cmd(":DEC1:THRE:AUTO 0");
    }
    else
    {
      mainwindow->set_cue_cmd(":DEC1:THRE:AUTO 1");
    }
  }

  if((spi_clk_src_combobox->currentIndex() > 0) && (idx == 0))
  {
    spi_clk_threshold_dspinbox->setEnabled(true);

    if(devparms->modelserie == 6)
    {
      spi_clk_threshold_dspinbox->setValue(devparms->math_decode_threshold[2]);

      sprintf(str, ":BUS1:SPI:SCLK:THR %e", devparms->math_decode_threshold[2]);

      mainwindow->set_cue_cmd(str);
    }
    else
    {
      spi_clk_threshold_dspinbox->setValue(devparms->math_decode_threshold[spi_clk_src_combobox->currentIndex() - 1]);

      sprintf(str, ":DEC1:SPI:CLK:CHAN%i", spi_clk_src_combobox->currentIndex());

      mainwindow->set_cue_cmd(str);
    }
  }
  else
  {
    spi_clk_threshold_dspinbox->setValue(0.0);

    spi_clk_threshold_dspinbox->setEnabled(false);
  }

  if((spi_mosi_src_combobox->currentIndex() > 0) && (idx == 0))
  {
    spi_mosi_threshold_dspinbox->setEnabled(true);

    if(devparms->modelserie == 6)
    {
      spi_mosi_threshold_dspinbox->setValue(devparms->math_decode_threshold[1]);

      sprintf(str, ":BUS1:SPI:MOSI:THR %e", devparms->math_decode_threshold[1]);

      mainwindow->set_cue_cmd(str);
    }
    else
    {
      spi_mosi_threshold_dspinbox->setValue(devparms->math_decode_threshold[spi_mosi_src_combobox->currentIndex() - 1]);

      sprintf(str, ":DEC1:SPI:MOSI:CHAN%i", spi_mosi_src_combobox->currentIndex());

      mainwindow->set_cue_cmd(str);
    }
  }
  else
  {
    spi_mosi_threshold_dspinbox->setValue(0.0);

    spi_mosi_threshold_dspinbox->setEnabled(false);
  }

  if((spi_miso_src_combobox->currentIndex() > 0) && (idx == 0))
  {
    spi_miso_threshold_dspinbox->setEnabled(true);

    if(devparms->modelserie == 6)
    {
      spi_miso_threshold_dspinbox->setValue(devparms->math_decode_threshold[0]);

      sprintf(str, ":BUS1:SPI:MISO:THR %e", devparms->math_decode_threshold[0]);

      mainwindow->set_cue_cmd(str);
    }
    else
    {
      spi_miso_threshold_dspinbox->setValue(devparms->math_decode_threshold[spi_miso_src_combobox->currentIndex() - 1]);

      sprintf(str, ":DEC1:SPI:MISO:CHAN%i", spi_miso_src_combobox->currentIndex());

      mainwindow->set_cue_cmd(str);
    }
  }
  else
  {
    spi_miso_threshold_dspinbox->setValue(0.0);

    spi_miso_threshold_dspinbox->setEnabled(false);
  }

  if((spi_cs_src_combobox->currentIndex() > 0) && (idx == 0))
  {
    spi_cs_threshold_dspinbox->setEnabled(true);

    if(devparms->modelserie == 6)
    {
      spi_cs_threshold_dspinbox->setValue(devparms->math_decode_threshold[3]);

      sprintf(str, ":BUS1:SPI:SS:THR %e", devparms->math_decode_threshold[3]);

      mainwindow->set_cue_cmd(str);
    }
    else
    {
      spi_cs_threshold_dspinbox->setValue(devparms->math_decode_threshold[spi_cs_src_combobox->currentIndex() - 1]);

      sprintf(str, ":DEC1:SPI:CS:CHAN%i", spi_cs_src_combobox->currentIndex());

      mainwindow->set_cue_cmd(str);
    }
  }
  else
  {
    spi_cs_threshold_dspinbox->setValue(0.0);

    spi_cs_threshold_dspinbox->setEnabled(false);
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



















