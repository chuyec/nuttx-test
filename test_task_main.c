/****************************************************************************
 * apps/examples/test_task/test_task_main.c
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include "libs/nml/nml.h"

#include "stm32_denis.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

// Плавающая точка нужна в отладочных целях для вывода в serial port
// double значений матриц, сгенерированных библиотекой nml 

#ifndef CONFIG_LIBC_FLOATINGPOINT
#  error CONFIG_LIBC_FLOATINGPOINT must be enabled
#endif

#define TASK_COUNTER_STACKSIZE    2048
#define TASK_COUNTER_PRIORITY     100
#define TASK_MATRIX_STACKSIZE     2048
#define TASK_MATRIX_PRIORITY      100

#define DENIS_DEVNAME    "/dev/denis0"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * task_counter
 * Task to generate a counter
 ****************************************************************************/

/**
 * @brief Задача для генерации счетчика и отправки его в SPI устройство "denis"
 * 
 * @param argc Не используется
 * @param argv Не используется
 * @return int Результат выполнения
 */
static int task_counter(int argc, char *argv[])
{
  int ret = OK;

  printf("%s: Running\n", __func__);
  printf("%s: Opening '%s' for write\n", __func__, DENIS_DEVNAME);

  // При старте задачи открываем устройство DENIS_DEVNAME ("/dev/denis0") на запись
  // Устройство будет закрыто только в случае ошибки и выхода из задачи

  int fd = open(DENIS_DEVNAME, O_WRONLY);
  if (fd < 0)
    {
      printf("%s: Failed to open %s: %d\n", __func__, DENIS_DEVNAME, errno);

      // Устройство открыть не удалось (не существует или не прошла инициализация)
      // Выходим из задачи с ошибкой. Закрывать устройство не требуется

      ret = EXIT_FAILURE;
      goto exit_without_close;
    }

  // Бесконечный цикл, в котором раз в секунду в DENIS_DEVNAME
  // отправляется текущиее время (timestamp) с начала старта программы

  while(1)
  {
    // Читаем текущий таймстамп

    time_t timestamp = time(NULL);
    size_t timestamp_len = sizeof(timestamp);

    // Пишем в открытое устройство DENIS_DEVNAME timestamp-данные
    // Длина данных sizeof(time_t) может быть как 32-х, так и 64-х битной

    int nbytes = write(fd, &timestamp, timestamp_len);
    if (nbytes != timestamp_len)
    {
      printf("%s: ERROR: write(%zd) returned %d\n", __func__, timestamp_len, nbytes);

      // Не удалось записать данные в устройство.
      // Завершаем задачу ошибкой, не забыв закрыть устройство

      ret = EXIT_FAILURE;
      goto exit_with_close;
    }
    
    // Засыпаем на 1 секунду до следующей итерации отправки данных

    sleep(1);
  }

  exit_with_close:

  // Метка для завершения задачи в случае, если требуется закрыть устройство

  printf("%s: Closinging '%s'\n", __func__, DENIS_DEVNAME);

  close(fd);

  exit_without_close:

  // Метка для завершения задачи в случае, если устройство закрывать не требуется

  printf("%s: Exit\n", __func__);

  exit(ret);
}

/****************************************************************************
 * task_matrix
 * Task to generate a matrix multiplication
 ****************************************************************************/

/**
 * @brief Задача для генерации матрицы и отправки ее в SPI устройство "denis"
 * 
 * Общий флоу такой:
 * 1. Генерируем две случайные матрицы (случайное количество элементов и случайные значения)
 * 2. Умножаем первую матрицу на вторую
 * 3. Утправляем в устройство DENIS_DEVNAME
 * 
 * Для работы с матрицами используется библиотека https://github.com/nomemory/neat-matrix-library
 * 
 * @param argc Не используется
 * @param argv Не используется
 * @return int Результат выполнения
 */
static int task_matrix(int argc, char *argv[])
{
  int ret = OK;

  printf("%s: Running\n", __func__);
  printf("%s: Opening '%s' for write\n", __func__, DENIS_DEVNAME);

  // При старте задачи открываем устройство DENIS_DEVNAME ("/dev/denis0") на запись
  // Устройство будет закрыто только в случае ошибки и выхода из задачи

  int fd = open(DENIS_DEVNAME, O_WRONLY);
  if (fd < 0)
    {
      printf("%s: Failed to open %s: %d\n", __func__, DENIS_DEVNAME, errno);

      // Устройство открыть не удалось (не существует или не прошла инициализация)
      // Выходим из задачи с ошибкой. Закрывать устройство не требуется

      ret = EXIT_FAILURE;
      goto exit_without_close;
    }

  // Инициализируем генератор случайных чисел rand()
  // Достаточно вызвать только одиин раз

  srand(time(NULL));
  
  // Бесконечный цикл, в котором в DENIS_DEVNAME периодически
  // отправляется специальным образом сгенерированная матрица

  while (1)
  {
    nml_mat *m1, *m2, *m3;

    // Генерируем случайные значения размеров двух матриц
    // При этом устанавливаем ограничение размера от 1 до 5 включительно
    // Для генерации случайных размеров используем методы библиотеки nml

    unsigned int nrows_m1 = nml_rand_interval(1, 6);
    unsigned int ncols_m1 = nml_rand_interval(1, 6);
    unsigned int nrows_m2 = ncols_m1;                 // Требование для осуществления умножения матриц
    unsigned int ncols_m2 = nml_rand_interval(1, 6);

    printf("\n\n");
    printf("%s: Creating a random m1 matrix %dx%d:\n", __func__, nrows_m1, ncols_m1);

    // Генерируем первую матрицу размером [nrows_m1 х ncols_m1]
    // и заполняем её случайными значениями от -100 до 100.

    m1 = nml_mat_rnd(nrows_m1, ncols_m1, -100.0, 100.0);
    nml_mat_printf(m1, "%.2lf\t\t");

    printf("%s: Creating a random m2 matrix %dx%d:\n", __func__, nrows_m2, ncols_m2);

    // Генерируем вторую матрицу размером [nrows_m2 х ncols_m2]
    // и заполняем её случайными значениями от -100 до 100

    m2 = nml_mat_rnd(nrows_m2, ncols_m2, -100.0, 100.0);
    nml_mat_printf(m2, "%.2lf\t\t");

    printf("%s: m1 and m2 matrix multiplication:\n", __func__);

    // Умножаем матрицу m1 на матрицу m2
    // Результат получаем в матрице m3

    m3 = nml_mat_dot(m1, m2);
    nml_mat_printf(m3, "%.2lf\t\t");

    // При создании матриц используется динамическое выделение памяти
    // Поэтому освобождаем память, выделенную под матрицы m1 и m2

    nml_mat_free(m1);
    nml_mat_free(m2);

    // Отправляем матрицу m3 в устройство DENIS_DEVNAME

    for (int i = 0; i < m3->num_rows; i++)
    {
      // Созданные при помощи библиотеки nml матрицы не лежат в одной непрерывной области памяти,
      // поэтому мы не можем просто отправить данные по указателю m3->data
      // 
      // Делаем допущение, что нам не требуется отправлять всю матрицу за одну транзакцию
      // Это позволяет нам не выделять дополнительную память-буфер для формирования неразрывных данных
      //
      // Отправляем матрицу построчно

      // Формируем указатель на очередную строку матрицы и фиксируем её длину
      
      void *pdata = m3->data[i];
      size_t data_len = m3->num_cols * sizeof(**m3->data);

      // Пишем в открытое устройство DENIS_DEVNAME строку матрицы

      int nbytes = write(fd, pdata, data_len);
      if (nbytes != data_len)
      {
        // Не удалось записать данные в устройство.
        // Освобождаем память, выделенную для матрицы m3

        nml_mat_free(m3);

        printf("%s: ERROR: write(%zd) returned %d\n", __func__, data_len, nbytes);

        // Завершаем задачу ошибкой, не забыв закрыть устройство

        ret = EXIT_FAILURE;
        goto exit_with_close;
      }
    }

    // Освобождаем память, выделенную для матрицы m3

    nml_mat_free(m3);

    // Засыпаем на 3 секунды
    // После пробуждения цикл генерации и отправки матрицы повторяется
    
    sleep(3);
  }

  exit_with_close:

  // Метка для завершения задачи в случае, если требуется закрыть устройство

  printf("%s: Closinging '%s'\n", __func__, DENIS_DEVNAME);

  close(fd);

  exit_without_close:

  // Метка для завершения задачи в случае, если устройство закрывать не требуется

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

  // Инициализируем устройство "denis" и вешаем его на SPI1.
  // 
  // В идеале платозависимая инициализация устройства
  // должна быть вызвана до main() в функции stm32_bringup()
  // Но для этого нужно вносить изменения в репозитории nuttx

  result = board_denis_initialize(1);
  if (result != 0)
    {
      // Не получилось инициализировать. Выходим с ошибкой

      printf("Failed to Denis initialization\n");
      goto errout;
    }

  // Создаем задачу task_counter для генерации и отпрвки счетчика
  
  result = task_create("task_counter", TASK_COUNTER_PRIORITY, TASK_COUNTER_STACKSIZE, task_counter, NULL);
  if (result < 0)
    {
      // Не получилось создать задачу. Выходим с ошибкой

      printf("Failed to start task_counter: %d\n", errno);
      goto errout;
    }

  // Создаем задачу task_matrix для генерации и отпрвки матрицы

  result = task_create("task_matrix", TASK_MATRIX_PRIORITY, TASK_MATRIX_STACKSIZE, task_matrix, NULL);
  if (result < 0)
    {
      // Не получилось создать задачу. Выходим с ошибкой

      printf("Failed to start task_matrix: %d\n", errno);
      goto errout;
    }

  printf("main exit\n");

  // Выходим из main с успехом

  return 0;

  errout:

  // Выходим из main с ошибкой

  exit(EXIT_FAILURE);
}
