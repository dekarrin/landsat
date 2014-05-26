CFLAGS=-Wall
LDADD=libstats.la libsort.la libdata.la
AM_LDFLAGS=

lib_LTLIBRARIES=libstats.la libsort.la libdata.la
libdata_la_SOURCES=data.cpp data.hpp
libstats_la_SOURCES=stats.cpp stats.hpp
libsort_la_SOURCES=sort.cpp sort.hpp

bin_PROGRAMS=test landsat
test_SOURCES=test.cpp
landsat_SOURCES=landsat.cpp landsat.hpp
landsat_LDADD=${LDADD} -ltiff
