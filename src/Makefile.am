lib_LTLIBRARIES += lib/libCsSignal.la
lib_libCsSignal_la_SOURCES =

lib_libCsSignal_la_CXXFLAGS = $(AM_CXXFLAGS) \
   -DBUILD_DATE=\"`date +%Y-%m-%d`\"   \
   -DBUILDING_LIB_CS_SIGNAL

lib_libCsSignal_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined

# library
lib_libCsSignal_la_SOURCES += src/cs_signal.cpp  src/cs_slot.cpp

include_HEADERS = src/cs_signal.h src/cs_slot.h src/cs_internal.h \
                   src/cs_macro.h src/rcu_guarded.hpp src/rcu_list.hpp


# unit test
check_PROGRAMS = bin/test
bin_test_CXXFLAGS = $(AM_CFLAGS) -I$(top_srcdir)/src

bin_test_SOURCES = test/main.cpp  test/peach.cpp

noinst_HEADERS += test/peach.h

bin_test_LDADD = lib/libCsSignal.la

TESTS = bin/test
