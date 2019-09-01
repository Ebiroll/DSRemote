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


#include "screen_thread.h"


#define SPECT_LOG_MINIMUM (0.00000001)
#define SPECT_LOG_MINIMUM_LOG (-80)


void screen_thread::set_device(struct tmcdev *tmdev)
{
  params.cmd_cue_idx_in = 0;
  params.cmd_cue_idx_out = 0;
  params.connected = 0;

  device = tmdev;
}


screen_thread::screen_thread()
{
  int i;

  device = NULL;

  h_busy = 0;

  for(i=0; i<MAX_CHNS; i++)
  {
    params.wavebuf[i] = (short *)malloc(WAVFRM_MAX_BUFSZ);
  }

  params.cmd_cue_idx_in = 0;
  params.cmd_cue_idx_out = 0;
  params.connected = 0;
}


screen_thread::~screen_thread()
{
  int i;

  for(i=0; i<MAX_CHNS; i++)
  {
    free(params.wavebuf[i]);
  }
}


void screen_thread::set_params(struct device_settings *dev_parms)
{
  deviceparms = dev_parms;
  params.connected = deviceparms->connected;
  params.modelserie = deviceparms->modelserie;
  params.chandisplay[0] = deviceparms->chandisplay[0];
  params.chandisplay[1] = deviceparms->chandisplay[1];
  params.chandisplay[2] = deviceparms->chandisplay[2];
  params.chandisplay[3] = deviceparms->chandisplay[3];
  params.chanscale[0] = deviceparms->chanscale[0];
  params.chanscale[1] = deviceparms->chanscale[1];
  params.chanscale[2] = deviceparms->chanscale[2];
  params.chanscale[3] = deviceparms->chanscale[3];
  params.countersrc = deviceparms->countersrc;
  params.cmd_cue_idx_in = deviceparms->cmd_cue_idx_in;
  params.math_fft_src = deviceparms->math_fft_src;
  params.math_fft = deviceparms->math_fft;
  params.math_fft_unit = deviceparms->math_fft_unit;
  params.fftbuf_in = deviceparms->fftbuf_in;
  params.fftbuf_out = deviceparms->fftbuf_out;
  params.fftbufsz = deviceparms->fftbufsz;
  params.k_cfg = deviceparms->k_cfg;
  params.kiss_fftbuf = deviceparms->kiss_fftbuf;
  params.current_screen_sf = deviceparms->current_screen_sf;
  params.debug_str[0] = 0;
  params.func_wrec_enable = deviceparms->func_wrec_enable;
  params.func_wrec_operate = deviceparms->func_wrec_operate;
  params.func_wplay_operate = deviceparms->func_wplay_operate;
  params.func_wplay_fcur = deviceparms->func_wplay_fcur;
  params.func_wrec_fmax = deviceparms->func_wrec_fmax;
  params.func_wrep_fmax = deviceparms->func_wplay_fmax;
}


void screen_thread::get_params(struct device_settings *dev_parms)
{
  int i;

  dev_parms->connected = params.connected;
  dev_parms->triggerstatus = params.triggerstatus;
  dev_parms->triggersweep = params.triggersweep;
  dev_parms->samplerate = params.samplerate;
  dev_parms->acquirememdepth = params.memdepth;
  dev_parms->counterfreq = params.counterfreq;
  dev_parms->wavebufsz = params.wavebufsz;
  for(i=0; i<MAX_CHNS; i++)
  {
    if(params.chandisplay[i])
    {
      memcpy(dev_parms->wavebuf[i], params.wavebuf[i], params.wavebufsz * sizeof(short));
      dev_parms->xorigin[i] = params.xorigin[i];
    }
  }
  dev_parms->thread_error_stat = params.error_stat;
  dev_parms->thread_error_line = params.error_line;
  dev_parms->cmd_cue_idx_out = params.cmd_cue_idx_out;
  dev_parms->thread_result = params.result;
  dev_parms->thread_job = params.job;
  if(dev_parms->thread_job == TMC_THRD_JOB_TRIGEDGELEV)
  {
    dev_parms->thread_value = params.triggeredgelevel;
  }
  if(dev_parms->thread_job == TMC_THRD_JOB_TIMDELAY)
  {
    dev_parms->timebasedelayoffset = params.timebasedelayoffset;
    dev_parms->timebasedelayscale = params.timebasedelayscale;
  }
  if(dev_parms->thread_job == TMC_THRD_JOB_FFTHZDIV)
  {
    dev_parms->math_fft_hscale = params.math_fft_hscale;
    dev_parms->math_fft_hcenter = params.math_fft_hcenter;
  }
  if(dev_parms->func_wrec_enable)
  {
    dev_parms->func_wrec_operate = params.func_wrec_operate;
    dev_parms->func_wplay_operate = params.func_wplay_operate;
    deviceparms->func_wplay_fcur = params.func_wplay_fcur;
    deviceparms->func_wrec_fmax = params.func_wrec_fmax;
    deviceparms->func_wplay_fmax = params.func_wrep_fmax;
  }
  if(params.debug_str[0])
  {
    params.debug_str[1023] = 0;

    printf("params.debug_str: ->%s<-\n", params.debug_str);
  }
}


int screen_thread::get_devicestatus()
{
  int line;

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:STAT?") != 11)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "TD"))
  {
    params.triggerstatus = 0;
  }
  else if(!strcmp(device->buf, "WAIT"))
    {
      params.triggerstatus = 1;
    }
    else if(!strcmp(device->buf, "RUN"))
      {
        params.triggerstatus = 2;
      }
      else if(!strcmp(device->buf, "AUTO"))
        {
          params.triggerstatus = 3;
        }
        else if(!strcmp(device->buf, "FIN"))
          {
            params.triggerstatus = 4;
          }
          else if(!strcmp(device->buf, "STOP"))
            {
              params.triggerstatus = 5;
            }
            else
            {
              line = __LINE__;
              goto OUT_ERROR;
            }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":TRIG:SWE?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(!strcmp(device->buf, "AUTO"))
  {
    params.triggersweep = 0;
  }
  else if(!strcmp(device->buf, "NORM"))
    {
      params.triggersweep = 1;
    }
    else if(!strcmp(device->buf, "SING"))
      {
        params.triggersweep = 2;
      }
      else
      {
        line = __LINE__;
        goto OUT_ERROR;
      }

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:SRAT?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  params.samplerate = atof(device->buf);

  usleep(TMC_GDS_DELAY);

  if(tmc_write(":ACQ:MDEP?") != 10)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  if(tmc_read() < 1)
  {
    line = __LINE__;
    goto OUT_ERROR;
  }

  params.memdepth = atoi(device->buf);

  if(params.countersrc)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":MEAS:COUN:VAL?") != 15)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    params.counterfreq = atof(device->buf);
  }

  if(params.func_wrec_enable)
  {
    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREC:OPER?") != 16)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(params.modelserie == 6)
    {
      if(!strcmp(device->buf, "REC"))
      {
        params.func_wrec_operate = 1;
      }
      else if(!strcmp(device->buf, "STOP"))
        {
          params.func_wrec_operate = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }
    }
    else
    {
      if(!strcmp(device->buf, "RUN"))
      {
        params.func_wrec_operate = 1;
      }
      else if(!strcmp(device->buf, "STOP"))
        {
          params.func_wrec_operate = 0;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }
    }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:OPER?") != 16)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(!strcmp(device->buf, "PLAY"))
    {
      params.func_wplay_operate = 1;
    }
    else if(!strcmp(device->buf, "STOP"))
      {
        params.func_wplay_operate = 0;
      }
      else if(!strcmp(device->buf, "PAUS"))
        {
          params.func_wplay_operate = 2;
        }
        else
        {
          line = __LINE__;
          goto OUT_ERROR;
        }

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FCUR?") != 16)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    params.func_wplay_fcur = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREC:FMAX?") != 16)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    params.func_wrec_fmax = atoi(device->buf);

    usleep(TMC_GDS_DELAY);

    if(tmc_write(":FUNC:WREP:FMAX?") != 16)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    if(tmc_read() < 1)
    {
      line = __LINE__;
      goto OUT_ERROR;
    }

    params.func_wrep_fmax = atoi(device->buf);
  }

  params.debug_str[0] = 0;

  return 0;

OUT_ERROR:

  strlcpy(params.debug_str, device->hdrbuf, 1024);

  params.error_line = line;

  return -1;
}


void screen_thread::run()
{
  int i, j, k, n=0, chns=0, line, cmd_sent=0;

  char str[512];

  double y_incr, binsz;

  params.error_stat = 0;

  params.result = TMC_THRD_RESULT_NONE;

  params.job = TMC_THRD_JOB_NONE;

  params.wavebufsz = 0;

  if(device == NULL)
  {
    return;
  }

  if(h_busy)
  {
    return;
  }

  if(!params.connected)
  {
    h_busy = 0;

    return;
  }

  h_busy = 1;

  while(params.cmd_cue_idx_out != params.cmd_cue_idx_in)
  {
    usleep(TMC_GDS_DELAY);

    tmc_write(deviceparms->cmd_cue[params.cmd_cue_idx_out]);

    if(deviceparms->cmd_cue_resp[params.cmd_cue_idx_out] != NULL)
    {
      usleep(TMC_GDS_DELAY);

      if(tmc_read() < 1)
      {
        printf("Can not read from device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      strlcpy(deviceparms->cmd_cue_resp[params.cmd_cue_idx_out], device->buf, 128);
    }

    if((!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":TLHA", 5)) ||
       ((!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":CHAN", 5)) &&
       (!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out] + 6, ":SCAL ", 6))))
    {
      usleep(TMC_GDS_DELAY);

      if(tmc_write(":TRIG:EDG:LEV?") != 14)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_read() < 1)
      {
        printf("Can not read from device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      params.triggeredgelevel = atof(device->buf);

      params.job = TMC_THRD_JOB_TRIGEDGELEV;
    }
    else if(!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":TIM:DEL:ENAB 1", 15))
      {
        usleep(TMC_GDS_DELAY);

        if(tmc_write(":TIM:DEL:OFFS?") != 14)
        {
          printf("Can not write to device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        if(tmc_read() < 1)
        {
          printf("Can not read from device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        params.timebasedelayoffset = atof(device->buf);

        usleep(TMC_GDS_DELAY);

        if(tmc_write(":TIM:DEL:SCAL?") != 14)
        {
          printf("Can not write to device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        if(tmc_read() < 1)
        {
          printf("Can not read from device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        params.timebasedelayscale = atof(device->buf);

        params.job = TMC_THRD_JOB_TIMDELAY;
      }

    if(params.math_fft)
    {
      if((!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":TIM:SCAL ", 10)) ||
         (!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":MATH:OPER FFT", 14)) ||
         (!strncmp(deviceparms->cmd_cue[params.cmd_cue_idx_out], ":CALC:MODE FFT", 14)))
      {
        usleep(TMC_GDS_DELAY * 10);

        if(params.modelserie != 1)
        {
          if(tmc_write(":CALC:FFT:HSP?") != 14)
          {
            line = __LINE__;
            goto OUT_ERROR;
          }
        }
        else
        {
          if(tmc_write(":MATH:FFT:HSC?") != 14)
          {
            printf("Can not write to device.\n");
            line = __LINE__;
            goto OUT_ERROR;
          }
        }

        if(tmc_read() < 1)
        {
          printf("Can not read from device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        params.math_fft_hscale = atof(device->buf);

        usleep(TMC_GDS_DELAY);

        if(params.modelserie != 1)
        {
          if(tmc_write(":CALC:FFT:HCEN?") != 15)
          {
            line = __LINE__;
            goto OUT_ERROR;
          }
        }
        else
        {
          if(tmc_write(":MATH:FFT:HCEN?") != 15)
          {
            printf("Can not write to device.\n");
            line = __LINE__;
            goto OUT_ERROR;
          }
        }

        if(tmc_read() < 1)
        {
          printf("Can not read from device.\n");
          line = __LINE__;
          goto OUT_ERROR;
        }

        params.math_fft_hcenter = atof(device->buf);

        params.job = TMC_THRD_JOB_FFTHZDIV;
      }
    }

    params.cmd_cue_idx_out++;

    params.cmd_cue_idx_out %= TMC_CMD_CUE_SZ;

    cmd_sent = 1;
  }

  if(cmd_sent)
  {
    h_busy = 0;

    params.result = TMC_THRD_RESULT_CMD;

    return;
  }

  params.error_stat = get_devicestatus();

  if(params.error_stat)
  {
    h_busy = 0;

    return;
  }

  if(!params.connected)
  {
    h_busy = 0;

    return;
  }

  for(i=0; i<MAX_CHNS; i++)
  {
    if(!params.chandisplay[i])  // Download data only when channel is switched on
    {
      continue;
    }

    chns++;
  }

  if(!chns)
  {
    h_busy = 0;

    return;
  }

  params.result = TMC_THRD_RESULT_SCRN;

//struct waveform_preamble wfp;

//  if(params.triggerstatus != 1)  // Don't download waveform data when triggerstatus is "wait"
  if(1)
  {
    for(i=0; i<MAX_CHNS; i++)
    {
      if(!params.chandisplay[i])  // Download data only when channel is switched on
      {
        continue;
      }

///////////////////////////////////////////////////////////

//     tmc_write(":WAV:PRE?");
//
//     n = tmc_read();
//
//     if(n < 0)
//     {
//       strlcpy(str, "Can not read from device.", 512);
//       goto OUT_ERROR;
//     }
//
//     printf("waveform preamble: %s\n", device->buf);
//
//     if(parse_preamble(device->buf, device->sz, &wfp, i))
//     {
//       strlcpy(str, "Preamble parsing error.", 512);
//       goto OUT_ERROR;
//     }
//
//     printf("waveform preamble:\n"
//            "format: %i\n"
//            "type: %i\n"
//            "points: %i\n"
//            "count: %i\n"
//            "xincrement: %e\n"
//            "xorigin: %e\n"
//            "xreference: %e\n"
//            "yincrement: %e\n"
//            "yorigin: %e\n"
//            "yreference: %i\n",
//            wfp.format, wfp.type, wfp.points, wfp.count,
//            wfp.xincrement[i], wfp.xorigin[i], wfp.xreference[i],
//            wfp.yincrement[i], wfp.yorigin[i], wfp.yreference[i]);
//
//     printf("chanoffset[] is %e\n", params.chanoffset[i]);

//     rec_len = wfp.xincrement[i] * wfp.points;

///////////////////////////////////////////////////////////

      snprintf(str, 512, ":WAV:SOUR CHAN%i", i + 1);

      if(tmc_write(str) != 15)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_write(":WAV:FORM BYTE") != 14)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_write(":WAV:MODE NORM") != 14)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_write(":WAV:XOR?") != 9)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(tmc_read() < 1)
      {
        printf("Can not read from device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      params.xorigin[i] = atof(device->buf);

      if(tmc_write(":WAV:DATA?") != 10)
      {
        printf("Can not write to device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      n = tmc_read();

      if(n < 0)
      {
        printf("Can not read from device.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(n > WAVFRM_MAX_BUFSZ)
      {
        printf("Datablock too big for buffer.\n");
        line = __LINE__;
        goto OUT_ERROR;
      }

      if(n < 32)
      {
        n = 0;
      }

      for(j=0; j<n; j++)
      {
        params.wavebuf[i][j] = (int)(((unsigned char *)device->buf)[j]) - 127;
      }

      if((n == (params.fftbufsz * 2)) && (params.math_fft == 1) && (i == params.math_fft_src))
      {
        if(params.modelserie == 6)
        {
          y_incr = params.chanscale[i] / 32.0;
        }
        else
        {
          y_incr = params.chanscale[i] / 25.0;
        }

        binsz = (double)params.current_screen_sf / (params.fftbufsz * 2.0);

        for(j=0; j<n; j++)
        {
          params.fftbuf_in[j] = params.wavebuf[i][j] * y_incr;
        }

        kiss_fftr(params.k_cfg, params.fftbuf_in, params.kiss_fftbuf);

        for(k=0; k<params.fftbufsz; k++)
        {
          params.fftbuf_out[k] = (((params.kiss_fftbuf[k].r * params.kiss_fftbuf[k].r) +
                     (params.kiss_fftbuf[k].i * params.kiss_fftbuf[k].i)) / params.fftbufsz);

          params.fftbuf_out[k] /= params.current_screen_sf;

          if(k==0)  // DC!
          {
            params.fftbuf_out[k] /= 2.0;
          }

          params.fftbuf_out[k] *= binsz;

          if(params.math_fft_unit)  // dBm
          {
            if(params.fftbuf_out[k] < SPECT_LOG_MINIMUM)
            {
              params.fftbuf_out[k] = SPECT_LOG_MINIMUM;
            }
            // convert to deciBel's, not to Bel's!
            params.fftbuf_out[k] = log10(params.fftbuf_out[k]) * 10.0;

            if(params.fftbuf_out[k] < SPECT_LOG_MINIMUM_LOG)
            {
              params.fftbuf_out[k] = SPECT_LOG_MINIMUM_LOG;
            }
          }
          else  // Vrms
          {
            params.fftbuf_out[k] = sqrt(params.fftbuf_out[k]);
          }
        }
      }
    }

    params.wavebufsz = n;
  }
  else  // triggerstatus is "wait"
  {
    params.wavebufsz = 0;
  }

  h_busy = 0;

  return;

OUT_ERROR:

  params.result = TMC_THRD_RESULT_NONE;

  snprintf(str, 512, "An error occurred while reading screen data from device.\n"
               "File %s line %i", __FILE__, line);

  h_busy = 0;

  params.error_line = line;

  params.error_stat = -1;

  return;
}
















