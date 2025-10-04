--TEST--
fnbind_function_add() closure and doc_comment
--SKIPIF--
<?php
	if(!extension_loaded("fnbind") ) print "skip";
?>
--INI--
display_errors=on
--FILE--
<?php
fnbind_function_add('fnbind_function', function () {}, 'new doc_comment');
$r1 = new ReflectionFunction('fnbind_function');
echo $r1->getDocComment(), "\n";
?>
--EXPECT--
new doc_comment
