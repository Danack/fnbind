--TEST--
fnbind_function_add() function should detect invalid return types passed in as a string
--SKIPIF--
<?php if(!extension_loaded("fnbind") ) print "skip"; ?>
--INI--
display_errors=on
--FILE--
<?php declare(strict_types=1);
ini_set('error_reporting', (string)(E_ALL));

$retval = fnbind_add_eval('fnbind_function', 'string $a, $valid=false', 'return $valid ? $a : new stdClass();', false, '/** doc comment */', 'string#');
printf("fnbind_function_add returned: %s\n", var_export($retval, true));
printf("Function exists: %s\n", var_export(function_exists('fnbind_function'), true));
?>
--EXPECTF--
Warning: fnbind_add_eval(): Return type should match regex %s in %s on line %d
fnbind_function_add returned: false
Function exists: false
