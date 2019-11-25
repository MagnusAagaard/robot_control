TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += AStar.cpp
SOURCES += Node.cpp

HEADERS += Node.h

CONFIG += link_pkgconfig
PKGCONFIG += opencv
PKGCONFIG += gazebo
