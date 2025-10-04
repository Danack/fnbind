--TEST--
fnbind_function_add() function
--SKIPIF--
<?php if(!extension_loaded("fnbind") ) print "skip"; ?>
--FILE--
<?php

function fnbindAlreadyExists() {
    echo "This function already exists";
}

$name = 'fnbindAlreadyExists';

try {
    fnbind_add_eval($name, '$a, $b, $c = "baz"', 'static $is="is"; for($i=0; $i<10; $i++) {} echo "a $is $a\nb $is $b\nc $is $c\n";');
    echo "Error, no exception thrown.";
}
catch (\FnBindException $e) {
    echo "FnBindException caught as expected.\n";
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
FnBindException caught as expected.
Function fnbindAlreadyExists() already exists