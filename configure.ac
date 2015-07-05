# Process this file with autoconf to produce a configure script.

# Prelude.
AC_INIT([Sollar], [0.1])
AM_PROG_AR
LT_INIT
AC_LANG(C)

# unique source file --- primitive safety check
AC_CONFIG_SRCDIR([src/])

# fairly severe build strictness
# change foreign to gnu or gnits to comply with gnu standards
AM_INIT_AUTOMAKE([-Wall foreign 1.9.6])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
AM_PROG_CC_C_O

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h SDL/SDL_audio.h SDL/SDL_mixer.h])
CFLAGS="-w -Wall -lGL -lglut -lGLU -lSDL -lm -lSDL_mixer -std=c11 -pedantic-errors"

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
