/**
 * @file stm32_denis.c
 * @author Denis Shreiber (chuyecd@gmail.com)
 * 
 * @brief Реализует связку драйвера с конретным чипсетом
 * 
 * @version 0.1
 * @date 2022-10-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <errno.h>
#include <debug.h>
#include <stdio.h>

#include <nuttx/spi/spi.h>
#include <arch/board/board.h>

// #include "stm32.h"

#include "denis.h"
#include "stm32_denis.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief Инициализируем интерфейс доступа к драйверу и регистрируем его
 * 
 * @param busno Номер шины, куда подключено устройство
 * @return 0 - в случае успеха, отрицательное значение в ином случае
 */
int board_denis_initialize(int busno)
{
    static struct denis_config_s denis0_config;
    struct spi_dev_s *spi;
    int ret;

    sninfo("Initializing Denis\n");

    // Инициализируем SPIn, где n (busno) - номер SPI на контроллере

    spi = stm32_spibus_initialize(busno);
    if (!spi)
    {
        spiinfo("Failed to initialize SPI port\n");
        ret = -ENODEV;
    }
    else
    {
        // Регистрируем устройство в системе по пути "dev/denis0"
        FAR const char *devpath = "/dev/denis0";
        ret = denis_register(devpath, spi, &denis0_config);
    }

    return ret;
}



