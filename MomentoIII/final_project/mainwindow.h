#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QResizeEvent>

#include "sprite.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    sprite *mostrarSprite(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY);
    void setEscena(short numEscena);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_botonInicio_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *fondo;
    QTimer *timer, *cronometro;
    QString rutaFondoActual;

    void mostrarMenuInicio();
    void ponerFondo(QString);

    void ajustarFondo();

    float x, y, ancho, alto;
};
#endif // MAINWINDOW_H
