PHP_ARG_WITH(path, whether to work,
[  --with-path		  Include path to Iris])

if test "$PHP_PATH" != "no"; then
	if test -r $PHP_PATH/libirisdbt.so; then
		PATH_DIR=$PHP_PATH
		AC_MSG_RESULT(found)
	else
		if test -r $PHP_PATH/bin/libirisdbt.so; then
			PATH_DIR=$PHP_PATH/bin
			AC_MSG_RESULT(found)
		else
			for i in /lib /usr/local/lib /usr/lib; do
				if test -r $i/libirisdbt.so; then
					PATH_DIR = $i
					AC_MSG_RESULT(found in $i)
				fi
			done
		fi
	fi

	if test -z "$PATH_DIR"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Enter path to Iris or libirisdbt.so (--with-path=<DIR>))
	else
		PHP_ADD_LIBRARY_WITH_PATH(irisdbt, $PATH_DIR, CFLAGS)
		PHP_ADD_INCLUDE(/usr)
		AC_MSG_RESULT($PATH_DIR)

		PHP_NEW_EXTENSION(iris, iris.c, $ext_shared)
		PHP_SUBST(GTM_SHARED_LIBADD)
	fi
else
	AC_MSG_RESULT(not found params)
	AC_MSG_ERROR(Enter path to Iris or libirisdbt.so (--with-path=<DIR>))
fi