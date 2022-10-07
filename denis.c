/**
 * @file denis.c
 * @author your name (you@domain.com)
 * @brief 
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

#include <assert.h>
#include <errno.h>
#include <debug.h>
#include <string.h>
#include <stdio.h>

#include <nuttx/kmalloc.h>
#include <nuttx/wqueue.h>
#include <nuttx/random.h>
#include <nuttx/fs/fs.h>
#include <nuttx/semaphore.h>

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
                                        * of the LIS3DSH sensor */
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

static void denis_write_dev(FAR struct denis_dev_s *dev, const void * data, size_t data_len)
{
  /* Lock the SPI bus so that only one device can access it at the same
   * time
   */

  SPI_LOCK(dev->spi, true);

  /* Set CS to low which selects the DENIS */

  SPI_SELECT(dev->spi, dev->config->spi_devid, true);

  SPI_SNDBLOCK(dev->spi, data, data_len);

  /* Set CS to high which deselects the DENIS */

  SPI_SELECT(dev->spi, dev->config->spi_devid, false);

  /* Unlock the SPI bus */

  SPI_LOCK(dev->spi, false);
}

/****************************************************************************
 * Name: denis_open
 ****************************************************************************/

static int denis_open(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: denis_close
 ****************************************************************************/

static int denis_close(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: denis_read
 ****************************************************************************/

static ssize_t denis_read(FAR struct file *filep, FAR char *buffer,
                            size_t buflen)
{
    return -ENOSYS;
}

/****************************************************************************
 * Name: denis_write
 ****************************************************************************/

static ssize_t denis_write(FAR struct file *filep, FAR const char *buffer,
                             size_t buflen)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct denis_dev_s *priv = inode->i_private;

    printf("%s: %d bytes\n", __func__, buflen);

    denis_write_dev(priv, buffer, buflen);

    printf("%s:\n", __func__);
    dump_hex(buffer, buflen);

    return buflen;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

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

  ret = register_driver(devpath, &g_denis_fops, 0666, priv);
  if (ret < 0)
    {
      snerr("ERROR: Failed to register driver: %d\n", ret);
      kmm_free(priv);
      return ret;
    }

  /* Since we support multiple LIS3DSH devices, we will need to add this new
   * instance to a list of device instances so that it can be found by the
   * interrupt handler based on the received IRQ number.
   */

  priv->flink = g_denis_list;
  g_denis_list = priv;

  return OK;
}
