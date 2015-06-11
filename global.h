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


#ifndef DEMO_GLOBAL_H
#define DEMO_GLOBAL_H


#define PROGRAM_NAME     "DSRemote"
#define PROGRAM_VERSION       "0.11"

#define MAX_PATHLEN            4096

#define MAX_CHNS                  4

#define SCREEN_TIMER_IVAL        50

#define ADJDIAL_TIMER_IVAL_1   3000
#define ADJDIAL_TIMER_IVAL_2   2000

#define SCRN_SHOT_BMP_SZ    1152054

#define WAVFRM_MAX_BUFSZ  (1024 * 1024 * 2)

#define ADJ_DIAL_FUNC_NONE        0
#define ADJ_DIAL_FUNC_HOLDOFF     1
#define ADJ_DIAL_FUNC_ACQ_AVG     2

#define NAV_DIAL_FUNC_NONE        0
#define NAV_DIAL_FUNC_HOLDOFF     1

#define LABEL_TIMER_IVAL       1000

#define LABEL_ACTIVE_NONE         0
#define LABEL_ACTIVE_CHAN1        1
#define LABEL_ACTIVE_CHAN2        2
#define LABEL_ACTIVE_CHAN3        3
#define LABEL_ACTIVE_CHAN4        4
#define LABEL_ACTIVE_TRIG         5



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
  char modelname[128];
  char serialnr[128];
  char softwvers[128];
  int modelserie;               // 1=DS1000, 2=DS2000, etc.
  int hordivisions;             // number of horizontal divisions

  double samplerate;            // Samplefrequency
  int memdepth;                 // Number of waveform points that the oscilloscope can store in a single trigger sample
  int channel_cnt;              // Device has 2 or 4 channels
  int bandwidth;                // Bandwidth in MHz

  int chanbwlimit[MAX_CHNS];    // 20, 250 or 0MHz (off)
  int chancoupling[MAX_CHNS];   // 0=GND, 1=DC, 2=AC
  int chandisplay[MAX_CHNS];    // 0=off, 1=on
  int chanimpedance[MAX_CHNS];  // 0=1MOhm, 1=50Ohm
  int chaninvert[MAX_CHNS];     // 0=normal, 1=inverted
  double chanoffset[MAX_CHNS];  // expressed in volts
  double chanprobe[MAX_CHNS];   // Probe attenuation ratio e.g. 10:1
  double chanscale[MAX_CHNS];
  int chanvernier[MAX_CHNS];    // Vernier 1=on, 0=off (fine adjustment of vertical scale)
  int activechannel;            // Last pressed channel button (used to know at which channel to apply scale change)

  double timebaseoffset;        // Main timebase offset in Sec
                                // MemDepth/SamplingRate to 1s (when TimeScale < 20ms)
                                // MemDepth/SamplingRate to 10�TimeScale (when TimeScale >=20ms)
  double timebasescale;         // Main timebase scale in Sec/div, 500pSec to 50Sec

  int timebasedelayenable;      // 1=on, 0=off
  double timebasedelayoffset;   //
  double timebasedelayscale;    //  (1 � 50 / sample rate) � 1 / 40  in seconds
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
  double triggerholdoff;        // min. is 100nSec
  int triggermode;              // 0=edge, 1=pulse, 2=slope, 3=video, 4=pattern, 5=rs232,
                                // 6=i2c, 7=spi, 8=can, 9=usb
  int triggerstatus;            // 0=td, 1=wait, 2=run, 3=auto, 4=fin, 5=stop
  int triggersweep;             // 0=auto, 1=normal, 2=single

  int displaygrid;              // 0=none, 1=half, 2=full
  int displaytype;              // 0=vectors, 1=dots

  int acquiretype;              // 0=normal, 1=average, 2=peak, 3=highres
  int acquireaverages;          // 2, 4, 8, 16, 32, 64, etc. to 8192

  int countersrc;               // 0=off, 1=ch1, 2=ch2, 3=ch3, 4=ch4
  double counterfreq;           // Value of frequency counter

  char *screenshot_buf;
  short *wavebuf[MAX_CHNS];
  int wavebufsz;

  int screenupdates_on;

  struct waveform_preamble preamble;
};



#endif














