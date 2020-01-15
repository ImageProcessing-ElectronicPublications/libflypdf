m4_define([flypdf_major_version], [0])
m4_define([flypdf_minor_version], [0])
m4_define([flypdf_micro_version], [0])
m4_define([flypdf_interface_age], [0])
m4_define([flypdf_binary_age],
          [m4_eval(100 * flypdf_minor_version + flypdf_micro_version)])
m4_define([flypdf_version],
          [flypdf_major_version.flypdf_minor_version.flypdf_micro_version])
m4_define([flypdf_lt_current],
          [m4_eval(100 * flypdf_minor_version + flypdf_micro_version - flypdf_interface_age)])
m4_define([flypdf_lt_revision], [flypdf_interface_age])
m4_define([flypdf_lt_age], [m4_eval(flypdf_binary_age - flypdf_interface_age)])

AC_INIT(libflypdf, [flypdf_version])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE


AM_CONFIG_HEADER(config.h)

m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL
AC_PROG_CXX
AM_PROG_CC_C_O

AC_ARG_WITH(python, AC_HELP_STRING([--with-python=NAME],
            [with python bindings, you may specify the python executable [[python]]]),
            [PYTHON="$with_python"],[with_python=auto])

AC_ARG_WITH(mono, AC_HELP_STRING([--with-mono],
	    [with mono/C# bindings [default=auto]]),,[with_mono=auto])

if test "$with_mono" != "no"
then
  AC_PATH_PROG(CSC, csc, no)
  AC_PATH_PROG(MCS, mcs, no)
  AC_PATH_PROG(MONO, mono, no)

  CS="C#"
  if test "x$CSC" = "xno" -a "x$MCS" = "xno"; then
    if test "$with_mono" = "yes"; then
      AC_MSG_ERROR([No $CS compiler found])
    else
      with_mono=no
    fi
  fi

  if test "x$MCS" = "xno"; then
    MCS=$CSC
  fi

  if test "x$MONO" = "xno"; then
    if test "$with_mono" = "yes"; then
      AC_MSG_ERROR([No mono runtime found])
    else
      with_mono=no
    fi
  fi
fi

AM_CONDITIONAL(WITH_MONO, [test "$with_mono" != "no"])


if test "$with_python" != "no"; then

  if test -z "${PYTHON}"; then
    AC_PATH_PROGS(PYTHON, python, [python],
                   [$PATH])
  fi

  AC_SUBST(PYTHON)

  PYTHON_VERSION=`${PYTHON} -c 'from distutils import sysconfig; \
    print sysconfig.get_config_var("VERSION")'`

  CPPFLAGS1=`${PYTHON} -c 'from distutils import sysconfig; \
    print "-I" + sysconfig.get_config_var("INCLUDEPY")'`

  CPPFLAGS2=`${PYTHON} -c 'from distutils import sysconfig; \
    print " ".join(filter(lambda x: x.startswith("-D"), \
    sysconfig.get_config_var("CFLAGS").split()))'`

  PYCFLAGS="${CPPFLAGS1} ${CPPFLAGS2}"

  AC_SUBST(PYCFLAGS)

  PYTHONFRAMEWORKDIR=`${PYTHON} -c 'from distutils import sysconfig; \
    print sysconfig.get_config_var("PYTHONFRAMEWORKDIR")'`

  PYTHONFRAMEWORK=`${PYTHON} -c 'from distutils import sysconfig; \
    print sysconfig.get_config_var("PYTHONFRAMEWORK")'`

  if test "${PYTHONFRAMEWORKDIR}" = "no-framework"; then
    LDFLAGS=`${PYTHON} -c 'import distutils.sysconfig; \
        print "-L" + distutils.sysconfig.get_python_lib(plat_specific=1, \
        standard_lib=1) +"/config"'`

    LDLIBS1="-lpython${PYTHON_VERSION}"
    LDLIBS2=`${PYTHON} -c 'from distutils import sysconfig; \
        print sysconfig.get_config_var("LIBS")'`

    LDLIBS="${LDLIBS1} ${LDLIBS2}"
  else
    LDFLAGS1="-framework ${PYTHONFRAMEWORK}"

    VERSION="${PYTHON_VERSION}"
    STRING="${PYTHONFRAMEWORKDIR}/Versions/${VERSION}/${PYTHONFRAMEWORK}"
    LDFLAGS2=`${PYTHON} -c "from distutils import sysconfig; \
        print sysconfig.get_config_var(\"LINKFORSHARED\").replace( \
        \"${STRING}\", '')"`

    LDFLAGS="${LDFLAGS1} ${LDFLAGS2}"

    LDLIBS=`${PYTHON} -c 'from distutils import sysconfig; \
        print sysconfig.get_config_var("LIBS")'`
  fi

  PYTHON_SITE_DIR="`$PYTHON -c 'from distutils import sysconfig; \
        print sysconfig.get_config_var("DESTLIB")'`/site-packages"

  if test -z "$PYTHON_SITE_DIR"; then
    if test "$with_python" != "auto"; then
      AC_MSG_ERROR([Unable to detect python site-packages path])
    else
      with_python=no
    fi
  fi

fi

if test "$with_python" != "no"; then
  PYLIBS="$LIBS $LDFLAGS $LDLIBS"
  AC_SUBST(PYLIBS)
  AC_SUBST(PYTHON_SITE_DIR)
fi

AM_CONDITIONAL(WITH_PYTHON, [test "$with_python" != "no"])


AC_DEFINE_UNQUOTED(ENCODINGS_PATH,
                    "./makefont",
                    [Define default fontdir])

AC_CHECK_LIB([z],[deflate])

AC_PATH_PROG(SWIG, swig, none)

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 flypdf_sharp/Makefile
		 libflypdf.pc
		 libflypdf_c.pc
		 flypdf_sharp/libflypdf-sharp.pc])

AC_OUTPUT
