QT += widgets

CONFIG+=c++17
QMAKE_CXXFLAGS+="-std=c++17"

HEADERS += car.h
SOURCES += car.cpp main.cpp
INCLUDEPATH += /usr/local/include/ /usr/include/

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
#CONFIG += system-zlib
# install
target.path = $$PWD
INSTALLS += target
LIBS += -L/usr/local/lib \
    -lcpprealm \
    -lz -lcurl \
    -lrealm-object-store-dbg \
    -lrealm-sync-dbg \
    -lrealm-dbg \
    -lrealm-parser-dbg

linux-g++*: LIBS += -lssl -lcrypto
macos: LIBS += -framework Foundation -framework Security -lcompression
DEFINES += REALM_ENABLE_SYNC REALM_PLATFORM_APPLE=1

RESOURCES += \
    resources.qrc
