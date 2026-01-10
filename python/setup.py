from setuptools import setup, Extension
from Cython.Build import cythonize

ext = Extension(
    name="factorial",
    sources=["factorial.pyx"],
    language="c++",
    extra_compile_args=["-O3", "-std=c++20"],
)

setup(
    ext_modules=cythonize(ext, compiler_directives={"language_level": "3"})
)


# python setup.py build_ext --inplace
