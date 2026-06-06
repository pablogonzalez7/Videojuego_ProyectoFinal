#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBrush>
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
#include <QPen>
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
#include <QtMath>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

#include "jugador.h"
#include "villano.h"
#include "proyectil.h"
#include "obstaculo.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
    MainWindow

    Conserva el flujo general del juego y el menú.
    La vida ahora pertenece a Personaje/Jugador/Villano y el dibujo
    del HUD se actualiza desde MainWindow.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
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
    void actualizarNivel2();
    void iniciarNivel2Facil();
    void iniciarNivel2Dificil();

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
    QPushButton *botonNivel2Facil;
    QPushButton *botonNivel2Dificil;
    QLabel *labelNivel2Titulo;
    QLabel *labelNivel2Texto;

    Jugador *vegito;
    Villano *freezer;
    Villano *gokuBlack;
    Sprite *gogeta;

    QGraphicsRectItem *fondoVidaVegito;
    QGraphicsRectItem *rellenoVidaVegito;
    QGraphicsRectItem *bordeVidaVegito;
    QGraphicsTextItem *textoVidaVegito;

    QGraphicsRectItem *fondoVidaFreezer;
    QGraphicsRectItem *rellenoVidaFreezer;
    QGraphicsRectItem *bordeVidaFreezer;
    QGraphicsTextItem *textoVidaFreezer;

    QGraphicsTextItem *textoEstadoKaioken;
    QGraphicsTextItem *textoEstadoNivel2;
    qreal anchoBarraVida;
    qreal altoBarraVida;

    Proyectil *bolaFreezer;
    Proyectil *bolaControlada;
    QList<Proyectil*> bolasFreezer;
    QList<Proyectil*> bolasDisponibles;
    QList<Proyectil*> bolasBlack;
    QList<Proyectil*> bolasBlackDisponibles;
    QList<Obstaculo*> obstaculos;
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
    QList<Villano::Ataque> ataquesFreezer;

    bool lanzamientoOscilatorio;
    bool esperandoLanzamientos;
    bool nivel1Activo;
    bool nivel2Activo;
    bool gogetaSaltando;
    bool puedeBatear;
    bool nivel2EnPausa;
    bool esperandoLanzamientoBlack;
    bool nivel2DificilActivo;
    bool gogetaKamehamehaActivo;
    bool danioKamehamehaAplicado;
    QString rutaSpriteGogetaActual;
    QString rutaSpriteGogetaDireccion;
    qreal posicionGogetaX;
    qreal posicionGogetaY;
    qreal posicionActualGogetaY;
    qreal velocidadVerticalGogeta;
    qreal anchoVisualGogeta;
    qreal altoVisualGogeta;
    qreal vidaGogetaNivel2;
    qreal vidaMaximaGogetaNivel2;
    qreal vidaBlackNivel2;
    qreal vidaMaximaBlackNivel2;
    int saltosUsadosGogeta;
    int tiempoAnimacionGogetaMs;
    int tiempoEntreFramesGogetaMs;
    int tiempoRetencionKamehamehaMs;
    int tiempoMovimientoBlackDificilMs;
    int tiempoAtaqueEspecialBlackMs;
    int pelotasAtrapadasNivel2;
    int pelotasPerdidasNivel2;
    int outsNivel2;
    int carrerasBlackNivel2;
    int cargaKamehamehaNivel2;
    int cargaMaximaKamehamehaNivel2;
    QTimer *timerNivel2;

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
    void ocultarElementosNivel2();

    void iniciarAudioInicio();
    void detenerAudioInicio();
    void iniciarAudiosBatazo();
    void reproducirAudiosBatazo();
    void configurarAtaquesFreezer();

    void configurarZonasBateo();
    void limpiarZonasDebug();
    void crearHUDNivel1();
    void crearHUDNivel2Dificil();
    void crearBarraVida(QGraphicsRectItem *&fondoBarra,QGraphicsRectItem *&rellenoBarra,QGraphicsRectItem *&bordeBarra,QGraphicsTextItem *&textoBarra,qreal x,qreal y,const QString &etiqueta,const QColor &colorRelleno);
    void actualizarBarraVida(QGraphicsRectItem *rellenoBarra,QGraphicsTextItem *textoBarra,qreal x,qreal y,const QString &etiqueta,float vidaActual,float vidaMaxima);
    void setVisibleHUDVida(bool visible);
    void destruirHUDVida();
    void actualizarHUD();
    void actualizarDificultad();
    void revisarEuforiaVegito();
    void iniciarVariablesNivel2();
    void programarSiguienteLanzamientoBlack();
    void lanzarBolaBlack();
    void lanzarAtaqueEspecialBlack();
    void eliminarBolaBlack(Proyectil *bola);
    void actualizarSaltoGogeta(int dtMs);
    void actualizarBlackDificil(int dtMs);
    void actualizarKamehamehaGogeta(int dtMs);
    void actualizarTextoNivel2();
    void finalizarNivel2(const QString &mensaje);
    void aplicarDanioGogetaNivel2(qreal dano);
    void aplicarDanioBlackNivel2(qreal dano);
    void dispararFinalKamehameha();
    bool spriteColisionaConProyectil(Sprite *sprite, Proyectil *bola) const;

    Villano::Ataque obtenerAtaqueActual() const;
    Proyectil *obtenerBolaDisponible(const Villano::Ataque &ataque,QPointF posicionInicial);
    bool bolaEnZonaBateo(Proyectil *bola);
    QPointF destinoAleatorioBateo();
    float calcularDanioFreezer(const QPointF &posicionCaida) const;

    void ganarNivel1();
    void perderNivel1();
    void moverGogeta(qreal deltaX, const QString &rutaSprite);
    void saltarGogeta();
};

#endif // MAINWINDOW_H
