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

#include "stm32_denis.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DENIS_DEVNAME    "/dev/denis0"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int result;
  int fd;

  printf("Test Task started!\n");

  result = board_denis_initialize(1);
  if (result != 0)
    {
      printf("Failed to Denis initialization\n");
      goto errout;
    }

  printf("Opening '%s' for write\n", DENIS_DEVNAME);

  fd = open(DENIS_DEVNAME, O_RDWR);
  if (fd < 0)
    {
      printf("Failed to open %s: %d\n", DENIS_DEVNAME, errno);
      goto errout;
    }

  printf("Done!\n");

  return 0;

  errout:

  exit(EXIT_FAILURE);
}
