<?php
// API for fnbind

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
 * @param bool $return_by_reference whether the function should return by reference
 * @param ?string $doc_comment The doc comment of the function
 * @param ?string $return_type Return type of this function (e.g. `stdClass`, `?string`(php 7.1))
 * @return bool - True on success or false on failure.
 */
function fnbind_function_add(string $funcname, string $arglist, string $code, bool $return_by_reference = null, string $doc_comment = null, string $return_type = null, bool $is_strict = null) : bool {
}

/**
 * Add a new function, similar to create_function()
 * Gives you more control over the type of function being created
 * (Signature 2 of 2)
 *
 * Aliases: fnbind_function_add
 *
 * @param string $funcname Name of function to be created
 * @param Closure $closure A closure to use as the source for this function. Static variables and `use` variables are copied.
 * @param ?string $doc_comment The doc comment of the function
 * @param ?bool $is_strict Set to true to make the redefined function use strict types.
 * @return bool - True on success or false on failure.
 */
function fnbind_function_add(string $funcname, Closure $closure, string $doc_comment = null, bool $is_strict = null) : bool {
}



