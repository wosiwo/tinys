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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tinys.h"

zend_class_entry *tinys_client_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinys_client__construct, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(tinys_client,__construct)
{
	createSocket();
}
//建立连接
PHP_METHOD(tinys_client, connect)
{
	char * ip;	//ip地址
	size_t ip_len;
	char *func_name = NULL;
	long port_in;
	int port;
	if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sl",&ip,  &ip_len, &port_in) == FAILURE)
	{
		return;
	}
	port = (int)port_in;
	cli_connect(ip,port);
}
//发送数据
PHP_METHOD(tinys_client, send)
{
	zval *zobject = getThis();
	int ret;
	zval *zdata;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zdata) == FAILURE)
	{
		return;
	}
	char *data;
	convert_to_string(zdata);
	int length = Z_STRLEN_P(zdata);
	data = Z_STRVAL_P(zdata);
	printf("break point 1\n");
	if (length < 0)
	{
		RETURN_FALSE;
	}
	else if (length == 0)
	{
		printf("data is empty.");
		RETURN_FALSE;
	}
	printf("break point 2\n");

	printf("break point 3\n");
	//发送数据给服务端
	ret = cli_send(data, length);
	printf("break point 4\n");
	if (ret < 0)
	{
		printf("sendto to reactor failed. Error: %s [%d]", strerror(errno), errno);
	}
	SW_CHECK_RETURN(ret);
}
//发送数据方法
PHP_METHOD(tinys_client, recv)
{
	char *ret;
	ret=cli_recv();
	SW_CHECK_RETURN(ret);
}
//然后，定义一个zend_function_entry
zend_function_entry tinys_client_method[]=
{
	ZEND_ME(tinys_client,    __construct,  NULL,   ZEND_ACC_PUBLIC)
	ZEND_ME(tinys_client,    connect,  NULL,   ZEND_ACC_PUBLIC)
	ZEND_ME(tinys_client,    send,  NULL,   ZEND_ACC_PUBLIC)
	ZEND_ME(tinys_client,    recv,  NULL,   ZEND_ACC_PUBLIC)
	//ZEND_ME(tinys_client,    close,  NULL,   ZEND_ACC_PUBLIC)
	{NULL,  NULL,   NULL}
};

void tinys_client_init()
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"tinys_client",tinys_client_method);
	tinys_client_ce=zend_register_internal_class(&ce TSRMLS_CC);
}
