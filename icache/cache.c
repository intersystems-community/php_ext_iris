/*
  PHP-extension "cach" for integration with InterSystems Cache and Iris.
  
  Commercial support, adding new features and tuning also available.
  Sergey Kamenev cacher13@inetstar.ru
*/
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
  NULL,
  PHP_RSHUTDOWN(cache),
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

#define PHP_WARNING(s) \
php_error(E_WARNING, "%s(): "s, get_active_function_name(TSRMLS_C));

#define PHP_ERROR(s) \
hp_error(E_ERROR, "%s(): "s, get_active_function_name(TSRMLS_C));

// Processing error in InterSystem Cache
static void __on_cache_error(int error)
{
  cache_errno = errno;
  char fl[255];
  CACHE_STR errmsg;
  CACHE_STR srcline;
  int offset, res;
  errmsg.len = 50;
  srcline.len = 100;
  if (CACHE_SUCCESS != (res = CacheError(&errmsg, &srcline, &offset)))
  {
    sprintf(fl, "Failed get error - rc = %i", res);
    cache_error = &fl[0];
  }
  else
  {
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
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &cparams) == FAILURE)
  {
    RETURN_FALSE;
  }

  if (cparams>0)
  {
    path[cparams] = '\0';
  }

  cache_pth = path;
  pth = 1;
  
  RETURN_TRUE;
}

PHP_FUNCTION(cach_connect)
{
  strsize_t argument_count, username_len, password_len;
  int errno, res = CACHE_NO_ERROR, tout = 0;
  char *username, *password;
  CACHE_STR pusername, ppassword, pexename;

  if (0 == pth)
  {
    cache_pth = zend_ini_string("cach.shdir", strlen("cach.shdir"), 0);
  }
  
  errno = CacheSetDir(cache_pth);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  argument_count = ZEND_NUM_ARGS();

  if ((argument_count == 1) || (argument_count > 3))
  {
    php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }

  strcpy((char *) pexename.str,"php");
  pexename.len = (unsigned short)strlen((char *) pexename.str);
  if (0 == argument_count)
  {
    username = zend_ini_string("cach.login", strlen("cach.login"), 0);
    password = zend_ini_string("cach.password", strlen("cach.password"), 0);
  }
  else if (2 == argument_count || 3 == argument_count)
  {
    #if ZEND_MODULE_API_NO <= 20131226
    zval **args[3];
    if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
    {
      cache_errno = CACHE_INVALID_PARAMETERS;
      RETURN_FALSE;
    }
 
    if (PZVAL_IS_REF(*args[2]))
    {
      cache_errno = CACHE_INVALID_PARAMETERS;
      RETURN_FALSE;
    }

    if(IS_LONG == Z_TYPE_PP(args[2]))
    {
      tout = Z_LVAL_PP(args[2]);
    }
    else
    {
      //php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
      PHP_WARNING("invalid parameters type");
    }
    // PHP 7.0
    #else
    zval args[3];

    if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
    {
      cache_errno = CACHE_INVALID_PARAMETERS;
      RETURN_FALSE;
    }

    if (IS_STRING != Z_TYPE(args[0]))
    {
      php_error(E_WARNING, "%s(): invalid login parameter type", get_active_function_name(TSRMLS_C));
      RETURN_FALSE;
    }

    if (IS_STRING != Z_TYPE(args[1]))
    {
      php_error(E_WARNING, "%s(): invalid password parameter type", get_active_function_name(TSRMLS_C));
      RETURN_FALSE;
    }

    username = Z_STRVAL(args[0]);
    password = Z_STRVAL(args[1]);

    if (3 == argument_count)
    {
      if(IS_LONG != Z_TYPE(args[2]))
      {
        php_error(E_WARNING, "%s(): invalid parameters type", get_active_function_name(TSRMLS_C));
        RETURN_FALSE;
      }

      tout = Z_LVAL(args[2]);
    }
    #endif
  }

  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);

  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);

  /* CACHE_PROGMODE т.к в случае ошибки соединение обрывается и не работает CacheError,
     CACHE_TTNONE чтобы не перехватывало I/O */
  errno = CacheSecureStart(&pusername,&ppassword,&pexename,CACHE_PROGMODE|CACHE_TTNONE,tout,NULL,NULL);

  if (CACHE_SUCCESS == errno)
    RETURN_TRUE;

  __on_cache_error(errno);
  RETURN_FALSE;
}

PHP_FUNCTION(cach_quit)
{
  int res = CacheEnd();

  if (CACHE_SUCCESS == res)
    RETURN_TRUE;

  __on_cache_error(errno);
  RETURN_FALSE;
}

/*
  Pushing php-arguments to InterSystems Cache
*/
static int __push_zval(zval* value)
{
  zval *new_value;
  int name_len, errno, count = 0, res = CACHE_NO_ERROR;
  char *name, *new_name;

  switch (Z_TYPE_P(value))
  {
    case IS_NULL:
      PHP_WARNING("one of params is NULL");
      return CACHE_ERROR;
      break;

    case IS_STRING:
    case IS_DOUBLE:
    case IS_LONG:
    #if ZEND_MODULE_API_NO <= 20131226
    case IS_BOOL:
    #endif
      break;

    default:
      PHP_WARNING("one of param has wrong type");
      return CACHE_ERROR;
  }

  switch (Z_TYPE_P(value)) {
    #if ZEND_MODULE_API_NO <= 20131226
      case IS_BOOL:
        if (CACHE_SUCCESS != (errno = CachePushInt(Z_LVAL_P(value)))) {
          __on_cache_error(errno);
          res = CACHE_ERROR;
        }
        break;
    #endif

    case IS_LONG:
      //php_printf("PUSH Int64 %d\n", Z_LVAL_P(value));
      if (CACHE_SUCCESS != (errno = CachePushInt64(Z_LVAL_P(value)))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;

    case IS_DOUBLE:
      //php_printf("PUSH Double %f\n", Z_DVAL_P(value));
      if (CACHE_SUCCESS != (errno = CachePushDbl(Z_DVAL_P(value)))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;

    case IS_STRING:
      //php_printf("PUSH String %s\n", Z_STRVAL_P(value));
      if (CACHE_SUCCESS != CachePushStr(Z_STRLEN_P(value),Z_STRVAL_P(value))) {
        __on_cache_error(errno);
        res = CACHE_ERROR;
      }
      break;
  }
  return res;
}

/*
  Working with all function arguments.
  RETURN: CACHE_ERROR or CACHE_NO_ERROR
*/
static int __push_pp_global(int argument_count)
{
  char fl[255];
  zval *one;
  HashTable *ht;
  char *name, *new_name;
  int name_len, errno, res = CACHE_NO_ERROR;

  // "z|" - only one required parameter, next are optional
  if (zend_parse_parameters(1, "z|", &one) == FAILURE)
  {
    return CACHE_ERROR;
  }

  if (1 > argument_count || 30 < argument_count)
  {
    PHP_WARNING("invalid numbers of parameters");
    return CACHE_ERROR;
  }

  // Untested for PHP 5. Possibly do not work.
  #if ZEND_MODULE_API_NO <= 20131226
  zval **args[30];
  if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
  {
    sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
    cache_error = &fl[0];
    cache_errno = CACHE_INVALID_PARAMETERS;
    return CACHE_ERROR;
  }

  errno = CachePushGlobal(name_len, new_name);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    return CACHE_ERROR;
  }

  int counter;
  for (counter = 1; counter < argument_count; counter++)
  {
    if (PZVAL_IS_REF(*args[counter]))
    {
      sprintf(fl, "%s(): invalid parameters", get_active_function_name(TSRMLS_C));
      cache_error = &fl[0];
      cache_errno = CACHE_INVALID_LOCAL;
      return CACHE_ERROR;
    }

    if (CACHE_ERROR == __push_zval(*args[counter]))
      return CACHE_ERROR;
  }

  //  ----------- PHP 7.x ------------------------------
  #else
  zval args[30];
  if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
  {
    PHP_WARNING("invalid parameters");
    return CACHE_ERROR;
  }

  int counter = 0;
  if (IS_STRING == Z_TYPE(args[0]))
  {
    name = Z_STRVAL(args[0]);
    if (name[0] != '^')
    {
      PHP_WARNING("the character \"^\" was not specified");
      return CACHE_ERROR;
    }
    // rewind pointer, skip ^
    new_name = name + 1;
    name_len = strlen(new_name);

    // Select Global for operation
    if (CACHE_SUCCESS != CachePushGlobal(name_len, new_name))
    {
      __on_cache_error(errno);
      return CACHE_ERROR;
    }
  }
  // If first argument is a array.
  else if (IS_ARRAY == Z_TYPE(args[0]))
  {
    ht = Z_ARRVAL(args[0]);

    if (zend_hash_num_elements(ht) == 0)
    {
      PHP_WARNING("first array argument cannot be empty");
      return CACHE_ERROR;
    }

    int i =0;

    ZEND_HASH_FOREACH_VAL(ht, one)
    {
      // First element in array is global name.
      if (i==0)
      {
        if (IS_STRING != Z_TYPE_P(one))
        {
          PHP_WARNING("first element in global array must be string");
          return CACHE_ERROR;
        }

        name = Z_STRVAL_P(one);
        if (name[0] != '^')
        {
          PHP_WARNING("the character \"^\" was not specified in first global array element");
          return CACHE_ERROR;
        }

        // rewind pointer, skip ^
        new_name = name + 1;
        name_len = strlen(new_name);

        // Select Global for operation
        if (CACHE_SUCCESS != CachePushGlobal(name_len, new_name))
        {
          __on_cache_error(errno);
          return CACHE_ERROR;
        }
      }
      // Working with subscripts in array
      else
      {
        switch (Z_TYPE_P(one))
        {
          case IS_STRING:
          case IS_DOUBLE:
          case IS_LONG:
            break;
          default:
            php_error(E_WARNING, "%s(): %d element in global array has wrong type %d", get_active_function_name(TSRMLS_C), i, Z_TYPE_P(one));
            return CACHE_ERROR;
        }

        // FIXME only 1 type error
        if (CACHE_ERROR == __push_zval(one))
          return CACHE_ERROR;
      }
      i++;
    } ZEND_HASH_FOREACH_END();
  }
  #endif

  counter++;

  for (;counter < argument_count; counter++)
  {
    if (CACHE_ERROR == __push_zval(&args[counter]))
      return CACHE_ERROR;
  }

  return CACHE_NO_ERROR;
}

/*
  RETURN zval(LONG, STRING, DOUBLE) or NULL (on error)
*/
static zval __pop_cache()
{
  zval res;
  int iTemp, nType;
  double dTemp;
  Callin_char_t *sPTemp;

  // Undocumented feature. We known type argument in stack with function CacheType designed for CacheEval[xxx]
  nType = CacheType();

  switch (nType)
  {
    // I don't see this type in stack
    case CACHE_INT:
      errno = CachePopInt(&iTemp);
      if(CACHE_SUCCESS != errno)
      {
        __on_cache_error(errno);
        ZVAL_NULL(&res);
        break;
      }

      ZVAL_LONG(&res, iTemp);
      break;

    case CACHE_IEEE_DBL:
      /* fall-through */
      //Function "CachePopIEEEDbl" is missing

    // CACHE_DOUBLE == 2 (REALLY USED)
    case CACHE_DOUBLE:
      errno = CachePopDbl(&dTemp);
      if(CACHE_SUCCESS != errno)
      {
        __on_cache_error(errno);
        ZVAL_NULL(&res);
        break;
      }

      ZVAL_DOUBLE(&res, dTemp);
      break;

    // I don't see this type in stack
    case CACHE_ASTRING:
    /* fall-through */
    // In most cases - 15 (REALLY USED). Only return type for CacheGlobalOrder.
    case CACHE_WSTRING:
      errno = CachePopStr(&iTemp, &sPTemp);
      if(CACHE_SUCCESS != errno)
      {
        __on_cache_error(errno);
        ZVAL_NULL(&res);

        break;
      }
      sPTemp[iTemp] = '\0';
      ZVAL_STRINGL(&res, sPTemp, iTemp);
      break;

    default:
      PHP_WARNING("Unsupported data type %d in Cache stack");
      php_printf("Unsupported type %d\n", nType);
      ZVAL_NULL(&res);
      break;
  }
  return res;
}

/*
  For processing variable count of arguments.
  Return count of all argument including arguments in first argument array (if exist).
*/
static int __received_parameters_count(int argc)
{
  zval args[30], *new_value;

  // This error processing in push_global
  if(zend_get_parameters_array_ex(argc, args) != SUCCESS)
  {
    return 0;
  }
  
  if (IS_ARRAY == Z_TYPE(args[0]))
  {
    argc--;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(args[0]), new_value)
    {
      argc++;
    } ZEND_HASH_FOREACH_END();
  }

  return argc;
}

PHP_FUNCTION(cach_set)
{
  int errno,
      res = CACHE_NO_ERROR,
      argc = ZEND_NUM_ARGS();

  if (argc < 2)
  {
    php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }

  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  // CacheGlobalSet has argument: Count of substript in stack
  errno = CacheGlobalSet(__received_parameters_count(argc)-2);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(cach_get)
{
  int errno, flag = 0, res, argc = ZEND_NUM_ARGS();

  if (CACHE_ERROR  == __push_pp_global(argc))
    RETURN_FALSE;
  
  errno = CacheGlobalGet(__received_parameters_count(argc)-1,flag);
  if (CACHE_SUCCESS != errno)
  {
    // Node value is undefined
    if (errno == CACHE_ERUNDEF)
    {
      RETURN_NULL();
    }

    __on_cache_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_cache();

  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;
  
  RETURN_ZVAL(&ret,1,1);
}

PHP_FUNCTION(cach_zkill)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = CacheGlobalKill(__received_parameters_count(argc)-1,1);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(cach_kill)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;
  
  errno = CacheGlobalKill(__received_parameters_count(argc)-1,0);
  if(CACHE_SUCCESS != errno) 
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(cach_order)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = CacheGlobalOrder(__received_parameters_count(argc)-1,1,0);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_cache();
  
  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;
 
  if (
    (IS_STRING == Z_TYPE(ret)) &&
    (0 == Z_STRLEN(ret)) 
  )
    RETURN_NULL();

  RETURN_ZVAL(&ret,1,1);
}

PHP_FUNCTION(cach_order_rev)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = CacheGlobalOrder(__received_parameters_count(argc)-1,-1,0);
  if (CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_cache();
  
  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;

  if (
    (IS_STRING == Z_TYPE(ret)) && (0 == Z_STRLEN(ret)) 
  )
    RETURN_NULL();
  
  RETURN_ZVAL(&ret,1,1);
}

static int __parse_query_res(zval* a, char* s, size_t len);

PHP_FUNCTION(cach_query)
{
  int count = 0, errno, argc = ZEND_NUM_ARGS();
  zval args[30], *new_value;

  // Push arguments to Cache stack.
  if (CACHE_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

/*
  if (CACHE_SUCCESS != CachePushStr(0,""))
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }
*/

  // Loading arguments in args array
  if(zend_get_parameters_array_ex(argc, args) != SUCCESS)
  {
    PHP_WARNING("invalid parameters");
    RETURN_FALSE;
  }

  // Real arguments count (in array arguments also counting)
  count = __received_parameters_count(argc);

  // Elements in stack = count - 1
  errno = CacheGlobalQuery(count-1, 1, 0);

  if(CACHE_SUCCESS != errno)
  {
    __on_cache_error(errno);
    RETURN_FALSE;
  }

  zval res, ret;
  res = __pop_cache();

  if (IS_STRING != Z_TYPE(res))
    RETURN_FALSE;

  // We reached full traverse?
  if (0 == Z_STRLEN(res))
    RETURN_NULL();

  array_init(&ret);

  // Parsing string
  __parse_query_res(&ret, Z_STRVAL(res), Z_STRLEN(res));

  RETURN_ZVAL(&ret,1,1);
}

PHP_FUNCTION(cach_exec)
{
  char *command_str;
  int res = CACHE_NO_ERROR;
  strsize_t command_len;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command_str, &command_len) == FAILURE)
  { 
    RETURN_FALSE;
  }

  CACHE_STR command;
  strcpy((char *) command.str,command_str);
  command.len = command_len;
  
  errno = CacheExecute(&command);
  if (CACHE_SUCCESS != errno)
  {
      __on_cache_error(errno);
      RETURN_FALSE;
  }
  
  RETURN_TRUE;
}

PHP_FUNCTION(cach_errno)
{
  RETURN_LONG(cache_errno);
}

PHP_FUNCTION(cach_error)
{
  _RETURN_STRING(cache_error);
}

static void __word(zval* a, char* s, size_t len)
{
  add_next_index_stringl(a, s, len);
}

static void __digit(zval* a, char* s, int len)
{
  zval temp;

  ZVAL_STRINGL(&temp, s, len);
  // No "." or "Е" - it's LONG
  if (
   (strchr(s, '.')==NULL) && (strchr(s, 'E')==NULL)
  )
  {
    convert_to_long(&temp);
  }
  else
  {
    convert_to_double(&temp);
  }
  add_next_index_zval(a, &temp);
}

// Parsing '^a("dfsdf",44,"value")'
static int __parse_query_res(zval* a, char* s, size_t len)
{
  size_t i=0, bS = 0, pos = 0;
  char buf[len+1];
  char* p;

  // Search global name.
  p = strchr(s, '(');
  if (p == NULL)
  {
    __word(a, s, len);
    return CACHE_NO_ERROR;
  }

  // add global
  __word(a, s, p-s);

  // len - begin^( -end)
  len = len-(p-s+1)-1;
  s = p+1;

  new_etap:

  pos = 0;
  bS = 0;

  while (i < len)
  {
    // Number pasing
    if (bS==0)
    {
      if (s[i] == '"')
      {
        bS=1;
        i++;
        continue;
      }

      buf[pos] = s[i];
      pos++;

      if ((i + 1) == len)
      {
        __digit(a, buf, pos);
        break;
      }
  
      if (s[i+1] == ',')
      {
        __digit(a, buf, pos);
        i = i + 2;
        goto new_etap;
      }
      i++;
      continue;
    }
    // Word parsing
    else
    {
      if (s[i] == '"')
      {
        if ((i + 1) == len)
        {
          __word(a, buf, pos);
          break;
        }

        if (s[i+1] == ',')
        {
          __word(a, buf, pos);

          i = i + 2;
          goto new_etap;
        }

        if (s[i+1]=='"')
        {
          i++;
        }
      }
      buf[pos] = s[i];
      pos++;
      i++;
    }
  }
  return CACHE_NO_ERROR;
}
