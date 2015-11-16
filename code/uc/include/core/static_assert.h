/*!
 * @file static_assert.h
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 22:37
 */

/*!
 * @brief Allows for compile-time checks.
 * @param[in] X A compiler error is generated if the condition X is untrue.
 * 
 * See http://stackoverflow.com/questions/3385515/static-assert-in-c/3385694#3385694
 * GCC 4.3 (I guess) introduced the "error" and "warning" function attributes.
 * If a call to a function with that attribute could not be eliminated through
 * dead code elimination (or other measures) then an error or warning is
 * generated. This can be used to make compile time asserts with user defined
 * failure descriptions. It remains to determine how they can be used in
 * namespace scope without resorting to a dummy function:
 */
#ifdef _DEBUG
#define static_assert(X) ({                                                   \
    extern int __attribute__((error("assertion failure: '" #X "' not true"))) \
    compile_time_check();                                                     \
    ((X)?0:compile_time_check()),0;                                           \
})
#else
#define static_assert(X)
#endif
