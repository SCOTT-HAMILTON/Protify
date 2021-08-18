QT += quickcontrols2 network

CONFIG += c++17 lrelease embed_translations network

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
		backend.cpp \
		communicationmanager.cpp \
		followedprocessitem.cpp \
		main.cpp \
		processitem.cpp \
		processlister.cpp

HEADERS += \
	backend.h \
	communicationmanager.h \
	followedprocessitem.h \
	processitem.h \
	processlister.h

LIBS +=	-lzmq

RESOURCES += qml.qrc \
	icons.qrc

TRANSLATIONS += \
	translations/Protify_fr_FR.ts
QM_FILES_RESOURCE_PREFIX = /translations


include(QtZeroConf/qtzeroconf.pri)
include(qt-qml-models/QtQmlModels.pri)
win32: include(libxprocs.pri)
unix:!android: include(libproc.pri)

win32: LIBS +=  -lWindowsApp

DEFINES += QZEROCONF_STATIC

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
target = Protify
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: {
	target.path = $${QT_INSTALL_PREFIX}/bin

	iconscalable.path = $${QT_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
	iconscalable.files = icons/scalable/Protify.svg

	desktop.path = $${QT_INSTALL_PREFIX}/share/applications
	desktop.files = desktop/protify.desktop

	INSTALLS += iconscalable desktop
}
!isEmpty(target.path): INSTALLS += target
