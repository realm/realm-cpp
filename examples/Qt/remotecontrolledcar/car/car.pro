QT += widgets

CONFIG+=c++17
QMAKE_CXXFLAGS+="-std=c++17"

HEADERS += car.h
SOURCES += car.cpp main.cpp
INCLUDEPATH += /usr/local/include/ /usr/include/

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

RESOURCES += \
    resources.qrc
