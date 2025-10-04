--TEST--
runkit7_function_add() closer and doc_comment from closure
--SKIPIF--
<?php
	if(!extension_loaded("runkit7") ) print "skip";
?>
--INI--
display_errors=on
--FILE--
<?php
runkit7_function_add('runkit_function',
                    /** new doc_comment */
                    function () {});
$r1 = new ReflectionFunction('runkit_function');
echo $r1->getDocComment(), "\n";
?>
--EXPECT--
/** new doc_comment */
