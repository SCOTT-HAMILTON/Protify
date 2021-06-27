HEADERS += \
        $$PWD/libproc/include/periodic_watcher.hh \
        $$PWD/libproc/include/helper.hh \
        $$PWD/libproc/include/process.hh \
        $$PWD/libproc/include/plib.hh \
        $$PWD/libproc/include/watcher.hh \
        $$PWD/libproc/include/pstat.hh \
        $$PWD/libproc/include/mstat.hh \
	$$PWD/libproc/include/error.hh
SOURCES +=  \
        $$PWD/libproc/src/plib.cc \
        $$PWD/libproc/src/periodic_watcher.cc \
        $$PWD/libproc/src/fs.cc \
        $$PWD/libproc/src/sort.cc \
        $$PWD/libproc/src/helper.cc \
        $$PWD/libproc/src/watcher.cc \
	$$PWD/libproc/src/process.cc

INCLUDEPATH += $$PWD/libproc/include
DEPENDPATH += $$PWD/libproc/include
