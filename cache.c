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
  PHP_FE(cach_zkill,NULL)
  PHP_FE(cach_order, NULL)
  PHP_FE(cach_order_rev, NULL)
  PHP_FE(cach_query, NULL)
  PHP_FE(cach_exec, NULL)
  PHP_FE(cach_errno, NULL)
  PHP_FE(cach_error, NULL)
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
  PHP_RSHUTDOWN(cache),
  NULL,
  NULL,
#if ZEND_MODULE_API_NO >= 20010901
  PHP_CACHE_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(cache)

#if ZEND_MODULE_API_NO <= 20131226
#define _RETURN_STRING(str) RETURN_STRING(str, 0)
#define _ZVAL_STRING(rev,str) ZVAL_STRING(rev,str,0)
typedef long zend_long;
typedef int strsize_t;
#else
#define _RETURN_STRING(str) RETURN_STRING(str)
#define _ZVAL_STRING(rev,str) ZVAL_STRING(rev,str)
typedef size_t strsize_t;
#endif

int cache_errno = 0, pth = 0;
char *cache_error, *cache_pth = "No error";

PHP_INI_BEGIN()
PHP_INI_ENTRY("cach.shdir", "/InterSystems/Cache/mgr", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("cach.login", "Admin", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("cach.password", "1234", PHP_INI_ALL, NULL)
PHP_INI_END()

PHP_MINIT_FUNCTION(cache)
{
  REGISTER_INI_ENTRIES();
}

PHP_RSHUTDOWN_FUNCTION(cache)
{
  CacheEnd();
}

PHP_MSHUTDOWN_FUNCTION(cache)
{
  CacheEndAll();
  UNREGISTER_INI_ENTRIES();
}

static void __on_cache_error(int error) {
  cache_errno = errno;
  char fl[255];
  CACHE_STR errmsg;
  CACHE_STR srcline;
  int offset, res;
  errmsg.len = 50;
  srcline.len = 100;
  if (CACHE_SUCCESS != (res = CacheError(&errmsg, &srcline, &offset))) {
    sprintf(fl, "Failed get error - rc = %i", res);
    cache_error = &fl[0];
  } else {
    errmsg.str[errmsg.len] = '\0';
    cache_error = errmsg.str;
  }
}

PHP_FUNCTION(cach_set_dir)
{
  strsize_t cparams;
  char *path;
  int errno, temp = CACHE_NO_ERROR;
  int argument_count = ZEND_NUM_ARGS();
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &cparams) == FAILURE) {
    temp = CACHE_ERROR;
  } else {
    if (cparams>0) {
      path[cparams] = '\0';
    }
    cache_pth = path;
    pth = 1;
  }
  if(0 == temp) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_connect)
{
  strsize_t argument_count, username_len, password_len;
  int errno, res = CACHE_NO_ERROR, tout = 0;
  char *username, *password;
  CACHE_STR pusername, ppassword, pexename;

  if (0 == pth) {
    cache_pth = zend_ini_string("cach.shdir", strlen("cach.shdir"), 0);
  }
  if (CACHE_SUCCESS != (errno = CacheSetDir(cache_pth))) {
    __on_cache_error(errno);
    res = CACHE_ERROR;
  } else {
    argument_count = ZEND_NUM_ARGS();
    if (0 == argument_count || 2 == argument_count || 3 == argument_count) {
      strcpy((char *) pexename.str,"php");
      pexename.len = (unsigned short)strlen((char *) pexename.str);
      if (0 == argument_count) {
        username = zend_ini_string("cach.login", strlen("cach.login"), 0);
        password = zend_ini_string("cach.password", strlen("cach.password"), 0);

      } else if (2 == argument_count || 3 == argument_count) {

        #if ZEND_MODULE_API_NO <= 20131226
          zval **args[3];
          if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
            cache_errno = CACHE_INVALID_PARAMETERS;
            res = CACHE_ERROR;
          } else {
            if (PZVAL_IS_REF(*args[2])) {
              cache_errno = CACHE_INVALID_PARAMETERS;
              res = CACHE_ERROR;
            } else {
              if(IS_LONG == Z_TYPE_PP(args[2])) {
                tout = Z_LVAL_PP(args[2]);
              } else {
                php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
              }
            }
          }
        #else
          zval args[3];
          if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
            cache_errno = CACHE_INVALID_PARAMETERS;
            res = CACHE_ERROR;
          } else {
            if(IS_STRING == Z_TYPE(args[0])) {
              username = Z_STRVAL(args[0]);
            } else {
              php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
            }
            if(IS_STRING == Z_TYPE(args[1])) {
              password = Z_STRVAL(args[1]);
            } else {
              php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
            } 
            if (3 == argument_count) {
              if(IS_LONG == Z_TYPE(args[2])) {
                tout = Z_LVAL(args[2]);
              } else {
                php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
              }
            }
          }
        #endif
      }

      strcpy((char *) pusername.str, username);
      pusername.len = strlen(pusername.str);

      strcpy((char *) ppassword.str, password);
      ppassword.len = strlen(ppassword.str);
      if (res) {
        /* CACHE_PROGMODE т.к в случае ошибки соединение обрывается и не работает CacheError,
        CACHE_TTNONE чтобы не перехватывало I/O */
        if (CACHE_SUCCESS != (errno = CacheSecureStart(&pusername,&ppassword,&pexename,CACHE_PROGMODE|CACHE_TTNONE,tout,NULL,NULL))) {
          __on_cache_error(errno);
          res = CACHE_ERROR;
        }
      }
    } else {
      php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    }
  }
  if(0 == res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_quit)
{
  int res = CacheEnd();
  if (CACHE_SUCCESS != res) __on_cache_error(errno);
  if(0 != res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

static int __push_zval(zval* value)
{
  zval *new_value;
  int name_len, errno, count = 0, res = CACHE_NO_ERROR;
  char *name, *new_name;
  switch (Z_TYPE_P(value)) {
    case IS_NULL:
      cache_errno = NULL_EXCEPTION;
      cache_error = "Null exception";
      res = CACHE_ERROR;
      break;

    #if ZEND_MODULE_API_NO <= 20131226
      case IS_BOOL:
        if (CACHE_SUCCESS != (errno = CachePushInt(Z_LVAL_P(value)))) {
          __on_cache_error(errno);
          res = CACHE_ERROR;
        }
        break;
    #endif

    case IS_LONG:
      if (CACHE_SUCCESS != (errno = CachePushInt64(Z_LVAL_P(value)))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;

    case IS_DOUBLE:
      if (CACHE_SUCCESS != (errno = CachePushDbl(Z_DVAL_P(value)))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;

    case IS_STRING:
      if (CACHE_SUCCESS != CachePushStr(Z_STRLEN_P(value),Z_STRVAL_P(value))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;

    case IS_ARRAY:
      ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), new_value) {
        if (0 == count) {
          if (IS_STRING == Z_TYPE_P(new_value)) {
            name = Z_STRVAL_P(new_value);
            if (name[0] != '^') {
              php_error(E_WARNING, "%s(): the character \"^\" was not specified", get_active_function_name(TSRMLS_C));
              errno = CACHE_INVALID_LOCAL;
              res = CACHE_ERROR;
            } else {
              new_name = name + 1;
              name_len = strlen(new_name);
              if (0 != CachePushGlobal(name_len,new_name)) {
                __on_cache_error(errno);
                res = CACHE_ERROR;
              }
            }
          }
        } else {
          __push_zval(new_value);
        }
        count++;
      } ZEND_HASH_FOREACH_END();
      if (0 == count) {
        cache_errno = WRONG_DATA_TYPE;
        cache_error = "Wrong data type";
        res = CACHE_ERROR;
      }
      break;

    default:
      cache_errno = WRONG_DATA_TYPE;
      cache_error = "Wrong data type";
      res = CACHE_ERROR;
      break;
  }
  return res;
}

static int __push_pp_global(int argument_count)
{
  char fl[255];
  zval *one;
  char *name, *new_name;
  int name_len, errno, res = CACHE_NO_ERROR;
  if (zend_parse_parameters(1, "z|", &one) == FAILURE) {
    res = CACHE_ERROR;
  } else {
    if (1 > argument_count || 30 < argument_count) {
      php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    } else {
      #if ZEND_MODULE_API_NO <= 20131226
        zval **args[30];
        if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
          sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
          cache_error = &fl[0];
          cache_errno = CACHE_INVALID_PARAMETERS;
          res = CACHE_ERROR;
        } else {
          if ((0 != (errno = CachePushGlobal(name_len,new_name)))) {
            __on_cache_error(errno);
            res = CACHE_ERROR;
          } else {
            int counter;
            for (counter = 1; counter < argument_count; counter++) {
              if (PZVAL_IS_REF(*args[counter])) {
                sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
                cache_error = &fl[0];
                cache_errno = CACHE_INVALID_LOCAL;
                res = CACHE_ERROR;
                break;
              } else {
                if(!(res = __push_zval(*args[counter]))) break;
              }
            }
          } 
        }
      #else
        zval args[30];
        if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS) {
          sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
          cache_error = &fl[0];
          cache_errno = CACHE_INVALID_PARAMETERS;
          res = CACHE_ERROR;
        } else {
          int counter = 0;
          if (IS_STRING == Z_TYPE(args[0])) {
            name = Z_STRVAL(args[0]);
            if (name[0] != '^') {
              php_error(E_WARNING, "%s(): the character \"^\" was not specified", get_active_function_name(TSRMLS_C));
              errno = CACHE_INVALID_LOCAL;
              res = CACHE_ERROR;
            } else {
              new_name = name + 1;
              name_len = strlen(new_name);
              if (0 != CachePushGlobal(name_len,new_name)) {
                __on_cache_error(errno);
                res = CACHE_ERROR;
              } else {
                counter++;
              }
              for (;counter < argument_count; counter++) {
                if(!(res = __push_zval(&args[counter]))) break;
              }
            }
          }
        }
      #endif
    }
  }
  return res;
}

static zval __pop_cache()
{
  zval return_value;
  int iTemp;
  double dTemp;
  Callin_char_t *sPTemp;
  switch (CacheType()) {
    case CACHE_INT:
      if(0 != (errno = CachePopInt(&iTemp))) {
        __on_cache_error(errno);
        ZVAL_LONG(&return_value, CACHE_ERROR);
      } else {
        ZVAL_LONG(&return_value, iTemp);
      }
      break;

    case CACHE_IEEE_DBL: //Function "CachePopIEEEDbl" is missing
      /* fall-through */
    case CACHE_DOUBLE:
      if(0 != (errno = CachePopDbl(&dTemp))) {
        __on_cache_error(errno);
        ZVAL_LONG(&return_value, CACHE_ERROR);
      } else {
        ZVAL_DOUBLE(&return_value, dTemp);
      }
      break;

    case CACHE_ASTRING:
      /* fall-through */
    case CACHE_WSTRING:
      if(0 != (errno = CachePopStr(&iTemp, &sPTemp))) {
        __on_cache_error(errno);
        ZVAL_LONG(&return_value, CACHE_ERROR);
      } else {
        sPTemp[iTemp] = '\0';
        char* endptr;
        sPTemp[iTemp]='\0';
        if (0 != iTemp) {
          iTemp = strtoimax(sPTemp, &endptr, 10);
          if (0 == strlen(endptr)) {
            ZVAL_LONG(&return_value,iTemp);
          } else {
            _ZVAL_STRING(&return_value, sPTemp);
          }
        } else {
          ZVAL_NULL(&return_value);
        }
      }
      break;

    default:
      cache_errno = WRONG_DATA_TYPE;
      cache_error = "Wrong data type";
      ZVAL_LONG(&return_value, CACHE_ERROR);
      break;
  }
  return return_value;
}

static int __received_parameters_count(int argc)
{
  char fl[255];
  zval args[30], *new_value;
  if(zend_get_parameters_array_ex(argc, args) != SUCCESS) {
    sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
    cache_error = &fl[0];
    cache_errno = CACHE_INVALID_PARAMETERS;
    argc = 0;
  } else {
    if (IS_ARRAY == Z_TYPE(args[0])) {
      argc--;
      ZEND_HASH_FOREACH_VAL(Z_ARRVAL(args[0]), new_value) {
        argc++;
      } ZEND_HASH_FOREACH_END();
    }
  }
  return argc;
}

PHP_FUNCTION(cach_set)
{
  int errno, res = CACHE_NO_ERROR, argc = ZEND_NUM_ARGS();
  if (1 < argc) {
    if(res = __push_pp_global(argc)) {
      if (0 != (errno = CacheGlobalSet(__received_parameters_count(argc)-2))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
    }
  } else {
    php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
  }
  if(0 == res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_get)
{
  int errno, flag = 0, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if (0 != (errno = CacheGlobalGet(__received_parameters_count(argc)-1,flag))) {
      if (errno == CACHE_ERUNDEF) {
        RETURN_NULL();
      }
      __on_cache_error(errno);
      res = CACHE_ERROR;
    } else {
      zval ret = __pop_cache();
      RETURN_ZVAL(&ret,1,1);
    }
  }
  RETURN_FALSE;
}

PHP_FUNCTION(cach_zkill)
{
  int errno, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if (0 != (errno = CacheGlobalKill(__received_parameters_count(argc)-1,1))) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    }
  }
  if(0 == res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_kill)
{
  int errno, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if(0 != (errno = CacheGlobalKill(__received_parameters_count(argc)-1,0))) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    }
  }
  if(0 == res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_order)
{
  int errno, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if (0 != (errno = CacheGlobalOrder(__received_parameters_count(argc)-1,1,0))) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    } else {
      zval ret = __pop_cache();
      RETURN_ZVAL(&ret,1,1);
    }
  }
  RETURN_NULL();
}

PHP_FUNCTION(cach_order_rev)
{
  int errno, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if(0 != (errno = CacheGlobalOrder(__received_parameters_count(argc)-1,-1,0))) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    } else {
      zval ret = __pop_cache();
      RETURN_ZVAL(&ret,1,1);
    }
  }
  RETURN_NULL();
}

PHP_FUNCTION(cach_query)
{
  int count = 0, errno, res, argc = ZEND_NUM_ARGS();
  if (res = __push_pp_global(argc)) {
    if (CACHE_SUCCESS != CachePushStr(0,"")) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    } else  {
      char fl[255];
      zval args[30], *new_value;
      if(zend_get_parameters_array_ex(argc, args) != SUCCESS) {
        sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
        cache_error = &fl[0];
        cache_errno = CACHE_INVALID_PARAMETERS;
        res = CACHE_ERROR;
      } else {
        count = __received_parameters_count(argc);
        if(0 != (errno = CacheGlobalOrder(count,1,0))) {
          __on_cache_error(errno);
          res = CACHE_ERROR;
        } else {
          zval myArray, ret = __pop_cache();
          if (IS_LONG == Z_TYPE(ret)) {
            if (0 == (Z_LVAL(ret))) {
              RETURN_NULL();
            }
          }
          array_init(&myArray);
          int counter = 0;
          if ((1 == argc) && (IS_ARRAY == Z_TYPE(args[0]))) {
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL(args[0]), new_value) {
              add_next_index_zval(&myArray, new_value);
            } ZEND_HASH_FOREACH_END();

          } else {
            for (counter = 0; counter < argc; counter++) {
              add_index_zval(&myArray, counter, &args[counter]);
            }
          }
          add_index_zval(&myArray, count, &ret);
          RETURN_ZVAL(&myArray,1,1);
        }
      }
    }
  }
  RETURN_NULL();
}

PHP_FUNCTION(cach_exec)
{
  char *command_str;
  int res = CACHE_NO_ERROR;
  strsize_t command_len;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command_str, &command_len) == FAILURE) { 
    res = CACHE_ERROR;
  } else {
    CACHE_STR command;
    strcpy((char *) command.str,command_str);
    command.len = command_len;
    if (0 != (errno = CacheExecute(&command))) {
      __on_cache_error(errno);
      res = CACHE_ERROR;
    }
  }
  if(0 == res) {
    RETURN_FALSE;
  } RETURN_TRUE;
}

PHP_FUNCTION(cach_errno)
{
  RETURN_LONG(cache_errno);
}

PHP_FUNCTION(cach_error)
{
  _RETURN_STRING(cache_error);
}