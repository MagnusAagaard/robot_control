TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += wavefrontplanner.cpp

CONFIG += link_pkgconfig
PKGCONFIG += opencv
PKGCONFIG += gazebo
