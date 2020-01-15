flypdf_sharpdir=$(pkglibdir)
flypdf_sharp_SCRIPTS = flypdf-sharp.dll
CLEANFILES = flypdf-sharp.dll

flypdf_sharp_sources = flypdf.cs Properties/AssemblyInfo.cs

flypdf-sharp.dll: $(flypdf_sharp_sources)
	$(MCS) -target:library -out:$@ $^

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libflypdf-sharp.pc

