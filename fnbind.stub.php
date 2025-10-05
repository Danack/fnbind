<?php
// These should be passed to php-src/build/gen_stub.php to generate fnbind_arginfo.h

/**
 * Add a new function from a closure
 * Gives you more control over the type of function being created
 * (Signature 2 of 2)
 *
 * Aliases: fnbind_function_add
 *
 * @param string $funcname Name of function to be created
 * @param Closure $closure A closure to use as the source for this function. Static variables and `use` variables are copied.
 * @param string|null $doc_comment The doc comment of the function
 * @return bool - True on success. Throws on failure.
 */
function fnbind_add_closure(
	string $funcname,
	Closure $closure,
	string|null $doc_comment = null
) : bool {
}


