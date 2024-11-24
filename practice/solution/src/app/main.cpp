/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below is a application designed to work as part of a water research module. 
 The application communicates with the microcontroller via the serial port and allows monitoring the operation of sensors.

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow gui;
    gui.show();
    return app.exec();
}
