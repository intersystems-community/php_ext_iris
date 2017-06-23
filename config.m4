PHP_ARG_WITH(path, whether to work,
[  --with-path           Include path to Cache])

PATH_DIR=$PHP_PATH/dev/cache/callin/samples/

if test -r $PATH_DIR/libcachet.so; then
	AC_MSG_RESULT(found)
	PHP_ADD_LIBRARY_WITH_PATH(cachet, $PATH_DIR, CFLAGS)
	PHP_ADD_INCLUDE(/usr)
	AC_MSG_RESULT($PATH_DIR)

	PHP_NEW_EXTENSION(cach, cache.c, $ext_shared)
	PHP_SUBST(GTM_SHARED_LIBADD)
else
	AC_MSG_RESULT(not found)
	AC_MSG_ERROR(path to cache failure)
fi