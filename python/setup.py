from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    name="fib",
    sources=["fib.pyx"],
    language="c++",
    extra_compile_args=["-O3", "-std=c++20"],
)

setup(
    name="fib",
    ext_modules=cythonize(
        [ext],
        compiler_directives={"language_level": "3"}
    ),
)

# python setup.py build_ext --inplace
