[Runkit7](https://github.com/fnbind/fnbind): Independent fork of fnbind for PHP 7.2+
======================================================================================

For all those things you.... probably shouldn't have been doing anyway.... but surely do!
__Supports PHP7.2-8.1!__ (function/method manipulation is recommended only for unit testing, but all other functionality works.)

- Function/method manipulation crashes in PHP 7.4+ when opcache is enabled (e.g. `opcache.enable_cli`) ([Issue #217](https://github.com/fnbind/fnbind/issues/217))

  Disabling opcache is the recommended workaround.
- This has been tested with php 8.2.0beta1

[![Build Status](https://github.com/fnbind/fnbind/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/fnbind/fnbind/actions/workflows/main.yml?query=branch%3Amaster)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/3jwsf76ge0yo8v74/branch/master?svg=true)](https://ci.appveyor.com/project/TysonAndre/fnbind/branch/master)

[Building and installing fnbind in unix](#building-and-installing-fnbind-in-unix)

[Building and installing fnbind in Windows](#building-and-installing-fnbind-in-windows)

This extension's documentation is available at [https://www.php.net/fnbind](https://www.php.net/fnbind).

Compatibility: PHP7.2 to PHP 8.1
--------------------------------

**See [fnbind-api.php](./fnbind-api.php) for the implemented functionality and method signatures.** New functionality was added to support usage with PHP7.

- This adds the ability to set return types (including nullable return types) on added/redefined functions.
- This adds the ability to set `declare(strict_types=1)` on added/redefined functions.


Class and function manipulation is recommended only for unit tests.


- Manipulating user-defined (i.e. not builtin or part of extensions) functions and methods via `fnbind_method_*` and `fnbind_function_*` generally works, **but is recommended only in unit tests** (unlikely to crash, but will cause memory leaks)



The following contributions are welcome:

-   Fixes and documentation.





#### Implemented APIs for PHP7

NOTE: Most `fnbind_*()` functions have aliases of `fnbind_*()`.

-   `fnbind_function_*`: Most tests are passing. There are some memory leaks when renaming internal functions.



### FURTHER WORK

See https://github.com/fnbind/fnbind/issues


### Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) for guidelines on issues and pull requests, as well as links to resources that are useful for php7 module and fnbind development.

UPSTREAM DOCUMENTATION
======================

**([fnbind](https://pecl.php.net/package/fnbind) is a fork of https://github.com/zenovich/fnbind, implementing php7.2+ support)**

Features
========

Runkit has two groups of features outlined below (Sandboxing was removed in fnbind):

### CUSTOM SUPERGLOBALS
A new .ini entry `fnbind.superglobal` is defined which may be specified as a simple variable, or list of simple variables to be registered as
[superglobals](https://secure.php.net/manual/en/language.variables.superglobals.php).  fnbind.superglobal is defined as PHP_INI_SYSTEM and must be set in the system-wide php.ini.

Example:

php.ini:
```ini
fnbind.superglobal=foo,bar
```

test.php:
```php
function testme() {
  echo "Foo is $foo\n";
  echo "Bar is $bar\n";
  echo "Baz is $baz\n";
}
$foo = 1;
$bar = 2;
$baz = 3;

testme();
```

Outputs:
```
Foo is 1
Bar is 2
Baz is
```


### USER DEFINED FUNCTION AND CLASS MANIPULATION

**NOTE: Only a subset of the APIs have been ported to PHP7. Some of these APIs have segmentation faults in corner cases** (when `fnbind.internal_override=On`)

User defined functions and user defined methods may now be renamed, delete, and redefined using the API described at http://www.php.net/fnbind

Examples for these functions may also be found in the tests folder.

#### `fnbind_lint` alternatives

`fnbind_lint` was  disabled with the rest of the sandbox code due to issues porting it to PHP 7 ([Issue #114](https://github.com/fnbind/fnbind/issues/114)).
As a replacement for `fnbind_lint`/`fnbind_lint_file` try any of the following:

- `php -l --no-php-ini $filename` will quickly check if a file is syntactically valid, but will not show you any php notices about deprecated code, etc.
- [`opcache_compile_file`](https://secure.php.net/manual/en/function.opcache-compile-file.php) may help, but will not show you any notices.
- [`token_get_all($code, TOKEN_PARSE)`](http://php.net/token_get_all) will detect invalid ASTs in php 7.0+
- Projects such as [PHP-Parser (Pure PHP)](https://github.com/nikic/PHP-Parser) and [php-ast (C module)](https://github.com/nikic/php-ast), which produce an Abstract Syntax Tree from php code.
  Alternately, `token_get_all()` will throw an error for syntax errors if the flag `TOKEN_PARSE` is passed in.
  (Unfortunately, it parses but does not detect erroneous code, e.g. duplicate classes/methods in the same file).

  ```php
  // Example replacement for fnbind_lint.
  try {
      $ast = token_get_all('<?php function foo(){}', TOKEN_PARSE)
      return true;
  } catch (ParseError $e) {
      return false;
  }
  ```

  Alternately, you may wish to use a different approach and run a PHP static analyzer such as [Phan](https://github.com/phan/phan), [Psalm](https://github.com/vimeo/psalm), or [PHPStan](https://github.com/phpstan/phpstan)

Installation
============


### BUILDING AND INSTALLING FNBIND IN UNIX

`pecl install fnbind` can be used to install [fnbind releases published on PECL](https://pecl.php.net/package/fnbind).

Tarballs can be downloaded from [PECL](https://github.com/fnbind/fnbind/releases) or [GitHub](https://github.com/fnbind/fnbind/releases).

An example of how to build the latest master branch from source is below:

```bash
git clone https://github.com/fnbind/fnbind.git
cd fnbind
phpize
# The sandbox related code and flags have been removed, no need to disable them.
# (--enable-fnbind-modify (on by default) controls function, method, class, manipulation, and will control property manipulation)
# (--enable-fnbind-super (on by default) allows you to add custom superglobals)
# ./configure --help lists available configuration options.
./configure
make
make test
sudo make install
```

### BUILDING AND INSTALLING FNBIND IN WINDOWS

#### Setting up php build environment

Read https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2 and https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2#building_pecl_extensions first. This is just a special case of these instructions.

For PHP7.2+, you need to install "Visual Studio 2017 Community Edition" (or other 2017 edition).
For PHP8.0+, you need to install "Visual Studio 2019 Community Edition" (or other 2019 edition).
Make sure that C++ is installed with Visual Studio.
The command prompt to use is "VS2017 x86 Native Tools Command Prompt" on 32-bit, "VS2017 x64 Native Tools Command Prompt" on 64-bit.

- Note that different visual studio versions are needed for different PHP versions.
  For PHP 8.0+, use Visual Studio 2019 and vs16 instead.

For 64-bit installations of php7, use "x64" instead of "x86" for the below commands/folders.

After completing setup steps mentioned, including for `C:\php-sdk\phpdev\vc14`

extract download of php-7.4.11-src (or any version of php 7) to C:\php-sdk\phpdev\vc15\x86\php-7.4.11-src

#### Installing fnbind on windows

There are currently no sources providing DLLs of this fork. Runkit7 and other extensions used must be built from source.

Create subdirectory C:\php-sdk\phpdev\vc14\x86\pecl, adjacent to php source directory)

extract download of fnbind to C:\php-sdk\phpdev\vc14\x86\pecl\fnbind (all of the c files and h files should be within fnbind)

Then, execute the following (Add `--enable-fnbind` to the configure flags you were already using)

```Batchfile
cd C:\php-sdk
C:\php-sdk\bin\phpsdk_setvars.bat
cd phpdev\vc15\x86\php-7.4.11\src
buildconf
configure --enable-fnbind
nmake
```

Then, optionally test it (Most of the tests should pass, be skipped, or be known failures.):

```Batchfile
nmake test TESTS="C:\php-sdk\vc14\x86\pecl\fnbind\tests
```
