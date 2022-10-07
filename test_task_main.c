/****************************************************************************
 * apps/examples/test_task/test_task_main.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "libs/nml/nml.h"

#include "stm32_denis.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_LIBC_FLOATINGPOINT
#  error CONFIG_LIBC_FLOATINGPOINT must be enabled
#endif

#define TASK_1_STACKSIZE 2048
#define TASK_1_PRIORITY 100
#define TASK_2_STACKSIZE 2048
#define TASK_2_PRIORITY 100

#define DENIS_DEVNAME    "/dev/denis0"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int task_1(int argc, char *argv[])
{
  int ret = OK;
  const char * msg = "task 1 message";
  const size_t msg_len = strlen(msg);

  printf("%s: Running\n", __func__);
  printf("%s: Opening '%s' for write\n", __func__, DENIS_DEVNAME);

  int fd = open(DENIS_DEVNAME, O_RDWR);
  if (fd < 0)
    {
      printf("%s: Failed to open %s: %d\n", __func__, DENIS_DEVNAME, errno);
      ret = EXIT_FAILURE;
      goto exit_without_close;
    }

  for (int q = 0; q < 2; q++)
  {
    int nbytes = write(fd, msg, msg_len);
    if (nbytes != msg_len)
    {
      printf("%s: ERROR: write(%ld) returned %ld\n",
              __func__, (long)msg_len, (long)nbytes);
      ret = EXIT_FAILURE;
      goto exit_with_close;
    }
    
    sleep(1);
  }

  exit_with_close:

  printf("%s: Closinging '%s'\n", __func__, DENIS_DEVNAME);

  close(fd);

  exit_without_close:

  printf("%s: Exit\n", __func__);

  exit(ret);
}

static int task_2(int argc, char *argv[])
{
  int ret = OK;
  // const char * msg = "task 2 message";
  // const size_t msg_len = strlen(msg);

  printf("%s: Running\n", __func__);
  printf("%s: Opening '%s' for write\n", __func__, DENIS_DEVNAME);

  int fd = open(DENIS_DEVNAME, O_RDWR);
  if (fd < 0)
    {
      printf("Ftask_2: ailed to open %s: %d\n", DENIS_DEVNAME, errno);
      ret = EXIT_FAILURE;
      goto exit_without_close;
    }

  while (1)
  {
    nml_mat* m;

    unsigned int nrows = nml_rand_interval(1, 4);
    unsigned int ncols = nml_rand_interval(1, 4);

    printf("%s: Creating a random %dx%d matrix\n", __func__, nrows, ncols);

    m = nml_mat_rnd(nrows, ncols, -10.0, 10.0);

    nml_mat_printf(m, "%.2lf\t\t");

    for (int i = 0; i < m->num_rows; i++)
    {
      void *pdata = m->data[i];
      size_t data_len = m->num_cols * sizeof(**m->data);

      int nbytes = write(fd, pdata, data_len);
      if (nbytes != data_len)
      {
        nml_mat_free(m);

        printf("%s: ERROR: write(%ld) returned %ld\n",
                __func__, (long)data_len, (long)nbytes);

        ret = EXIT_FAILURE;
        goto exit_with_close;
      }
    }

    nml_mat_free(m);
    
    sleep(1);
  }

  exit_with_close:

  printf("%s: Closinging '%s'\n", __func__, DENIS_DEVNAME);

  close(fd);

  exit_without_close:

  printf("%s: Exit\n", __func__);

  exit(ret);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int result;

  printf("\n");
  printf("Test Task started!\n");

  result = board_denis_initialize(1);
  if (result != 0)
    {
      printf("Failed to Denis initialization\n");
      goto errout;
    }

  srand(time(NULL)); // Should be called once per program
  
  result = task_create("task_1", TASK_1_PRIORITY,
                    TASK_1_STACKSIZE, task_1, NULL);
  if (result < 0)
    {
      printf("Failed to start task_1: %d\n", errno);
      goto errout;
    }

  result = task_create("task_2", TASK_2_PRIORITY,
                    TASK_2_STACKSIZE, task_2, NULL);
  if (result < 0)
    {
      printf("Failed to start task_2: %d\n", errno);
      goto errout;
    }

  printf("main exit\n");

  return 0;

  errout:

  exit(EXIT_FAILURE);
}
