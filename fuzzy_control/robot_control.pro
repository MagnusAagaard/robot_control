TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += obstacleavoidance.cpp

CONFIG += link_pkgconfig
PKGCONFIG += gazebo
LIBS += -L/home/magnus/libraries/fuzzylite/fuzzylite/release/bin -lfuzzylite-static
INCLUDEPATH += "/home/magnus/libraries/fuzzylite/fuzzylite"
