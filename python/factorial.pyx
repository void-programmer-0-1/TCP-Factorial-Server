# cython: language_level=3
# cython: boundscheck=False
# cython: wraparound=False

from libcpp.string cimport string

cdef extern from "big_factorial.hpp":
    string factorial_cpp(int n)

cpdef str factorial(int n):
    cdef string res = factorial_cpp(n)
    return res.decode("utf-8")
