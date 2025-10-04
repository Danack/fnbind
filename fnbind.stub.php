<?php
// These should be passed to php-src/build/gen_stub.php to generate fnbind_arginfo.h
// Use a checkout of php-src 8.0 to generate this.
//


//
///** @param Closure|string $argument_list_or_closure */
//function fnbind_function_add(string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment = null, ?bool $return_by_reference = null, ?string $doc_comment = null, ?string $return_type = null, ?bool $is_strict = null): bool {}

///**
// * @param Closure|string $argument_list_or_closure
// * @alias fnbind_function_add
// * @deprecated
// */
//function fnbind_function_add(string $function_name, $argument_list_or_closure, ?string $code_or_doc_comment = null, ?bool $return_by_reference = null, ?string $doc_comment = null, ?string $return_type = null, ?bool $is_strict = null): bool {}
//


/**
 * Add a new function, similar to create_function()
 * Gives you more control over the type of function being created
 * (Signature 1 of 2)
 *
 * Aliases: fnbind_function_add
 *
 * @param string $funcname Name of function to be created
 * @param string $arglist Comma separated argument list
 * @param string $code Code making up the function
 * @param bool|null $return_by_reference whether the function should return by reference
 * @param string|null $doc_comment The doc comment of the function
 * @param string|null $return_type Return type of this function (e.g. `stdClass`, `?string`(php 7.1))
 * @return bool - True on success or false on failure.
 */
function fnbind_add_eval(
	string $funcname,
	string $arglist, string $code,
	?bool $return_by_reference = null,
	?string $doc_comment = null,
	?string $return_type = null,
	?bool $is_strict = null
) : bool {
}


//function fnbind_function_add(
//string $function_name,   			// 0
// $closure,  						 // 1
//?string $code_or_doc_comment = null,  // 2
//?bool $return_by_reference = null,  // 3
//?string $doc_comment = null,    // 4
//?string $return_type = null,    // 5
//?bool $is_strict = null  // 6
//): bool {}

/**
 * Add a new function, similar to create_function()
 * Gives you more control over the type of function being created
 * (Signature 2 of 2)
 *
 * Aliases: fnbind_function_add
 *
 * @param string $funcname Name of function to be created
 * @param Closure $closure A closure to use as the source for this function. Static variables and `use` variables are copied.
 * @param string|null $doc_comment The doc comment of the function
 * @return bool - True on success or false on failure.
 */
function fnbind_add_closure(
	string $funcname,
	Closure $closure,
	string|null $doc_comment = null
) : bool {
}
