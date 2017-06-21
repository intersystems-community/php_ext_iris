# Cache_ext
INSTALLATION
------------
    PHP Api Version:         20160303
    To install the extension you have to do these actions:
    1) execute command phpize
    2) execute command ./configure
    3) execute command make install
    4) The most common way to load extension is to include it in your php.ini configuration
    file, example: extension=cache.so
    5) Enable interface callin and select desired authentication type.
    By default this php module uses empty namespace, for using php module you have to change namespace. 
    You can use WEB User Interface or php function to do so.
    For example: cach_exec("znspace \"SAMPLES\"");

    Для установки расширения выполните следующие действия:
    1) выполните команду phpize
    2) выполните команду ./configure
    3) выполните команду make install
    4) для использования необходимо добавить в файл php.ini, например: extension=cache.so
    5) Включить интерфейс callin и выбрать желаемый тип аутентификации
    По умолчанию модуль php использует пустой параметр для namespace. Для использования модуля необходимо изменить параметр namespace.
    Для этого вы можете использовать веб-интерфейс или php функцию.
    Пример: cach_exec("znspace \"SAMPLES\"");