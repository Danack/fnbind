/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group, (c) 2008-2015 Dmitry Zenovich |
  | "fnbind" patches (c) 2015-2020 Tyson Andre                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the new BSD license,                  |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.opensource.org/licenses/BSD-3-Clause                      |
  | or at https://github.com/fnbind/fnbind/blob/master/LICENSE         |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  | Modified by Dmitry Zenovich <dzenovich@gmail.com>                    |
  | Modified for php7 by Tyson Andre <tysonandre775@hotmail.com>         |
  +----------------------------------------------------------------------+
*/

#include "fnbind.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_extensions.h"
#include "SAPI.h"

/* PHP 8.0+ allows extensions to see the representation of default values. Older versions don't. */
#if PHP_VERSION_ID < 80000
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) ZEND_ARG_INFO(pass_by_ref, name)
#endif

#include "fnbind_arginfo.h"

zend_class_entry *php_fnbind_exception_class_entry;

ZEND_DECLARE_MODULE_GLOBALS(fnbind)


#ifndef ZEND_ARG_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) \
	ZEND_ARG_INFO(pass_by_ref, name)
#endif

PHP_FUNCTION(fnbind_function_add);

/* {{{ fnbind_functions[]
 */
zend_function_entry fnbind_functions[] = {
	PHP_FE(fnbind_function_add, arginfo_fnbind_function_add)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ fnbind_module_entry
 */
zend_module_entry fnbind_module_entry = {
	STANDARD_MODULE_HEADER,
	"fnbind",
	fnbind_functions,
	PHP_MINIT(fnbind),
	PHP_MSHUTDOWN(fnbind),
	PHP_RINIT(fnbind),
	PHP_RSHUTDOWN(fnbind),
	PHP_MINFO(fnbind),
	PHP_FNBIND_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

PHP_INI_BEGIN()
PHP_INI_END()


#ifdef COMPILE_DL_FNBIND
ZEND_GET_MODULE(fnbind)
#endif

ZEND_FUNCTION(_php_fnbind_removed_function)
{
	php_error_docref(NULL, E_ERROR, "A function removed by fnbind was somehow invoked");
}
ZEND_FUNCTION(_php_fnbind_removed_method)
{
	php_error_docref(NULL, E_ERROR, "A method removed by fnbind was somehow invoked");
}

static inline void _php_fnbind_init_stub_function(const char *name, ZEND_NAMED_FUNCTION(handler), zend_function **result)
{
	zend_function *fn = pemalloc(sizeof(zend_function), 1);
	*result = fn;
	/* Ensure memory is initialized for fields that aren't listed here in stub functions. */
	memset(fn, 0, sizeof(zend_function));
	fn->common.function_name = zend_string_init(name, strlen(name), 1);  // TODO: Can this be persistent?
	fn->common.scope = NULL;
	fn->common.arg_info = NULL;
	fn->common.num_args = 0;
	fn->common.type = ZEND_INTERNAL_FUNCTION;
	fn->common.fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_STATIC;
	fn->common.arg_info = NULL;
	/* fn->common.T was added in 8.2.0beta2 */
	/* fn->common.T = 0; */
	fn->internal_function.handler = handler;
	fn->internal_function.module = &fnbind_module_entry;
}


static void php_fnbind_globals_ctor(void *pDest)
{
	zend_fnbind_globals *fnbind_global = (zend_fnbind_globals *)pDest;
	fnbind_global->replaced_internal_functions = NULL;
	fnbind_global->misplaced_internal_functions = NULL;
	// fnbind_global->removed_default_class_members = NULL;
	fnbind_global->name_str = "name";
	fnbind_global->removed_method_str = "__method_removed_by_fnbind__";
	fnbind_global->removed_function_str = "__function_removed_by_fnbind__";
	fnbind_global->removed_parameter_str = "__parameter_removed_by_fnbind__";

#if PHP_VERSION_ID >= 80000
	fnbind_global->original_func_resource_handle = zend_get_resource_handle("fnbind");
#else
	{
		zend_extension placeholder = {0};
		fnbind_global->original_func_resource_handle = zend_get_resource_handle(&placeholder);
	}
#endif

	_php_fnbind_init_stub_function("__function_removed_by_fnbind__", ZEND_FN(_php_fnbind_removed_function), &fnbind_global->removed_function);
	_php_fnbind_init_stub_function("__method_removed_by_fnbind__", ZEND_FN(_php_fnbind_removed_method), &fnbind_global->removed_method);

}


#define REGISTER_FNBIND_LONG_CONSTANT(fnbind_const_name, unused_const_name, value) \
	REGISTER_LONG_CONSTANT(fnbind_const_name,  value, CONST_CS | CONST_PERSISTENT);

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fnbind)
{
	zend_class_entry ce;

#ifdef ZTS
	ts_allocate_id(&fnbind_globals_id, sizeof(zend_fnbind_globals), php_fnbind_globals_ctor, NULL);
#else
	php_fnbind_globals_ctor(&fnbind_globals);
#endif

	INIT_CLASS_ENTRY(ce, "FnBindException", NULL);


	php_fnbind_exception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fnbind)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(fnbind)
{


	FNBIND_G(replaced_internal_functions) = NULL;
	FNBIND_G(misplaced_internal_functions) = NULL;
	FNBIND_G(module_moved_to_front) = 0;
#if PHP_VERSION_ID >= 80200
	/* ZEND_INIT_FCALL now asserts that the function exists in the symbol table at runtime. */
	CG(compiler_options) |= ZEND_COMPILE_IGNORE_USER_FUNCTIONS | ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS;
#endif
	// FNBIND_G(removed_default_class_members) = NULL;

	return SUCCESS;
}
/* }}} */



/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(fnbind)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fnbind)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fnbind support", "enabled");
	php_info_print_table_header(2, "version", PHP_FNBIND_VERSION);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
