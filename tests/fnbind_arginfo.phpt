--TEST--
reflection should pick up fnbind arginfo
--SKIPIF--
<?php
if(!extension_loaded("fnbind")) print "skip";
?>
--INI--
error_reporting=E_ALL
--FILE--
<?php
foreach ([
//    'fnbind_add_eval',
    'fnbind_add_closure',
] as $name) {
$function = new ReflectionFunction($name);
printf("%-30s: %d to %d args: (%s)%s\n",
    $name,
    $function->getNumberOfRequiredParameters(),
    $function->getNumberOfParameters(),
    implode(', ', array_map(function(ReflectionParameter $param) {
        $raw_type = $param->getType();
        $type = @(string) $raw_type;  // __toString deprecated in 7.4 but reversed in 8.0
        if ($raw_type && $raw_type->allowsNull() && strpos($type, '?') === false) {
            $type = '?' . $type;
        }
        $result = '$' . $param->getName();
        if ($type) {
            return "$type $result";
        }
        return $result;
    }, $function->getParameters())),
    $function->isDeprecated() ? ' (Deprecated)' : ''
);
}


?>
--EXPECT--
fnbind_add_closure            : 2 to 3 args: (string $funcname, Closure $closure, ?string $doc_comment)