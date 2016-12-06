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




void UI_Mainwindow::serial_decoder(void)
{
  int i, j,
      threshold[MAX_CHNS],
      uart_tx_start,
      uart_tx_data_bit,
      uart_rx_start,
      uart_rx_data_bit;
//       spi_mosi_start,
//       spi_data_mosi_bit,
//       spi_miso_start,
//       spi_data_miso_bit;

  unsigned int val=0;

  short s_max, s_min;

  double uart_sample_per_bit,
         uart_tx_x_pos,
         uart_rx_x_pos,
//         spi_clk_x_pos,
         bit_per_volt;

  devparms.math_decode_uart_tx_nval = 0;

  devparms.math_decode_uart_rx_nval = 0;

  devparms.math_decode_spi_mosi_nval = 0;

  devparms.math_decode_spi_miso_nval = 0;

  if(devparms.wavebufsz < 32)  return;

  if(devparms.math_decode_threshold_auto)
  {
    for(j=0; j<MAX_CHNS; j++)
    {
      if(!devparms.chandisplay[j])  continue;

      s_max = -32768;
      s_min = 32767;

      for(i=0; i<devparms.wavebufsz; i++)
      {
        if(devparms.wavebuf[j][i] > s_max)  s_max = devparms.wavebuf[j][i];
        if(devparms.wavebuf[j][i] < s_min)  s_min = devparms.wavebuf[j][i];
      }

      threshold[j] = (s_max + s_min) / 2;
    }
  }
  else
  {
    if(devparms.math_decode_mode == DECODE_MODE_UART)
    {
      if(devparms.math_decode_uart_tx)
      {
        if(devparms.modelserie == 6)
        {
          bit_per_volt = 32.0 / devparms.chanscale[devparms.math_decode_uart_tx - 1];

          threshold[devparms.math_decode_uart_tx - 1] =
            (devparms.math_decode_threshold_uart_tx +
            devparms.chanoffset[devparms.math_decode_uart_tx - 1])
            * bit_per_volt;
        }
        else
        {
          bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_uart_tx - 1];

          threshold[devparms.math_decode_uart_tx - 1] =
            (devparms.math_decode_threshold[devparms.math_decode_uart_tx - 1] +
            devparms.chanoffset[devparms.math_decode_uart_tx - 1])
            * bit_per_volt;
        }
      }

      if(devparms.math_decode_uart_rx)
      {
        if(devparms.modelserie == 6)
        {
          bit_per_volt = 32.0 / devparms.chanscale[devparms.math_decode_uart_rx - 1];

          threshold[devparms.math_decode_uart_rx - 1] =
            (devparms.math_decode_threshold_uart_rx +
            devparms.chanoffset[devparms.math_decode_uart_rx - 1])
            * bit_per_volt;
        }
        else
        {
          bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_uart_rx - 1];

          threshold[devparms.math_decode_uart_rx - 1] =
            (devparms.math_decode_threshold[devparms.math_decode_uart_rx - 1] +
            devparms.chanoffset[devparms.math_decode_uart_rx - 1])
            * bit_per_volt;
        }
      }
    }
    else if(devparms.math_decode_mode == DECODE_MODE_SPI)
      {
        if(devparms.modelserie == 6)
        {
        // :FIXME
        }
        else
        {
          bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_spi_clk];

          threshold[devparms.math_decode_spi_clk] =
            (devparms.math_decode_threshold[devparms.math_decode_spi_clk] +
            devparms.chanoffset[devparms.math_decode_spi_clk])
            * bit_per_volt;
        }

        if(devparms.math_decode_spi_mosi)
        {
          if(devparms.modelserie == 6)
          {
          // :FIXME
          }
          else
          {
            bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_spi_mosi];

            threshold[devparms.math_decode_spi_mosi] =
              (devparms.math_decode_threshold[devparms.math_decode_spi_mosi] +
              devparms.chanoffset[devparms.math_decode_spi_mosi])
              * bit_per_volt;
          }
        }

        if(devparms.math_decode_spi_miso)
        {
          if(devparms.modelserie == 6)
          {
          // :FIXME
          }
          else
          {
            bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_spi_miso];

            threshold[devparms.math_decode_spi_miso] =
              (devparms.math_decode_threshold[devparms.math_decode_spi_miso] +
              devparms.chanoffset[devparms.math_decode_spi_miso])
              * bit_per_volt;
          }
        }

        if(devparms.math_decode_spi_cs)
        {
          if(devparms.modelserie == 6)
          {
          // :FIXME
          }
          else
          {
            bit_per_volt = 25.0 / devparms.chanscale[devparms.math_decode_spi_cs];

            threshold[devparms.math_decode_spi_cs] =
              (devparms.math_decode_threshold[devparms.math_decode_spi_cs] +
              devparms.chanoffset[devparms.math_decode_spi_cs])
              * bit_per_volt;
          }
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

    uart_tx_data_bit = 0;

    uart_tx_x_pos = 1;

    uart_rx_start = 0;

    uart_rx_data_bit = 0;

    uart_rx_x_pos = 1;

//     if(devparms.modelserie == 6)
//     {
//       printf("chanscale: %f\n"
//             "chanoffset: %f\n"
//             "math_decode_threshold_uart_tx: %f\n"
//             "math_decode_uart_tx: %i\n"
//             "threshold: %i\n"
//             "uart_sample_per_bit: %f\n"
//             "wavebufsz: %i\n",
//             devparms.chanscale[devparms.math_decode_uart_tx - 1],
//             devparms.chanoffset[devparms.math_decode_uart_tx - 1],
//             devparms.math_decode_threshold_uart_tx,
//             devparms.math_decode_uart_tx,
//             threshold[devparms.math_decode_uart_tx - 1],
//             uart_sample_per_bit,
//             devparms.wavebufsz);
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
//             devparms.chanscale[devparms.math_decode_uart_tx - 1],
//             devparms.chanoffset[devparms.math_decode_uart_tx - 1],
//             devparms.math_decode_threshold[devparms.math_decode_uart_tx - 1],
//             devparms.math_decode_uart_tx,
//             threshold[devparms.math_decode_uart_tx - 1],
//             uart_sample_per_bit,
//             devparms.wavebufsz);
//     }

    if(devparms.math_decode_uart_tx)
    {
      if(devparms.chandisplay[devparms.math_decode_uart_tx - 1])  // don't try to decode if channel isn't enabled...
      {
        for(i=1; i<devparms.wavebufsz; i++)
        {
          if(devparms.math_decode_uart_tx_nval >= DECODE_MAX_CHARS)
          {
            break;
          }

          if(!uart_tx_start)
          {
            if(devparms.math_decode_uart_pol)  // positive, line level RS-232
            {
              if(devparms.modelserie == 6)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i-1] >= devparms.math_decode_threshold_uart_tx)
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] < devparms.math_decode_threshold_uart_tx)
                  {
                    uart_tx_start = 1;

                    val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
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
            }
            else  // negative, cpu level TTL/CMOS
            {
              if(devparms.modelserie == 6)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i-1] < devparms.math_decode_threshold_uart_tx)
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] >= devparms.math_decode_threshold_uart_tx)
                  {
                    uart_tx_start = 1;

                    val = 0;

                    uart_tx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_tx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
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
          }
          else  // uart_rx_start != 0
          {
            if(devparms.modelserie == 6)
            {
              if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] >= devparms.math_decode_threshold_uart_tx)
              {
                val += (1 << uart_tx_data_bit);
              }
            }
            else  // modelserie = 1, 2 or 4
            {
              if(devparms.wavebuf[devparms.math_decode_uart_tx - 1][i] >= threshold[devparms.math_decode_uart_tx - 1])
              {
                val += (1 << uart_tx_data_bit);
              }
            }

            if(++uart_tx_data_bit == devparms.math_decode_uart_width)
            {
              if((devparms.math_decode_uart_end) && (devparms.math_decode_format != 4))  // big endian?
              {
                val = reverse_bitorder(val);

                val >>= (8 - uart_tx_data_bit);
              }

              if(!devparms.math_decode_uart_pol)  // positive, line level RS-232 or negative, cpu level TTL/CMOS?
              {
                val = ~val;
              }

              devparms.math_decode_uart_tx_val[devparms.math_decode_uart_tx_nval] = val;

              devparms.math_decode_uart_tx_val_pos[devparms.math_decode_uart_tx_nval++] = i - (uart_tx_data_bit * uart_sample_per_bit);

              uart_tx_data_bit = 0;

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

    if(devparms.math_decode_uart_rx)
    {
      if(devparms.chandisplay[devparms.math_decode_uart_rx - 1])  // don't try to decode if channel isn't enabled...
      {
        for(i=1; i<devparms.wavebufsz; i++)
        {
          if(devparms.math_decode_uart_rx_nval >= DECODE_MAX_CHARS)
          {
            break;
          }

          if(!uart_rx_start)
          {
            if(devparms.math_decode_uart_pol)  // positive, line level RS-232
            {
              if(devparms.modelserie == 6)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i-1] >= devparms.math_decode_threshold_uart_rx)
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] < devparms.math_decode_threshold_uart_rx)
                  {

                    uart_rx_start = 1;

                    val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
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
            }
            else  // negative, cpu level TTL/CMOS
            {
              if(devparms.modelserie == 6)
              {
                if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i-1] < devparms.math_decode_threshold_uart_rx)
                {
                  if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] >= devparms.math_decode_threshold_uart_rx)
                  {

                    uart_rx_start = 1;

                    val = 0;

                    uart_rx_x_pos = (uart_sample_per_bit * 1.5) + i;

                    i = uart_rx_x_pos - 1;
                  }
                }
              }
              else  // modelserie = 1, 2 or 4
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
          }
          else  // uart_rx_start != 0
          {
            if(devparms.modelserie == 6)
            {
              if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] >= devparms.math_decode_threshold_uart_rx)
              {
                val += (1 << uart_rx_data_bit);
              }
            }
            else  // modelserie = 1, 2 or 4
            {
              if(devparms.wavebuf[devparms.math_decode_uart_rx - 1][i] >= threshold[devparms.math_decode_uart_rx - 1])
              {
                val += (1 << uart_rx_data_bit);
              }
            }

            if(++uart_rx_data_bit == devparms.math_decode_uart_width)
            {
              if((devparms.math_decode_uart_end) && (devparms.math_decode_format != 4))  // big endian?
              {
                val = reverse_bitorder(val);

                val >>= (8 - uart_rx_data_bit);
              }

              if(!devparms.math_decode_uart_pol)  // positive, line level RS-232 or negative, cpu level TTL/CMOS?
              {
                val = ~val;
              }

              devparms.math_decode_uart_rx_val[devparms.math_decode_uart_rx_nval] = val;

              devparms.math_decode_uart_rx_val_pos[devparms.math_decode_uart_rx_nval++] = i - (uart_rx_data_bit * uart_sample_per_bit);

              uart_rx_data_bit = 0;

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

//   if(devparms.math_decode_mode == DECODE_MODE_SPI)
//   {
//     devparms.math_decode_spi_mosi_nval = 0;
//
//     devparms.math_decode_spi_miso_nval = 0;
//
//     spi_mosi_start = 0;
//
//     spi_data_mosi_bit = 0;
//
//     spi_clk_x_pos = 1;
//
//     spi_miso_start = 0;
//
//     spi_data_miso_bit = 0;
//
//     if(devparms.math_decode_spi_mosi)
//     {
//       if(devparms.chandisplay[devparms.math_decode_spi_mosi - 1])  // don't try to decode if channel isn't enabled...
//       {
//         for(i=1; i<devparms.wavebufsz; i++)
//         {
//           if(devparms.math_decode_spi_mosi_nval >= DECODE_MAX_CHARS)
//           {
//             break;
//           }
//
//           if(!spi_mosi_start)
//           {
//             if(devparms.math_decode_uart_pol)
//             {
//               if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i-1] >= threshold[devparms.math_decode_spi_mosi - 1])
//               {
//                 if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i] < threshold[devparms.math_decode_spi_mosi - 1])
//                 {
//                   spi_mosi_start = 1;
//
//                   val = 0;
//
//                   spi_clk_x_pos = (uart_sample_per_bit * 1.5) + i;
//
//                   i = spi_clk_x_pos - 1;
//                 }
//               }
//             }
//             else
//             {
//               if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i-1] < threshold[devparms.math_decode_spi_mosi - 1])
//               {
//                 if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i] >= threshold[devparms.math_decode_spi_mosi - 1])
//                 {
//                   spi_mosi_start = 1;
//
//                   val = 0;
//
//                   spi_clk_x_pos = (uart_sample_per_bit * 1.5) + i;
//
//                   i = spi_clk_x_pos - 1;
//                 }
//               }
//             }
//           }
//           else
//           {
//             if(devparms.math_decode_uart_pol)
//             {
//               if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i] >= threshold[devparms.math_decode_spi_mosi - 1])
//               {
//                 val += (1 << spi_data_mosi_bit);
//               }
//             }
//             else
//             {
//               if(devparms.wavebuf[devparms.math_decode_spi_mosi - 1][i] < threshold[devparms.math_decode_spi_mosi - 1])
//               {
//                 val += (1 << spi_data_mosi_bit);
//               }
//             }
//
//             if(++spi_data_mosi_bit == devparms.math_decode_uart_width)
//             {
//               if((devparms.math_decode_uart_end) && (devparms.math_decode_format != 4))  // little endian?
//               {
//                 val = reverse_bitorder(val);
//
//                 val >>= (8 - spi_data_mosi_bit);
//               }
//
//               devparms.math_decode_spi_mosi_val[devparms.math_decode_spi_mosi_nval] = val;
//
//               devparms.math_decode_spi_mosi_val_pos[devparms.math_decode_spi_mosi_nval++] = i - (spi_data_mosi_bit * uart_sample_per_bit);
//
//               spi_data_mosi_bit = 0;
//
//               spi_mosi_start = 0;
//
//               spi_clk_x_pos += uart_sample_per_bit;
//
//               if(devparms.math_decode_uart_stop == 1)
//               {
//                 spi_clk_x_pos += uart_sample_per_bit / 2;
//               }
//               else if(devparms.math_decode_uart_stop == 2)
//                 {
//                   spi_clk_x_pos += uart_sample_per_bit;
//                 }
//
//               if(devparms.math_decode_uart_par)
//               {
//                 spi_clk_x_pos += uart_sample_per_bit;
//               }
//
//               i = spi_clk_x_pos - 1;
//             }
//             else
//             {
//               spi_clk_x_pos += uart_sample_per_bit;
//
//               i = spi_clk_x_pos - 1;
//             }
//           }
//         }
//       }
//     }
//   }
}


inline unsigned char UI_Mainwindow::reverse_bitorder(unsigned char byte)
{
  byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
  byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
  byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;

  return byte;
}






















