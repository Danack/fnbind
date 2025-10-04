--TEST--
fnbind_add_closure() closer and doc_comment from closure
--SKIPIF--
<?php
	if(!extension_loaded("fnbind") ) print "skip";
?>
--INI--
display_errors=on
--FILE--
<?php
fnbind_add_closure('fnbind_function',
                    /** new doc_comment */
                    function () {});
$r1 = new ReflectionFunction('fnbind_function');
echo $r1->getDocComment(), "\n";
?>
--EXPECT--
/** new doc_comment */
