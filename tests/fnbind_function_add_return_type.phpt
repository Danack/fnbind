--TEST--
fnbind_function_add() function should accept valid return types passed in as a string
--SKIPIF--
<?php if(!extension_loaded("fnbind") ) print "skip"; ?>
--INI--
display_errors=on
--FILE--
<?php declare(strict_types=1);
ini_set('error_reporting', (string)(E_ALL));

fnbind_function_add('fnbind_function', 'string $a, $valid=false', 'return $valid ? $a : new stdClass();', false, '/** doc comment */', 'string');
printf("Returned %s\n", fnbind_function('foo', true));
try {
	printf("Returned %s\n", fnbind_function('notastring', false));
} catch (TypeError $e) {
	printf("TypeError: %s", $e->getMessage());
}
?>
--EXPECTF--
Returned foo
TypeError: %Sfnbind_function()%S must be of%stype string, %s returned
