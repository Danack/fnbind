dnl config.m4 for extension fnbind

PHP_ARG_ENABLE(fnbind, whether to enable fnbind support,
[  --enable-fnbind          Enable fnbind support], no, yes)


if test "$PHP_FNBIND" != "no"; then
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
    AC_MSG_ERROR([PHP_FNBIND requires PHP 7.2 or newer, phpize was run with PHP < 7.2]);
  ])
fi

if test "$PHP_FNBIND" != "no"; then
  PHP_NEW_EXTENSION(fnbind, fnbind.c fnbind_functions.c \
fnbind_common.c \
fnbind_zend_execute_API.c \
, $ext_shared,, -Werror -Wall -Wno-deprecated-declarations -Wno-pedantic)
dnl use Makefile.frag to echo notice about upgrading to fnbind 3.x
  PHP_ADD_MAKEFILE_FRAGMENT
fi
