/**
 * @file denis.c
 * @author Denis Shreiber (chuyecd@gmail.com)
 * 
 * @brief Драйвер для работы с устройством "denis"
 *        Реализован на основе драйвера lis3dsh
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

#include <debug.h>
#include <stdio.h>

#include <nuttx/kmalloc.h>
#include <nuttx/fs/fs.h>

#include "denis.h"

/****************************************************************************
 * Private
 ****************************************************************************/

struct denis_dev_s
{
  FAR struct denis_dev_s *flink;       /* Supports a singly linked list of
                                        * drivers */
  FAR struct spi_dev_s *spi;           /* Pointer to the SPI instance */
  FAR struct denis_config_s *config;   /* Pointer to the configuration
                                        * of the DENIS device */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int denis_open(FAR struct file *filep);
static int denis_close(FAR struct file *filep);
static ssize_t denis_read(FAR struct file *, FAR char *buffer,
                            size_t buflen);
static ssize_t denis_write(FAR struct file *filep, FAR const char *buffer,
                             size_t buflen);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/**
 * @brief Набор хэндлеров операций с файлами
 * для низкоуровневой реализации драйвера и взаимодействия с SPI устройством
 * 
 */
static const struct file_operations g_denis_fops =
{
  denis_open,      /* open */
  denis_close,     /* close */
  denis_read,      /* read */
  denis_write,     /* write */
  NULL,            /* seek */
  NULL,            /* ioctl */
  NULL             /* poll */
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
  , NULL           /* unlink */
#endif
};

/* Single linked list to store instances of drivers */

static struct denis_dev_s *g_denis_list = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * @brief Вывести в serial port бинарные данные в hex формате
 * 
 * @param data Указатель на данные
 * @param size Размер данных
 */
void dump_hex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

/****************************************************************************
 * Name: denis_write_dev
 ****************************************************************************/

/**
 * @brief Прямая запись данных в устройство Denis
 * 
 * @param dev Указатель на структуру объекта драйвера
 * @param data Указатель на данные для записи
 * @param data_len Длинна данных
 */
static void denis_write_dev(FAR struct denis_dev_s *dev, const void * data, size_t data_len)
{
  /* Lock the SPI bus so that only one device can access it at the same
   * time
   */

  SPI_LOCK(dev->spi, true);

  /* Set CS to low which selects the DENIS */

#warning CS pin functionality is not implemented for SPI

  /// @warning CS pin functionality is not implemented
  SPI_SELECT(dev->spi, dev->config->spi_devid, true);
//   stm32_gpiowrite(GPIO_SPI1_NSS, false);

  SPI_SNDBLOCK(dev->spi, data, data_len);

  /* Set CS to high which deselects the DENIS */

  /// @warning CS pin functionality is not implemented
  SPI_SELECT(dev->spi, dev->config->spi_devid, false);
//   stm32_gpiowrite(GPIO_SPI1_NSS, true);

  /* Unlock the SPI bus */

  SPI_LOCK(dev->spi, false);
}

/****************************************************************************
 * Name: denis_open
 ****************************************************************************/

/**
 * @brief Открыть устройство Denis
 * 
 * Здесь при необходимости осуществляется первичная настройка устройства
 * и инициализация приватных параметров драйвера.
 * Не требуется реализация в текущем случае
 * 
 * @param filep Указатель на дескриптор файла
 * @return 0 - в случае успеха, отрицательное значение в ином случае
 */
static int denis_open(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: denis_close
 ****************************************************************************/

/**
 * @brief Закрыть устройство Denis
 * 
 * Здесь при необходимости осуществляются действия с устройством
 * по окончании работы с ним.
 * Не требуется реализация в текущем случае
 * 
 * @param filep Указатель на дескриптор файла
 * @return 0 - в случае успеха, отрицательное значение в ином случае
 */
static int denis_close(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: denis_read
 ****************************************************************************/

/**
 * @brief Прочитать данные из устройства Denis
 * 
 * Здесь реализуется POSIX интерфейс доступа к устройству для чтения
 * Не требуется реализация в текущем случае. Чтение запрещено
 * 
 * @param filep Указатель на дескриптор файла
 * @param buffer Указатель на данные
 * @param buflen Длина данных
 * @return -ENOSYS (Invalid system call number)
 */
static ssize_t denis_read(FAR struct file *filep, FAR char *buffer,
                            size_t buflen)
{
    return -ENOSYS;
}

/****************************************************************************
 * Name: denis_write
 ****************************************************************************/

/**
 * @brief Записать данные в устройство Denis
 * 
 * Здесь реализуется POSIX интерфейс доступа к устройству для записи
 * 
 * @param filep Указатель на дескриптор файла
 * @param buffer Указатель на данные
 * @param buflen Длина данных
 * @return Количество записанных данных
 */
static ssize_t denis_write(FAR struct file *filep, FAR const char *buffer,
                             size_t buflen)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct denis_dev_s *priv = inode->i_private;

    printf("%s: %d bytes\n", __func__, buflen);

    // Прямая запись в устройство через конкретный интерфейс
    denis_write_dev(priv, buffer, buflen);

    printf("%s:\n", __func__);
    dump_hex(buffer, buflen);

    return buflen;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief Зарегистрировать устройство Denis в системе
 * 
 * @param devpath Путь монтирования устройства
 * @param spi Дескриптор SPI
 * @param config Пользовательский конфиг, специфичный для устройства
 * @return 0 - в случае успеха, отрицательное значение в ином случае
 */
int denis_register(FAR const char *devpath, FAR struct spi_dev_s *spi,
                    FAR struct denis_config_s *config)
{
  FAR struct denis_dev_s *priv;
  int ret;

  /* Sanity check */

  DEBUGASSERT(spi != NULL);
  DEBUGASSERT(config != NULL);

  /* Initialize the Denis device structure */

  priv =
      (FAR struct denis_dev_s *)kmm_malloc(sizeof(struct denis_dev_s));
  if (priv == NULL)
    {
      snerr("ERROR: Failed to allocate instance\n");
      return -ENOMEM;
    }

  priv->spi         = spi;
  priv->config      = config;

  /* Setup SPI frequency and mode */

  SPI_SETFREQUENCY(spi, DENIS_SPI_FREQUENCY);
  SPI_SETMODE(spi, DENIS_SPI_MODE);

  /* Register the character driver */

  // Делаем связку между путем монтирования и файловыми операциями устройства
  ret = register_driver(devpath, &g_denis_fops, 0666, priv);
  if (ret < 0)
    {
      snerr("ERROR: Failed to register driver: %d\n", ret);
      kmm_free(priv);
      return ret;
    }

  /* Since we support multiple devices, we will need to add this new
   * instance to a list of device instances so that it can be found by the
   * interrupt handler based on the received IRQ number.
   */

  priv->flink = g_denis_list;
  g_denis_list = priv;

  return OK;
}
