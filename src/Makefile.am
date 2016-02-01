lib_LTLIBRARIES += lib/libCsSignal.la
lib_libCsSignal_la_SOURCES =

lib_libCsSignal_la_CXXFLAGS = $(AM_CXXFLAGS) \
   -DBUILD_DATE=\"`date +%Y-%m-%d`\"

lib_libCsSignal_la_CFLAGS  = $(AM_CFLAGS)
lib_libCsSignal_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined

noinst_HEADERS += \
   src/signal.h

lib_libCsSignal_la_SOURCES +=


check_PROGRAMS = bin/test
bin_test_SOURCES = test/main.cpp
TESTS = bin/test
