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
    'fnbind_function_add',
    'fnbind_function_add',
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
fnbind_function_add           : 2 to 7 args: (string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment, ?bool $return_by_reference, ?string $doc_comment, ?string $return_type, ?bool $is_strict)
fnbind_function_add           : 2 to 7 args: (string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment, ?bool $return_by_reference, ?string $doc_comment, ?string $return_type, ?bool $is_strict)