<?php
// These should be passed to php-src/build/gen_stub.php to generate runkit7_arginfo.h
// Use a checkout of php-src 8.0 to generate this.
//
// FOR A HUMAN-READABLE VERSION, SEE runkit-api.php


/** @param Closure|string $argument_list_or_closure */
function runkit7_function_add(string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment = null, ?bool $return_by_reference = null, ?string $doc_comment = null, ?string $return_type = null, ?bool $is_strict = null): bool {}
/**
 * @param Closure|string $argument_list_or_closure
 * @alias runkit7_function_add
 * @deprecated
 */
function runkit_function_add(string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment = null, ?bool $return_by_reference = null, ?string $doc_comment = null, ?string $return_type = null, ?bool $is_strict = null): bool {}
#endif



