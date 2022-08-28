QT += widgets

CONFIG+=c++2a
QMAKE_CXXFLAGS+="-std=c++2a"
linux-g++*: QMAKE_CXXFLAGS += "-fcoroutines -fconcepts"
INCLUDEPATH += $$PWD/../
FORMS += controller.ui
HEADERS += controller.h ../car/car.h
SOURCES += main.cpp controller.cpp ../car/car.cpp
INCLUDEPATH += /usr/local/include/ /usr/include

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# Work-around CI issue. Not needed in user code.
CONFIG += no_batch
CONFIG += system-zlib
# install
target.path = $$PWD
INSTALLS += target
LIBS += -L/usr/local/lib \
    -lcpprealm \
    -lrealm-object-store-dbg \
    -lrealm-sync-dbg \
    -lrealm-dbg \
    -lrealm-parser-dbg  \
    -lz -lcurl

linux-g++*: LIBS += -lssl -lcrypto
macos: LIBS += -framework Foundation -framework Security
DEPENDPATH += $$PWD/../car/
INCLUDEPATH += $$PWD/../car/
DEFINES += REALM_ENABLE_SYNC
