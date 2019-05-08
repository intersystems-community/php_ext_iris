/*
  PHP-extension "iris" for integration with InterSystems Iris.
  
  Commercial support, adding new features and tuning also available.
  Sergey Kamenev cacher13@inetstar.ru
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_iris.h"

#include "iris-callin.h"
#include "stdlib.h"

const zend_function_entry iris_functions[] = {
  PHP_FE(iris_set_dir, NULL)
  PHP_FE(iris_connect, NULL)
  PHP_FE(iris_quit, NULL)
  PHP_FE(iris_set, NULL)
  PHP_FE(iris_get,NULL)
  PHP_FE(iris_kill, NULL)
  PHP_FE(iris_zkill,NULL)
  PHP_FE(iris_order, NULL)
  PHP_FE(iris_order_rev, NULL)
  PHP_FE(iris_query, NULL)
  PHP_FE(iris_exec, NULL)
  PHP_FE(iris_errno, NULL)
  PHP_FE(iris_error, NULL)
  PHP_FE_END
};

zend_module_entry iris_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_IRIS_EXT_NAME,
  iris_functions,
  PHP_MINIT(iris),
  PHP_MSHUTDOWN(iris),
  NULL,
  PHP_RSHUTDOWN(iris),
  NULL,
#if ZEND_MODULE_API_NO >= 20010901
  PHP_IRIS_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(iris)

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

int nIrisErrno = 0, pth = 0;
char *sIrisError, *iris_pth = "No error";

PHP_INI_BEGIN()
PHP_INI_ENTRY("iris.shdir", "/usr/irissys/mgr", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("iris.login", "Admin", PHP_INI_ALL, NULL)
PHP_INI_ENTRY("iris.password", "1234", PHP_INI_ALL, NULL)
PHP_INI_END()

PHP_MINIT_FUNCTION(iris)
{
  REGISTER_INI_ENTRIES();
}

PHP_RSHUTDOWN_FUNCTION(iris)
{
  IrisEnd();
}

PHP_MSHUTDOWN_FUNCTION(iris)
{
  IrisEndAll();
  UNREGISTER_INI_ENTRIES();
}

#define PHP_WARNING(s) \
php_error(E_WARNING, "%s(): "s, get_active_function_name(TSRMLS_C));

#define PHP_ERROR(s) \
hp_error(E_ERROR, "%s(): "s, get_active_function_name(TSRMLS_C));

// Processing error in InterSystem Iris
static void __on_iris_error(int error)
{
  nIrisErrno = error; //errno

  char* fl;
  IRIS_ASTR errmsg;
  IRIS_ASTR srcline;

  int offset, res;

  errmsg.len = 50;
  srcline.len = 100;

  res = IrisErrorA(&errmsg, &srcline, &offset);

  if (IRIS_SUCCESS == res)
  {
    errmsg.str[errmsg.len] = '\0';
    sIrisError = errmsg.str;

    return;
  }

  switch (res)
  {
    case IRIS_CONBROKEN: 
      fl = "Failed get error: Connection has been broken";
      break;

    case IRIS_NOCON: 
      fl = "Failed get error: No connection has been established";
      break;

    case IRIS_RETTOOSMALL: 
      fl = "Failed get error: The length of the return value for either errmsg or errsrc was not of the valid size";
      break;

    default:
      fl = "Failed get error: Unknown error in IrisErrorA";
  }

  sIrisError = fl;
  return;
}

PHP_FUNCTION(iris_set_dir)
{
  strsize_t cparams;
  char *path;

  int argument_count = ZEND_NUM_ARGS();

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &cparams) == FAILURE)
  {
    RETURN_FALSE;
  }

  if (cparams>0)
  {
    path[cparams] = '\0';
  }

  iris_pth = path;
  pth = 1;
  
  RETURN_TRUE;
}

PHP_FUNCTION(iris_connect)
{
  strsize_t argument_count, username_len, password_len;

  int errno, res = IRIS_NO_ERROR, timeout = 0;

  char *username, *password;
  IRIS_ASTR pusername, ppassword, pexename;

  if (0 == pth)
  {
    iris_pth = zend_ini_string("iris.shdir", strlen("iris.shdir"), 0);
  }
  
  errno = IrisSetDir(iris_pth);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  argument_count = ZEND_NUM_ARGS();

  if ((argument_count == 1) || (argument_count > 3))
  {
    php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }

  strcpy((char *) pexename.str, "php");
  pexename.len = (unsigned short)strlen((char *) pexename.str);

  if (0 == argument_count)
  {
    username = zend_ini_string("iris.login", strlen("iris.login"), 0);
    password = zend_ini_string("iris.password", strlen("iris.password"), 0);
  }
  else if (2 == argument_count || 3 == argument_count)
  {
    #if ZEND_MODULE_API_NO <= 20131226
    zval **args[3];
    if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
    {
      PHP_WARNING("Invalid parameter");
      RETURN_FALSE;
    }
 
    if (PZVAL_IS_REF(*args[2]))
    {
      PHP_WARNING("Invalid parameter");
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
      PHP_WARNING("Invalid parameter");
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

      timeout = Z_LVAL(args[2]);
    }
    #endif
  }

  strcpy((char *) pusername.str, username);
  pusername.len = strlen(pusername.str);

  strcpy((char *) ppassword.str, password);
  ppassword.len = strlen(ppassword.str);

  /* IRIS_PROGMODE т.к в случае ошибки соединение обрывается и не работает CacheError,
     IRIS_TTNONE чтобы не перехватывало I/O */
  errno = IrisSecureStartA(&pusername, &ppassword, &pexename, IRIS_PROGMODE|IRIS_TTNONE, timeout, NULL, NULL);

  if (IRIS_SUCCESS == errno)
    RETURN_TRUE;

  nIrisErrno = errno;

  char* sMes;

  switch (errno)
  {
    case IRIS_ACCESSDENIED:
      sMes = "Authentication has failed. Check the audit log for the real authentication error.";
      break;
    case IRIS_ALREADYCON:
      sMes = "Connection already existed. Returned if you call IrisSecureStartH from a $ZF function.";
      break;
    case IRIS_CHANGEPASSWORD:
      sMes = "Password change required. This return value is only returned if you are using InterSystems authentication.";
      break;
    case IRIS_CONBROKEN:
      sMes = "Connection was formed and then broken, and IrisEnd has not been called to clean up.";
      break;
    case IRIS_STRTOOLONG:
      sMes = "prinp or prout is too long.";
      break;
    case IRIS_FAILURE:
    default:
      sMes = "An unexpected error has occurred.";
  }

  sIrisError = sMes;

  RETURN_FALSE;
}

PHP_FUNCTION(iris_quit)
{
  int res = IrisEnd();

  if (IRIS_SUCCESS == res)
    RETURN_TRUE;

  __on_iris_error(res);
  RETURN_FALSE;
}

/*
  Pushing php-arguments to InterSystems Iris
*/
static int __push_zval(zval* value)
{
  zval *new_value;
  int name_len, errno, count = 0, res = IRIS_NO_ERROR;
  char *name, *new_name;

  switch (Z_TYPE_P(value))
  {
    case IS_NULL:
      PHP_WARNING("one of params is NULL");
      return IRIS_ERROR;
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
      return IRIS_ERROR;
  }

  switch (Z_TYPE_P(value)) {
    #if ZEND_MODULE_API_NO <= 20131226
      case IS_BOOL:
        errno = IrisPushInt(Z_LVAL_P(value));
        if (IRIS_SUCCESS != errno)
        {
          __on_iris_error(errno);
          res = IRIS_ERROR;
        }
        break;
    #endif

    case IS_LONG:
      //php_printf("PUSH Int64 %d\n", Z_LVAL_P(value));
      if (IRIS_SUCCESS != (errno = IrisPushInt64(Z_LVAL_P(value)))) {
        __on_iris_error(errno);
        res = IRIS_ERROR;
      }
      break;

    case IS_DOUBLE:
      //php_printf("PUSH Double %f\n", Z_DVAL_P(value));
      if (IRIS_SUCCESS != (errno = IrisPushDbl(Z_DVAL_P(value)))) {
        __on_iris_error(errno);
        res = IRIS_ERROR;
      }
      break;

    case IS_STRING:
      //php_printf("PUSH String %s\n", Z_STRVAL_P(value));
      if (IRIS_SUCCESS != IrisPushStr(Z_STRLEN_P(value),Z_STRVAL_P(value))) {
        __on_iris_error(errno);
        res = IRIS_ERROR;
      }
      break;
  }
  return res;
}

/*
  Working with all function arguments.
  RETURN: IRIS_ERROR or IRIS_NO_ERROR
*/
static int __push_pp_global(int argument_count)
{
  char fl[255];
  zval *one;
  HashTable *ht;
  char *name, *new_name;
  int name_len, errno, res = IRIS_NO_ERROR;

  // "z|" - only one required parameter, next are optional
  if (zend_parse_parameters(1, "z|", &one) == FAILURE)
  {
    return IRIS_ERROR;
  }

  if (1 > argument_count || 30 < argument_count)
  {
    PHP_WARNING("invalid numbers of parameters");
    return IRIS_ERROR;
  }

  // Untested for PHP 5. Possibly do not work.
  #if ZEND_MODULE_API_NO <= 20131226
  zval **args[30];
  if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
  {
    PHP_WARNING("invalid parameters");
    return IRIS_ERROR;
  }

  errno = IrisPushGlobal(name_len, new_name);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    return IRIS_ERROR;
  }

  int counter;
  for (counter = 1; counter < argument_count; counter++)
  {
    if (PZVAL_IS_REF(*args[counter]))
    {
      PHP_WARNING("invalid parameters");
      return IRIS_ERROR;
    }

    if (IRIS_ERROR == __push_zval(*args[counter]))
      return IRIS_ERROR;
  }

  //  ----------- PHP 7.x ------------------------------
  #else
  zval args[30];
  if(zend_get_parameters_array_ex(argument_count, args) != SUCCESS)
  {
    PHP_WARNING("invalid parameters");
    return IRIS_ERROR;
  }

  int counter = 0;
  if (IS_STRING == Z_TYPE(args[0]))
  {
    name = Z_STRVAL(args[0]);
    if (name[0] != '^')
    {
      PHP_WARNING("the character \"^\" was not specified");
      return IRIS_ERROR;
    }
    // rewind pointer, skip ^
    new_name = name + 1;
    name_len = strlen(new_name);

    // Select Global for operation
    errno = IrisPushGlobal(name_len, new_name);
    if (IRIS_SUCCESS != errno)
    {
      __on_iris_error(errno);
      return IRIS_ERROR;
    }
  }
  // If first argument is a array.
  else if (IS_ARRAY == Z_TYPE(args[0]))
  {
    ht = Z_ARRVAL(args[0]);

    if (zend_hash_num_elements(ht) == 0)
    {
      PHP_WARNING("first array argument cannot be empty");
      return IRIS_ERROR;
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
          return IRIS_ERROR;
        }

        name = Z_STRVAL_P(one);
        if (name[0] != '^')
        {
          PHP_WARNING("the character \"^\" was not specified in first global array element");
          return IRIS_ERROR;
        }

        // rewind pointer, skip ^
        new_name = name + 1;
        name_len = strlen(new_name);

        // Select Global for operation
        if (IRIS_SUCCESS != IrisPushGlobal(name_len, new_name))
        {
          __on_iris_error(errno);
          return IRIS_ERROR;
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
            return IRIS_ERROR;
        }

        // FIXME only 1 type error
        if (IRIS_ERROR == __push_zval(one))
          return IRIS_ERROR;
      }
      i++;
    } ZEND_HASH_FOREACH_END();
  }
  #endif

  counter++;

  for (;counter < argument_count; counter++)
  {
    if (IRIS_ERROR == __push_zval(&args[counter]))
      return IRIS_ERROR;
  }

  return IRIS_NO_ERROR;
}

/*
  RETURN zval(LONG, STRING, DOUBLE) or NULL (on error)
*/
static zval __pop_iris()
{
  zval res;
  int iTemp, nType;
  double dTemp;
  Callin_char_t *sPTemp;

  // Undocumented feature. We known type argument in stack with function IrisType designed for IrisEval[xxx]
  nType = IrisType();

  switch (nType)
  {
    // I don't see this type in stack
    case IRIS_INT:
      errno = IrisPopInt(&iTemp);
      if(IRIS_SUCCESS != errno)
      {
        __on_iris_error(errno);
        ZVAL_NULL(&res);
        break;
      }

      ZVAL_LONG(&res, iTemp);
      break;

    case IRIS_IEEE_DBL:
      /* fall-through */
      //Function "IrisPopIEEEDbl" is missing

    // IRIS_DOUBLE == 2 (REALLY USED)
    case IRIS_DOUBLE:
      errno = IrisPopDbl(&dTemp);
      if(IRIS_SUCCESS != errno)
      {
        __on_iris_error(errno);
        ZVAL_NULL(&res);
        break;
      }

      ZVAL_DOUBLE(&res, dTemp);
      break;

    // I don't see this type in stack
    case IRIS_ASTRING:
    /* fall-through */
    // In most cases - 15 (REALLY USED). Only return type for IrisGlobalOrder.
    case IRIS_WSTRING:
      errno = IrisPopStr(&iTemp, &sPTemp);
      if(IRIS_SUCCESS != errno)
      {
        __on_iris_error(errno);
        ZVAL_NULL(&res);

        break;
      }
      sPTemp[iTemp] = '\0';
      ZVAL_STRINGL(&res, sPTemp, iTemp);
      break;

    default:
      PHP_WARNING("Unsupported data type %d in Iris stack");
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

PHP_FUNCTION(iris_set)
{
  int errno,
      res = IRIS_NO_ERROR,
      argc = ZEND_NUM_ARGS();

  if (argc < 2)
  {
    php_error(E_WARNING, "%s(): invalid number of parameters", get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }

  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  // IrisGlobalSet has argument: Count of substript in stack
  errno = IrisGlobalSet(__received_parameters_count(argc)-2);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(iris_get)
{
  int errno, flag = 0, res, argc = ZEND_NUM_ARGS();

  if (IRIS_ERROR  == __push_pp_global(argc))
    RETURN_FALSE;
  
  errno = IrisGlobalGet(__received_parameters_count(argc)-1,flag);
  if (IRIS_SUCCESS != errno)
  {
    // Node value is undefined
    if (errno == IRIS_ERUNDEF)
    {
      RETURN_NULL();
    }

    __on_iris_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_iris();

  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;
  
  RETURN_ZVAL(&ret,1,1);
}

PHP_FUNCTION(iris_zkill)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = IrisGlobalKill(__received_parameters_count(argc)-1,1);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(iris_kill)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;
  
  errno = IrisGlobalKill(__received_parameters_count(argc)-1,0);
  if(IRIS_SUCCESS != errno) 
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  RETURN_TRUE;
}

PHP_FUNCTION(iris_order)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = IrisGlobalOrder(__received_parameters_count(argc)-1,1,0);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_iris();
  
  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;
 
  if (
    (IS_STRING == Z_TYPE(ret)) &&
    (0 == Z_STRLEN(ret)) 
  )
    RETURN_NULL();

  RETURN_ZVAL(&ret,1,1);
}

PHP_FUNCTION(iris_order_rev)
{
  int errno, res, argc = ZEND_NUM_ARGS();

  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

  errno = IrisGlobalOrder(__received_parameters_count(argc)-1,-1,0);
  if (IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  zval ret = __pop_iris();
  
  if (IS_NULL == Z_TYPE(ret))
    RETURN_FALSE;

  if (
    (IS_STRING == Z_TYPE(ret)) && (0 == Z_STRLEN(ret)) 
  )
    RETURN_NULL();
  
  RETURN_ZVAL(&ret,1,1);
}

static int __parse_query_res(zval* a, char* s, size_t len);

PHP_FUNCTION(iris_query)
{
  int count = 0, errno, argc = ZEND_NUM_ARGS();
  zval args[30], *new_value;

  // Push arguments to Iris stack.
  if (IRIS_ERROR == __push_pp_global(argc))
    RETURN_FALSE;

/*
  if (IRIS_SUCCESS != IrisPushStr(0,""))
  {
    __on_iris_error(errno);
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
  errno = IrisGlobalQuery(count-1, 1, 0);

  if(IRIS_SUCCESS != errno)
  {
    __on_iris_error(errno);
    RETURN_FALSE;
  }

  zval res, ret;
  res = __pop_iris();

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

PHP_FUNCTION(iris_exec)
{
  char *command_str;
  int res = IRIS_NO_ERROR;
  strsize_t command_len;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command_str, &command_len) == FAILURE)
  { 
    RETURN_FALSE;
  }

  IRIS_STR command;
  strcpy((char *) command.str,command_str);
  command.len = command_len;
  
  errno = IrisExecute(&command);
  if (IRIS_SUCCESS != errno)
  {
      __on_iris_error(errno);
      RETURN_FALSE;
  }
  
  RETURN_TRUE;
}

PHP_FUNCTION(iris_errno)
{
  RETURN_LONG(nIrisErrno);
}

PHP_FUNCTION(iris_error)
{
  _RETURN_STRING(sIrisError);
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
    return IRIS_NO_ERROR;
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
  return IRIS_NO_ERROR;
}
