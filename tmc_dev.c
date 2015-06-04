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


#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/usb/tmc.h>
#include <stdio.h>
#include <errno.h>

#include "tmc_dev.h"



#define MAX_CMD_LEN     (255)
#define MAX_RESP_LEN    (1024 * 1024 * 2)



struct tmcdev * tmcdev_open(const char *device)
{
  struct tmcdev *dev;


  dev = (struct tmcdev *)calloc(1, sizeof(struct tmcdev));
  if(dev == NULL)
  {
    return NULL;
  }

  dev->hdrbuf = calloc(1, MAX_RESP_LEN + 1024);
  if(dev->hdrbuf == NULL)
  {
    free(dev);

    return NULL;
  }

  dev->buf = dev->hdrbuf;

  dev->fd = open(device, O_RDWR);

  if(dev->fd == -1)
  {
    free(dev->hdrbuf);

    free(dev);

    return NULL;
  }

  return dev;
}


void tmcdev_close(struct tmcdev *dev)
{
  if(dev == NULL)
  {
    return;
  }

  close(dev->fd);

  free(dev->hdrbuf);

  free(dev);
}


int tmcdev_write(struct tmcdev *dev, const char *cmd)
{
  int size;

  char buf[MAX_CMD_LEN + 16];

  if(dev == NULL)
  {
    return -1;
  }

  if(strlen(cmd) > MAX_CMD_LEN)
  {
    printf("tmcdev error: command too long\n");

    return -1;
  }

  if(strlen(cmd) < 2)
  {
    printf("tmcdev error: command too short\n");

    return -1;
  }

  strncpy(buf, cmd, MAX_CMD_LEN);

  buf[MAX_CMD_LEN] = 0;

  strcat(buf, "\n");

  if(!(!strncmp(buf, ":TRIG:STAT?", 11) ||
       !strncmp(buf, ":TRIG:SWE?", 10) ||
       !strncmp(buf, ":WAV:DATA?", 10) ||
       !strncmp(buf, ":WAV:MODE NORM", 14) ||
       !strncmp(buf, ":WAV:FORM BYTE", 14) ||
       !strncmp(buf, ":WAV:SOUR CHAN", 14) ||
       !strncmp(buf, ":TRIG:SWE?", 10) ||
       !strncmp(buf, ":ACQ:SRAT?", 10) ||
       !strncmp(buf, ":ACQ:MDEP?", 10) ||
       !strncmp(buf, ":MEAS:COUN:VAL?", 15)))
  {
    printf("tmc_dev write: %s", buf);
  }

  size = write(dev->fd, buf, strlen(buf));

  if(size < 0)
  {
    printf("tmcdev error: device write error");
  }

  return size - 1;
}


/*
 * TMC Blockheader ::= #NXXXXXX: is used to describe
 * the length of the data stream, wherein, # is the start denoter of
 * the data stream; N is less than or equal to 9; the N figures
 * followed N represents the length of the data stream in bytes.
 * For example, #9001152054. Wherein, N is 9 and 001152054
 * represents that the data stream contains 1152054 bytes
 * effective data.
 */
int tmcdev_read(struct tmcdev *dev)
{
  int size, size2, len;

  char blockhdr[32];

  if(dev == NULL)
  {
    return -1;
  }

  dev->hdrbuf[0] = 0;

  dev->sz = 0;

  size = read(dev->fd, dev->hdrbuf, MAX_RESP_LEN);

  if(size >= 0)
  {
    dev->hdrbuf[size] = 0;
  }

  if(size == 0)
  {
    return 0;
  }

  if((size < 2) || (size > MAX_RESP_LEN))
  {
    return -1;
  }

  if(dev->hdrbuf[0] != '#')
  {
    if(dev->hdrbuf[size - 1] == '\n')
    {
      dev->hdrbuf[--size] = 0;
    }

    dev->buf = dev->hdrbuf;

    dev->sz = size;

    return dev->sz;
  }

  strncpy(blockhdr, dev->hdrbuf, 16);

  len = blockhdr[1] - '0';

  if((len < 1) || (len > 9))
  {
    return -1;
  }

  blockhdr[len + 2] = 0;

  size2 = atoi(blockhdr + 2);

  size--;  // remove the last character

  if(size != (size2 + len + 2))
  {
    return -1;
  }

  dev->buf = dev->hdrbuf + len + 2;

  dev->sz = size2;

  return dev->sz;
}
























