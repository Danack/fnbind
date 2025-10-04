/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group, (c) 2008-2015 Dmitry Zenovich |
  | "runkit7" patches (c) 2015-2020 Tyson Andre                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the new BSD license,                  |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.opensource.org/licenses/BSD-3-Clause                      |
  | or at https://github.com/runkit7/runkit7/blob/master/LICENSE         |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  | Modified by Dmitry Zenovich <dzenovich@gmail.com>                    |
  | Modified for php7 by Tyson Andre <tysonandre775@hotmail.com>         |
  +----------------------------------------------------------------------+
*/

#include "runkit.h"
#include "Zend/zend_extensions.h"
#include "SAPI.h"

/* PHP 8.0+ allows extensions to see the representation of default values. Older versions don't. */
#if PHP_VERSION_ID < 80000
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) ZEND_ARG_INFO(pass_by_ref, name)
#endif

#include "runkit7_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(runkit7)


#ifndef ZEND_ARG_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value) \
	ZEND_ARG_INFO(pass_by_ref, name)
#endif

#define PHP_FE_AND_FALIAS(runkit_function_name, runkit7_function_name) \
	PHP_FE(runkit7_function_name,									arginfo_ ## runkit_function_name) \
	PHP_DEP_FALIAS(runkit_function_name, runkit7_function_name,		arginfo_ ## runkit7_function_name)

PHP_FUNCTION(runkit7_function_add);



/* {{{ runkit7_functions[]
 */
zend_function_entry runkit7_functions[] = {

#ifdef PHP_RUNKIT_MANIPULATION
	PHP_FE_AND_FALIAS(runkit_function_add,			runkit7_function_add)
#endif /* PHP_RUNKIT_MANIPULATION */

	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ runkit7_module_entry
 */
zend_module_entry runkit7_module_entry = {
	STANDARD_MODULE_HEADER,
	"runkit7",
	runkit7_functions,
	PHP_MINIT(runkit7),
	PHP_MSHUTDOWN(runkit7),
	PHP_RINIT(runkit7),
	PHP_RSHUTDOWN(runkit7),
	PHP_MINFO(runkit7),
	PHP_RUNKIT7_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(PHP_RUNKIT_SUPERGLOBALS) || defined(PHP_RUNKIT_MANIPULATION)
PHP_INI_BEGIN()
PHP_INI_END()
#endif

#ifdef COMPILE_DL_RUNKIT7
ZEND_GET_MODULE(runkit7)
#endif

#ifdef PHP_RUNKIT_MANIPULATION
ZEND_FUNCTION(_php_runkit_removed_function)
{
	php_error_docref(NULL, E_ERROR, "A function removed by runkit7 was somehow invoked");
}
ZEND_FUNCTION(_php_runkit_removed_method)
{
	php_error_docref(NULL, E_ERROR, "A method removed by runkit7 was somehow invoked");
}

static inline void _php_runkit_init_stub_function(const char *name, ZEND_NAMED_FUNCTION(handler), zend_function **result)
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
	fn->internal_function.module = &runkit7_module_entry;
}
#endif

#if defined(PHP_RUNKIT_MANIPULATION)
static void php_runkit7_globals_ctor(void *pDest)
{
	zend_runkit7_globals *runkit_global = (zend_runkit7_globals *)pDest;
#ifdef PHP_RUNKIT_MANIPULATION
	runkit_global->replaced_internal_functions = NULL;
	runkit_global->misplaced_internal_functions = NULL;
	// runkit_global->removed_default_class_members = NULL;
	runkit_global->name_str = "name";
	runkit_global->removed_method_str = "__method_removed_by_runkit__";
	runkit_global->removed_function_str = "__function_removed_by_runkit__";
	runkit_global->removed_parameter_str = "__parameter_removed_by_runkit__";
#if defined(PHP_RUNKIT_MANIPULATION)
#if PHP_VERSION_ID >= 80000
	runkit_global->original_func_resource_handle = zend_get_resource_handle("runkit7");
#else
	{
		zend_extension placeholder = {0};
		runkit_global->original_func_resource_handle = zend_get_resource_handle(&placeholder);
	}
#endif
#endif

	_php_runkit_init_stub_function("__function_removed_by_runkit__", ZEND_FN(_php_runkit_removed_function), &runkit_global->removed_function);
	_php_runkit_init_stub_function("__method_removed_by_runkit__", ZEND_FN(_php_runkit_removed_method), &runkit_global->removed_method);
#endif
}
#endif

#define REGISTER_RUNKIT7_LONG_CONSTANT(runkit_const_name, runkit7_const_name, value) \
	REGISTER_LONG_CONSTANT(runkit_const_name,  value, CONST_CS | CONST_PERSISTENT); \
	REGISTER_LONG_CONSTANT(runkit7_const_name, value, CONST_CS | CONST_PERSISTENT);

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(runkit7)
{
#ifdef ZTS
#if defined(PHP_RUNKIT_MANIPULATION)
	ts_allocate_id(&runkit7_globals_id, sizeof(zend_runkit7_globals), php_runkit7_globals_ctor, NULL);
#endif
#else
#if defined(PHP_RUNKIT_MANIPULATION)
	php_runkit7_globals_ctor(&runkit7_globals);
#endif
#endif

#if defined(PHP_RUNKIT_SUPERGLOBALS) || defined(PHP_RUNKIT_MANIPULATION)
	REGISTER_INI_ENTRIES();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(runkit7)
{
#if defined(PHP_RUNKIT_SUPERGLOBALS) || defined(PHP_RUNKIT_MANIPULATION)
	UNREGISTER_INI_ENTRIES();
#endif

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(runkit7)
{

#ifdef PHP_RUNKIT_MANIPULATION
	RUNKIT_G(replaced_internal_functions) = NULL;
	RUNKIT_G(misplaced_internal_functions) = NULL;
	RUNKIT_G(module_moved_to_front) = 0;
#if PHP_VERSION_ID >= 80200
	/* ZEND_INIT_FCALL now asserts that the function exists in the symbol table at runtime. */
	CG(compiler_options) |= ZEND_COMPILE_IGNORE_USER_FUNCTIONS | ZEND_COMPILE_IGNORE_INTERNAL_FUNCTIONS;
#endif
	// RUNKIT_G(removed_default_class_members) = NULL;
#endif

	return SUCCESS;
}
/* }}} */



/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(runkit7)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(runkit7)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "runkit7 support", "enabled");
	php_info_print_table_header(2, "version", PHP_RUNKIT7_VERSION);

	php_info_print_table_end();

#if defined(PHP_RUNKIT_SUPERGLOBALS) || defined(PHP_RUNKIT_MANIPULATION)
	DISPLAY_INI_ENTRIES();
#endif
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
