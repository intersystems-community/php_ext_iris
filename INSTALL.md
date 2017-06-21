# FTS_cache
INSTALLATION
------------
    PHP Api Version:         20160303
    To install the extension you have to do these actions:
    1) execute command phpize
    2) execute command ./configure
    3) execute command make install
    4)The most common way to load extension is to include it in your php.ini configuration
    file. example: extension=cache.so
    By default this php module uses empty namespace, for using php module you have to change namespace.
    For example: cach_exec("znspace \"SAMPLES\"");

    Для установки расширения выполните следующие действия:
    1) выполните команду phpize
    2) выполните команду ./configure
    3) выполните команду make install
    4) для использования необходимо добавить в файл php.ini, например: extension=cache.so
    По умолчанию модуль php использует пустой параметр для namespace. Для использования модуля необходимо изменить параметр namespace.
    Пример: cach_exec("znspace \"SAMPLES\"");