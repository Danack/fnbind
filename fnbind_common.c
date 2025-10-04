/*
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group, (c) 2008-2012 Dmitry Zenovich |
  | "runkit7" patches (c) 2015-2020 Tyson Andre                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the new BSD license,                  |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.opensource.org/licenses/BSD-3-Clause                      |
  | or at https://github.com/danack/fnbind/blob/master/LICENSE         |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  | Modified by Dmitry Zenovich <dzenovich@gmail.com>                    |
  | Modified for php7 by Tyson Andre<tysonandre775@hotmail.com>          |
  +----------------------------------------------------------------------+
*/

#include "fnbind.h"

/* {{{ */
void ensure_all_objects_of_class_have_magic_methods(zend_class_entry *ce)
{
	uint32_t i;
	if (ce->ce_flags & ZEND_ACC_USE_GUARDS) {
		return;  // Nothing to do?
	}
	// Make sure that new classes will be able to call magic methods
	ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	// Make sure that existing classes will be able to call magic methods
  // TODO: Add test of adding magic methods working on objects initialized before the call to fnbind.
	PHP_FNBIND_ITERATE_THROUGH_OBJECTS_STORE_BEGIN(i)
	if (object->ce == ce) {
		// based on Zend/zend_objects.c zend_object_std_init.
		// TODO: Figure out if anything else needs to be done
#if PHP_VERSION_ID < 70300
		GC_FLAGS(object) |= IS_OBJ_USE_GUARDS;  // see zend_object_std_init
#else
		// TODO: Also undefined this element for php 7.2, but not 7.0
		ZVAL_UNDEF(object->properties_table + object->ce->default_properties_count);
#endif
	}
	PHP_FNBIND_ITERATE_THROUGH_OBJECTS_STORE_END
}
/* }}} */

/* {{{ PHP_FNBIND_ADD_MAGIC_METHOD */
void PHP_FNBIND_ADD_MAGIC_METHOD(zend_class_entry *ce, zend_string *lcmname, zend_function *fe, const zend_function *orig_fe)
{
	if (zend_string_equals_literal(lcmname, ZEND_CLONE_FUNC_NAME)) {
		(ce)->clone = (fe);
	} else if (zend_string_equals_literal(lcmname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		if (!(ce)->constructor || (ce)->constructor == (orig_fe)) {
			(ce)->constructor = (fe);
#if PHP_VERSION_ID < 70400
			(fe)->common.fn_flags |= ZEND_ACC_CTOR;
#endif
		}
	} else if (zend_string_equals_literal(lcmname, ZEND_DESTRUCTOR_FUNC_NAME)) {
		(ce)->destructor = (fe);
#if PHP_VERSION_ID < 70400
		(fe)->common.fn_flags |= ZEND_ACC_DTOR;
#endif
	} else if (zend_string_equals_literal(lcmname, ZEND_GET_FUNC_NAME)) {
		(ce)->__get = (fe);
		ensure_all_objects_of_class_have_magic_methods(ce);
	} else if (zend_string_equals_literal(lcmname, ZEND_SET_FUNC_NAME)) {
		(ce)->__set = (fe);
		ensure_all_objects_of_class_have_magic_methods(ce);
	} else if (zend_string_equals_literal(lcmname, ZEND_CALL_FUNC_NAME)) {
		(ce)->__call = (fe);
	} else if (zend_string_equals_literal(lcmname, ZEND_UNSET_FUNC_NAME)) {
		(ce)->__unset = (fe);
		ensure_all_objects_of_class_have_magic_methods(ce);
	} else if (zend_string_equals_literal(lcmname, ZEND_ISSET_FUNC_NAME)) {
		(ce)->__isset = (fe);
		ensure_all_objects_of_class_have_magic_methods(ce);
	} else if (zend_string_equals_literal(lcmname, ZEND_CALLSTATIC_FUNC_NAME)) {
		(ce)->__callstatic = (fe);
	} else if (zend_string_equals_literal(lcmname, ZEND_TOSTRING_FUNC_NAME)) {
		(ce)->__tostring = (fe);
	} else if (zend_string_equals_literal(lcmname, ZEND_DEBUGINFO_FUNC_NAME)) {
		(ce)->__debugInfo = (fe);
#if PHP_VERSION_ID >= 80000
	} else if (zend_string_equals_literal(lcmname, "__serialize")) {
		(ce)->__serialize = (fe);
	} else if (zend_string_equals_literal(lcmname, "__unserialize")) {
		(ce)->__unserialize = (fe);
#else /* PHP_VERSION_ID < 80000 */
	} else if (zend_class_implements_interface(ce, zend_ce_serializable) && zend_string_equals_literal(lcmname, "serialize")) {
		(ce)->serialize_func = (fe);
	} else if (zend_class_implements_interface(ce, zend_ce_serializable) && zend_string_equals_literal(lcmname, "unserialize")) {
		(ce)->unserialize_func = (fe);
	} else if (zend_string_equals_ci(lcmname, (ce)->name)) {
        /* Only support class name as alias for constructor in php < 8 */
		// TODO: Re-examine the changes to the constructor code for any bugs.
		if (!(ce)->constructor || (ce)->constructor == (orig_fe)) {
			(ce)->constructor = (fe);
#if PHP_VERSION_ID < 70400
			(fe)->common.fn_flags |= ZEND_ACC_CTOR;
#endif
		}
#endif
	}
}
/** }}} */

/* {{{ PHP_FNBIND_DEL_MAGIC_METHOD */
void PHP_FNBIND_DEL_MAGIC_METHOD(zend_class_entry *ce, const zend_function *fe)
{
	if (ce->constructor == fe) {
		ce->constructor = NULL;
	} else if (ce->destructor == fe) {
		ce->destructor = NULL;
	} else if (ce->__get == fe) {
		ce->__get = NULL;
	} else if (ce->__set == fe) {
		ce->__set = NULL;
	} else if (ce->__unset == fe) {
		ce->__unset = NULL;
	} else if (ce->__isset == fe) {
		ce->__isset = NULL;
	} else if (ce->__call == fe) {
		ce->__call = NULL;
	} else if (ce->__callstatic == fe) {
		ce->__callstatic = NULL;
	} else if (ce->__tostring == fe) {
		ce->__tostring = NULL;
	} else if (ce->__debugInfo == fe) {
		ce->__debugInfo = NULL;
	} else if (ce->clone == fe) {
		ce->clone = NULL;
#if PHP_VERSION_ID >= 80000
	} else if (ce->__serialize == fe) {
		ce->__serialize = NULL;
	} else if (ce->__unserialize == fe) {
		ce->__unserialize = NULL;
#else
	} else if (zend_class_implements_interface(ce, zend_ce_serializable) && ce->serialize_func == fe) {
		ce->serialize_func = NULL;
	} else if (zend_class_implements_interface(ce, zend_ce_serializable) && ce->unserialize_func == fe) {
		ce->unserialize_func = NULL;
#endif
	}
}
/* }}} */
