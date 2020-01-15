EXTRA_DIST = flypdf.i

lib_LTLIBRARIES = libflypdf.la libflypdf_c.la

libflypdf_la_SOURCES =  images.cpp \
			flypdf.cpp \
			fonts.cpp \
			jpegsize.cpp \
			truetype.cpp \
			unicode.cpp \
			ttreduction.cpp

libflypdf_c_la_SOURCES = flypdf_c.cpp
libflypdf_c_la_LIBADD = libflypdf.la


if WITH_PYTHON
pythonSitePackagesdir = $(PYTHON_SITE_DIR)

pythonSitePackages_LTLIBRARIES = _flypdfmodule.la
pythonSitePackages_SCRIPTS = flypdf.py

_flypdfmodule_la_LDFLAGS = -avoid-version -module
_flypdfmodule_la_SOURCES = flypdf_wrap.cxx
_flypdfmodule_la_LIBADD = libflypdf.la
_flypdfmodule_la_CXXFLAGS = $(PYCFLAGS) $(PYLDFLAGS)

SWIG_ARGS = -c++

flypdf_wrap.cxx: flypdf.i
	$(SWIG) -python $(SWIG_ARGS) -o $@ $<
endif
