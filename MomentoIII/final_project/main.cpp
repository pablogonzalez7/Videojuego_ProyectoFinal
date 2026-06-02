#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // QApplication prepara el sistema de eventos de Qt y mantiene viva la app
    // mientras existan ventanas abiertas o el ciclo principal siga ejecutándose.
    QApplication a(argc, argv);

    // MainWindow concentra el menú y la lógica del videojuego.
    MainWindow w;
    w.show();

    // Aquí se entra al loop principal que procesa teclas, timers, repintados y
    // señales/slots hasta que el usuario cierre la aplicación.
    return QCoreApplication::exec();
}
