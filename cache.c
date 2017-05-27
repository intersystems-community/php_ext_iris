#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_cache.h"

#include "callin.h"
#include "stdlib.h"

const zend_function_entry cache_functions[] = {
	PHP_FE(cach_set_dir, NULL)
	PHP_FE(cach_connect, NULL)
	PHP_FE(cach_quit, NULL)
	PHP_FE(cach_set, NULL)
	PHP_FE(cach_get,NULL)
	PHP_FE(cach_kill, NULL)
	PHP_FE(cach_kill_tree,NULL)
	PHP_FE(cach_order, NULL)
	PHP_FE(cach_order_rev, NULL)
	PHP_FE(test, NULL)
	PHP_FE_END
};

zend_module_entry cache_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_CACHE_EXT_NAME,
	cache_functions,
	PHP_MINIT(cache),
	PHP_MSHUTDOWN(cache),
	NULL,
	NULL,
	NULL,
#if ZEND_MODULE_API_NO >= 20010901
	PHP_CACHE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(cache)

static PHP_INI_MH(onActivateChange)
{
  zend_bool val = atoi(new_value);
}

PHP_INI_BEGIN()
PHP_INI_ENTRY("cache.test", "14", PHP_INI_ALL, onActivateChange) // FIX THIS
PHP_INI_ENTRY("cache.shdir", "/usr/lib/abadon/mgr", PHP_INI_ALL, NULL) // and this
PHP_INI_END()

PHP_MINIT_FUNCTION(cache)
{
	REGISTER_INI_ENTRIES();
	CacheSetDir(INI_STR("cache.shdir"));
}

PHP_MSHUTDOWN_FUNCTION(cache)
{
	UNREGISTER_INI_ENTRIES();
	CacheEnd();
}

PHP_FUNCTION(cach_set_dir)
{
	int cparams, temp = 0;
	char *path;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &cparams) == FAILURE) { 
		temp = BRIDGE_INVALID_PARAMETERS;
	}
	if (0 == temp) temp = CacheSetDir(path);
	RETURN_LONG(0 == temp);
}

PHP_FUNCTION(cach_connect)
{
	zval **args[3];
	int	username_len, password_len, argument_count, res = 0, tout = 0;
	char *username, *password;
	CACHE_STR pusername, ppassword, pexename;
	if (zend_parse_parameters(2, "ss|", 
		&username, &username_len, &password, &password_len) == FAILURE) { 
		res = BRIDGE_INVALID_PARAMETERS;
	} else {
		argument_count = ZEND_NUM_ARGS();
		if (2 == argument_count || 3 == argument_count) {
			strcpy((char *) pexename.str,"php");
			pexename.len = (unsigned short)strlen((char *) pexename.str);

			strcpy((char *) pusername.str,(char *) username);
			pusername.len = username_len;

			strcpy((char *) ppassword.str,(char *) password);
			ppassword.len = password_len;

			if (3 == argument_count) {  
				if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
					res = BRIDGE_INVALID_PARAMETERS;
				} else {
					if (PZVAL_IS_REF(*args[2])) {
						res = BRIDGE_INVALID_PARAMETERS;
					} else {
						if(IS_LONG == Z_TYPE_P(*args[2])) {
							tout = Z_LVAL_P(*args[2]);
						} else {
							res = BRIDGE_INVALID_PARAMETERS;
						}
					}
				}
			}
			if (0 == res) {
				res = CacheSecureStart(&pusername,&ppassword,&pexename,CACHE_TTNONE,tout,NULL,NULL);
			}
		} else {
			res = WRONG_PARAMS_COUNT;
		}
	}
	RETURN_LONG(0 == res);
}

PHP_FUNCTION(cach_quit)
{
	int con_res = CacheEnd();
	RETURN_LONG(0 == con_res);
}

static int __push_zval(zval* value)
{
	int res = 0;
	switch (Z_TYPE_P(value)) {
		case IS_NULL:	
			res = NULL_EXCEPTION;
			break;

		case IS_BOOL:
			res = CachePushInt(Z_LVAL_P(value));
			break;

		case IS_LONG:
			res = CachePushInt64(Z_LVAL_P(value));
			break;

		case IS_DOUBLE:
			res = CachePushDbl(Z_DVAL_P(value));
			break;

		case IS_STRING:
			res = CachePushStr(Z_STRLEN_P(value),Z_STRVAL_P(value));
			break;

		default:
			res = WRONG_DATA_TYPE;
			break;
	}
	return res;
}

static int __push_pp_global(int argument_count)
{
	zval **args[12];
	char *name;
	int name_len, res;
	if (zend_parse_parameters(1, "s|", &name, &name_len) == FAILURE) { 
		res = BRIDGE_INVALID_PARAMETERS;
	} else {
		if (1 > argument_count || 12 < argument_count) {
			res = WRONG_PARAMS_COUNT;
		} else {
			if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
				res = BRIDGE_INVALID_PARAMETERS;
			} else {
				if (!(res = CachePushGlobal(name_len,name))) {
					int counter;
					for (counter = 1; counter < argument_count; counter++) {
						if (PZVAL_IS_REF(*args[counter])) {
							res = BRIDGE_INVALID_PARAMETERS;
							break;
						} else {
							if(res = __push_zval(*args[counter])) break;
						}
					}
				}
			}
		}
	}
	return res;
}

PHP_FUNCTION(cach_set)
{
	int res = -1;
	if (1 < ZEND_NUM_ARGS()) {
		res = __push_pp_global(ZEND_NUM_ARGS());
		if (0 == res) res = CacheGlobalSet(ZEND_NUM_ARGS()-2);
	}
	RETURN_LONG(0 == res);
}

PHP_FUNCTION(cach_get)
{
	int flag = 1, res = __push_pp_global(ZEND_NUM_ARGS());
	if (0 == res) {
		res = CacheGlobalGet(ZEND_NUM_ARGS()-1,flag);
		if (0 == res) {
			int iTemp;
			double dTemp;
			Callin_char_t *sPTemp;
			switch (CacheType()) {
				case CACHE_INT:
					res = CachePopInt(&iTemp);
					RETURN_LONG(iTemp);
					break;

				case CACHE_IEEE_DBL: //Function "CachePopIEEEDbl" is missing
					/* fall-through */
				case CACHE_DOUBLE:
					res = CachePopDbl(&dTemp);
					RETURN_DOUBLE(dTemp);
					break;

				case CACHE_ASTRING:	
					/* fall-through */
				case CACHE_WSTRING:
					res = CachePopStr(&iTemp, &sPTemp);
					sPTemp[iTemp]='\0';
					RETURN_STRING(sPTemp, 1);
					break;

				default:
					res = CACHE_INVALID_PARAMETERS;
					break;
			}
		}
	}
	RETURN_LONG(0 == res);
}

PHP_FUNCTION(cach_kill)
{
	int res = 0;
	int argc = ZEND_NUM_ARGS();
		res = __push_pp_global(argc);
		if (0 == res) {
			res = CacheGlobalKill(argc-1,1);
		}
	RETURN_LONG(0 == res);
}

PHP_FUNCTION(cach_kill_tree)
{
	int res = 0;
	int argc = ZEND_NUM_ARGS();
		res = __push_pp_global(argc);
		if (0 == res) {
			res = CacheGlobalKill(argc-1,0);
		}
	RETURN_LONG(0 == res);
}

PHP_FUNCTION(cach_order)
{
	int res = 0;
	int argc = ZEND_NUM_ARGS();
		res = __push_pp_global(argc);
		if (0 == res) {
			res = CacheGlobalOrder(argc-1,1,0);
			if (0 == res) {
				int iTemp;
				double dTemp;
				Callin_char_t *sPTemp;
				switch (CacheType()) {
					case CACHE_INT:
						res = CachePopInt(&iTemp);
						RETURN_LONG(iTemp);
						break;

					case CACHE_IEEE_DBL: //Function "CachePopIEEEDbl" is missing
						/* fall-through */
					case CACHE_DOUBLE:
						res = CachePopDbl(&dTemp);
						RETURN_DOUBLE(dTemp);
						break;

					case CACHE_ASTRING:	
						/* fall-through */
					case CACHE_WSTRING:
						res = CachePopStr(&iTemp, &sPTemp);
						sPTemp[iTemp]='\0';
						RETURN_STRING(sPTemp, 1);
						break;

					default:
						res = CACHE_INVALID_PARAMETERS;
						break;
				}				
			}
		}
	RETURN_LONG(0 == res);	
}

PHP_FUNCTION(cach_order_rev)
{
	int res = 0;
	int argc = ZEND_NUM_ARGS();
		res = __push_pp_global(argc);
		if (0 == res) {
			res = CacheGlobalOrder(argc-1,-1,0);
			if (0 == res) {
				int iTemp;
				double dTemp;
				Callin_char_t *sPTemp;
				switch (CacheType()) {
					case CACHE_INT:
						res = CachePopInt(&iTemp);
						RETURN_LONG(iTemp);
						break;

					case CACHE_IEEE_DBL: //Function "CachePopIEEEDbl" is missing
						/* fall-through */
					case CACHE_DOUBLE:
						res = CachePopDbl(&dTemp);
						RETURN_DOUBLE(dTemp);
						break;

					case CACHE_ASTRING:	
						/* fall-through */
					case CACHE_WSTRING:
						res = CachePopStr(&iTemp, &sPTemp);
						sPTemp[iTemp]='\0';
						RETURN_STRING(sPTemp, 1);
						break;

					default:
						res = CACHE_INVALID_PARAMETERS;
						break;
				}				
			}
		}
	RETURN_LONG(0 == res);	
}

/*****************************************************************************************/

PHP_FUNCTION(test)
{

}