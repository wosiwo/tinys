/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TINYS_H
#define PHP_TINYS_H

extern zend_module_entry tinys_module_entry;
#define phpext_tinys_ptr &tinys_module_entry

#define PHP_TINYS_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_TINYS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_TINYS_API __attribute__ ((visibility("default")))
#else
#	define PHP_TINYS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_SERVER_CALLBACK_NUM 10
#define SW_PHP_MAX_PARAMS_NUM     20

#define SW_WORKER_WAIT_TIMEOUT     1000

enum swEvent_type
{
    SW_EVENT_DEAULT = 256,
    SW_EVENT_READ = 1u << 9,
    SW_EVENT_WRITE = 1u << 10,
    SW_EVENT_ERROR = 1u << 11,
    SW_EVENT_ONCE = 1u << 12,
};
#define SW_OK                  0
#define SW_ERR                -1
#define SW_AGAIN              -2
#define SW_BUSY               -3
#define SW_DONE               -4
#define SW_DECLINED           -5
#define SW_ABORT              -6

//#define SW_USE_PHP        1
#define SW_CHECK_RETURN(s)         if(s<0){RETURN_FALSE;}else{RETURN_TRUE;}return
#define SW_LOCK_CHECK_RETURN(s)    if(s==0){RETURN_TRUE;}else{RETURN_FALSE;}return

zval *php_sw_server_callbacks[PHP_SERVER_CALLBACK_NUM];
/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(tinys)
	zend_long  global_value;
	char *global_string;
	zval *php_sw_server_callbacks[PHP_SERVER_CALLBACK_NUM];

ZEND_END_MODULE_GLOBALS(tinys)
*/

/* Always refer to the globals in your function as TINYS_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define TINYS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(tinys, v)

#if defined(ZTS) && defined(COMPILE_DL_TINYS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_TINYS_H */

//php_alinq.h
#ifndef alinq_H
#define alinq_H

//加载config.h，如果配置了的话
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//加载php头文件
#include "php.h"
#define phpext_alinq_ptr &alinq_module_entry
extern zend_module_entry alinq_module_entry;

#endif

// #include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_list.h"
#include "Zend/zend_interfaces.h"

//#include "php7_wrapper.h"
#include <sys/poll.h>

#define SW_MAKE_STD_ZVAL(p)             zval _stack_zval_##p; p = &(_stack_zval_##p)
#define SW_ZVAL_STRINGL(z, s, l, dup)         ZVAL_STRINGL(z, s, l)
#define SW_ZVAL_STRING(z,s,dup)               ZVAL_STRING(z,s)


//调用用户空间函数
int walu_call_user_function(zval** retval, zval* obj, char* function_name, char* paras, ...);

//server主函数
int server(char* ip,int port);

static  zval* sw_zend_read_property(zend_class_entry *class_ptr, zval *obj, char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
