dnl config.m4 for extension runkit7

PHP_ARG_ENABLE(runkit7, whether to enable runkit7 support,
[  --enable-runkit7          Enable runkit7 support], no, yes)

PHP_ARG_ENABLE(runkit7-modify, whether to enable runtime manipulation of functions/classes/constants,
[  --enable-runkit7-modify    Enable runtime manipulation], inherit, no)

PHP_ARG_ENABLE(runkit7-super, whether to enable registration of user-defined autoglobals,
[  --enable-runkit7-super     Enable registration of user-defined autoglobals], inherit, yes)

if test "$PHP_RUNKIT7" != "no"; then
  AC_MSG_CHECKING([if this is built with PHP >= 7.2])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  #include <$phpincludedir/main/php_version.h>
  ]], [[
#if PHP_VERSION_ID < 70200
#error PHP < 7.2
#endif
  ]])],[
    AC_MSG_RESULT([this is PHP 7.2 or newer])
  ],[
    AC_MSG_ERROR([Runkit7 3.x requires PHP 7.2 or newer, phpize was run with PHP < 7.2]);
  ])
  if test "$PHP_RUNKIT7_SUPER" = "inherit"; then
    PHP_RUNKIT7_SUPER=yes
  fi
else
  if test "$PHP_RUNKIT7_MODIFY" = "inherit"; then
    PHP_RUNKIT7_MODIFY=no
  fi
  if test "$PHP_RUNKIT7_SUPER" = "inherit"; then
    PHP_RUNKIT7_SUPER=no
  fi
fi

if test "$PHP_RUNKIT7" != "no"; then
  if test "$PHP_RUNKIT7_MODIFY" != "no"; then
    AC_DEFINE(PHP_RUNKIT7_FEATURE_MODIFY, 1, [Whether to export runtime modification features])
  fi
  if test "$PHP_RUNKIT7_SUPER" != "no"; then
    AC_DEFINE(PHP_RUNKIT7_FEATURE_SUPER, 1, [Whether to export custom autoglobal registration feature])
  fi
  PHP_NEW_EXTENSION(runkit7, runkit.c runkit_functions.c \
runkit_common.c \
runkit_zend_execute_API.c \
, $ext_shared,, -Werror -Wall -Wno-deprecated-declarations -Wno-pedantic)
dnl use Makefile.frag to echo notice about upgrading to runkit7 3.x
  PHP_ADD_MAKEFILE_FRAGMENT
fi
