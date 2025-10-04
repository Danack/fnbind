--TEST--
runkit7_function_add() function should accept valid return types passed in as a string
--SKIPIF--
<?php if(!extension_loaded("runkit7") ) print "skip"; ?>
--INI--
display_errors=on
--FILE--
<?php declare(strict_types=1);
ini_set('error_reporting', (string)(E_ALL));

runkit7_function_add('runkit_function', 'string $a, $valid=false', 'return $valid ? $a : new stdClass();', false, '/** doc comment */', 'string');
printf("Returned %s\n", runkit_function('foo', true));
try {
	printf("Returned %s\n", runkit_function('notastring', false));
} catch (TypeError $e) {
	printf("TypeError: %s", $e->getMessage());
}
?>
--EXPECTF--
Returned foo
TypeError: %Srunkit_function()%S must be of%stype string, %s returned
