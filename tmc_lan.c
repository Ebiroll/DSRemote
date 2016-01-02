/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016 Teunis van Beelen
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stropts.h>

#include "tmc_dev.h"
#include "utils.h"
#include "tmc_lan.h"



#define TMC_LAN_TIMEOUT   5000

#define MAX_CMD_LEN     (255)
#define MAX_RESP_LEN    (1024 * 1024 * 2)


int sockfd;

struct sockaddr_in inet_address;

struct timeval timeout;

fd_set tcp_fds;  /* filedescriptor pool */




static int tmclan_send(const char *str)
{
  int len;

  fd_set temp_tcp_fds = tcp_fds;  /* because select overwrites the arguments */
  struct timeval temp_timeout = timeout;

  len = strlen(str);

  if(select(sockfd + 1, 0, &temp_tcp_fds, 0, &temp_timeout) != -1)
  {
    if(FD_ISSET(sockfd, &temp_tcp_fds))  /* check if our file descriptor is set */
    {
      return send(sockfd, str, len, MSG_NOSIGNAL);
    }
  }

  return -1;
}


static int tmclan_recv(char *buf, int sz)
{
  fd_set temp_tcp_fds = tcp_fds;  /* because select overwrites the arguments */
  struct timeval temp_timeout = timeout;

  if(select(sockfd + 1, &temp_tcp_fds, 0, 0, &temp_timeout) != -1)
  {
    if(FD_ISSET(sockfd, &temp_tcp_fds))  /* check if our file descriptor is set */
    {
      return recv(sockfd, buf, sz, MSG_NOSIGNAL);
    }
  }

  return -1;
}


struct tmcdev * tmclan_open(const char *ip_address)
{
  struct tmcdev *tmc_device;

  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
  {
    return NULL;
  }

  FD_ZERO(&tcp_fds);                      /* clear file descriptor pool       */
  FD_SET(sockfd, &tcp_fds);               /* add our filedescriptor to pool   */

  timeout.tv_sec = 0;
  timeout.tv_usec = TMC_LAN_TIMEOUT;
  timeout.tv_usec *= 1000;

  memset(&inet_address, 0, sizeof(struct sockaddr_in));

  inet_address.sin_family = AF_INET;
  if(inet_aton(ip_address, &inet_address.sin_addr) == 0)
  {
    return NULL;
  }
  inet_address.sin_port = htons(5555);

  if(connect(sockfd, (struct sockaddr *) &inet_address, sizeof(struct sockaddr)) < 0)
  {
    return NULL;
  }

  tmc_device = (struct tmcdev *)calloc(1, sizeof(struct tmcdev));
  if(tmc_device == NULL)
  {
    close(sockfd);
    sockfd = -1;
    return NULL;
  }

  tmc_device->hdrbuf = (char *)calloc(1, MAX_RESP_LEN + 1024);
  if(tmc_device->hdrbuf == NULL)
  {
    close(sockfd);
    sockfd = -1;
    return NULL;
  }

  tmc_device->buf = tmc_device->hdrbuf;

  return tmc_device;
}


void tmclan_close(struct tmcdev *tmc_device)
{
  if(sockfd != -1)
  {
    close(sockfd);
    sockfd = -1;
  }

  if(tmc_device != NULL)
  {
    free(tmc_device->hdrbuf);

    free(tmc_device);

    tmc_device = NULL;
  }
}


int tmclan_write(struct tmcdev *tmc_device __attribute__ ((unused)), const char *cmd)
{
  int i, n, len, qry=0;

  char buf[MAX_CMD_LEN + 16],
       str[256];

  if(sockfd == -1)
  {
    return -1;
  }

  len = strlen(cmd);

  if(len > MAX_CMD_LEN)
  {
    printf("tmc_lan error: command too long\n");

    return -1;
  }

  if(len < 2)
  {
    printf("tmc_lan error: command too short\n");

    return -1;
  }

  if(cmd[len - 1] == '?')
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

  n = tmclan_send(buf);

  if(n != (len + 1))
  {
    return -1;
  }

  if(!qry)
  {
    for(i=0; i<20; i++)
    {
      usleep(25000);

      if(tmclan_send("*OPC?\n") != 6)
      {
        printf("tmcdev error: device write error");

        return -1;
      }

      n = tmclan_recv(str, 128);

      if(n < 0)
      {
        printf("tmcdev error: device read error");

        return -1;
      }

      if(n == 2)
      {
        if(str[0] == '1')
        {
          break;
        }
      }
    }
  }

  return len;
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
int tmclan_read(struct tmcdev *tmc_device)
{
  int n, size, size2, len;

  char blockhdr[32];

  if(sockfd == -1)
  {
    return -1;
  }

  tmc_device->hdrbuf[0] = 0;

  tmc_device->sz = 0;

  size = 0;

  while(1)
  {
    n = tmclan_recv(tmc_device->hdrbuf + size, MAX_RESP_LEN - size);

    if(n < 1)
    {
      return -2;
    }

    size += n;

    if(tmc_device->hdrbuf[size - 1] == '\n')
    {
      break;
    }
  }

  if((size < 2) || (size > MAX_RESP_LEN))
  {
    tmc_device->hdrbuf[0] = 0;

    tmc_device->buf[0] = 0;

    return -3;
  }

  tmc_device->hdrbuf[size] = 0;

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













