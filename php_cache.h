#define PHP_CACHE_VERSION "0.1"
#define PHP_CACHE_EXT_NAME "cache"

PHP_MINIT_FUNCTION(cache);
PHP_MSHUTDOWN_FUNCTION(cache);
PHP_FUNCTION(cach_set_dir);
PHP_FUNCTION(cach_connect);
PHP_FUNCTION(cach_quit);
PHP_FUNCTION(cach_global_set);
PHP_FUNCTION(cach_global_get);
PHP_FUNCTION(cach_global_kill);
PHP_FUNCTION(cach_global_order);
PHP_FUNCTION(test);

#define BRIDGE_INVALID_PARAMETERS  -101
#define NULL_EXCEPTION -102
#define WRONG_DATA_TYPE -103
#define WRONG_PARAMS_COUNT -104
#define CACHE_INVALID_PARAMETERS -105