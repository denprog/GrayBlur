TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

INCLUDEPATH += $(OPENCV)

SOURCES += \
        main.cpp \
        pool.cpp

HEADERS += \
    pool.h

LIBS += -lpthread -lopencv_core -lopencv_imgcodecs -lboost_program_options

DISTFILES += \
    CMakeLists.txt