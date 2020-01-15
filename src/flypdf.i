%module(directors="1") flypdf

%feature("director") Pdf;

%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}
	

%{
#include "flypdf.h"
%}

%include "std_string.i"
%include "fonts.h"
%include "flypdf.h"

