#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QDebug>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPainterPath>
#include <QPointF>
#include <QPushButton>
#include <QRandomGenerator>
#include <QRect>
#include <QResizeEvent>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QPolygonF>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

#include "barravida.h"
#include "personaje.h"
#include "proyectil.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
    MainWindow

    Conserva el flujo general del juego y el menú, pero varias tareas
    puntuales ahora se delegan a Personaje y BarraVida.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct AtaqueFreezer {
        float velocidad;
        float dano;
        QString sprite;
    };

    enum DificultadJuego {
        Facil,
        Dificil
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setEscena(short numEscena);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void on_botonInicio_clicked();
    void iniciarModoFacil();
    void iniciarModoDificil();
    void abrirNivel1DesdeSelector();
    void abrirNivel2DesdeSelector();
    void actualizarNivel1();

public slots:
    void moveFig();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *fondo;

    // Elementos antiguos conservados por compatibilidad.
    QGraphicsTextItem *text;
    QGraphicsLineItem *l1;
    QGraphicsEllipseItem *e1;
    QGraphicsItem *figEn;
    QGraphicsItem *fig;

    QTimer *timer;
    QTimer *cronometro;
    QTimer *timerFr;
    QTimer *timerNivel1;

    QString rutaFondoActual;

    QMediaPlayer *musicaInicio;
    QMediaPlayer *audioVegitoYosha;
    QMediaPlayer *audioBatazoMuchedumbre;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *salidaAudioInicio;
    QAudioOutput *salidaAudioVegitoYosha;
    QAudioOutput *salidaAudioBatazoMuchedumbre;
#endif

    QPushButton *botonFacil;
    QPushButton *botonDificil;
    QLabel *labelReglasTitulo;
    QLabel *labelReglasTexto;
    QPushButton *botonNivel1;
    QPushButton *botonNivel2;
    QLabel *labelNivel2Titulo;
    QLabel *labelNivel2Texto;

    Personaje *vegito;
    Personaje *freezer;

    BarraVida barraVidaVegito;
    BarraVida barraVidaFreezer;
    QGraphicsTextItem *textoEstadoKaioken;

    Proyectil *bolaFreezer;
    Proyectil *bolaControlada;
    QList<Proyectil*> bolasFreezer;
    QList<Proyectil*> bolasDisponibles;
    QList<QGraphicsRectItem*> obst;
    QList<QGraphicsPathItem*> debugZonas;

    QPainterPath zonaDanioBajo;
    QPainterPath zonaDanioMedio;
    QPainterPath zonaDanioAlto;

    float rachaDanio;
    int bolasPendientesPorLanzar;
    float velocidadFreezer;
    float x;
    float y;
    float ancho;
    float alto;
    DificultadJuego dificultadSeleccionada;
    QList<AtaqueFreezer> ataquesFreezer;

    bool lanzamientoOscilatorio;
    bool esperandoLanzamientos;
    bool nivel1Activo;
    bool puedeBatear;

    void mostrarMenuInicio();
    void ponerFondo(QString ruta, float opacity = 0.8);
    void ajustarFondo();
    void iniciarVariablesNivel1();
    void iniciarNivel1();
    void lanzarBolaFreezer();
    void programarSiguienteLanzamiento();
    void revisarCaidaBola(Proyectil *bola);
    void eliminarBola(Proyectil *bola);

    void crearBotonesDificultad();
    void mostrarOpcionesDificultad();
    void ocultarOpcionesDificultad();
    void crearTextosReglas();
    void mostrarPantallaReglas();
    void ocultarPantallaReglas();
    void crearPantallaNivel2();
    void mostrarPantallaNivel2();
    void ocultarPantallaNivel2();
    void ocultarElementosNivel1();

    void iniciarAudioInicio();
    void detenerAudioInicio();
    void iniciarAudiosBatazo();
    void reproducirAudiosBatazo();
    void configurarAtaquesFreezer();

    void configurarZonasBateo();
    void limpiarZonasDebug();
    void crearHUDNivel1();
    void actualizarHUD();
    void actualizarDificultad();
    void revisarEuforiaVegito();

    AtaqueFreezer obtenerAtaqueActual() const;
    Proyectil *obtenerBolaDisponible(const AtaqueFreezer &ataque,
                                     QPointF posicionInicial);
    bool bolaEnZonaBateo(Proyectil *bola);
    QPointF destinoAleatorioBateo();
    float calcularDanioFreezer(const QPointF &posicionCaida) const;

    void ganarNivel1();
    void perderNivel1();
};

#endif // MAINWINDOW_H
