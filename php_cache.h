#define PHP_CACHE_VERSION "0.2"
#define PHP_CACHE_EXT_NAME "cache_ext"

PHP_MINIT_FUNCTION(cache);
PHP_RINIT_FUNCTION(cache);
PHP_RSHUTDOWN_FUNCTION(cache);
PHP_MSHUTDOWN_FUNCTION(cache);
PHP_FUNCTION(cach_set_dir);
PHP_FUNCTION(cach_connect);
PHP_FUNCTION(cach_quit);
PHP_FUNCTION(cach_set);
PHP_FUNCTION(cach_get);
PHP_FUNCTION(cach_kill);
PHP_FUNCTION(cach_kill_tree);
PHP_FUNCTION(cach_order);
PHP_FUNCTION(cach_order_rev);
PHP_FUNCTION(cach_exec);
PHP_FUNCTION(test);

#define CACHE_ERROR 0
#define CACHE_NO_ERROR 1