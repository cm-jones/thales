from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext
import os
import sys
import subprocess
import platform

# Find pybind11 headers
import pybind11
PYBIND11_INCLUDE = pybind11.get_include()

# Determine the root directory of the thales project
THALES_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the extension")
            
        for ext in self.extensions:
            self.build_extension(ext)
            
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DTHALES_ROOT=' + THALES_ROOT
        ]
        
        # Configure build type
        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]
        
        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
        build_args += ['--', '-j2']
        
        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
            
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
            
        # CMake configure and build
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, 
                             cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, 
                             cwd=self.build_temp)

setup(
    name="thales_backtester",
    version="0.1.0",
    author="Thales Trading Systems",
    author_email="info@thalestrading.com",
    description="Python backtesting framework for Thales options trading strategies",
    long_description="",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.8",
    install_requires=[
        "pandas>=1.3.0",
        "numpy>=1.20.0",
        "matplotlib>=3.4.0",
        "seaborn>=0.11.0",
        "pybind11>=2.6.0",
        "pytest>=6.2.5",
        "scipy>=1.7.0",
    ],
    ext_modules=[
        CMakeExtension("thales_backtester.thales_cpp_bindings"),
    ],
    cmdclass={
        "build_ext": CMakeBuild,
    },
)
