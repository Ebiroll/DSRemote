/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2016, 2017, 2018 Teunis van Beelen
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
*
* RS-232 protocol:
*
* settings in this example: 8N1
*
* character value in this example: 'G', 71, 0x47, 0b01000111
*
*
*
*    idle                   start   1     1     1     0     0     0     1     0   stop            idle
*                            bit                                                   bit
* -------------------------+     +-----+-----+-----+                 +-----+     +-----+-----------------------------
*             +12V         |     |                 |                 |     |     |
*                          |     |                 |                 |     |     |
*                          |     |                 |                 |     |     |
*                          |     |                 |                 |     |     |
*                    -12V  +-----+                 +-----+-----+-----+     +-----+
*
*                                   0     1     2     3     4     5     6     7
*
*                                  LSB                                       MSB
*
*
* default (positive )line RS-232 LSB first (little endian)
*
* negative: everything inverted, used by microcontrollers, TTL/CMOS level
*
***************************************************************************
*/




void UI_Mainwindow::serial_decoder(struct device_settings *d_parms)
{
  int i, j,
      threshold[MAX_CHNS],
      uart_tx_start,
      uart_tx_data_bit,
      uart_rx_start,
      uart_rx_data_bit,
      uart_parity_bit,
      uart_parity,
      spi_data_mosi_bit,
      spi_data_miso_bit,
      spi_mosi_bit0_pos=0,
      spi_miso_bit0_pos=0,
      spi_clk_new,
      spi_clk_old,
      spi_chars=1,
      spi_timeout,
      spi_timeout_cntr,
      stop_bit_error;

  unsigned int uart_val=0,
               spi_mosi_val=0,
               spi_miso_val=0;

  short s_max, s_min;

  double uart_sample_per_bit,
         uart_tx_x_pos,
         uart_rx_x_pos,
         bit_per_volt;

  d_parms->math_decode_uart_tx_nval = 0;

  d_parms->math_decode_uart_rx_nval = 0;

  d_parms->math_decode_spi_mosi_nval = 0;

  d_parms->math_decode_spi_miso_nval = 0;

  if(d_parms->wavebufsz < 32)  return;

  if(d_parms->math_decode_threshold_auto)
  {
    for(j=0; j<MAX_CHNS; j++)
    {
      if(!d_parms->chandisplay[j])  continue;

      s_max = -32768;
      s_min = 32767;

      for(i=0; i<d_parms->wavebufsz; i++)
      {
        if(d_parms->wavebuf[j][i] > s_max)  s_max = d_parms->wavebuf[j][i];
        if(d_parms->wavebuf[j][i] < s_min)  s_min = d_parms->wavebuf[j][i];
      }

      threshold[j] = (s_max + s_min) / 2;
    }
  }
  else
  {
    if(d_parms->math_decode_mode == DECODE_MODE_UART)
    {
      if(d_parms->math_decode_uart_tx)
      {
        if(d_parms->modelserie == 6)
        {
          bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_uart_tx - 1];

          threshold[d_parms->math_decode_uart_tx - 1] =
            (d_parms->math_decode_threshold_uart_tx +
            d_parms->chanoffset[d_parms->math_decode_uart_tx - 1])
            * bit_per_volt;
        }
        else
        {
          bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_uart_tx - 1];

          threshold[d_parms->math_decode_uart_tx - 1] =
            (d_parms->math_decode_threshold[d_parms->math_decode_uart_tx - 1] +
            d_parms->chanoffset[d_parms->math_decode_uart_tx - 1])
            * bit_per_volt;
        }
      }

      if(d_parms->math_decode_uart_rx)
      {
        if(d_parms->modelserie == 6)
        {
          bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_uart_rx - 1];

          threshold[d_parms->math_decode_uart_rx - 1] =
            (d_parms->math_decode_threshold_uart_rx +
            d_parms->chanoffset[d_parms->math_decode_uart_rx - 1])
            * bit_per_volt;
        }
        else
        {
          bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_uart_rx - 1];

          threshold[d_parms->math_decode_uart_rx - 1] =
            (d_parms->math_decode_threshold[d_parms->math_decode_uart_rx - 1] +
            d_parms->chanoffset[d_parms->math_decode_uart_rx - 1])
            * bit_per_volt;
        }
      }
    }
    else if(d_parms->math_decode_mode == DECODE_MODE_SPI)
      {
        if(d_parms->modelserie == 6)
        {
          bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_spi_clk];

          threshold[d_parms->math_decode_spi_clk] =
            (d_parms->math_decode_threshold[2] +
            d_parms->chanoffset[d_parms->math_decode_spi_clk])
            * bit_per_volt;
        }
        else
        {
          bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_spi_clk];

          threshold[d_parms->math_decode_spi_clk] =
            (d_parms->math_decode_threshold[d_parms->math_decode_spi_clk] +
            d_parms->chanoffset[d_parms->math_decode_spi_clk])
            * bit_per_volt;
        }

        if(d_parms->math_decode_spi_mosi)
        {
          if(d_parms->modelserie == 6)
          {
            bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_spi_mosi - 1];

            threshold[d_parms->math_decode_spi_mosi - 1] =
              (d_parms->math_decode_threshold[1] +
              d_parms->chanoffset[d_parms->math_decode_spi_mosi - 1])
              * bit_per_volt;
          }
          else
          {
            bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_spi_mosi - 1];

            threshold[d_parms->math_decode_spi_mosi - 1] =
              (d_parms->math_decode_threshold[d_parms->math_decode_spi_mosi - 1] +
              d_parms->chanoffset[d_parms->math_decode_spi_mosi - 1])
              * bit_per_volt;
          }
        }

        if(d_parms->math_decode_spi_miso)
        {
          if(d_parms->modelserie == 6)
          {
            bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_spi_miso - 1];

            threshold[d_parms->math_decode_spi_miso - 1] =
              (d_parms->math_decode_threshold[0] +
              d_parms->chanoffset[d_parms->math_decode_spi_miso -1])
              * bit_per_volt;
          }
          else
          {
            bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_spi_miso - 1];

            threshold[d_parms->math_decode_spi_miso - 1] =
              (d_parms->math_decode_threshold[d_parms->math_decode_spi_miso - 1] +
              d_parms->chanoffset[d_parms->math_decode_spi_miso -1])
              * bit_per_volt;
          }
        }

        if(d_parms->math_decode_spi_cs)
        {
          if(d_parms->modelserie == 6)
          {
            bit_per_volt = 32.0 / d_parms->chanscale[d_parms->math_decode_spi_cs - 1];

            threshold[d_parms->math_decode_spi_cs - 1] =
              (d_parms->math_decode_threshold[3] +
              d_parms->chanoffset[d_parms->math_decode_spi_cs - 1])
              * bit_per_volt;
          }
          else
          {
            bit_per_volt = 25.0 / d_parms->chanscale[d_parms->math_decode_spi_cs - 1];

            threshold[d_parms->math_decode_spi_cs - 1] =
              (d_parms->math_decode_threshold[d_parms->math_decode_spi_cs - 1] +
              d_parms->chanoffset[d_parms->math_decode_spi_cs - 1])
              * bit_per_volt;
          }
        }
      }
  }

  if(d_parms->math_decode_mode == DECODE_MODE_UART)
  {
    d_parms->math_decode_uart_tx_nval = 0;

    d_parms->math_decode_uart_rx_nval = 0;

    if(d_parms->wave_mem_view_enabled)
    {
      uart_sample_per_bit = d_parms->samplerate / (double)d_parms->math_decode_uart_baud;
    }
    else
    {
      if(d_parms->timebasedelayenable)
      {
        uart_sample_per_bit = (100.0 / d_parms->timebasedelayscale) / (double)d_parms->math_decode_uart_baud;
      }
      else
      {
        uart_sample_per_bit = (100.0 / d_parms->timebasescale) / (double)d_parms->math_decode_uart_baud;
      }
    }

    if(uart_sample_per_bit < 3)  return;

    uart_tx_start = 0;

    uart_tx_data_bit = 0;

    uart_tx_x_pos = 1;

    uart_rx_start = 0;

    uart_rx_data_bit = 0;

    uart_rx_x_pos = 1;

//     if(d_parms->modelserie == 6)
//     {
//       printf("chanscale: %f\n"
//             "chanoffset: %f\n"
//             "math_decode_threshold_uart_tx: %f\n"
//             "math_decode_uart_tx: %i\n"
//             "threshold: %i\n"
//             "uart_sample_per_bit: %f\n"
//             "wavebufsz: %i\n",
//             d_parms->chanscale[d_parms->math_decode_uart_tx - 1],
//             d_parms->chanoffset[d_parms->math_decode_uart_tx - 1],
//             d_parms->math_decode_threshold_uart_tx,
//             d_parms->math_decode_uart_tx,
//             threshold[d_parms->math_decode_uart_tx - 1],
//             uart_sample_per_bit,
//             d_parms->wavebufsz);
//     }
//     else
//     {
//
//       printf("chanscale: %f\n"
//             "chanoffset: %f\n"
//             "math_decode_threshold: %f\n"
//             "math_decode_uart_tx: %i\n"
//             "threshold: %i\n"
//             "uart_sample_per_bit: %f\n"
//             "wavebufsz: %i\n",
//             d_parms->chanscale[d_parms->math_decode_uart_tx - 1],
//             d_parms->chanoffset[d_parms->math_decode_uart_tx - 1],
//             d_parms->math_decode_threshold[d_parms->math_decode_uart_tx - 1],
//             d_parms->math_decode_uart_tx,
//             threshold[d_parms->math_decode_uart_tx - 1],
//             uart_sample_per_bit,
//             d_parms->wavebufsz);
//     }

    if(d_parms->math_decode_uart_tx)
    {
      if(d_parms->chandisplay[d_parms->math_decode_uart_tx - 1])  // don't try to decode if channel isn't enabled...
      {
        for(i=1; i<d_parms->wavebufsz; i++)
        {
          if(d_parms->math_decode_uart_tx_nval >= DECODE_MAX_CHARS)
          {
            break;
          }

          if(!uart_tx_start)
          {
            if(d_parms->math_decode_uart_pol)  // positive, line level RS-232
            {
              if(d_parms->modelserie == 6)
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i-1] >= d_parms->math_decode_threshold_uart_tx)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] < d_parms->math_decode_threshold_uart_tx)
                  {
                    uart_tx_start = 1;

                    uart_val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i-1] >= threshold[d_parms->math_decode_uart_tx - 1])
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] < threshold[d_parms->math_decode_uart_tx - 1])
                  {
                    uart_tx_start = 1;

                    uart_val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
            }
            else  // negative, cpu level TTL/CMOS
            {
              if(d_parms->modelserie == 6)
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i-1] < d_parms->math_decode_threshold_uart_tx)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= d_parms->math_decode_threshold_uart_tx)
                  {
                    uart_tx_start = 1;

                    uart_val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i-1] < threshold[d_parms->math_decode_uart_tx - 1])
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= threshold[d_parms->math_decode_uart_tx - 1])
                  {
                    uart_tx_start = 1;

                    uart_val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
            }
          }
          else  // uart_rx_start != 0
          {
            if(d_parms->modelserie == 6)
            {
              if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= d_parms->math_decode_threshold_uart_tx)
              {
               uart_val += (1 << uart_tx_data_bit);
              }
            }
            else  // modelserie = 1, 2 or 4
            {
              if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= threshold[d_parms->math_decode_uart_tx - 1])
              {
               uart_val += (1 << uart_tx_data_bit);
              }
            }

            if(++uart_tx_data_bit == d_parms->math_decode_uart_width)
            {
              if((d_parms->math_decode_uart_end) && (d_parms->math_decode_format != 4))  // big endian?
              {
               uart_val = reverse_bitorder_8(uart_val);

               uart_val >>= (8 - uart_tx_data_bit);
              }

              if(!d_parms->math_decode_uart_pol)  // positive, line level RS-232 or negative, cpu level TTL/CMOS?
              {
               uart_val = ~uart_val;

               uart_val &= (0xff >> (8 - uart_tx_data_bit));
              }

              d_parms->math_decode_uart_tx_val[d_parms->math_decode_uart_tx_nval] = uart_val;

              d_parms->math_decode_uart_tx_val_pos[d_parms->math_decode_uart_tx_nval] =
               i - (uart_tx_data_bit * uart_sample_per_bit) + (0.5 * uart_sample_per_bit);

              uart_tx_data_bit = 0;

              uart_tx_start = 0;

              d_parms->math_decode_uart_tx_err[d_parms->math_decode_uart_tx_nval] = 0;

              if(d_parms->math_decode_uart_par)
              {
                uart_tx_x_pos += uart_sample_per_bit;

                i = uart_tx_x_pos;

                if(i < d_parms->wavebufsz)
                {
                  if(d_parms->modelserie == 6)
                  {
                    if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= d_parms->math_decode_threshold_uart_tx)
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 1;
                      }
                      else
                      {
                        uart_parity_bit = 0;
                      }
                    }
                    else
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 0;
                      }
                      else
                      {
                        uart_parity_bit = 1;
                      }
                    }
                  }
                  else
                  {
                    if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= threshold[d_parms->math_decode_uart_tx - 1])
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 1;
                      }
                      else
                      {
                        uart_parity_bit = 0;
                      }
                    }
                    else
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 0;
                      }
                      else
                      {
                        uart_parity_bit = 1;
                      }
                    }
                  }

                  for(j=0, uart_parity=0; j<d_parms->math_decode_uart_width; j++)
                  {
                    uart_parity += ((uart_val >> j) & 1);
                  }

                  if(d_parms->math_decode_uart_par & 1)
                  {
                    uart_parity++;
                  }

                  if((uart_parity & 1) != uart_parity_bit)
                  {
                    d_parms->math_decode_uart_tx_err[d_parms->math_decode_uart_tx_nval] = 1;
                  }
                }
              }

              uart_tx_x_pos += uart_sample_per_bit;

              i = uart_tx_x_pos;

              stop_bit_error = 0;  // check stop bit

              if(i < d_parms->wavebufsz)
              {
                if(d_parms->modelserie == 6)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= d_parms->math_decode_threshold_uart_tx)
                  {
                    stop_bit_error = 1;
                  }
                }
                else  // modelserie = 1, 2 or 4
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_tx - 1][i] >= threshold[d_parms->math_decode_uart_tx - 1])
                  {
                    stop_bit_error = 1;
                  }
                }

                if(d_parms->math_decode_uart_pol)
                {
                  if(stop_bit_error)
                  {
                    stop_bit_error = 0;
                  }
                  else
                  {
                    stop_bit_error = 1;
                  }
                }

                if(stop_bit_error)
                {
                  d_parms->math_decode_uart_tx_err[d_parms->math_decode_uart_tx_nval] = 1;
                }
              }

              if(d_parms->math_decode_uart_stop == 1)
              {
                uart_tx_x_pos += uart_sample_per_bit / 2;
              }
              else if(d_parms->math_decode_uart_stop == 2)
                {
                  uart_tx_x_pos += uart_sample_per_bit;
                }

              i = uart_tx_x_pos - 1;

              d_parms->math_decode_uart_tx_nval++;
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

    if(d_parms->math_decode_uart_rx)
    {
      if(d_parms->chandisplay[d_parms->math_decode_uart_rx - 1])  // don't try to decode if channel isn't enabled...
      {
        for(i=1; i<d_parms->wavebufsz; i++)
        {
          if(d_parms->math_decode_uart_rx_nval >= DECODE_MAX_CHARS)
          {
            break;
          }

          if(!uart_rx_start)
          {
            if(d_parms->math_decode_uart_pol)  // positive, line level RS-232
            {
              if(d_parms->modelserie == 6)
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i-1] >= d_parms->math_decode_threshold_uart_rx)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] < d_parms->math_decode_threshold_uart_rx)
                  {
                    uart_rx_start = 1;

                    uart_val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i-1] >= threshold[d_parms->math_decode_uart_rx - 1])
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] < threshold[d_parms->math_decode_uart_rx - 1])
                  {
                    uart_rx_start = 1;

                    uart_val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
            }
            else  // negative, cpu level TTL/CMOS
            {
              if(d_parms->modelserie == 6)
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i-1] < d_parms->math_decode_threshold_uart_rx)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= d_parms->math_decode_threshold_uart_rx)
                  {
                    uart_rx_start = 1;

                    uart_val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
              {
                if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i-1] < threshold[d_parms->math_decode_uart_rx - 1])
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= threshold[d_parms->math_decode_uart_rx - 1])
                  {
                    uart_rx_start = 1;

                    uart_val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
            }
          }
          else  // uart_rx_start != 0
          {
            if(d_parms->modelserie == 6)
            {
              if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= d_parms->math_decode_threshold_uart_rx)
              {
               uart_val += (1 << uart_rx_data_bit);
              }
            }
            else  // modelserie = 1, 2 or 4
            {
              if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= threshold[d_parms->math_decode_uart_rx - 1])
              {
               uart_val += (1 << uart_rx_data_bit);
              }
            }

            if(++uart_rx_data_bit == d_parms->math_decode_uart_width)
            {
              if((d_parms->math_decode_uart_end) && (d_parms->math_decode_format != 4))  // big endian?
              {
               uart_val = reverse_bitorder_8(uart_val);

               uart_val >>= (8 - uart_rx_data_bit);
              }

              if(!d_parms->math_decode_uart_pol)  // positive, line level RS-232 or negative, cpu level TTL/CMOS?
              {
               uart_val = ~uart_val;

               uart_val &= (0xff >> (8 - uart_rx_data_bit));
              }

              d_parms->math_decode_uart_rx_val[d_parms->math_decode_uart_rx_nval] = uart_val;

              d_parms->math_decode_uart_rx_val_pos[d_parms->math_decode_uart_rx_nval] =
               i - (uart_rx_data_bit * uart_sample_per_bit) + (0.5 * uart_sample_per_bit);

              uart_rx_data_bit = 0;

              uart_rx_start = 0;

              d_parms->math_decode_uart_rx_err[d_parms->math_decode_uart_rx_nval] = 0;

              if(d_parms->math_decode_uart_par)
              {
                uart_rx_x_pos += uart_sample_per_bit;

                i = uart_rx_x_pos;

                if(i < d_parms->wavebufsz)
                {
                  if(d_parms->modelserie == 6)
                  {
                    if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= d_parms->math_decode_threshold_uart_rx)
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 1;
                      }
                      else
                      {
                        uart_parity_bit = 0;
                      }
                    }
                    else
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 0;
                      }
                      else
                      {
                        uart_parity_bit = 1;
                      }
                    }
                  }
                  else
                  {
                    if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= threshold[d_parms->math_decode_uart_rx - 1])
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 1;
                      }
                      else
                      {
                        uart_parity_bit = 0;
                      }
                    }
                    else
                    {
                      if(d_parms->math_decode_uart_pol)
                      {
                        uart_parity_bit = 0;
                      }
                      else
                      {
                        uart_parity_bit = 1;
                      }
                    }
                  }

                  for(j=0, uart_parity=0; j<d_parms->math_decode_uart_width; j++)
                  {
                    uart_parity += ((uart_val >> j) & 1);
                  }

                  if(d_parms->math_decode_uart_par & 1)
                  {
                    uart_parity++;
                  }

                  if((uart_parity & 1) != uart_parity_bit)
                  {
                    d_parms->math_decode_uart_rx_err[d_parms->math_decode_uart_rx_nval] = 1;
                  }
                }
              }

              uart_rx_x_pos += uart_sample_per_bit;

              i = uart_rx_x_pos;

              stop_bit_error = 0;  // check stop bit

              if(i < d_parms->wavebufsz)
              {
                if(d_parms->modelserie == 6)
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= d_parms->math_decode_threshold_uart_rx)
                  {
                    stop_bit_error = 1;
                  }
                }
                else  // modelserie = 1, 2 or 4
                {
                  if(d_parms->wavebuf[d_parms->math_decode_uart_rx - 1][i] >= threshold[d_parms->math_decode_uart_rx - 1])
                  {
                    stop_bit_error = 1;
                  }
                }

                if(d_parms->math_decode_uart_pol)
                {
                  if(stop_bit_error)
                  {
                    stop_bit_error = 0;
                  }
                  else
                  {
                    stop_bit_error = 1;
                  }
                }

                if(stop_bit_error)
                {
                  d_parms->math_decode_uart_rx_err[d_parms->math_decode_uart_rx_nval] = 1;
                }
              }

              if(d_parms->math_decode_uart_stop == 1)
              {
                uart_rx_x_pos += uart_sample_per_bit / 2;
              }
              else if(d_parms->math_decode_uart_stop == 2)
                {
                  uart_rx_x_pos += uart_sample_per_bit;
                }

              i = uart_rx_x_pos - 1;

              d_parms->math_decode_uart_rx_nval++;
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

  if(d_parms->math_decode_mode == DECODE_MODE_SPI)
  {
    d_parms->math_decode_spi_mosi_nval = 0;

    d_parms->math_decode_spi_miso_nval = 0;

    spi_data_mosi_bit = 0;

    spi_data_miso_bit = 0;

    spi_mosi_val = 0;

    spi_miso_val = 0;

    spi_timeout_cntr = 0;

    if(d_parms->math_decode_spi_width > 24)
    {
      spi_chars = 4;
    }
    else if(d_parms->math_decode_spi_width > 16)
      {
        spi_chars = 3;
      }
      else if(d_parms->math_decode_spi_width > 8)
        {
          spi_chars = 2;
        }
        else
        {
          spi_chars = 1;
        }

    if(d_parms->math_decode_spi_edge)  // sample at rising edge of spi clock?
    {
      spi_clk_old = 1;
    }
    else
    {
      spi_clk_old = 0;
    }

    if(!d_parms->chandisplay[d_parms->math_decode_spi_clk])  // without a clock we can't do much...
    {
      goto SPI_DECODE_OUT;
    }

    if(d_parms->math_decode_spi_mode)  // use chip select line?
    {
      if(d_parms->math_decode_spi_cs)  // is chip select channel selected?
      {
        if(!d_parms->chandisplay[d_parms->math_decode_spi_cs])  // is selected channel for CS enabled?
        {
          goto SPI_DECODE_OUT;
        }
      }
      else
      {
        goto SPI_DECODE_OUT;
      }
    }
    else  // use timeout to detect start of frame
    {
      if(d_parms->timebasedelayenable)
      {
        spi_timeout = d_parms->math_decode_spi_timeout / (d_parms->timebasedelayscale / 100.0);
      }
      else
      {
        spi_timeout = d_parms->math_decode_spi_timeout / (d_parms->timebasescale / 100.0);
      }
    }

    for(i=0; i<d_parms->wavebufsz; i++)
    {
      if(d_parms->math_decode_spi_mosi_nval >= DECODE_MAX_CHARS)
      {
        break;
      }

      if(d_parms->math_decode_spi_mode)  // use chip select line?
      {
        if(d_parms->math_decode_spi_select)  // use positive chip select?
        {
          if(d_parms->wavebuf[d_parms->math_decode_spi_cs - 1][i] < threshold[d_parms->math_decode_spi_cs - 1])
          {
            spi_data_mosi_bit = 0;

            spi_data_miso_bit = 0;

            spi_mosi_val = 0;

            spi_miso_val = 0;

            continue;  // chip select is not active
          }
        }
        else  // use negative chip select?
        {
          if(d_parms->wavebuf[d_parms->math_decode_spi_cs - 1][1] >= threshold[d_parms->math_decode_spi_cs - 1])
          {
            spi_data_mosi_bit = 0;

            spi_data_miso_bit = 0;

            spi_mosi_val = 0;

            spi_miso_val = 0;

            continue;  // chip select is not active
          }
        }
      }
      else  // use timeout to detect start of frame
      {
        if(spi_timeout_cntr > spi_timeout)
        {
          spi_data_mosi_bit = 0;

          spi_data_miso_bit = 0;

          spi_mosi_val = 0;

          spi_miso_val = 0;
        }
        else
        {
          spi_timeout_cntr++;
        }
      }

      if(d_parms->wavebuf[d_parms->math_decode_spi_clk][i] >= threshold[d_parms->math_decode_spi_clk])
      {
        spi_clk_new = 1;
      }
      else
      {
        spi_clk_new = 0;
      }

      if(spi_clk_old == spi_clk_new)
      {
        continue;  // no clock change
      }

      if(d_parms->math_decode_spi_edge != spi_clk_new)  // wrong clock edge?
      {
        spi_clk_old = spi_clk_new;

        continue;
      }

      spi_timeout_cntr = 0;

      spi_clk_old = spi_clk_new;

      if(d_parms->math_decode_spi_mosi)
      {
        if(d_parms->chandisplay[d_parms->math_decode_spi_mosi - 1])  // don't try to decode if channel isn't enabled...
        {
          if(d_parms->wavebuf[d_parms->math_decode_spi_mosi - 1][i] >= threshold[d_parms->math_decode_spi_mosi - 1])
          {
            spi_mosi_val += (1 << spi_data_mosi_bit);
          }

          if(!spi_data_mosi_bit)  spi_mosi_bit0_pos = i;

          if(++spi_data_mosi_bit == d_parms->math_decode_spi_width)
          {
            if((d_parms->math_decode_spi_end) && (d_parms->math_decode_format != 4))  // big endian?
            {
              spi_mosi_val = reverse_bitorder_32(spi_mosi_val);

              spi_mosi_val >>= (32 - spi_data_mosi_bit);
            }

            if(!d_parms->math_decode_spi_pol)
            {
              spi_mosi_val = ~spi_mosi_val;

              switch(spi_chars)
              {
                case 1: spi_mosi_val &= 0xff;
                        break;
                case 2: spi_mosi_val &= 0xffff;
                        break;
                case 3: spi_mosi_val &= 0xffffff;
                        break;
              }
            }

            d_parms->math_decode_spi_mosi_val[d_parms->math_decode_spi_mosi_nval] = spi_mosi_val;

            d_parms->math_decode_spi_mosi_val_pos[d_parms->math_decode_spi_mosi_nval] = spi_mosi_bit0_pos;

            d_parms->math_decode_spi_mosi_val_pos_end[d_parms->math_decode_spi_mosi_nval++] = i;

            spi_data_mosi_bit = 0;

            spi_mosi_val = 0;
          }
        }
      }

      if(d_parms->math_decode_spi_miso)
      {
        if(d_parms->chandisplay[d_parms->math_decode_spi_miso - 1])  // don't try to decode if channel isn't enabled...
        {
          if(d_parms->wavebuf[d_parms->math_decode_spi_miso - 1][i] >= threshold[d_parms->math_decode_spi_miso - 1])
          {
            spi_miso_val += (1 << spi_data_miso_bit);
          }

          if(!spi_data_miso_bit)  spi_miso_bit0_pos = i;

          if(++spi_data_miso_bit == d_parms->math_decode_spi_width)
          {
            if((d_parms->math_decode_spi_end) && (d_parms->math_decode_format != 4))  // big endian?
            {
              spi_miso_val = reverse_bitorder_32(spi_miso_val);

              spi_miso_val >>= (32 - spi_data_miso_bit);
            }

            if(!d_parms->math_decode_spi_pol)
            {
             spi_miso_val = ~spi_miso_val;
            }

            d_parms->math_decode_spi_miso_val[d_parms->math_decode_spi_miso_nval] = spi_miso_val;

            d_parms->math_decode_spi_miso_val_pos[d_parms->math_decode_spi_miso_nval] = spi_miso_bit0_pos;

            d_parms->math_decode_spi_miso_val_pos_end[d_parms->math_decode_spi_miso_nval++] = i;

            spi_data_miso_bit = 0;

            spi_miso_val = 0;
          }
        }
      }
    }

    SPI_DECODE_OUT:

    i = 0;  // FIXME
  }
}


inline unsigned char UI_Mainwindow::reverse_bitorder_8(unsigned char byte)
{
  byte = (byte & 0xf0) >> 4 | (byte & 0x0f) << 4;
  byte = (byte & 0xcc) >> 2 | (byte & 0x33) << 2;
  byte = (byte & 0xaa) >> 1 | (byte & 0x55) << 1;

  return byte;
}


inline unsigned int UI_Mainwindow::reverse_bitorder_32(unsigned int val)
{
  val = (val & 0xffff0000) >> 16 | (val & 0x0000ffff) << 16;
  val = (val & 0xff00ff00) >>  8 | (val & 0x00ff00ff) <<  8;
  val = (val & 0xf0f0f0f0) >>  4 | (val & 0x0f0f0f0f) <<  4;
  val = (val & 0xcccccccc) >>  2 | (val & 0x33333333) <<  2;
  val = (val & 0xaaaaaaaa) >>  1 | (val & 0x55555555) <<  1;

  return val;
}






























