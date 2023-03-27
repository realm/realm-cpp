QT += qml quick

CONFIG += c++17
QMAKE_CXXFLAGS+="-std=c++17"
INCLUDEPATH += /usr/local/include/ /usr/include/

SOURCES += main.cpp \
    coffee_manager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/imports

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

qnx: target.path = /tmp/$${TARGET}/bin
else: win32|unix: target.path = $$[QT_INSTALL_EXAMPLES]/demos/$${TARGET}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/ui components/loader/loading bar 1.png \
    images/ui components/loader/loading bar 2.png \
    images/ui components/loader/loading bar 3.png \
    images/ui components/loader/loading bg.png

HEADERS += \
    coffee_manager.hpp \
    realm_models.hpp

CONFIG += qmltypes
QML_IMPORT_NAME = io.mongo.barista
QML_IMPORT_MAJOR_VERSION = 1

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
CONFIG += system-zlib
# install
target.path = $$PWD
INSTALLS += target
LIBS += -L/usr/local/lib \
    -lcpprealm \
    -lrealm-object-store \
    -lrealm-sync \
    -lrealm \
    -lrealm-parser \
    -lz -lcurl

linux-g++*: LIBS += -lssl -lcrypto
macos: LIBS += -framework Foundation -framework Security -lcompression
DEFINES += REALM_ENABLE_SYNC
