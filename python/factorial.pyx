# cython: language_level=3
# cython: boundscheck=False
# cython: wraparound=False
# cython: nonecheck=False
# cython: cdivision=True

from libcpp.string cimport string

cdef extern from "big_factorial.hpp":
    string factorial_cpp(int n) nogil

cpdef bytes factorial(int n):
    cdef string res
    with nogil:
        res = factorial_cpp(n)
    return res   # std::string → bytes (no decode cost)
