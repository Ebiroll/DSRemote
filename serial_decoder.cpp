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




void UI_Mainwindow::serial_decoder(void)
{
  int i, j,
      threshold[MAX_CHNS],
      y_range[MAX_CHNS],
      uart_tx_start,
      data_tx_bit,
      uart_rx_start,
      data_rx_bit;

  unsigned int val=0;

  short s_max, s_min;

  double uart_sample_per_bit,
         uart_tx_x_pos,
         uart_rx_x_pos;

  devparms.math_decode_uart_tx_nval = 0;

  devparms.math_decode_uart_rx_nval = 0;

  if(devparms.wavebufsz < 32)  return;

  if(devparms.math_decode_threshold_auto)
  {
    for(j=0; j<MAX_CHNS; j++)
    {
      s_max = -32768;
      s_min = 32767;

      for(i=0; i<devparms.wavebufsz; i++)
      {
        if(devparms.wavebuf[j][i] > s_max)  s_max = devparms.wavebuf[j][i];
        if(devparms.wavebuf[j][i] < s_min)  s_min = devparms.wavebuf[j][i];
      }

      y_range[j] = s_max - s_min;

      threshold[j] = (s_max + s_min) / 2;
    }
  }
  else
  {
    for(j=0; j<MAX_CHNS; j++)
    {
      if(devparms.modelserie == 6)
      {
        // FIXME
      }
      else
      {
        threshold[j] = devparms.math_decode_threshold[j];
      }
    }
  }

  if(devparms.math_decode_mode == DECODE_MODE_UART)
  {
    devparms.math_decode_uart_tx_nval = 0;

    devparms.math_decode_uart_rx_nval = 0;

    if(devparms.timebasedelayenable)
    {
      uart_sample_per_bit = (100.0 / devparms.timebasedelayscale) / (double)devparms.math_decode_uart_baud;
    }
    else
    {
      uart_sample_per_bit = (100.0 / devparms.timebasescale) / (double)devparms.math_decode_uart_baud;
    }

    if(uart_sample_per_bit < 3)  return;

    uart_tx_start = 0;

    data_tx_bit = 0;

    uart_tx_x_pos = 1;

    uart_rx_start = 0;

    data_rx_bit = 0;

    uart_rx_x_pos = 1;

    if(devparms.math_decode_uart_tx)
    {
      if(devparms.chandisplay[devparms.math_decode_uart_tx - 1])  // don't try to decode if channel isn't enabled...
      {
        if(y_range[devparms.math_decode_uart_tx - 1] > 10)  // don't try to decode if amplitude of signal is too low...
        {
          for(i=1; i<devparms.wavebufsz; i++)
          {
            if(devparms.math_decode_uart_tx_nval >= DECODE_MAX_UART_CHARS)
            {
              break;
            }

            if(!uart_tx_start)
            {
              if(devparms.math_decode_uart_pol)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i-1] >= threshold[devparms.math_decode_uart_tx - 1])
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] < threshold[devparms.math_decode_uart_tx - 1])
                  {
                    uart_tx_start = 1;

                    val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
              else
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i-1] < threshold[devparms.math_decode_uart_tx - 1])
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] >= threshold[devparms.math_decode_uart_tx - 1])
                  {
                    uart_tx_start = 1;

                    val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
            }
            else
            {
              if(devparms.math_decode_uart_pol)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] >= threshold[devparms.math_decode_uart_tx - 1])
                {
                  val += (1 << data_tx_bit);
                }
              }
              else
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] < threshold[devparms.math_decode_uart_tx - 1])
                {
                  val += (1 << data_tx_bit);
                }
              }

              if(++data_tx_bit == devparms.math_decode_uart_width)
              {
                if((devparms.math_decode_uart_end) && (devparms.math_decode_format != 4))  // little endian?
                {
                  val = reverse_bitorder(val);

                  val >>= (8 - data_tx_bit);
                }

                devparms.math_decode_uart_tx_val[devparms.math_decode_uart_tx_nval] = val;

                devparms.math_decode_uart_tx_val_pos[devparms.math_decode_uart_tx_nval++] = i - (data_tx_bit * uart_sample_per_bit);

                data_tx_bit = 0;

                uart_tx_start = 0;

                uart_tx_x_pos += uart_sample_per_bit;

                if(devparms.math_decode_uart_stop == 1)
                {
                  uart_tx_x_pos += uart_sample_per_bit / 2;
                }
                else if(devparms.math_decode_uart_stop == 2)
                  {
                    uart_tx_x_pos += uart_sample_per_bit;
                  }

                if(devparms.math_decode_uart_par)
                {
                  uart_tx_x_pos += uart_sample_per_bit;
                }

                i = uart_tx_x_pos - 1;
              }
              else
              {
                uart_tx_x_pos += uart_sample_per_bit;

                i = uart_tx_x_pos - 1;
              }
            }
          }
        }
      }
    }

    if(devparms.math_decode_uart_rx)
    {
      if(devparms.chandisplay[devparms.math_decode_uart_rx - 1])  // don't try to decode if channel isn't enabled...
      {
        if(y_range[devparms.math_decode_uart_rx - 1] > 10)  // don't try to decode if amplitude of signal is too low...
        {
          for(i=1; i<devparms.wavebufsz; i++)
          {
            if(devparms.math_decode_uart_rx_nval >= DECODE_MAX_UART_CHARS)
            {
              break;
            }

            if(!uart_rx_start)
            {
              if(devparms.math_decode_uart_pol)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i-1] >= threshold[devparms.math_decode_uart_rx - 1])
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] < threshold[devparms.math_decode_uart_rx - 1])
                  {
                    uart_rx_start = 1;

                    val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
              else
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i-1] < threshold[devparms.math_decode_uart_rx - 1])
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] >= threshold[devparms.math_decode_uart_rx - 1])
                  {
                    uart_rx_start = 1;

                    val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
            }
            else
            {
              if(devparms.math_decode_uart_pol)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] >= threshold[devparms.math_decode_uart_rx - 1])
                {
                  val += (1 << data_rx_bit);
                }
              }
              else
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] < threshold[devparms.math_decode_uart_rx - 1])
                {
                  val += (1 << data_rx_bit);
                }
              }

              if(++data_rx_bit == devparms.math_decode_uart_width)
              {
                if((devparms.math_decode_uart_end) && (devparms.math_decode_format != 4))  // little endian?
                {
                  val = reverse_bitorder(val);

                  val >>= (8 - data_rx_bit);
                }

                devparms.math_decode_uart_rx_val[devparms.math_decode_uart_rx_nval] = val;

                devparms.math_decode_uart_rx_val_pos[devparms.math_decode_uart_rx_nval++] = i - (data_rx_bit * uart_sample_per_bit);

                data_rx_bit = 0;

                uart_rx_start = 0;

                uart_rx_x_pos += uart_sample_per_bit;

                if(devparms.math_decode_uart_stop == 1)
                {
                  uart_rx_x_pos += uart_sample_per_bit / 2;
                }
                else if(devparms.math_decode_uart_stop == 2)
                  {
                    uart_rx_x_pos += uart_sample_per_bit;
                  }

                if(devparms.math_decode_uart_par)
                {
                  uart_rx_x_pos += uart_sample_per_bit;
                }

                i = uart_rx_x_pos - 1;
              }
              else
              {
                uart_rx_x_pos += uart_sample_per_bit;

                i = uart_rx_x_pos - 1;
              }
            }
          }
        }
      }
    }
  }
}


inline unsigned char UI_Mainwindow::reverse_bitorder(unsigned char byte)
{
  byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
  byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
  byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;

  return byte;
}






















