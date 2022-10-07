#/***************************************************
# Water Research Module
# <https://github.com/TonyCooT/water_research_module>
#
# ***************************************************
# The code below is a application designed to work as part of a water research module.
# The application communicates with the microcontroller via the serial port and allows monitoring the operation of sensors.
#
# Created 2022-09-28
# By TonyCooT <https://github.com/TonyCooT>
#
# Apache License 2.0.
# See <https://www.apache.org/licenses/> for details.
# All above must be included in any redistribution.
# ****************************************************/

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
