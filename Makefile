############################################################################
# apps/examples/hello/Make.defs
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

# Test Task built-in application info

PROGNAME  = $(CONFIG_EXAMPLES_TEST_TASK_PROGNAME)
PRIORITY  = $(CONFIG_EXAMPLES_TEST_TASK_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_TEST_TASK_STACKSIZE)
MODULE    = $(CONFIG_EXAMPLES_TEST_TASK)

# Test Task Example

MAINSRC = test_task_main.c
CSRCS += stm32_denis.c
CSRCS += denis.c
CSRCS += libs/nml/nml.c
CSRCS += libs/nml/nml_util.c

include $(APPDIR)/Application.mk
