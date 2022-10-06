# О проекте

Это первый пробный проект на базе nuttx.

## Задача

Разработать драйвер SPI и приложение для взаимодействия с ним.

Приложение должно отправлять в SPI два типа данных:
1. Счетчик
2. Произведение двух случайно сгенерированных матриц

Реализовать работу драйвера в двух задачах.

# Сборка

Проект основан на [nuttx](https://github.com/apache/incubator-nuttx) версии 11.0.0-RC2

### 1. Подготовить окружение:

Необходимо, чтобы в общей папке были помещены два репозитория:

```sh
git clone https://github.com/apache/incubator-nuttx.git nuttx
git clone https://github.com/apache/incubator-nuttx-apps apps
```

Более подробно можно посмотреть [тут](https://nuttx.apache.org/docs/latest/quickstart/install.html)

### 2. Склонировать текущий репозиторий в папку apps/examples/test-task:

```sh
cd apps/examples
git clone git@github.com:chuyec/nuttx-test-task.git test-task
```

### 3. Выбрать стандартную плату и конфигурацию:

```sh
cd nuttx
tools/configure.sh -l nucleo-f4x1re:f401-nsh
```

### 4. Сконфигурировать Kconfig:

```sh
cd nuttx
make apps_distclean
make menuconfig
```

1. _`Application Configuration ---> Examples ---> `_ Select _`[*] Test Task`_
2. _`RTOS Features ---> Tasks and Scheduling ---> Application entry point ---> `_ Type _`'test_task_main'`_
3. _`System Type ---> STM32 Peripheral Support ---> `_ Select _`SPI1`_

### 4. Собрать:

```sh
cd nuttx
make
```

### 5. Зашить:

```sh
openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c 'init' -c 'program nuttx verify reset' -c 'shutdown'
```

### 6. Наблюдать работу приложения в терминале:

```sh
picocom -b 115200 /dev/ttyACM0
```
