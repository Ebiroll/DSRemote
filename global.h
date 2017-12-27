/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016, 2017 Teunis van Beelen
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


#ifndef DSR_GLOBAL_H
#define DSR_GLOBAL_H

#include <pthread.h>

#include "third_party/kiss_fft/kiss_fftr.h"


#define PROGRAM_NAME          "DSRemote"
#define PROGRAM_VERSION       "0.35_1712271629"

#define MAX_PATHLEN            4096

#define MAX_CHNS                  4

#define ADJDIAL_TIMER_IVAL_1   4000
#define ADJDIAL_TIMER_IVAL_2   2000

#define SCRN_SHOT_BMP_SZ    1152054

#define WAVFRM_MAX_BUFSZ  (1024 * 1024 * 2)

#define FFT_MAX_BUFSZ          4096

#define ADJ_DIAL_FUNC_NONE        0
#define ADJ_DIAL_FUNC_HOLDOFF     1
#define ADJ_DIAL_FUNC_ACQ_AVG     2

#define NAV_DIAL_FUNC_NONE        0
#define NAV_DIAL_FUNC_HOLDOFF     1

#define LABEL_TIMER_IVAL       1500

#define LABEL_ACTIVE_NONE         0
#define LABEL_ACTIVE_CHAN1        1
#define LABEL_ACTIVE_CHAN2        2
#define LABEL_ACTIVE_CHAN3        3
#define LABEL_ACTIVE_CHAN4        4
#define LABEL_ACTIVE_TRIG         5
#define LABEL_ACTIVE_FFT          6

#define TMC_GDS_DELAY         10000

#define TMC_CMD_CUE_SZ         1024

#define TMC_THRD_RESULT_NONE      0
#define TMC_THRD_RESULT_SCRN      1
#define TMC_THRD_RESULT_CMD       2

#define TMC_THRD_JOB_NONE         0
#define TMC_THRD_JOB_TRIGEDGELEV  1
#define TMC_THRD_JOB_TIMDELAY     2
#define TMC_THRD_JOB_FFTHZDIV     3

#define TMC_DIAL_TIMER_DELAY    300

#define DECODE_MODE_TAB_PAR       0
#define DECODE_MODE_TAB_UART      1
#define DECODE_MODE_TAB_SPI       2
#define DECODE_MODE_TAB_I2C       3

#define DECODE_MODE_PAR           0
#define DECODE_MODE_UART          1
#define DECODE_MODE_SPI           2
#define DECODE_MODE_I2C           3

#define DECODE_MAX_CHARS        512




struct waveform_preamble
{
  int format;
  int type;
  int points;
  int count;
  double xincrement[MAX_CHNS];
  double xorigin[MAX_CHNS];
  double xreference[MAX_CHNS];
  double yincrement[MAX_CHNS];
  double yorigin[MAX_CHNS];
  int yreference[MAX_CHNS];
};


struct device_settings
{
  int connected;
  int connectiontype;           // 0=USB, 1=LAN
  char modelname[128];
  char serialnr[128];
  char softwvers[128];
  int modelserie;               // 1=DS1000, 2=DS2000, etc.
  int hordivisions;             // number of horizontal divisions, 12 or 14
  int vertdivisions;            // number of vertical divisions, 8 or 10
  int use_extra_vertdivisions;  // If 1: use 10 vertical divisions instead of 8, DS1000Z only

  char hostname[128];

  int screentimerival;

  int channel_cnt;              // Device has 2 or 4 channels
  int bandwidth;                // Bandwidth in MHz

  int chanbwlimit[MAX_CHNS];    // 20, 250 or 0MHz (off)
  int chancoupling[MAX_CHNS];   // 0=GND, 1=DC, 2=AC
  int chandisplay[MAX_CHNS];    // 0=off, 1=on
  int chanimpedance[MAX_CHNS];  // 0=1MOhm, 1=50Ohm
  int chaninvert[MAX_CHNS];     // 0=normal, 1=inverted
  int chanunit[MAX_CHNS];       // 0=V, 1=W, 2=A, 3=U
  char chanunitstr[4][2];
  double chanoffset[MAX_CHNS];  // expressed in volts
  double chanprobe[MAX_CHNS];   // Probe attenuation ratio e.g. 10:1
  double chanscale[MAX_CHNS];
  int chanvernier[MAX_CHNS];    // Vernier 1=on, 0=off (fine adjustment of vertical scale)
  int activechannel;            // Last pressed channel button (used to know at which channel to apply scale change)

  double timebaseoffset;        // Main timebase offset in Sec
                                // MemDepth/SamplingRate to 1s (when TimeScale < 20ms)
                                // MemDepth/SamplingRate to 10xTimeScale (when TimeScale >=20ms)
  double timebasescale;         // Main timebase scale in Sec/div, 500pSec to 50Sec

  int timebasedelayenable;      // 1=on, 0=off
  double timebasedelayoffset;   //
  double timebasedelayscale;    //  (1 x 50 / sample rate) x 1 / 40  in seconds
  int timebasehrefmode;         // 0=center, 1=tpos, 2=user
  int timebasehrefpos;
  int timebasemode;             // 0=MAIN, 1=XY, 2=ROLL
  int timebasevernier;          // Vernier 1=on, 0=off (fine adjustment of timebase)
  int timebasexy1display;       // XY mode for channel 1 & 2,  1=on, 0=off
  int timebasexy2display;       // XY mode for channel 3 & 4,  1=on, 0=off

  int triggercoupling;          // 0=AC, 1=DC, 2=LFReject, 3=HFReject
  double triggeredgelevel[7];   // Trigger level
  int triggeredgeslope;         // 0=POS, 1=NEG, 2= RFAL
  int triggeredgesource;        // 0=chan1, 1=chan2, 2=chan3, 3=chan4, 4=ext, 5=ext5, 6=acl
  double triggerholdoff;        // min. is 16nSec or 100nSec depends on series
  int triggermode;              // 0=edge, 1=pulse, 2=slope, 3=video, 4=pattern, 5=rs232,
                                // 6=i2c, 7=spi, 8=can, 9=usb
  int triggerstatus;            // 0=td, 1=wait, 2=run, 3=auto, 4=fin, 5=stop
  int triggersweep;             // 0=auto, 1=normal, 2=single

  int displaygrid;              // 0=none, 1=half, 2=full
  int displaytype;              // 0=vectors, 1=dots
  int displaygrading;           // 0=minimum, 1=0.1, 2=0.2, 5=0.5, 1=10, 2=20, 5=50, 10000=infinite

  double samplerate;            // Samplefrequency
  int acquiretype;              // 0=normal, 1=average, 2=peak, 3=highres
  int acquireaverages;          // 2, 4, 8, 16, 32, 64, etc. to 8192
  int acquirememdepth;          // Number of waveform points that the oscilloscope can
                                // store in a single trigger sample. 0=AUTO

  int countersrc;               // 0=off, 1=ch1, 2=ch2, 3=ch3, 4=ch4
  double counterfreq;           // Value of frequency counter

  int math_decode_display;      // 0=off, 1=on
  int math_decode_mode;         // 0=par, 1=uart, 2=spi, 3=iic
  int math_decode_format;       // 0=hex, 1=ascii, 2=dec, 3=bin, 4=line
  int math_decode_pos;          // vertical position of the decode trace,
                                // the screen is divided into 400 parts vertically which
                                // are marked as 0 to 400 from top to bottom respectively
                                // the range of <pos> is from 50 to 350
  double math_decode_threshold[MAX_CHNS];  // 0: threshold of decode channel 1 (SPI:MISO for modelserie 6)
                                           // 1: threshold of decode channel 2 (SPI:MOSI for modelserie 6)
                                           // 2: threshold of decode channel 3 (SPI:SCLK for modelserie 6)
                                           // 3: threshold of decode channel 4 (SPI:SS for modelserie 6)
                                           // (-4 x VerticalScale - VerticalOffset) to
                                           // (4 x VerticalScale - VerticalOffset)
  double math_decode_threshold_uart_tx;    // threshold of RS232:TX for modelserie 6
  double math_decode_threshold_uart_rx;    // threshold of RS232:RX for modelserie 6

  int math_decode_threshold_auto;  // 0=off, 1=on

  int math_decode_spi_clk;      // channel - 1
  int math_decode_spi_miso;     // channel (0=off)
  int math_decode_spi_mosi;     // channel (0=off)
  int math_decode_spi_cs;       // channel (0=off)
  int math_decode_spi_select;   // select cs level, 0=low, 1=high
  int math_decode_spi_mode;     // frame mode, 0=timeout, 1=cs (chip select)
  double math_decode_spi_timeout;  // timeout
  int math_decode_spi_pol;      // polarity of serial data line, 0=negative, 1=positive
  int math_decode_spi_edge;     // edge, 0=falling edge of clock, 1=rising edge of clock
  int math_decode_spi_end;      // endian, 0=lsb, 1=msb
  int math_decode_spi_width;    // databits, 8-32
  int math_decode_spi_mosi_nval;  // number of decoded characters
  unsigned int math_decode_spi_mosi_val[DECODE_MAX_CHARS];  // array with decoded characters
  int math_decode_spi_mosi_val_pos[DECODE_MAX_CHARS];  // array with position of the decoded characters
  int math_decode_spi_mosi_val_pos_end[DECODE_MAX_CHARS];  // array with endposition of the decoded characters
  int math_decode_spi_miso_nval;    // number of decoded characters
  unsigned int math_decode_spi_miso_val[DECODE_MAX_CHARS];  // array with decoded characters
  int math_decode_spi_miso_val_pos[DECODE_MAX_CHARS];  // array with position of the decoded characters
  int math_decode_spi_miso_val_pos_end[DECODE_MAX_CHARS];  // array with endposition of the decoded characters

  int math_decode_uart_tx;      // channel (0=off)
  int math_decode_uart_rx;      // channel (0=off)
  int math_decode_uart_pol;     // polarity, 0=negative, 1=positive
  int math_decode_uart_end;     // endian, 0=lsb, 1=msb
  int math_decode_uart_baud;    // baudrate
  int math_decode_uart_width;   // databits, 5-8
  int math_decode_uart_stop;    // stopbits, 0=1, 1=1.5, 2=2
  int math_decode_uart_par;     // parity, 0=none, 1=odd, 2=even
  int math_decode_uart_tx_nval;    // number of decoded characters
  unsigned char math_decode_uart_tx_val[DECODE_MAX_CHARS];  // array with decoded characters
  int math_decode_uart_tx_val_pos[DECODE_MAX_CHARS];  // array with position of the decoded characters
  int math_decode_uart_tx_err[DECODE_MAX_CHARS];  // array with protocol errors, non zero means an error
  int math_decode_uart_rx_nval;    // number of decoded characters
  unsigned char math_decode_uart_rx_val[DECODE_MAX_CHARS];  // array with decoded characters
  int math_decode_uart_rx_val_pos[DECODE_MAX_CHARS];  // array with position of the decoded characters
  int math_decode_uart_rx_err[DECODE_MAX_CHARS];  // array with protocol errors, non zero means an error

  char *screenshot_buf;
  short *wavebuf[MAX_CHNS];
  int wavebufsz;
  double yinc[MAX_CHNS];
  int yor[MAX_CHNS];

  int screenshot_inv;           // 0=normal, 1=inverted colors

  int screenupdates_on;

  pthread_mutex_t mutexx;

  int thread_error_stat;
  int thread_error_line;
  int thread_result;
  int thread_job;
  double thread_value;

  struct waveform_preamble preamble;

  char cmd_cue[TMC_CMD_CUE_SZ][128];
  char *cmd_cue_resp[TMC_CMD_CUE_SZ];
  int cmd_cue_idx_in;
  int cmd_cue_idx_out;

  int math_fft_src;             // 0=ch1, 1=ch2, 2=ch3, 3=ch4
  int math_fft;                 // 0=off, 1=on
  int math_fft_split;           // 0=off, 1=on
  int math_fft_unit;            // 0=VRMS, 1=DB
  double *fftbuf_in;
  double *fftbuf_out;
  int fftbufsz;
  kiss_fftr_cfg k_cfg;
  kiss_fft_cpx *kiss_fftbuf;
  double math_fft_hscale;
  double math_fft_hcenter;
  double fft_vscale;
  double fft_voffset;

  int current_screen_sf;

  int show_fps;

  // below here is use for the wave inspector
  int wave_mem_view_sample_start;
  int wave_mem_view_enabled;

  double viewer_center_position;

  int func_wrec_enable;
  int func_wrec_fend;
  int func_wrec_fmax;
  double func_wrec_fintval;
  int func_wrec_prompt;
  int func_wrec_operate;
  int func_wplay_fstart;
  int func_wplay_fend;
  int func_wplay_fmax;
  double func_wplay_fintval;
  int func_wplay_mode;
  int func_wplay_dir;
  int func_wplay_operate;
  int func_wplay_fcur;
  int func_has_record;
};



#endif














