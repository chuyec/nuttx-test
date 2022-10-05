/**
 * @file denis.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __INCLUDE_DENIS_H
#define __INCLUDE_DENIS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/spi/spi.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DENIS_SPI_FREQUENCY    (5000000)        /* 5 MHz */
#define DENIS_SPI_MODE         (SPIDEV_MODE3)   /* Device uses SPI Mode 3: CPOL=1, CPHA=1 */

struct denis_config_s
{
    /* Since multiple sensors can be connected to the same SPI bus we need
     * to use multiple spi device ids which are employed by NuttX to select/
     * deselect the desired LIS3DSH chip via their chip select inputs.
     */

    int spi_devid;
};

int denis_register(FAR const char *devpath, FAR struct spi_dev_s *spi,
                    FAR struct denis_config_s *config);

#endif /* __INCLUDE_NUTTX_SENSORS_LIS3DSH_H */
