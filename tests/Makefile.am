## Process this file with automake to produce Makefile.in

TESTS = check_sollar
check_PROGRAMS = check_sollar
check_sollar_SOURCES = check_sollar.c $(top_builddir)/src/sollar.h
check_sollar_CFLAGS = @CHECK_CFLAGS@
check_sollar_LDADD = $(top_builddir)/src/libsollar.la @CHECK_LIBS@
