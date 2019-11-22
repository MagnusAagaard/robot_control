TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
SOURCES += Particle.cpp
SOURCES += ParticleFilter.cpp

HEADERS += ParticleFilter.h
HEADERS += Particle.h

CONFIG += link_pkgconfig
PKGCONFIG += opencv
PKGCONFIG += gazebo
