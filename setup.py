from distutils.core import setup, Extension
from distutils.extension import Extension


# define the extension module
helium = Extension('helium', 
	sources=['helium.c'],
	 include_dirs=['/usr/include/'],
	extra_link_args=['-lhe','-lpthread'],
	extra_compile_args=['-lhe','-lpthread']
)

# run the setup
setup(
    ext_modules=[helium]
)
