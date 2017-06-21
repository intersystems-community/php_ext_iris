PHP_ARG_WITH(cach, whether to enable test support,
[  --with-test           Include test support.
library base directory.])

PHP_ADD_LIBRARY_WITH_PATH(cachet, ., CFLAGS)
PHP_ADD_INCLUDE(/usr)

PHP_NEW_EXTENSION(cach, cache.c, $ext_shared)
PHP_SUBST(GTM_SHARED_LIBADD)