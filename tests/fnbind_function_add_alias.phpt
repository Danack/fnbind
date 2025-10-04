--TEST--
fnbind_function_add() function
--INI--
error_reporting=E_ALL
display_errors=on
--SKIPIF--
<?php if(!extension_loaded("fnbind") ) print "skip"; ?>
--FILE--
<?php
$name = 'fnbindSample';
fnbind_function_add($name, '$a, $b, $c = "baz"', 'static $is="is"; for($i=0; $i<10; $i++) {} echo "a $is $a\nb $is $b\nc $is $c\n";');
fnbindSample('foo','bar');
echo $name;
?>
--EXPECTF--
a is foo
b is bar
c is baz
fnbindSample