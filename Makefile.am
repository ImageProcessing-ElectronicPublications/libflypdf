SUBDIRS = src
ACLOCAL_AMFLAGS = -I m4

if WITH_MONO
 SUBDIRS += flypdf_sharp
endif

libincludedir = $(includedir)/flypdf
libinclude_HEADERS =    src/flypdf.h \
			src/flypdf_c.h \
			src/images.h \
			src/truetype.h \
			src/fonts.h
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libflypdf.pc \
		 libflypdf_c.pc

