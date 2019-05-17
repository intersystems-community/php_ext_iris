# INSTALLATION

PHP Api Version shoul be > 20131106

You able build your module or use our precompiled module *iris.so* for various version Intersystems Iris.

## Build docker image and test php-extension (simpliest way).

1. Build own IRIS docker-image in current folder:
  `docker-compose build`
  
2. Run IRIS-container
  `docker-compose up -d`
  
3. Testing php demo-page on localhost:52080 in browser.
  
4. Stopping container
  `docker-compose stop`

## Manual setup with previsiously installed IRIS or on base official IRIS docker image.
  
1. Install Iris or setup oficial container.

2. Download official container:
  https://hub.docker.com/_/intersystems-iris-data-platform/plans/222f869e-567c-4928-b572-eb6a29706fbd?tab=instructions
  
3. Setup IRIS in container:
  http://localhost:9092/csp/sys/UtilHome.csp

4. URL for creating DB in container:
  http://localhost:9092/csp/sys/mgr/%25CSP.UI.Portal.Databases.zen

5. Create name space:
  http://localhost:9092/csp/sys/mgr/%25CSP.UI.Portal.Namespaces.zen

6. Enable call-in service:
  http://localhost:9092/csp/sys/sec/%25CSP.UI.Portal.Services.zen#

7. Create user, define namespace for user and grant access role %DB_%DEFAULT for user:
  http://localhost:9092/csp/sys/sec/%25CSP.UI.Portal.Users.zen
  
8. Installing packages `httpd`, `php` и `php-dev`.
  Example for Ubuntu: `apt-get update && apt-get install apache2 php php-dev`

9. Adding user irisusr in web-data group `usermod -a -G irisusr www-data`

## Build & install
1. `cd` in source dir
2. execute command `phpize` from php disribution
3. `./configure --with-path=[DIR]`
   DIR - path to Iris lib directory or directly to lbirisdbt.so
   example: ./configure --with-path=/usr/irissys/bin
4. `make`
5. `make install`

## Setup module

1. The most common way to load extension is to include it in your `php.ini` configuration file, example: `extension=iris.so`
2. Enable interface callin (Service tab) and select login&password authentication type in IRIS web interface.
3. **Necessarily** select "Startup namespace". You can use Caché WEB User Interface ("Users editing" tab, "Startup Namespace" field) or php function to do so.

### By default this php module uses empty namespace and using globals is unpossible.
**For using php module you need select namespace.**

For example:

`iris_exec('znspace "SAMPLES"');`


# Russian

Версия PHP Api должна быть > 20131106. 

Вы можете собрать самостоятельно модуль или использовать предкомпилированный из данного репозитория.

## Перед установкой
1. Собираем свой образ в текущей папке:
  docker-compose build
  
  Запускаем контейнер-контейнер
  docker-compose up -d
  
  Тестируем демо-страницу на localhost:52080 в браузере.
  
  Останавливаем контейнер
  docker-compose stop

  * Настройка на основе докер-образа Iris или установленного Iris. *
  
  Устанавливаем Iris (или скачиваем готовый docker-контейнер)
  Скачать контейнер:
  https://hub.docker.com/_/intersystems-iris-data-platform/plans/222f869e-567c-4928-b572-eb6a29706fbd?tab=instructions
  
  Настройка IRIS в контейнере:
  http://localhost:9092/csp/sys/UtilHome.csp

  Создать базу в контейнере
  http://localhost:9092/csp/sys/mgr/%25CSP.UI.Portal.Databases.zen

  Создать область
  http://localhost:9092/csp/sys/mgr/%25CSP.UI.Portal.Namespaces.zen

  Включить сервис call-in
  http://localhost:9092/csp/sys/sec/%25CSP.UI.Portal.Services.zen#

  Создать пользователя, присвоить ему область для запуска и дать пользователю роль %DB_%DEFAULT
  http://localhost:9092/csp/sys/sec/%25CSP.UI.Portal.Users.zen
  
2. Устанавливаем пакеты `httpd`, `php` и `php-dev`
Например (для Ubuntu) `apt-get install apache2 php php-dev`
3. Добавляем пользователя cacheusr в группу web-users командой `usermod -a -G irisusr www-data`

## Сборка и установка
1. Перейдите в папку исходного кода модуля
2. выполните команду `phpize`
3. выполните команду `./configure --with-path=[DIR]`
   DIR - путь к Caché или напрямую к libirisdbt.so
   например: ./configure --with-path=/usr/irissys/bin
4. выполните команду `make`
5. выполните команду `make install`

## Настройка модуля

1. для использования необходимо добавить в файл php.ini, например: extension=iris.so
   Для ubuntu: Создать файл /etc/php/7.2/mods-available/iris.ini c текстом: extension=iris.so,
   а потом создать на него символическую ссылку iris.so в папке /etc/php/7.2/mods-enabled.
2. Включить интерфейс callin (в сервисах) и выбрать аутентификацию по пользователю и паролю в web-интерфейсе Iris.
3. **ОБЯЗАТЕЛЬНО** выбрать "Startup namespace". Для этого вы можете использовать веб-интерфейс IRIS (вкладка редактирования пользователей, поле "Startup Namespace") или php функцию.

### По умолчанию модуль не может работать с глобалами, пока не выбрана область (namespace).
**Для использования модуля необходимо установить область (namespace).**

Пример:

`iris_exec('znspace "SAMPLES"');`
