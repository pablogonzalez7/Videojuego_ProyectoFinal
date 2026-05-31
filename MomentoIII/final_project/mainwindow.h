#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QRectF>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QList>
#include <QPointF>
#include <QPushButton>
#include <QRandomGenerator>
#include <QFont>
#include <QBrush>
#include <QPen>

#include "personaje.h"
#include "proyectil.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
    MainWindow

    Maneja la ventana principal, la escena gráfica, el menú,
    el nivel 1, los personajes, las bolas, el puntaje y las vidas.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /*
        Función reservada para cambiar escenas.

        Por ahora no se usa de forma completa, pero se mantiene
        para futuras pantallas o niveles.
    */
    void setEscena(short numEscena);

protected:
    /*
        Ajusta el fondo y los elementos visuales cuando cambia
        el tamaño de la ventana.
    */
    void resizeEvent(QResizeEvent *event) override;

    /*
        Maneja las teclas del juego.

        P: batear.
        A, D, W, S: ajustar caída de la bola después del batazo.
    */
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    /*
        Slot del botón de inicio.

        Oculta el menú y carga el nivel 1.
    */
    void on_botonInicio_clicked();

    /*
        Timer principal del nivel 1.

        Actualiza la posición de las bolas, revisa vidas,
        puntaje, caída y victoria/derrota.
    */
    void actualizarNivel1();

public slots:
    /*
        Slot conservado por compatibilidad con el código anterior.

        Llama a lanzarBolaFreezer().
    */
    void moveFig();

private:
    Ui::MainWindow *ui;

    // Escena principal del juego.
    QGraphicsScene *scene;

    // Fondo de la escena.
    QGraphicsPixmapItem *fondo;

    // Elementos antiguos o de prueba que se conservan.
    QGraphicsTextItem *text;
    QGraphicsLineItem *l1;
    QGraphicsEllipseItem *e1;

    // Timers generales.
    QTimer *timer;
    QTimer *cronometro;
    QTimer *timerFr;

    // Timer principal para actualizar el nivel 1.
    QTimer *timerNivel1;

    // Ruta del fondo actual, usada al redimensionar.
    QString rutaFondoActual;

    // Items antiguos o de prueba conservados.
    QGraphicsItem *figEn;
    QGraphicsItem *fig;

    // Personajes principales del nivel 1.
    Personaje *vegito;
    Personaje *freezer;

    // Bola actual y bola que el jugador está controlando.
    Proyectil *bolaFreezer;
    Proyectil *bolaControlada;

    // Lista de bolas activas. Permite manejar una o dos bolas.
    QList<Proyectil*> bolasFreezer;

    // Lista de obstáculos de prueba o futuros obstáculos.
    QList<QGraphicsRectItem*> obst;

    // Panel y textos del HUD.
    QGraphicsRectItem *panelHUD;
    QGraphicsTextItem *textoPuntaje;
    QGraphicsTextItem *textoVidas;

    // Zonas de puntaje del campo.
    QRectF zona10;
    QRectF zona20;
    QRectF zona30;

    // Variables de puntaje.
    int puntaje;
    int rachaPuntos;
    int bolasPendientesPorLanzar;

    // Variables de vidas y dificultad.
    float vidas;
    float velocidadFreezer;
    float x, y, ancho, alto;

    // Estados del nivel.
    bool lanzamientoOscilatorio;
    bool esperandoLanzamientos;
    bool nivel1Activo;
    bool puedeBatear;

    /*
        Muestra el menú inicial.
    */
    void mostrarMenuInicio();

    /*
        Coloca el fondo de la escena.

        opacity permite controlar transparencia del fondo.
    */
    void ponerFondo(QString ruta, float opacity = 0.8);

    /*
        Ajusta el fondo al tamaño actual de la ventana.
    */
    void ajustarFondo();

    /*
        Inicializa las variables del nivel 1.
    */
    void iniciarVariablesNivel1();

    /*
        Crea una nueva bola desde Freezer.
    */
    void lanzarBolaFreezer();

    /*
        Programa el siguiente lanzamiento.

        Si hay racha suficiente, puede programar dos bolas.
    */
    void programarSiguienteLanzamiento();

    /*
        Revisa dónde cayó la bola bateada.

        Suma puntos o resta vida según la zona.
    */
    void revisarCaidaBola(Proyectil *bola);

    /*
        Elimina una bola de la escena y de la lista.
    */
    void eliminarBola(Proyectil *bola);

    /*
        Actualiza los textos de puntaje y vidas.
    */
    void actualizarHUD();

    /*
        Actualiza dificultad según el puntaje:

        0-250: parabólico.
        250-500: oscilatorio.
        500-750: parabólico.
        750-1000: oscilatorio.
    */
    void actualizarDificultad();

    /*
        Finaliza el nivel con victoria.
    */
    void ganarNivel1();

    /*
        Finaliza el nivel con derrota.
    */
    void perderNivel1();

    /*
        Revisa si la bola está tocando a Vegito.

        Se usa para decidir si el batazo fue válido.
    */
    bool bolaEnZonaBateo(Proyectil *bola);

    /*
        Genera un destino aleatorio para la bola bateada.

        A veces cae dentro, a veces afuera, para obligar a usar A/D/W/S.
    */
    QPointF destinoAleatorioBateo();
};

#endif // MAINWINDOW_H
