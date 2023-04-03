QT += widgets

CONFIG+=c++17
QMAKE_CXXFLAGS+="-std=c++17"
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
    -lrealm-object-store \
    -lrealm-sync \
    -lrealm \
    -lrealm-parser \
    -lz -lcurl

linux-g++*: LIBS += -lssl -lcrypto
macos: LIBS += -framework Foundation -framework Security -lcompression
DEPENDPATH += $$PWD/../car/
INCLUDEPATH += $$PWD/../car/
DEFINES += REALM_ENABLE_SYNC REALM_PLATFORM_APPLE=0
