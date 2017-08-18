/*
 +----------------------------------------------------------------------+
 | Swoole                                                               |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | license@swoole.com so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
 +----------------------------------------------------------------------+
 */

#ifndef EXT_SWOOLE_PHP7_WRAPPER_H_
#define EXT_SWOOLE_PHP7_WRAPPER_H_

#include "ext/standard/php_http.h"

#if PHP_MAJOR_VERSION < 7
typedef zend_rsrc_list_entry zend_resource;
#define SW_RETURN_STRING                      RETURN_STRING
#define SW_Z_ARRVAL_P                         Z_ARRVAL_P
#define IS_TRUE                               1

static inline int sw_zend_hash_find(HashTable *ht, char *k, int len, void **v)
{
    zval **tmp = NULL;
    if (zend_hash_find(ht, k, len, (void **) &tmp) == SUCCESS)
    {
        *v = *tmp;
        return SUCCESS;
    }
    else
    {
        *v = NULL;
        return FAILURE;
    }
}

#define sw_zend_hash_del                      zend_hash_del
#define sw_zend_hash_update                   zend_hash_update
#define sw_zend_hash_index_find               zend_hash_index_find
#define SW_ZVAL_STRINGL                       ZVAL_STRINGL
#define SW_ZEND_FETCH_RESOURCE_NO_RETURN      ZEND_FETCH_RESOURCE_NO_RETURN
#define SW_ZEND_FETCH_RESOURCE                ZEND_FETCH_RESOURCE
#define SW_ZEND_REGISTER_RESOURCE             ZEND_REGISTER_RESOURCE
#define SW_MAKE_STD_ZVAL(p)                   MAKE_STD_ZVAL(p)
#define SW_ALLOC_INIT_ZVAL(p)                 ALLOC_INIT_ZVAL(p)
#define SW_ZVAL_STRING                        ZVAL_STRING
#define SW_RETVAL_STRINGL                     RETVAL_STRINGL
#define sw_smart_str                          smart_str
#define sw_php_var_unserialize                php_var_unserialize
#define sw_zend_is_callable                   zend_is_callable
#define sw_zend_is_callable_ex                zend_is_callable_ex
#define sw_zend_hash_add                      zend_hash_add
#define sw_zend_hash_index_update             zend_hash_index_update
#define sw_call_user_function_ex              call_user_function_ex
#define sw_copy_to_stack(a, b)
#define SWOOLE_GET_TSRMLS                     TSRMLS_FETCH_FROM_CTX(sw_thread_ctx ? sw_thread_ctx : NULL)

//----------------------------------Array API------------------------------------
#define sw_add_assoc_string                   add_assoc_string
#define sw_add_assoc_stringl_ex               add_assoc_stringl_ex
#define sw_add_assoc_stringl                  add_assoc_stringl
#define sw_add_assoc_double_ex                add_assoc_double_ex
#define sw_add_assoc_long_ex                  add_assoc_long_ex
#define sw_add_next_index_stringl             add_next_index_stringl

#define sw_zval_ptr_dtor                      zval_ptr_dtor
#define sw_zend_hash_copy                     zend_hash_copy
#define sw_zval_add_ref                       zval_add_ref
#define sw_zval_dup(val)                      (val)
#define sw_zval_free(val)                     (sw_zval_ptr_dtor(&val))
#define sw_zend_hash_exists                   zend_hash_exists
#define sw_php_format_date                    php_format_date
#define sw_php_url_encode                     php_url_encode
#define sw_php_array_merge(dest,src)          php_array_merge(dest,src,1 TSRMLS_CC)
#define SW_RETURN_STRINGL                     RETURN_STRINGL
#define SW_RETVAL_STRING                      RETVAL_STRING
#define sw_zend_register_internal_class_ex    zend_register_internal_class_ex

#define sw_zend_call_method_with_0_params     zend_call_method_with_0_params
#define sw_zend_call_method_with_1_params     zend_call_method_with_1_params
#define sw_zend_call_method_with_2_params     zend_call_method_with_2_params

typedef int zend_size_t;

#define SW_HASHTABLE_FOREACH_START(ht, entry)\
    zval **tmp = NULL;\
    for (zend_hash_internal_pointer_reset(ht);\
        zend_hash_has_more_elements(ht) == SUCCESS; \
        zend_hash_move_forward(ht)) {\
        if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
            continue;\
        }\
        entry = *tmp;

#if defined(HASH_KEY_NON_EXISTANT) && !defined(HASH_KEY_NON_EXISTENT)
#define HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT
#endif

#define SW_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, entry)\
    zval **tmp = NULL; ulong_t idx;\
    for (zend_hash_internal_pointer_reset(ht); \
            (ktype = zend_hash_get_current_key_ex(ht, &k, &klen, &idx, 0, NULL)) != HASH_KEY_NON_EXISTENT; \
            zend_hash_move_forward(ht)\
        ) { \
    if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
        continue;\
    }\
    entry = *tmp;\
    klen --;

#define SW_HASHTABLE_FOREACH_END() }
#define sw_zend_read_property                  zend_read_property
#define sw_zend_hash_get_current_key(a,b,c,d)  zend_hash_get_current_key_ex(a,b,c,d,0,NULL)

static inline int SW_Z_TYPE_P(zval *z)
{
    if (Z_TYPE_P(z) == IS_BOOL)
    {
        if ((uint8_t) Z_BVAL_P(z) == 1)
        {
            return IS_TRUE;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return Z_TYPE_P(z);
    }
}

#define sw_php_var_serialize(a,b,c)         php_var_serialize(a,&b,c)
#define sw_zend_get_executed_filename()     zend_get_executed_filename(TSRMLS_C)
#define IS_TRUE    1
inline int SW_Z_TYPE_P(zval *z);
#define SW_Z_TYPE_PP(z)        SW_Z_TYPE_P(*z)
static sw_inline char* sw_http_build_query(zval *data, zend_size_t *length, smart_str *formstr TSRMLS_DC)
{
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3
    if (php_url_encode_hash_ex(HASH_OF(data), formstr, NULL, 0, NULL, 0, NULL, 0, NULL, NULL TSRMLS_CC) == FAILURE)
#else
    if (php_url_encode_hash_ex(HASH_OF(data), formstr, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, (int) PHP_QUERY_RFC1738 TSRMLS_CC) == FAILURE)
#endif
    {
        if (formstr->c)
        {
            smart_str_free(formstr);
        }
        return NULL;
    }
    if (!formstr->c)
    {
        return NULL;
    }
    smart_str_0(formstr);
    *length = formstr->len;
    return formstr->c;
}

#else /* PHP Version 7 */
#define sw_php_var_serialize                php_var_serialize
typedef size_t zend_size_t;
#define ZEND_SET_SYMBOL(ht,str,arr)         zval_add_ref(arr); zend_hash_str_update(ht, str, sizeof(str)-1, arr);

static sw_inline char* sw_http_build_query(zval *data, zend_size_t *length, smart_str *formstr TSRMLS_DC)
{
    if (php_url_encode_hash_ex(HASH_OF(data), formstr, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, (int) PHP_QUERY_RFC1738) == FAILURE)
    {
        if (formstr->s)
        {
            smart_str_free(formstr);
        }
        return NULL;
    }
    if (!formstr->s)
    {
        return NULL;
    }
    smart_str_0(formstr);
    *length = formstr->s->len;
    return formstr->s->val;
}
#endif /* PHP Version */

#endif /* EXT_SWOOLE_PHP7_WRAPPER_H_ */
