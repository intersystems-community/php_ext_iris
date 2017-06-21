# INSTALLATION

PHP Api Version:         20160303

You able build your module or use our precompiled module.

## Build & install
1. `cd` in source dir
2. execute command `phpize` from php disribution
3. `./configure`
4. `make`
4. `make install`

## Setup module

1. The most common way to load extension is to include it in your php.ini configuration file, example: extension=cache.so
2. Enable interface callin and select login&password authentication type in Cache web interface.

### By default this php module uses empty namespace and using globals is unpossible.
**For using php module you need select namespace.**

You can use Caché WEB User Interface ("Users editing" tab, "Startup Namespace" field)
or php function to do so.
For example:

`cach_exec('znspace "SAMPLES"');`

# Russian

Вы можете собрать самостоятельно модуль или использовать предкомпилированный из данного репозитория.

## Сборка и установка
1. Перейдите в папку исходного кода модуля
2. выполните команду `phpize`
3. выполните команду `./configure`
3. выполните команду `make`
4. выполните команду `make install`

## Настройка модуля

1. для использования необходимо добавить в файл php.ini, например: extension=cache.so
2. Включить интерфейс callin и выбрать аутентификацию по пользователю и паролю в web-интерфейсе Caché.

### По умолчанию модуль не может работать с глобалами, т.к. не выбрана область (namespace).
**Для использования модуля необходимо установить область (namespace).**

Для этого вы можете использовать веб-интерфейс Caché (вкладка редактирования пользователей, поле "Startup Namespace")
или php функцию.

Пример:

`cach_exec('znspace "SAMPLES"');`
