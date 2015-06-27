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


#include "connection.h"


#define TMC_LAN_TIMEOUT   5000

#define MAX_CMD_LEN     (255)
#define MAX_RESP_LEN    (1024 * 1024 * 2)


int tmc_connection_type;

struct tmcdev *tmc_device;

QTcpSocket *sck;



struct tmcdev * tmc_open_usb(const char *device)
{
  tmc_connection_type = 0;

  tmc_device = tmcdev_open(device);

  return tmc_device;
}


void tmc_close(void)
{
  if(tmc_connection_type == 0)
  {
    tmcdev_close(tmc_device);

    tmc_device = NULL;
  }
  else
  {
    if(sck != NULL)
    {
      sck->abort();

      delete sck;

      sck = NULL;
    }

    if(tmc_device != NULL)
    {
      free(tmc_device->hdrbuf);

      free(tmc_device);

      tmc_device = NULL;
    }
  }
}


int tmc_write(const char *cmd)
{
  int qry=0;

  char buf[MAX_CMD_LEN + 16],
       str[256];

  if(tmc_connection_type == 0)
  {
    return tmcdev_write(tmc_device, cmd);
  }
  else
  {
    if(sck == NULL)
    {
      return -1;
    }

    if(strlen(cmd) > MAX_CMD_LEN)
    {
      printf("tmc_lan error: command too long\n");

      return -1;
    }

    if(strlen(cmd) < 2)
    {
      printf("tmc_lan error: command too short\n");

      return -1;
    }

    if(cmd[strlen(cmd) - 1] == '?')
    {
      qry = 1;
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
      printf("tmc_lan write: %s", buf);
    }

    sck->write(buf);

    if(sck->waitForBytesWritten(TMC_LAN_TIMEOUT) == false)
    {
      return -1;
    }

    if(!qry)
    {
      for(int i=0; i<20; i++)
      {
        usleep(50000);

        sck->write("*OPC?\n");

        if(sck->waitForBytesWritten(TMC_LAN_TIMEOUT) == false)
        {
          return -1;
        }

        if(sck->waitForReadyRead(TMC_LAN_TIMEOUT) == false)
        {
          return -1;
        }

        if(sck->read(str, 128) == 2)
        {
          if(str[0] == '1')
          {
            break;
          }
        }
      }
    }

    return strlen(cmd);
  }

  return -1;
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
int tmc_read(void)
{
  int size, size2, len;

  char blockhdr[32];

  if(tmc_connection_type == 0)
  {
    return tmcdev_read(tmc_device);
  }
  else
  {
    if(sck == NULL)
    {
      return -1;
    }

    tmc_device->hdrbuf[0] = 0;

    tmc_device->sz = 0;

    size = 0;

    while(1)
    {
      if(sck->waitForReadyRead(TMC_LAN_TIMEOUT) == false)
      {
        return -1;
      }

      size += sck->read(tmc_device->hdrbuf + size, MAX_RESP_LEN - size);

      if(tmc_device->hdrbuf[size - 1] == '\n')
      {
        break;
      }
    }

    if((size < 2) || (size > MAX_RESP_LEN))
    {
      tmc_device->hdrbuf[0] = 0;

      tmc_device->buf[0] = 0;

      return -1;
    }

    if(size >= 0)
    {
      tmc_device->hdrbuf[size] = 0;
    }

    if(size == 0)
    {
      return 0;
    }

    if(tmc_device->hdrbuf[0] != '#')
    {
      if(tmc_device->hdrbuf[size - 1] == '\n')
      {
        tmc_device->hdrbuf[--size] = 0;
      }

      tmc_device->buf = tmc_device->hdrbuf;

      tmc_device->sz = size;

      return tmc_device->sz;
    }

    strncpy(blockhdr, tmc_device->hdrbuf, 16);

    len = blockhdr[1] - '0';

    if((len < 1) || (len > 9))
    {
      blockhdr[31] = 0;

      return -1;
    }

    blockhdr[len + 2] = 0;

    size2 = atoi(blockhdr + 2);

    size--;  // remove the last character

    if(size < size2)
    {
      blockhdr[31] = 0;

      return -1;
    }

    tmc_device->buf = tmc_device->hdrbuf + len + 2;

    tmc_device->sz = size2;

    return tmc_device->sz;
  }

  return -1;
}


struct tmcdev * tmc_open_lan(const char *address)
{
  tmc_connection_type = 1;

  sck = new QTcpSocket;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  sck->connectToHost(address, 5555);

  qApp->processEvents();

  if(sck->waitForConnected(TMC_LAN_TIMEOUT) == false)
  {
    sck->abort();
    delete sck;
    sck = NULL;

    QApplication::restoreOverrideCursor();

    return NULL;
  }

  QApplication::restoreOverrideCursor();

  qApp->processEvents();

  tmc_device = (struct tmcdev *)calloc(1, sizeof(struct tmcdev));
  if(tmc_device == NULL)
  {
    sck->abort();
    delete sck;
    sck = NULL;
    return NULL;
  }

  tmc_device->hdrbuf = (char *)calloc(1, MAX_RESP_LEN + 1024);
  if(tmc_device->hdrbuf == NULL)
  {
    free(tmc_device);
    sck->abort();
    delete sck;
    sck = NULL;
    return NULL;
  }

  tmc_device->buf = tmc_device->hdrbuf;

  return tmc_device;
}







