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
  | Modified for php7 "fnbind" by Tyson Andre<tysonandre775@hotmail.com>|
  +----------------------------------------------------------------------+
*/

#ifndef FNBIND_H
#define FNBIND_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "Zend/zend_closures.h"

#include "Zend/zend_interfaces.h"

#if PHP_VERSION_ID < 70200
#error Support for php versions < 7.2 was dropped in the earlier major release fnbind 3.0 - Use the older fnbind 2.x or 1.x releases from github instead
#endif

#include <stdbool.h>
#include <stdint.h>

#ifdef __GNUC__
// Make it easy to verify that format strings are correct in recent versions of gcc.
extern PHPAPI ZEND_COLD void php_error_docref(const char *docref, int type, const char *format, ...)
    __attribute__((format(printf, 3, 4)));
#endif
#ifdef DEBUGGING
#define debug_printf(...) printf(__VA_ARGS__)
static inline void *_debug_emalloc(void *data, int bytes, char *file, int line)
{
	debug_printf("%llx: Wrapped emalloc %d bytes, %s:%d\n", (long long)data, bytes, file, line);
	return data;
}
#undef emalloc
#define emalloc(size) _debug_emalloc(_emalloc((size) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC), (size), __FILE__, __LINE__)
#define zend_string_release(s) do{ debug_printf("%llx: Pretending to zend_string_release %s\n", (long long) (s), ZSTR_VAL((s)));} while(0)

#undef efree
#undef pefree
#define efree(m) do {debug_printf("%llx: efree at %s:%d\n", (long long) (m), __FILE__, __LINE__); _efree((m) ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC); } while(0)
#define pefree(m, persistent) do {if ((persistent)) {debug_printf("%llx: (pe)free at %s:%d\n", (long long) (m), __FILE__, __LINE__); free((m));} else { efree(m); }} while(0)
#else
#define debug_printf(...) do { } while(0)
#endif

#define PHP_FNBIND_VERSION					"4.0.0a6"

#define PHP_FNBIND_OVERRIDE_OBJECTS           0x8000


extern zend_class_entry *php_fnbind_exception_class_entry;


#include "Zend/zend_object_handlers.h"


PHP_MINIT_FUNCTION(fnbind);
PHP_MSHUTDOWN_FUNCTION(fnbind);
PHP_RINIT_FUNCTION(fnbind);
PHP_RSHUTDOWN_FUNCTION(fnbind);
PHP_MINFO_FUNCTION(fnbind);


PHP_FUNCTION(fnbind_function_add);

ZEND_BEGIN_MODULE_GLOBALS(fnbind)
	HashTable *misplaced_internal_functions;
	HashTable *replaced_internal_functions;
	// php_fnbind_default_class_members_list_element *removed_default_class_members;
	zend_bool internal_override;
	const char *name_str, *removed_method_str, *removed_function_str, *removed_parameter_str;
	zend_function *removed_function, *removed_method;
	zend_bool module_moved_to_front;
	int original_func_resource_handle;
ZEND_END_MODULE_GLOBALS(fnbind)


extern ZEND_DECLARE_MODULE_GLOBALS(fnbind)

#ifdef ZTS
#define		FNBIND_G(v)		TSRMG(fnbind_globals_id, zend_fnbind_globals *, v)
#else
#define		FNBIND_G(v)		(fnbind_globals.v)
#endif

#define FNBIND_IS_CALLABLE(cb_zv, flags, cb_sp) zend_is_callable((cb_zv), (flags), (cb_sp))
#define FNBIND_FILE_HANDLE_DTOR(pHandle)        zend_file_handle_dtor((pHandle))

#ifdef ZEND_ACC_RETURN_REFERENCE
#     define PHP_FNBIND_ACC_RETURN_REFERENCE         ZEND_ACC_RETURN_REFERENCE
#else
#     define PHP_FNBIND_ACC_RETURN_REFERENCE         0x4000000
#endif

#ifndef ALLOC_PERMANENT_ZVAL
# define ALLOC_PERMANENT_ZVAL(z) \
    (z) = (zval*)malloc(sizeof(zval))
#endif

#if !defined(zend_hash_add_or_update) && PHP_VERSION_ID < 70300
/* Why doesn't ZE2 define this? */
#define zend_hash_add_or_update(ht, key, data, mode) \
        _zend_hash_add_or_update((ht), (key), (data), (mode) ZEND_FILE_LINE_CC)
#endif
// Similar to zend_hash.h zend_hash_add_ptr
static inline void *fnbind_zend_hash_add_or_update_ptr(HashTable *ht, zend_string *key, void *pData, uint32_t flag)
{
	zval tmp, *zv;
	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_add_or_update(ht, key, &tmp, flag);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

#ifndef Z_ADDREF_P
#     define Z_ADDREF_P(x)                           ZVAL_ADDREF(x)
#endif

#ifndef IS_CONSTANT_AST
#define IS_CONSTANT_AST IS_CONSTANT_ARRAY
#endif

/* fnbind_functions.c */
#define FNBIND_TEMP_FUNCNAME  "__fnbind_temporary_function__"
int php_fnbind_check_call_stack(zend_op_array *op_array);
//void php_fnbind_clear_all_functions_runtime_cache();
//void php_fnbind_fix_all_hardcoded_stack_sizes(zend_string *called_name_lower, zend_function *called_f);

//void php_fnbind_remove_function_from_reflection_objects(zend_function *fe);
// void php_fnbind_function_copy_ctor(zend_function *fe, zend_string *newname, char orig_fe_type);
zend_function *php_fnbind_function_clone(zend_function *fe, zend_string *newname, char orig_fe_type);
void php_fnbind_function_dtor(zend_function *fe);
int php_fnbind_remove_from_function_table(HashTable *function_table, zend_string *func_lower);
void *php_fnbind_update_function_table(HashTable *function_table, zend_string *func_lower, zend_function *f);
int php_fnbind_generate_lambda_method(const zend_string *arguments, const zend_string *return_type, const zend_bool is_strict, const zend_string *phpcode,
                                      zend_function **pfe, zend_bool return_ref, zend_bool is_static);
int php_fnbind_generate_lambda_function(const zend_string *arguments, const zend_string *return_type, const zend_bool is_strict, const zend_string *phpcode,
                                      zend_function **pfe, zend_bool return_ref);
int php_fnbind_cleanup_lambda_method();
int php_fnbind_cleanup_lambda_function();
int php_fnbind_destroy_misplaced_functions(zval *pDest);
void php_fnbind_restore_internal_function(zend_string *fname_lower, zend_function *f);

/* fnbind_methods.c */
zend_class_entry *php_fnbind_fetch_class(zend_string *classname);
zend_class_entry *php_fnbind_fetch_class_int(zend_string *classname);
void php_fnbind_clean_children_methods(zend_class_entry *ce, zend_class_entry *ancestor_class, zend_class_entry *parent_class, zend_string *fname_lower, zend_function *orig_cfe);
void php_fnbind_clean_children_methods_foreach(HashTable *ht, zend_class_entry *ancestor_class, zend_class_entry *parent_class, zend_string *fname_lower, zend_function *orig_cfe);
void php_fnbind_update_children_methods(zend_class_entry *ce, zend_class_entry *ancestor_class, zend_class_entry *parent_class, zend_function *fe, zend_string *fname_lower, zend_function *orig_fe);
void php_fnbind_update_children_methods_foreach(HashTable *ht, zend_class_entry *ancestor_class, zend_class_entry *parent_class, zend_function *fe, zend_string *fname_lower, zend_function *orig_fe);
int php_fnbind_fetch_interface(zend_string *classname, zend_class_entry **pce);

#define PHP_FNBIND_NOT_ENOUGH_MEMORY_ERROR php_error_docref(NULL, E_ERROR, "Not enough memory")

/* fnbind_constants.c */
void php_fnbind_update_children_consts(zend_class_entry *ce, zend_class_entry *parent_class, zval *c, zend_string *cname, zend_long access_type);
void php_fnbind_update_children_consts_foreach(HashTable *ht, zend_class_entry *parent_class, zval *c, zend_string *cname, zend_long access_type);

/* fnbind_classes.c */
int php_fnbind_class_copy(zend_class_entry *src, zend_string *classname);

/* fnbind_props.c */
void php_fnbind_update_children_def_props(zend_class_entry *ce, zend_class_entry *parent_class, zval *p, zend_string *pname, int access_type, zend_class_entry *definer_class, int override, int override_in_objects);
int php_fnbind_def_prop_add_int(zend_class_entry *ce, zend_string *propname, zval *copyval, long visibility,
				zend_string *doc_comment, zend_class_entry *definer_class, int override,
                                int override_in_objects);
int php_fnbind_def_prop_remove_int(zend_class_entry *ce, zend_string *propname, zend_class_entry *definer_class,
                                   zend_bool was_static, zend_bool remove_from_objects, zend_property_info *parent_property);

typedef struct _zend_closure {
    zend_object    std;
    zend_function  func;
    HashTable     *debug_info;
} zend_closure;

typedef struct _parsed_return_type {
	zend_string *return_type;
	zend_bool   valid;
} parsed_return_type;

typedef struct _parsed_is_strict {
	zend_bool   overridden;
	zend_bool   is_strict;
	zend_bool   valid;
} parsed_is_strict;


/* {{{ php_fnbind_modify_function_doc_comment */
/** Replace the doc comment of the copied/created function with that of the original */
static inline void php_fnbind_modify_function_doc_comment(zend_function *fe, zend_string *doc_comment)
{
	if (fe->type == ZEND_USER_FUNCTION) {
		if (doc_comment) {
			// TODO: Fix memory leak warnings related to doc comments for created/renamed functions.
			zend_string *tmp = fe->op_array.doc_comment;
			zend_string_addref(doc_comment);
			fe->op_array.doc_comment = doc_comment;
			if (tmp) {
				zend_string_delref(tmp);
			}
		}
	}
}
/* }}} */

#define PHP_FNBIND_FREE_INTERNAL_FUNCTION_NAME(fe) \
	if ((fe)->type == ZEND_INTERNAL_FUNCTION && (fe)->internal_function.function_name) { \
		zend_string_release((fe)->internal_function.function_name); \
		(fe)->internal_function.function_name = NULL; \
	}



/* This macro iterates through all instances of objects. */
#define PHP_FNBIND_ITERATE_THROUGH_OBJECTS_STORE_BEGIN(i) { \
	if (EG(objects_store).object_buckets) { \
		for (i = 1; i < EG(objects_store).top; i++) { \
			if (EG(objects_store).object_buckets[i] && \
			   IS_OBJ_VALID(EG(objects_store).object_buckets[i]) && (!(GC_FLAGS(EG(objects_store).object_buckets[i]) & IS_OBJ_DESTRUCTOR_CALLED))) { \
				zend_object *object; \
				object = EG(objects_store).object_buckets[i];

#define PHP_FNBIND_ITERATE_THROUGH_OBJECTS_STORE_END \
			} \
		}\
	} \
}



// Split pnname into classname and pnname, if it contains the string "::"
// FIXME: Need to correctly do reference tracking for the original and created strings.
#define PHP_FNBIND_SPLIT_PN(classname, pnname) { \
	char *colon; \
	if (ZSTR_LEN(pnname) > 3 && (colon = memchr(ZSTR_VAL(pnname), ':', ZSTR_LEN(pnname) - 2)) && (colon[1] == ':')) { \
		classname = zend_string_init(ZSTR_VAL(pnname), colon - ZSTR_VAL(pnname), 0); \
		pnname = zend_string_init(colon + 2, ZSTR_LEN(pnname) - (ZSTR_LEN(classname) + 2), 0); \
	} else { \
		classname = NULL; \
	} \
}

// If the input string was split into two allocated zend_strings, then decrement the refcount of both of those strings.
#define PHP_FNBIND_SPLIT_PN_CLEANUP(classname, pnname) \
	if (classname != NULL) { \
		zend_string_release(classname); \
		zend_string_release(constname); \
	}


/* {{{ php_fnbind_parse_doc_comment_arg */
/* Validate that the provided doc comment is a string or null */
inline static zend_string *php_fnbind_parse_doc_comment_arg(int argc, zval *args, int arg_pos)
{
	if (argc > arg_pos) {
		if (Z_TYPE(args[arg_pos]) == IS_STRING) {
			/* Return doc comment without increasing reference count. */
			return Z_STR(args[arg_pos]);
		} else if (Z_TYPE(args[arg_pos]) != IS_NULL) {
			php_error_docref(NULL, E_WARNING, "Doc comment should be a string or NULL");
		}
	}
	return NULL;
}
/* }}} */

/* {{{ php_fnbind_is_valid_return_type */
inline static zend_bool php_fnbind_is_valid_return_type(const zend_string *return_type)
{
	const char *it = ZSTR_VAL(return_type);
	const char *const end = it + ZSTR_LEN(return_type);
	if (it >= end) {
		return 0;
	}
	if (*it == '?') {
		it++;
	}
	if (it >= end) {
		return 0;
	}
	if (*it == '\\') {
		it++;
	}
	if (it >= end) {
		return 0;
	}
	// The format of a valid class identifier is documented at https://secure.php.net/manual/en/language.oop5.basic.php
	while (1) {
		unsigned char c = *it;
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c >= 128) {
			for (++it; it < end; ++it) {
				c = *it;
				if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c >= 128) {
					continue;
				}
				if (c == '\\') {
					break;
				}
				return 0;
			}
			if (c == '\\') {
				if (it + 1 == end) {  // "\A\" is invalid
					return 0;
				}
				++it;
				continue;
			}
			return 1;
		}
		return 0;
	}
}
/* }}} */

/* {{{ php_fnbind_parse_return_type_arg */
inline static parsed_return_type php_fnbind_parse_return_type_arg(int argc, zval *args, int arg_pos)
{
	parsed_return_type retval;
	retval.return_type = NULL;
	retval.valid = 1;
	if (argc <= arg_pos) {
		return retval;
	}
	if (Z_TYPE(args[arg_pos]) == IS_STRING) {
		// Return return type without increasing reference count.
		zend_string *return_type = Z_STR(args[arg_pos]);
		if (ZSTR_LEN(return_type) == 0) {
			// NOTE: "" and NULL may mean different things in the future - "" could mean remove the return type (if one exists).
			return retval;
		}
		if (php_fnbind_is_valid_return_type(return_type)) {
			retval.return_type = return_type;
			return retval;
		}
		php_error_docref(NULL, E_WARNING, "Return type should match regex ^\\??[a-zA-Z_\\x7f-\\xff][a-zA-Z0-9_\\x7f-\\xff]*(\\\\[a-zA-Z_\\x7f-\\xff][a-zA-Z0-9_\\x7f-\\xff]*)*$");
		retval.valid = 0;
		return retval;
	} else if (Z_TYPE(args[arg_pos]) != IS_NULL) {
		php_error_docref(NULL, E_WARNING, "Return type should be a string or NULL");
		retval.valid = 0;
	}
	return retval;
}
/* }}} */

/* {{{ php_fnbind_parse_is_strict_arg */
inline static parsed_is_strict php_fnbind_parse_is_strict_arg(int argc, zval *args, int arg_pos)
{
	parsed_is_strict retval;
	retval.is_strict = 0;
	retval.overridden = 0;
	retval.valid = 1;
	if (argc <= arg_pos) {
		return retval;
	}
	if (Z_TYPE(args[arg_pos]) == IS_TRUE || Z_TYPE(args[arg_pos]) == IS_FALSE) {
		// Return return type without increasing reference count.
		retval.is_strict = Z_TYPE(args[arg_pos]) == IS_TRUE;
		retval.overridden = 1;
		return retval;
	} else if (Z_TYPE(args[arg_pos]) != IS_NULL) {
		php_error_docref(NULL, E_WARNING, "is_strict should be a boolean or NULL");
		retval.valid = 0;
	}
	return retval;
}
/* }}} */

/* {{{ php_fnbind_parse_args_to_zvals */
inline static zend_bool php_fnbind_parse_args_to_zvals(int argc, zval **pargs)
{
	*pargs = (zval *)emalloc(argc * sizeof(zval));
	if (*pargs == NULL) {
		PHP_FNBIND_NOT_ENOUGH_MEMORY_ERROR;
		return 0;
	}
	if (zend_get_parameters_array_ex(argc, *pargs) == FAILURE) {
		php_error_docref(NULL, E_ERROR, "Internal error occurred while parsing arguments");
		efree(*pargs);
		return 0;
	}
	return 1;
}
/* }}} */

#define PHP_FNBIND_BODY_ERROR_MSG "%s's body should be either a closure or two strings"

/* {{{ zend_bool php_fnbind_parse_function_arg */
/** Parses either multiple strings (1. function args, 2. body 3. (optional) return type), or a Closure. */
inline static zend_bool php_fnbind_parse_function_arg(int argc, zval *args, int arg_pos, zend_function **fe, zend_string **arguments, zend_string **phpcode, long *opt_arg_pos, char *type)
{
	// If is successful, it returns true, advances opt_arg_pos. There are two was this could succeed
	// 1. *fe = zend_function extracted from a closure.
	// 2. *arguments, *phpcode = strings extracted from arguments
	if (Z_TYPE(args[arg_pos]) == IS_OBJECT && Z_OBJCE(args[arg_pos]) == zend_ce_closure) {
#if PHP_VERSION_ID >= 80000
		*fe = (zend_function *)zend_get_closure_method_def(Z_OBJ(args[arg_pos]));
#else
		*fe = (zend_function *)zend_get_closure_method_def(&(args[arg_pos]));
#endif
	} else if (Z_TYPE(args[arg_pos]) == IS_STRING) {
		(*opt_arg_pos)++;
		*arguments = Z_STR(args[arg_pos]);
		if (argc < arg_pos + 2 || Z_TYPE(args[arg_pos + 1]) != IS_STRING) {
			php_error_docref(NULL, E_ERROR, PHP_FNBIND_BODY_ERROR_MSG, type);
			return 0;
		}
		*phpcode = Z_STR(args[arg_pos + 1]);
	} else {
		php_error_docref(NULL, E_ERROR, PHP_FNBIND_BODY_ERROR_MSG, type);
		return 0;
	}
	return 1;
}
/* }}} */

// TODO: redundant and part of an unsupported feature
#	define PHP_FNBIND_DESTROY_FUNCTION(fe) 	destroy_zend_function(fe);

// TODO: move to a separate file.
void php_fnbind_update_reflection_object_name(zend_object *object, int handle, const char *name);

	// These struct definitions must be identical to those in ext/reflection/php_reflection.c

	/* Struct for properties */
	typedef struct _property_reference {
#if PHP_VERSION_ID < 70400
		zend_class_entry *ce;
#endif
#if PHP_VERSION_ID >= 80000
		zend_property_info *prop;
#else
		zend_property_info prop;
#endif
		zend_string *unmangled_name;
	} property_reference;

	/* Struct for parameters */
	typedef struct _parameter_reference {
		uint32_t offset;
		uint32_t required;
		struct _zend_arg_info *arg_info;
		zend_function *fptr;
	} parameter_reference;

#if PHP_VERSION_ID >= 80000
	struct _zend_attribute;
	typedef struct {
		HashTable *attributes;
		struct _zend_attribute *data;
		zend_class_entry *scope;
		zend_string *filename;
		uint32_t target;
	} attribute_reference;
#endif

	typedef struct _type_reference {
		zend_type type;
		/* Whether to use backwards compatible null representation */
		bool legacy_behavior;
	} type_reference;

	typedef enum {
		REF_TYPE_OTHER,      /* Must be 0 */
		REF_TYPE_FUNCTION,
		REF_TYPE_GENERATOR,
#if PHP_VERSION_ID >= 80100
		REF_TYPE_FIBER,
#endif
		REF_TYPE_PARAMETER,
		REF_TYPE_TYPE,
		REF_TYPE_PROPERTY,
#if PHP_VERSION_ID < 70300
		REF_TYPE_DYNAMIC_PROPERTY,
#endif
		REF_TYPE_CLASS_CONSTANT,
#if PHP_VERSION_ID >= 80000
		REF_TYPE_ATTRIBUTE,
#endif
		REF_TYPE_COUNT,
	} reflection_type_t;

	/* Struct for reflection objects */
	typedef struct {
		zval dummy; /* holder for the second property */
		zval obj;
		void *ptr;
		zend_class_entry *ce;
		reflection_type_t ref_type;
#if PHP_VERSION_ID < 80100
		/* Removed in php 8.1 with setAccessible becoming a no-op https://wiki.php.net/rfc/make-reflection-setaccessible-no-op */
		unsigned int ignore_visibility : 1;
#endif
		zend_object zo;
	} reflection_object;


#if PHP_VERSION_ID >= 70300
#define FNBIND_RT_CONSTANT(op_array, opline, node) RT_CONSTANT((opline), (node))
#else
#define FNBIND_RT_CONSTANT(op_array, opline, node) RT_CONSTANT((op_array), (node))
#endif

#if PHP_VERSION_ID < 80000
#define zend_class_implements_interface(class_ce, interface_ce) instanceof_function_ex((class_ce), (interface_ce), 1)
#endif


/* {{{ PHP < 7.3 compatibility for zend_constant */
#ifndef ZEND_CONSTANT_SET_FLAGS
#define ZEND_CONSTANT_SET_FLAGS(c, _flags, _module_number) do { \
		(c)->flags = (_flags); \
		(c)->module_number = (_module_number); \
	} while (0)
#endif

#ifndef ZEND_CONSTANT_FLAGS
#define ZEND_CONSTANT_FLAGS(c) ((c)->flags)
#endif

#ifndef ZEND_CONSTANT_MODULE_NUMBER
#define ZEND_CONSTANT_MODULE_NUMBER(c) ((c)->module_number)
#endif
/* }}} */

#endif	/* FNBIND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
