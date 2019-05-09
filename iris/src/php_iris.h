#define PHP_IRIS_VERSION "1.0"
#define PHP_IRIS_EXT_NAME "iris_ext"

PHP_MINIT_FUNCTION(iris);
PHP_MSHUTDOWN_FUNCTION(iris);
PHP_RSHUTDOWN_FUNCTION(iris);
PHP_FUNCTION(iris_set_dir);
PHP_FUNCTION(iris_connect);
PHP_FUNCTION(iris_quit);
PHP_FUNCTION(iris_set);
PHP_FUNCTION(iris_get);
PHP_FUNCTION(iris_kill);
PHP_FUNCTION(iris_zkill);
PHP_FUNCTION(iris_order);
PHP_FUNCTION(iris_order_rev);
PHP_FUNCTION(iris_query);
PHP_FUNCTION(iris_exec);
PHP_FUNCTION(iris_errno);
PHP_FUNCTION(iris_error);

#define IRIS_ERROR 0
#define IRIS_NO_ERROR 1
#define NULL_EXCEPTION -101
#define WRONG_DATA_TYPE -102
#define IRIS_INVALID_PARAMETERS -103
#define IRIS_INVALID_LOCAL -133723