# INSTALLATION

PHP Api Version shoul be > 20131106

You able build your module or use our precompiled module *cach.so* for various version Intersystems Caché.

## Before install
1. Install Caché
2. Install `httpd`, `php` and `php-dev` packet by your packet manager.
Example (for Ubuntu) `app-get httpd php php-dev`
3. Add cache to web-users group by `usermod -a -G cacheusr www-data` command

## Build & install
1. `cd` in source dir
2. execute command `phpize` from php disribution
3. `./configure`
4. `make`
4. `make install`

## Setup module

1. The most common way to load extension is to include it in your `php.ini` configuration file, example: `extension=cach.so`
2. Enable interface callin and select login&password authentication type in Cache web interface.
3. *Necessarily* select "Startup namespace". You can use Caché WEB User Interface ("Users editing" tab, "Startup Namespace" field) or php function to do so.

### By default this php module uses empty namespace and using globals is unpossible.
**For using php module you need select namespace.**

For example:

`cach_exec('znspace "SAMPLES"');`


# Russian

Версия PHP Api должна быть > 20131106. 

Вы можете собрать самостоятельно модуль или использовать предкомпилированный из данного репозитория.

## Перед установкой
1. Устанавливаем Caché
2. Устанавливаем пакеты `httpd`, `php` и `php-dev`
Например (для Ubuntu) `app-get httpd php php-dev`
3. Добавляем пользователя cacheusr в группу web-users командой `usermod -a -G cacheusr www-data`

## Сборка и установка
1. Перейдите в папку исходного кода модуля
2. выполните команду `phpize`
3. выполните команду `./configure`
4. выполните команду `make`
5. выполните команду `make install`

## Настройка модуля

1. для использования необходимо добавить в файл php.ini, например: extension=cache.so
2. Включить интерфейс callin и выбрать аутентификацию по пользователю и паролю в web-интерфейсе Caché.
3. *ОБЯЗАТЕЛЬНО* выбрать "Startup namespace". Для этого вы можете использовать веб-интерфейс Caché (вкладка редактирования пользователей, поле "Startup Namespace") или php функцию.

### По умолчанию модуль не может работать с глобалами, пока не выбрана область (namespace).
**Для использования модуля необходимо установить область (namespace).**

Пример:

`cach_exec('znspace "SAMPLES"');`


  

  

  
