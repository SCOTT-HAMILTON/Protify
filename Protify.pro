QT += quick network

CONFIG += c++17 lrelease embed_translations

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        backend.cpp \
        communicationmanager.cpp \
        followedprocessitem.cpp \
        main.cpp \
        processitem.cpp

RESOURCES += qml.qrc \
    icons.qrc

TRANSLATIONS += \
    translations/Protify_fr_FR.ts
QM_FILES_RESOURCE_PREFIX = /translations


include(QtZeroConf/qtzeroconf.pri)
include(qt-qml-models/QtQmlModels.pri)
include(libproc.pri)

DEFINES += QZEROCONF_STATIC

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${QT_INSTALL_PREFIX}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    backend.h \
    communicationmanager.h \
    followedprocessitem.h \
    processitem.h
