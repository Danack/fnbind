--TEST--
fnbind_add_closure() function with closure from class.
--SKIPIF--
<?php
	if(!extension_loaded("fnbind") ) print "skip";
?>
--INI--
display_errors=on
--FILE--
<?php

class Foo {
    public function bar()
    {
        echo "This is bar\n";
    }
}


$foo = new Foo();

fnbind_add_closure(
    'great_success',
    $foo->bar(...)
);

great_success();

?>
--EXPECTREGEX--
This is bar
