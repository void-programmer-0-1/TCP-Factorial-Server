# cython: language_level=3
# cython: boundscheck=False
# cython: wraparound=False
# cython: cdivision=True

from libc.stdint cimport uint64_t

cdef extern from "fib.hpp":
    uint64_t fib(uint64_t n) nogil


cpdef unsigned long long fibonacci(unsigned long long n):
    """
    Fast Fibonacci using C++ fast-doubling
    """
    cdef uint64_t result
    with nogil:
        result = fib(n)
    return result
