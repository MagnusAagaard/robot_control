TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += path.cpp

CONFIG += link_pkgconfig
PKGCONFIG += gazebo
LIBS += -L/home/jacob/Desktop/fuzzylite-6.0/fuzzylite/release/bin -lfuzzylite-static
INCLUDEPATH += "/home/jacob/Desktop/fuzzylite-6.0/fuzzylite"
