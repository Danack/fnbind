--TEST--
runkit_function_add() function
--INI--
error_reporting=E_ALL
display_errors=on
--SKIPIF--
<?php if(!extension_loaded("runkit7") ) print "skip"; ?>
--FILE--
<?php
$name = 'runkitSample';
runkit_function_add($name, '$a, $b, $c = "baz"', 'static $is="is"; for($i=0; $i<10; $i++) {} echo "a $is $a\nb $is $b\nc $is $c\n";');
runkitSample('foo','bar');
echo $name;
?>
--EXPECTF--
Deprecated: Function runkit_function_add() is deprecated in %srunkit_function_add_alias.php on line 3
a is foo
b is bar
c is baz
runkitSample