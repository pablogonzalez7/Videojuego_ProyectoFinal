#include "mainwindow.h"
#include "jugador.h"
#include "proyectil.h"
#include "ui_mainwindow.h"

#include <QAudioOutput>
#include <QColor>
#include <QFont>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QLabel>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QPolygonF>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QTimer>
#include <QUrl>
#include <QtMath>
#include <exception>
#include <stdexcept>

namespace {
constexpr qreal kAnchoBarraVida = 280.0;
constexpr qreal kAltoBarraVida = 24.0;

void configurarReinicioAudio(QMediaPlayer *player, QObject *context)
{
    QObject::connect(player,&QMediaPlayer::mediaStatusChanged,context,[player](QMediaPlayer::MediaStatus status)
     {
            if (status == QMediaPlayer::EndOfMedia && player != nullptr) {
                player->setPosition(0);
                player->play();
            }
        });
}

void configurarAudio(QMediaPlayer *player, QAudioOutput *salidaAudio, const QString &ruta, int volumenPorcentaje)
{
    salidaAudio->setVolume(volumenPorcentaje / 100.0);
    player->setAudioOutput(salidaAudio);
    player->setSource(QUrl(ruta));
}

bool audioEstaReproduciendose(QMediaPlayer *player)
{
    return player != nullptr && player->playbackState() == QMediaPlayer::PlayingState;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , fondo(nullptr)
    , timerNivel1(nullptr)
    , musicaInicio(nullptr)
    , audioVegitoYosha(nullptr)
    , audioBatazoMuchedumbre(nullptr)
    , audioAmbienteEstadio(nullptr)
    , audioFinalKamehameha(nullptr)
    , botonFacil(nullptr)
    , botonDificil(nullptr)
    , labelReglasTitulo(nullptr)
    , labelReglasTexto(nullptr)
    , botonNivel1(nullptr)
    , botonNivel2Facil(nullptr)
    , botonNivel2Dificil(nullptr)
    , labelNivel2Titulo(nullptr)
    , labelNivel2Texto(nullptr)
    , labelOverlayTitulo(nullptr)
    , labelOverlayTexto(nullptr)
    , botonOverlayPrimario(nullptr)
    , botonOverlaySecundario(nullptr)
    , botonOverlayTerciario(nullptr)
    , vegito(nullptr)
    , freezer(nullptr)
    , gokuBlack(nullptr)
    , gogeta(nullptr)
    , rafagaFinalKamehameha(nullptr)
    , textoEstadoKaioken(nullptr)
    , textoEstadoNivel2(nullptr)
    , bolaControlada(nullptr)
    , timerNivel2(nullptr)
    , salidaAudioInicio(nullptr)
    , salidaAudioVegitoYosha(nullptr)
    , salidaAudioBatazoMuchedumbre(nullptr)
    , salidaAudioAmbienteEstadio(nullptr)
    , salidaAudioFinalKamehameha(nullptr)
{
    ui->setupUi(this);

    connect(ui->botonInicio,
            &QPushButton::clicked,
            this,
            &MainWindow::on_botonInicio_clicked);

    rachaDaño = 0.0f;
    velocidadFreezer = 3.6f;
    lanzamientoOscilatorio = false;
    dificultadSeleccionada = Facil;
    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;
    nivel1Activo = false;
    nivel2Activo = false;
    gogetaSaltando = false;
    puedeBatear = true;
    nivel2EnPausa = false;
    esperandoLanzamientoBlack = false;
    nivel2DificilActivo = false;
    gogetaKamehamehaActivo = false;
    dañoKamehamehaAplicado = false;
    rafagaFinalKamehamehaLanzada = false;
    moverGogetaIzquierdaActivo = false;
    moverGogetaDerechaActivo = false;
    rutaSpriteGogetaActual = "";
    rutaSpriteGogetaDireccion = ":/images/sprites/gogeta_derecha.png";
    posicionGogetaX = 650;
    posicionGogetaY = 585;
    posicionActualGogetaY = posicionGogetaY;
    velocidadVerticalGogeta = 0.0;
    anchoVisualGogeta = 90.0;
    altoVisualGogeta = 150.0;
    vidaGogetaNivel2 = 100.0;
    vidaMaximaGogetaNivel2 = 100.0;
    vidaBlackNivel2 = 100.0;
    vidaMaximaBlackNivel2 = 100.0;
    saltosUsadosGogeta = 0;
    tiempoAnimacionGogetaMs = 0;
    tiempoEntreFramesGogetaMs = 60;
    tiempoRetencionKamehamehaMs = 0;
    tiempoMovimientoBlackDificilMs = 0;
    tiempoAtaqueEspecialBlackMs = 0;
    pelotasAtrapadasNivel2 = 0;
    pelotasPerdidasNivel2 = 0;
    outsNivel2 = 0;
    carrerasBlackNivel2 = 0;
    cargaKamehamehaNivel2 = 0;
    cargaMaximaKamehamehaNivel2 = 5;
    velocidadMovimientoGogeta = 11.5;
    velocidadRafagaFinalKamehameha = 14.0;

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

   ponerFondo(":/images/backgrounds/background_main.png");
    scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

    ui->botonInicio->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/images/backgrounds/botonInicio.png) 0 0 0 0 stretch stretch;"
        "border: none;"
        "background: transparent;"
        "}"
        );

    QPixmap pixmap(":/images/backgrounds/titulo.png");
    ui->labelTitulo->setPixmap(pixmap.scaled(400,150,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->labelTitulo->setAlignment(Qt::AlignCenter);

    iniciarAudioInicio();
    iniciarAudiosBatazo();
    crearBotonesDificultad();
    crearTextosReglas();
    crearPantallaNivel2();
    crearOverlayEstado();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    ocultarOverlayEstado();
}

MainWindow::~MainWindow()
{
    limpiarRafagaFinalKamehameha();

    while (!bolasFreezer.isEmpty()) {
        delete bolasFreezer.takeLast();
    }

    while (!bolasBlack.isEmpty()) {
        delete bolasBlack.takeLast();
    }

    destruirHUDVida();

    delete gogeta;
    delete vegito;
    delete freezer;
    delete gokuBlack;
    delete ui;
}

void MainWindow::ponerFondo(QString ruta, float opacity)
{
    QPixmap imagenFondo(ruta);
    rutaFondoActual = ruta;

    if (fondo != nullptr) {
        scene->removeItem(fondo);
        delete fondo;
        fondo = nullptr;
    }

    imagenFondo = imagenFondo.scaled(ui->graphicsView->width(),ui->graphicsView->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    fondo = scene->addPixmap(imagenFondo);
    fondo->setPos(0, 0);
    fondo->setZValue(-10);
    fondo->setOpacity(opacity);
}

void MainWindow::mostrarMenuInicio()
{
    ocultarElementosNivel1();
    ocultarElementosNivel2();
    ocultarOverlayEstado();
    detenerAudioAmbienteEstadio();
    ponerFondo(":/images/backgrounds/background_main.png");
    iniciarAudioInicio();
}

void MainWindow::volverAlMenuPrincipal()
{
    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    nivel1Activo = false;
    nivel2Activo = false;
    nivel2EnPausa = false;
    esperandoLanzamientos = false;
    esperandoLanzamientoBlack = false;
    ui->labelTitulo->show();
    ui->botonInicio->show();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    mostrarMenuInicio();
}

void MainWindow::manejarErrorJuego(const QString &contexto, const std::exception &error)
{
    nivel1Activo = false;
    nivel2Activo = false;
    nivel2EnPausa = true;
    esperandoLanzamientos = false;
    esperandoLanzamientoBlack = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    QMessageBox::critical(this,"Error al cargar recursos",contexto +"\n\nDetalle: "
     +QString::fromLocal8Bit(error.what()) +"\n\nRevise que el archivo exista en el .qrc y que la ruta coincida exactamente.");
}

void MainWindow::on_botonInicio_clicked()
{
    detenerAudioInicio();
    detenerAudioAmbienteEstadio();
    ui->botonInicio->hide();
    mostrarPantallaReglas();
}

void MainWindow::iniciarModoFacil()
{
    dificultadSeleccionada = Facil;
    iniciarNivel1();
}

void MainWindow::iniciarModoDificil()
{
    dificultadSeleccionada = Dificil;
    iniciarNivel1();
}

void MainWindow::abrirNivel1DesdeSelector()
{
    nivel2Activo = false;
    detenerAudioAmbienteEstadio();
    ponerFondo(":/images/backgrounds/background_main.png");
    ui->labelTitulo->show();
    ocultarPantallaNivel2();
    ocultarElementosNivel2();
    ui->botonInicio->hide();
    mostrarPantallaReglas();
}

void MainWindow::iniciarNivel2Facil() try
{
    detenerAudioInicio();
    iniciarAudioAmbienteEstadio();
    ocultarOverlayEstado();
    dificultadSeleccionada = Facil;
    nivel1Activo = false;
    nivel2Activo = true;
    nivel2EnPausa = false;
    nivel2DificilActivo = false;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    ui->labelTitulo->hide();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    ocultarElementosNivel1();

    ponerFondo(":/images/backgrounds/nivel2.png", 1.0);

    if (gogeta == nullptr) {
        gogeta = new Sprite(":/images/sprites/gogeta_derecha.png", 3);
        gogeta->setMantenerTamañoVisual(true);
        scene->addItem(gogeta);
        gogeta->setZValue(8);
    }
    else if (gogeta->scene() != scene) {
        scene->addItem(gogeta);
    }

    const QRectF rectaNivel2 = scene->sceneRect();

    posicionGogetaX = rectaNivel2.left() + rectaNivel2.width() * 0.18;
    posicionGogetaY = rectaNivel2.bottom() - 170;
    posicionActualGogetaY = posicionGogetaY;
    rutaSpriteGogetaActual = ":/images/sprites/gogeta_derecha.png";
    rutaSpriteGogetaDireccion = rutaSpriteGogetaActual;
    gogetaSaltando = false;
    gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
    gogeta->fijarTamañoVisual(anchoVisualGogeta, altoVisualGogeta);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    gogeta->setVisible(true);

    if (gokuBlack != nullptr) {
        delete gokuBlack;
        gokuBlack = nullptr;
    }

    gokuBlack = new Villano(":/images/sprites/black_quieto.png",1,":/images/sprites/black_lanzamiento.png",5,650,135,10.0f);
    gokuBlack->getSprite()->setMantenerTamañoVisual(true);
    gokuBlack->getSprite()->fijarTamañoVisual(82.0, 132.0);
    gokuBlack->configurarAtaques({{4.8f,
                                   1.0f,
                                   ":/images/sprites/bolaBlack_actualizada.png",
                                   ":/images/sprites/black_lanzamiento.png",
                                   6,
                                   {
                                       QRectF(7, 0, 73, 130),
                                       QRectF(110, 0, 71, 130),
                                       QRectF(214, 0, 57, 130),
                                       QRectF(313, 0, 81, 130),
                                       QRectF(434, 0, 64, 130),
                                       QRectF(530, 0, 58, 130)
                                   }}});

    gokuBlack->agregarAEscena(scene);
    gokuBlack->setVisible(true);
    gokuBlack->setPos(rectaNivel2.right() - 160, posicionGogetaY - 40);
    gokuBlack->reiniciarVida();

    iniciarVariablesNivel2();

    if (timerNivel2 == nullptr) {
        timerNivel2 = new QTimer(this);
        connect(timerNivel2, &QTimer::timeout, this, &MainWindow::actualizarNivel2);
    }

    timerNivel2->start(16);
    programarSiguienteLanzamientoBlack();
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo iniciar el Nivel 2 facil.", error);
}

void MainWindow::iniciarNivel2Dificil() try
{
    detenerAudioInicio();
    iniciarAudioAmbienteEstadio();
    ocultarOverlayEstado();
    dificultadSeleccionada = Dificil;
    nivel1Activo = false;
    nivel2Activo = true;
    nivel2EnPausa = false;
    nivel2DificilActivo = true;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    ui->labelTitulo->hide();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    ocultarElementosNivel1();

    ponerFondo(":/images/backgrounds/nivel2.png", 1.0);

    if (gogeta == nullptr) {
        gogeta = new Sprite(":/images/sprites/gogeta_derecha.png", 3);
        gogeta->setMantenerTamañoVisual(true);
        scene->addItem(gogeta);
        gogeta->setZValue(8);
    }
    else if (gogeta->scene() != scene) {
        scene->addItem(gogeta);
    }

    const QRectF rectaNivel2 = scene->sceneRect();

    posicionGogetaX = rectaNivel2.left() + rectaNivel2.width() * 0.18;
    posicionGogetaY = rectaNivel2.bottom() - 170;
    posicionActualGogetaY = posicionGogetaY;
    rutaSpriteGogetaActual = ":/images/sprites/gogeta_derecha.png";
    rutaSpriteGogetaDireccion = rutaSpriteGogetaActual;
    gogetaSaltando = false;
    gogetaKamehamehaActivo = false;
    gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
    gogeta->fijarTamañoVisual(anchoVisualGogeta, altoVisualGogeta);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    gogeta->setVisible(true);

    if (gokuBlack != nullptr) {
        delete gokuBlack;
        gokuBlack = nullptr;
    }

    gokuBlack = new Villano(":/images/sprites/black_dificil_quieto.png",1,":/images/sprites/black_lanzamiento_dificil.png",4,650,135,100.0f);
    gokuBlack->getSprite()->setMantenerTamañoVisual(true);
    gokuBlack->getSprite()->fijarTamañoVisual(84.0, 136.0);
    gokuBlack->configurarAtaques({{5.0f,
                                   1.0f,
                                   ":/images/sprites/bolaBlack_actualizada.png",
                                   ":/images/sprites/black_lanzamiento_dificil.png",
                                   4,
                                   {}}});
    gokuBlack->agregarAEscena(scene);
    gokuBlack->setVisible(true);
    gokuBlack->setPos(rectaNivel2.right() - 160, posicionGogetaY - 55);
    gokuBlack->reiniciarVida();

    iniciarVariablesNivel2();

    if (timerNivel2 == nullptr) {
        timerNivel2 = new QTimer(this);
        connect(timerNivel2, &QTimer::timeout, this, &MainWindow::actualizarNivel2);
    }

    timerNivel2->start(16);
    programarSiguienteLanzamientoBlack();
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo iniciar el Nivel 2 dificil.", error);
}

void MainWindow::iniciarNivel1() try
{
    detenerAudioInicio();
    iniciarAudioAmbienteEstadio();
    ocultarOverlayEstado();
    nivel2Activo = false;
    ui->labelTitulo->hide();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    ocultarElementosNivel2();

    ponerFondo(":/images/backgrounds/nivel1.png", 1.0);

    if (vegito == nullptr) {
        vegito = new Jugador(":/images/sprites/vegitoBateoQuieto.png",1,":/images/sprites/vegitoBateo.png",5,650,540,20.0f);

        vegito->configurarSpritesEuforia(":/images/sprites/transformacion_vegito.png",5,":/images/sprites/vegito_bateo_kaioken.png",5);
        vegito->configurarSpriteImpacto(":/images/sprites/vegito_impacto.png", 1);
    }

    vegito->agregarAEscena(scene);
    vegito->setVisible(true);
    vegito->setPos(650, 540);

    if (freezer == nullptr) {
        freezer = new Villano(":/images/sprites/freezerPitcher.png",6,":/images/sprites/freezerPitcher.png",6,650,270,200.0f);
        freezer->configurarSpriteImpacto(":/images/sprites/freezer_impacto.png", 1);
    }

    freezer->getSprite()->setMantenerTamañoVisual(true);
    freezer->agregarAEscena(scene);
    freezer->setVisible(true);
    freezer->setPos(650, 270);

    iniciarVariablesNivel1();

    if (timerNivel1 == nullptr) {
        timerNivel1 = new QTimer(this);
        connect(timerNivel1, &QTimer::timeout, this, &MainWindow::actualizarNivel1);
    }

    timerNivel1->start(16);
    programarSiguienteLanzamiento();
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo iniciar el Nivel 1.", error);
}

void MainWindow::iniciarVariablesNivel1()
{
    rachaDaño = 0.0f;
    velocidadFreezer = 3.6f;
    lanzamientoOscilatorio = false;
    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;
    nivel1Activo = true;
    puedeBatear = true;
    bolaControlada = nullptr;

    configurarAtaquesFreezer();
    configurarZonasBateo();

    if (vegito != nullptr) {
        vegito->reiniciarVida();
    }

    if (freezer != nullptr) {
        freezer->reiniciarVida();
    }

    if (textoEstadoKaioken != nullptr) {
        scene->removeItem(textoEstadoKaioken);
        delete textoEstadoKaioken;
        textoEstadoKaioken = nullptr;
    }

    desactivarBolasFreezer();
    prepararPoolFreezer();

    crearHUDNivel1();
    actualizarHUD();
}

void MainWindow::crearHUDNivel1()
{
    destruirHUDVida();
    barraVidaJugador.crearBarraVida(scene, 20, 48, kAnchoBarraVida, kAltoBarraVida, "Vegito", QColor(30, 160, 255));
    barraVidaEnemigo.crearBarraVida(scene, 20, 108, kAnchoBarraVida, kAltoBarraVida, "Freezer", QColor(220, 40, 40));

    textoEstadoKaioken = scene->addText("");
    textoEstadoKaioken->setDefaultTextColor(QColor(255, 210, 80));
    textoEstadoKaioken->setFont(QFont("Arial", 12, QFont::Bold));
    textoEstadoKaioken->setPos(20, 144);
    textoEstadoKaioken->setZValue(24);
}

void MainWindow::crearHUDNivel2Dificil()
{
    destruirHUDVida();

    const qreal xBlack = scene != nullptr
                             ? scene->sceneRect().width() - kAnchoBarraVida - 20
                             : 20;

    barraVidaJugador.crearBarraVida(scene, 20, 48, kAnchoBarraVida, kAltoBarraVida, "Gogeta", QColor(60, 170, 255));
    barraVidaEnemigo.crearBarraVida(scene, xBlack, 48, kAnchoBarraVida, kAltoBarraVida, "Black", QColor(220, 50, 70));
    crearMarcadoresNivel2();
}

void MainWindow::setVisibleHUDVida(bool visible)
{
    barraVidaJugador.setVisible(visible);
    barraVidaEnemigo.setVisible(visible);
    marcadorNivel2Jugador.setVisible(visible);
    marcadorNivel2Enemigo.setVisible(visible);
}

void MainWindow::destruirHUDVida()
{
    barraVidaJugador.destruir();
    barraVidaEnemigo.destruir();
    marcadorNivel2Jugador.destruir();
    marcadorNivel2Enemigo.destruir();
}

void MainWindow::configurarZonasBateo()
{
    const QRectF rectaEscena = scene != nullptr ? scene->sceneRect() : QRectF(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    const QPointF centroDiamante(rectaEscena.width() * 0.5, rectaEscena.height() * 0.72);
    const qreal anguloInicio = 211.5;
    const qreal anguloFin = 328.5;

    auto crearSector = [&](qreal radioInterno, qreal radioExterno) {
        QPolygonF poligono;
        const int muestras = 48;

        for (int i = 0; i <= muestras; i++) {
            const qreal t = static_cast<qreal>(i) / muestras;
            const qreal angulo = qDegreesToRadians(anguloInicio + (anguloFin - anguloInicio) * t);
            poligono << QPointF(centroDiamante.x() + radioExterno * qCos(angulo),
                                centroDiamante.y() + radioExterno * qSin(angulo));
        }

        for (int i = muestras; i >= 0; i--) {
            const qreal t = static_cast<qreal>(i) / muestras;
            const qreal angulo = qDegreesToRadians(anguloInicio + (anguloFin - anguloInicio) * t);
            poligono << QPointF(centroDiamante.x() + radioInterno * qCos(angulo),
                                centroDiamante.y() + radioInterno * qSin(angulo));
        }

        QPainterPath zona;
        zona.addPolygon(poligono);
        return zona.simplified();
    };

    const qreal radioInicio = rectaEscena.width() * 0.182;
    const qreal radio10 = rectaEscena.width() * 0.247;
    const qreal radio20 = rectaEscena.width() * 0.308;
    const qreal radio30 = rectaEscena.width() * 0.365;

    zonaDañoBajo.crearHitboxTrayectoria(scene, crearSector(radioInicio, radio10));
    zonaDañoMedio.crearHitboxTrayectoria(scene, crearSector(radio10, radio20));
    zonaDañoAlto.crearHitboxTrayectoria(scene, crearSector(radio20, radio30));
}

void MainWindow::crearMarcadoresNivel2()
{
    if (scene == nullptr) {
        return;
    }

    const qreal xBlack = scene->sceneRect().width() - kAnchoBarraVida - 20;
    marcadorNivel2Jugador.crearTextoEscena(scene,20,82,"",QColor(220, 236, 255),13,true);
    marcadorNivel2Enemigo.crearTextoEscena(scene,xBlack,82,"",QColor(255, 220, 225),13,true);
}

void MainWindow::crearBotonesDificultad()
{
    botonFacil = new QPushButton("Facil", ui->centralwidget);
    botonDificil = new QPushButton("Dificil", ui->centralwidget);

    botonFacil->setGeometry(width() / 2 - 150, height() * 3 / 4, 120, 45);
    botonDificil->setGeometry(width() / 2 + 30, height() * 3 / 4, 120, 45);

    botonFacil->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(30, 120, 30, 220);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "}"
        );

    botonDificil->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(150, 30, 30, 220);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "}"
        );

    connect(botonFacil, &QPushButton::clicked, this, &MainWindow::iniciarModoFacil);
    connect(botonDificil, &QPushButton::clicked, this, &MainWindow::iniciarModoDificil);
}

void MainWindow::crearTextosReglas()
{
    labelReglasTitulo = new QLabel("Reglas del Nivel 1", ui->centralwidget);
    labelReglasTexto = new QLabel(ui->centralwidget);

    labelReglasTitulo->setGeometry(width() / 2 - 220, height() / 8, 440, 55);
    labelReglasTexto->setGeometry(width() / 2 - 260, height() / 4, 520, height() / 3);

    labelReglasTitulo->setAlignment(Qt::AlignCenter);
    labelReglasTexto->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    labelReglasTexto->setWordWrap(true);

    Items::aplicarEstiloPanel(labelReglasTitulo,"rgba(0, 0, 0, 190)","white","white",12,8,22,true);

    Items::aplicarEstiloPanel(labelReglasTexto,"rgba(0, 0, 0, 190)","white","white",12,12,15,true);

    labelReglasTexto->setText(
        "->Golpea la bola con la tecla P cuando llegue a Vegito.\n"
        "->Despues del bateo usa A, D, W y S para corregir su caida.\n"
        "->Cada batazo resta vida a Freezer segun la zona donde caiga.\n"
        "->Si dejas pasar la bola, Vegito pierde vida.\n"
        "->Cuando Vegito baje del 30% de vida entrara en kaioken.\n"
        "->En kaioken, cada bateo conectado tambien lastima directamente a Freezer.\n"
        "->Facil: un solo tipo de ataque.\n"
        "->Dificil: Freezer puede lanzar 3 ataques con distinto daño y velocidad."
        );
}

void MainWindow::crearPantallaNivel2()
{
    botonNivel1 = new QPushButton("Volver a Nivel 1", ui->centralwidget);
    botonNivel2Facil = new QPushButton("Facil", ui->centralwidget);
    botonNivel2Dificil = new QPushButton("Dificil", ui->centralwidget);
    labelNivel2Titulo = new QLabel("Nivel 2: Seleccion de dificultad", ui->centralwidget);
    labelNivel2Texto = new QLabel(ui->centralwidget);

    botonNivel1->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(35, 85, 160, 220);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "}"
        );

    botonNivel2Facil->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(30, 135, 65, 220);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "}"
        );

    botonNivel2Dificil->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(150, 40, 40, 220);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "}"
        );

    labelNivel2Titulo->setAlignment(Qt::AlignCenter);
    labelNivel2Texto->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    labelNivel2Texto->setWordWrap(true);

    Items::aplicarEstiloPanel(labelNivel2Titulo,"rgba(0, 0, 0, 150)","white","white",12,8,22,true);

    Items::aplicarEstiloPanel(labelNivel2Texto,"rgba(0, 0, 0, 150)","white","white",12,12,15,true);

    labelNivel2Texto->setText(
        "Escoge la dificultad del Nivel 2.\n"
        "En modo facil, Gogeta atrapa las pelotas de Goku Black.\n"
        "En modo dificil, las atrapadas cargan el Final Kamehameha con Q.\n"
        "Controles del modo facil:\n"
        "-> A mueve a Gogeta hacia la izquierda.\n"
        "-> D mueve a Gogeta hacia la derecha.\n"
        "-> W permite hasta quintuple salto.\n"
        "-> Cada 3 pelotas atrapadas suman 1 out para Gogeta.\n"
        "-> Cada 3 pelotas perdidas suman 1 carrera para Black.\n"
        "-> Si una pelota cruza el aro amarillo, se agranda y vale 2 puntos.\n"
        "En dificil, las 3 atrapadas para out deben ser consecutivas."
        );

    connect(botonNivel1, &QPushButton::clicked, this, &MainWindow::abrirNivel1DesdeSelector);
    connect(botonNivel2Facil, &QPushButton::clicked, this, &MainWindow::iniciarNivel2Facil);
    connect(botonNivel2Dificil, &QPushButton::clicked, this, &MainWindow::iniciarNivel2Dificil);

    labelNivel2Titulo->setGeometry(width() / 2 - 220, height() / 8, 440, 55);
    labelNivel2Texto->setGeometry(width() / 2 - 260, height() / 4, 520, height() / 3);
    botonNivel2Facil->setGeometry(width() / 2 - 110, height() * 3 / 4, 95, 45);
    botonNivel2Dificil->setGeometry(width() / 2 + 15, height() * 3 / 4, 95, 45);
    botonNivel1->setGeometry(width() / 2 - 90, height() * 3 / 4 + 62, 180, 45);
}

void MainWindow::crearOverlayEstado()
{
    labelOverlayTitulo = new QLabel(ui->centralwidget);
    labelOverlayTexto = new QLabel(ui->centralwidget);
    botonOverlayPrimario = new QPushButton(ui->centralwidget);
    botonOverlaySecundario = new QPushButton(ui->centralwidget);
    botonOverlayTerciario = new QPushButton(ui->centralwidget);

    labelOverlayTitulo->setAlignment(Qt::AlignCenter);
    labelOverlayTexto->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    labelOverlayTexto->setWordWrap(true);

    Items::aplicarEstiloPanel(labelOverlayTitulo,"rgba(0, 0, 0, 205)","#f7e7a7","white",14,8,24,true);

    Items::aplicarEstiloPanel(labelOverlayTexto,"rgba(0, 0, 0, 205)","#f7e7a7","white",14,14,15,true);

    const QString estiloBotonPrincipal =
        "QPushButton {"
        "background-color: rgba(40, 125, 60, 230);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "padding: 8px 14px;"
        "}";
    const QString estiloBotonSecundario =
        "QPushButton {"
        "background-color: rgba(35, 85, 160, 230);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "padding: 8px 14px;"
        "}";
    const QString estiloBotonTerciario =
        "QPushButton {"
        "background-color: rgba(150, 40, 40, 230);"
        "color: white;"
        "font-weight: bold;"
        "border: 2px solid white;"
        "border-radius: 10px;"
        "padding: 8px 14px;"
        "}";

    botonOverlayPrimario->setStyleSheet(estiloBotonPrincipal);
    botonOverlaySecundario->setStyleSheet(estiloBotonSecundario);
    botonOverlayTerciario->setStyleSheet(estiloBotonTerciario);

    connect(botonOverlayPrimario, &QPushButton::clicked, this, [this]() {
        if (accionOverlayPrimaria) {
            accionOverlayPrimaria();
        }
    });

    connect(botonOverlaySecundario, &QPushButton::clicked, this, [this]() {
        if (accionOverlaySecundaria) {
            accionOverlaySecundaria();
        }
    });

    connect(botonOverlayTerciario, &QPushButton::clicked, this, [this]() {
        if (accionOverlayTerciaria) {
            accionOverlayTerciaria();
        }
    });

    labelOverlayTitulo->setGeometry(width() / 2 - 260, height() / 7, 520, 60);
    labelOverlayTexto->setGeometry(width() / 2 - 280, height() / 4, 560, 185);
    botonOverlayPrimario->setGeometry(width() / 2 - 235, height() * 3 / 4, 150, 45);
    botonOverlaySecundario->setGeometry(width() / 2 - 75, height() * 3 / 4, 150, 45);
    botonOverlayTerciario->setGeometry(width() / 2 + 85, height() * 3 / 4, 150, 45);
}

void MainWindow::iniciarAudioInicio()
{
    if (musicaInicio == nullptr) {
        musicaInicio = new QMediaPlayer(this);
        salidaAudioInicio = new QAudioOutput(this);
        configurarAudio(musicaInicio, salidaAudioInicio, "qrc:/images/audios/new_main_audio.mp3", 35);
        configurarReinicioAudio(musicaInicio, this);
    }

    if (!audioEstaReproduciendose(musicaInicio)) {
        musicaInicio->play();
    }
}

void MainWindow::detenerAudioInicio()
{
    if (musicaInicio != nullptr) {
        musicaInicio->stop();
    }
}

void MainWindow::iniciarAudiosBatazo()
{
    if (audioVegitoYosha == nullptr) {
        audioVegitoYosha = new QMediaPlayer(this);
        salidaAudioVegitoYosha = new QAudioOutput(this);
        configurarAudio(audioVegitoYosha, salidaAudioVegitoYosha, "qrc:/images/audios/vegito-yosha.mp3", 100);
    }

    if (audioBatazoMuchedumbre == nullptr) {
        audioBatazoMuchedumbre = new QMediaPlayer(this);
        salidaAudioBatazoMuchedumbre = new QAudioOutput(this);
        configurarAudio(audioBatazoMuchedumbre, salidaAudioBatazoMuchedumbre, "qrc:/images/audios/sonido_bateo_muchedumbre.wav", 95);
    }

    if (audioAmbienteEstadio == nullptr) {
        audioAmbienteEstadio = new QMediaPlayer(this);
        salidaAudioAmbienteEstadio = new QAudioOutput(this);
        configurarAudio(audioAmbienteEstadio, salidaAudioAmbienteEstadio, "qrc:/images/audios/muchedumbre_estadio.mp3", 42);
        configurarReinicioAudio(audioAmbienteEstadio, this);
    }

    if (audioFinalKamehameha == nullptr) {
        audioFinalKamehameha = new QMediaPlayer(this);
        salidaAudioFinalKamehameha = new QAudioOutput(this);
        configurarAudio(audioFinalKamehameha, salidaAudioFinalKamehameha, "qrc:/images/audios/final_kamehameha.mp3", 95);
    }
}

void MainWindow::reproducirAudiosBatazo()
{
    if (audioVegitoYosha != nullptr) {
        audioVegitoYosha->setPosition(0);
        audioVegitoYosha->play();
    }

    if (audioBatazoMuchedumbre != nullptr) {
        audioBatazoMuchedumbre->setPosition(0);
        audioBatazoMuchedumbre->play();
    }
}

void MainWindow::iniciarAudioAmbienteEstadio()
{
    if (audioAmbienteEstadio == nullptr) {
        iniciarAudiosBatazo();
    }

    if (audioAmbienteEstadio == nullptr) {
        return;
    }

    if (!audioEstaReproduciendose(audioAmbienteEstadio)) {
        audioAmbienteEstadio->setPosition(0);
        audioAmbienteEstadio->play();
    }
}

void MainWindow::detenerAudioAmbienteEstadio()
{
    if (audioAmbienteEstadio != nullptr) {
        audioAmbienteEstadio->stop();
    }
}

void MainWindow::reproducirAudioFinalKamehameha()
{
    if (audioFinalKamehameha == nullptr) {
        iniciarAudiosBatazo();
    }

    if (audioFinalKamehameha != nullptr) {
        audioFinalKamehameha->setPosition(0);
        audioFinalKamehameha->play();
    }
}

void MainWindow::mostrarEfectoTajoEspada() try
{
    if (scene == nullptr || freezer == nullptr) {
        return;
    }

    Sprite *efectoTajo = new Sprite(":/images/sprites/tajo_espada.png", 5);
    efectoTajo->setMantenerTamañoVisual(true);
    efectoTajo->fijarTamañoVisual(170.0, 170.0);
    scene->addItem(efectoTajo);

    const QRectF freezerRect = freezer->getSprite()->sceneBoundingRect();
    efectoTajo->setPos(freezerRect.center().x() + 25.0, freezerRect.center().y() - 10.0);
    efectoTajo->setZValue(18);

    for (int frame = 1; frame < 5; frame++) {
        QTimer::singleShot(frame * 45, this, [efectoTajo]() {
            if (efectoTajo->scene() == nullptr) {
                return;
            }

            efectoTajo->avanzarFrame();
        });
    }

    QTimer::singleShot(260, this, [efectoTajo]() {
        if (efectoTajo->scene() != nullptr) {
            efectoTajo->scene()->removeItem(efectoTajo);
        }

        delete efectoTajo;
    });
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo mostrar el efecto de tajo.", error);
}

void MainWindow::mostrarOpcionesDificultad()
{
    if (botonFacil != nullptr) {
        botonFacil->show();
    }

    if (botonDificil != nullptr) {
        botonDificil->show();
    }
}

void MainWindow::ocultarOpcionesDificultad()
{
    if (botonFacil != nullptr) {
        botonFacil->hide();
    }

    if (botonDificil != nullptr) {
        botonDificil->hide();
    }
}

void MainWindow::mostrarPantallaReglas()
{
    ocultarOverlayEstado();

    if (labelReglasTitulo != nullptr) {
        labelReglasTitulo->show();
    }

    if (labelReglasTexto != nullptr) {
        labelReglasTexto->show();
    }

    mostrarOpcionesDificultad();
}

void MainWindow::ocultarPantallaReglas()
{
    if (labelReglasTitulo != nullptr) {
        labelReglasTitulo->hide();
    }

    if (labelReglasTexto != nullptr) {
        labelReglasTexto->hide();
    }
}

void MainWindow::mostrarPantallaNivel2()
{
    detenerAudioInicio();
    detenerAudioAmbienteEstadio();
    nivel1Activo = false;
    nivel2Activo = false;
    ocultarOverlayEstado();
    ponerFondo(":/images/backgrounds/nivel2.png", 1.0);
    ui->labelTitulo->hide();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarElementosNivel1();
    ocultarElementosNivel2();

    if (labelNivel2Texto != nullptr) {
        labelNivel2Texto->setText(
            "Escoge la dificultad del Nivel 2.\n"
            "En modo facil, Gogeta atrapa las pelotas de Goku Black.\n"
            "En modo dificil, las atrapadas cargan el Final Kamehameha con Q.\n"
            "Controles del modo facil:\n"
            "-> A mueve a Gogeta hacia la izquierda.\n"
            "-> D mueve a Gogeta hacia la derecha.\n"
            "-> W permite hasta quintuple salto.\n"
            "-> Cada 3 pelotas atrapadas suman 1 out para Gogeta.\n"
            "-> Cada 3 pelotas perdidas suman 1 carrera para Black.\n"
            "En dificil, las 3 atrapadas para out deben ser consecutivas."
            );
    }

    if (labelNivel2Titulo != nullptr) {
        labelNivel2Titulo->show();
    }

    if (labelNivel2Texto != nullptr) {
        labelNivel2Texto->show();
    }

    if (botonNivel1 != nullptr) {
        botonNivel1->show();
    }

    if (botonNivel2Facil != nullptr) {
        botonNivel2Facil->show();
    }

    if (botonNivel2Dificil != nullptr) {
        botonNivel2Dificil->show();
    }
}

void MainWindow::ocultarPantallaNivel2()
{
    if (labelNivel2Titulo != nullptr) {
        labelNivel2Titulo->hide();
    }

    if (labelNivel2Texto != nullptr) {
        labelNivel2Texto->hide();
    }

    if (botonNivel1 != nullptr) {
        botonNivel1->hide();
    }

    if (botonNivel2Facil != nullptr) {
        botonNivel2Facil->hide();
    }

    if (botonNivel2Dificil != nullptr) {
        botonNivel2Dificil->hide();
    }
}

void MainWindow::mostrarOverlayEstado(const QString &titulo, const QString &texto, const QString &textoBotonPrimario, const std::function<void()> &accionPrimaria, const QString &textoBotonSecundario, const std::function<void()> &accionSecundaria, const QString &textoBotonTerciario, const std::function<void()> &accionTerciaria)
{
    if (labelOverlayTitulo == nullptr ||
        labelOverlayTexto == nullptr ||
        botonOverlayPrimario == nullptr ||
        botonOverlaySecundario == nullptr ||
        botonOverlayTerciario == nullptr) {
        return;
    }

    accionOverlayPrimaria = accionPrimaria;
    accionOverlaySecundaria = accionSecundaria;
    accionOverlayTerciaria = accionTerciaria;

    labelOverlayTitulo->setText(titulo);
    labelOverlayTexto->setText(texto);
    botonOverlayPrimario->setText(textoBotonPrimario);
    botonOverlaySecundario->setText(textoBotonSecundario);
    botonOverlayTerciario->setText(textoBotonTerciario);

    labelOverlayTitulo->show();
    labelOverlayTexto->show();
    botonOverlayPrimario->show();
    botonOverlayPrimario->setVisible(!textoBotonPrimario.isEmpty());
    botonOverlaySecundario->setVisible(!textoBotonSecundario.isEmpty());
    botonOverlayTerciario->setVisible(!textoBotonTerciario.isEmpty());
    labelOverlayTitulo->raise();
    labelOverlayTexto->raise();
    botonOverlayPrimario->raise();
    botonOverlaySecundario->raise();
    botonOverlayTerciario->raise();
}

void MainWindow::ocultarOverlayEstado()
{
    accionOverlayPrimaria = {};
    accionOverlaySecundaria = {};
    accionOverlayTerciaria = {};

    if (labelOverlayTitulo != nullptr) {
        labelOverlayTitulo->hide();
    }

    if (labelOverlayTexto != nullptr) {
        labelOverlayTexto->hide();
    }

    if (botonOverlayPrimario != nullptr) {
        botonOverlayPrimario->hide();
    }

    if (botonOverlaySecundario != nullptr) {
        botonOverlaySecundario->hide();
    }

    if (botonOverlayTerciario != nullptr) {
        botonOverlayTerciario->hide();
    }
}

void MainWindow::ocultarElementosNivel1()
{
    desactivarBolasFreezer();

    if (vegito != nullptr) {
        vegito->setVisible(false);
    }

    if (freezer != nullptr) {
        freezer->setVisible(false);
    }

    setVisibleHUDVida(false);
    mensajeTransicionNivel1.destruir();

    if (textoEstadoKaioken != nullptr) {
        textoEstadoKaioken->setVisible(false);
    }
}

void MainWindow::ocultarElementosNivel2()
{
    nivel2Activo = false;
    nivel2EnPausa = false;
    esperandoLanzamientoBlack = false;
    nivel2DificilActivo = false;
    gogetaKamehamehaActivo = false;
    dañoKamehamehaAplicado = false;

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    desactivarBolasBlack();

    if (gogeta != nullptr) {
        gogeta->setVisible(false);
    }

    if (gokuBlack != nullptr) {
        gokuBlack->setVisible(false);
    }

    setVisibleHUDVida(false);
    aroNivel2.setVisible(false);

    if (textoEstadoNivel2 != nullptr) {
        textoEstadoNivel2->setVisible(false);
    }
}

void MainWindow::configurarAtaquesFreezer()
{
    ataquesFreezer.clear();
    ataquesFreezer.append({3.6f,2.0f,":/images/sprites/bolaFreezer.png",":/images/sprites/freezerPitcher.png",6});

    if (dificultadSeleccionada == Dificil) {
        ataquesFreezer.clear();
        ataquesFreezer.append({3.6f,2.0f,":/images/sprites/bolaFreezer.png",":/images/sprites/freezerPitcher.png",6});
        ataquesFreezer.append({4.3f,2.8f,":/images/sprites/bolaFreezer2.png",":/images/sprites/freezerPitcher2.png",5});
        ataquesFreezer.append({5.0f,3.6f,":/images/sprites/bolafreezer3.png",":/images/sprites/freezerPitcher3.png",3});
    }

    if (freezer != nullptr) {
        freezer->configurarAtaques(ataquesFreezer);
    }
}

void MainWindow::desactivarBolasFreezer()
{
    for (int i = 0; i < bolasFreezer.size(); i++) {
        if (bolasFreezer.at(i) != nullptr) {
            bolasFreezer.at(i)->desactivar();
        }
    }

    bolaControlada = nullptr;
}

bool MainWindow::hayBolasFreezerActivas() const
{
    for (int i = 0; i < bolasFreezer.size(); i++) {
        if (bolasFreezer.at(i) != nullptr && bolasFreezer.at(i)->estaActiva()) {
            return true;
        }
    }

    return false;
}

void MainWindow::prepararPoolFreezer()
{
    if (scene == nullptr) {
        return;
    }

    for (int i = 0; i < ataquesFreezer.size(); i++) {
        const QString ruta = ataquesFreezer.at(i).spriteProyectil;
        bool yaExiste = false;

        for (int j = 0; j < bolasFreezer.size(); j++) {
            if (bolasFreezer.at(j) != nullptr &&
                bolasFreezer.at(j)->getRutaSprite() == ruta) {
                yaExiste = true;
                break;
            }
        }

        if (!yaExiste) {
            Proyectil *bola = new Proyectil(scene, ruta, QPointF(-200, -200));
            bola->desactivar();
            bolasFreezer.append(bola);
        }
    }
}

Proyectil *MainWindow::obtenerBolaFreezer(const Villano::Ataque &ataque, QPointF posicionInicial)
{
    for (int i = 0; i < bolasFreezer.size(); i++) {
        Proyectil *bola = bolasFreezer.at(i);

        if (bola != nullptr &&
            !bola->estaActiva() &&
            bola->getRutaSprite() == ataque.spriteProyectil) {
            bola->reiniciar(posicionInicial, ataque.spriteProyectil);
            bola->configurarAtaque(ataque.velocidad, ataque.daño);
            return bola;
        }
    }
    for (int i = 0; i < bolasFreezer.size(); i++) {
        Proyectil *bola = bolasFreezer.at(i);

        if (bola != nullptr && bola->getRutaSprite() == ataque.spriteProyectil) {
            return nullptr;
        }
    }

    Proyectil *bola = new Proyectil(scene, ataque.spriteProyectil, posicionInicial);
    bola->configurarAtaque(ataque.velocidad, ataque.daño);
    bolasFreezer.append(bola);
    return bola;
}

void MainWindow::programarSiguienteLanzamiento()
{
    if (!nivel1Activo || esperandoLanzamientos) {
        return;
    }

    esperandoLanzamientos = true;

    if (rachaDaño >= 12.0f) {
        bolasPendientesPorLanzar = 2;
        QTimer::singleShot(2000, this, &MainWindow::lanzarBolaFreezer);
        QTimer::singleShot(4000, this, &MainWindow::lanzarBolaFreezer);
    }
    else {
        bolasPendientesPorLanzar = 1;
        QTimer::singleShot(2000, this, &MainWindow::lanzarBolaFreezer);
    }
}

void MainWindow::lanzarBolaFreezer() try
{
    if (!nivel1Activo || scene == nullptr || freezer == nullptr) {
        return;
    }

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
        return;
    }

    Villano::Ataque ataque = freezer->elegirAtaque();
    bolasPendientesPorLanzar--;

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
    }

    QTimer::singleShot(220, this, [this, ataque]() {
        try {
            if (!nivel1Activo || scene == nullptr || freezer == nullptr) {
                return;
            }

            QPointF posicionInicial = freezer->getSprite()->sceneBoundingRect().center();
            posicionInicial.setY(posicionInicial.y() + 45);

            Proyectil *nuevaBola = obtenerBolaFreezer(ataque, posicionInicial);

            if (nuevaBola == nullptr) {
                return;
            }

            freezer->reproducirAtaqueActual();
        }
        catch (const std::exception &error) {
            manejarErrorJuego("No se pudo crear el proyectil de Freezer.", error);
        }
    });
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo lanzar la bola de Freezer.", error);
}

void MainWindow::actualizarNivel1() try
{
    if (!nivel1Activo) {
        return;
    }

    const float dt = 0.016f;

    if (vegito != nullptr) {
        vegito->actualizar(16);
    }

    if (freezer != nullptr) {
        freezer->actualizar(16);
    }

    revisarEuforiaVegito();

    for (int i = bolasFreezer.size() - 1; i >= 0; i--) {
        Proyectil *bola = bolasFreezer.at(i);

        if (bola == nullptr || !bola->estaActiva()) {
            continue;
        }

        if (bola->getEstado() == Proyectil::LanzadaPorFreezer) {
            float velocidadAtaque = velocidadFreezer;

            if (dificultadSeleccionada == Dificil) {
                velocidadAtaque += bola->getVelocidadAtaque() - 3.6f;
            }

            bola->moverLanzamiento(velocidadAtaque, lanzamientoOscilatorio);

            if (bola->pasoLinea(vegito->getSprite()->y() + 70)) {
                vegito->recibirDaño(bola->getDañoAtaque());
                vegito->reproducirImpacto();
                freezer->aprender(false);
                rachaDaño = 0.0f;

                eliminarBola(bola);
                actualizarHUD();

                if (vegito->getVidaActual() <= 0.0f) {
                    perderNivel1();
                    return;
                }
            }
        }
        else if (bola->getEstado() == Proyectil::BateadaPorVegito) {
            bola->actualizarBateo(dt);

            if (bola->terminoVuelo()) {
                revisarCaidaBola(bola);
                eliminarBola(bola);
                actualizarHUD();

                if (freezer->getVidaActual() <= 0.0f) {
                    ganarNivel1();
                    return;
                }

                if (vegito->getVidaActual() <= 0.0f) {
                    perderNivel1();
                    return;
                }
            }
        }
    }

    if (!hayBolasFreezerActivas() && !esperandoLanzamientos) {
        programarSiguienteLanzamiento();
    }
}
catch (const std::exception &error)
{
    manejarErrorJuego("Ocurrio un error durante la actualizacion del Nivel 1.", error);
}

void MainWindow::actualizarNivel2() try
{
    if (!nivel2Activo || nivel2EnPausa || gogeta == nullptr || gokuBlack == nullptr) {
        return;
    }

    const int metaNivel2 = nivel2DificilActivo ? 8 : 3;
    const float dt = 0.016f;

    gokuBlack->actualizar(16);

    if (moverGogetaIzquierdaActivo && !moverGogetaDerechaActivo) {
        moverGogeta(-velocidadMovimientoGogeta, ":/images/sprites/gogeta_izquierda.png");
    }
    else if (moverGogetaDerechaActivo && !moverGogetaIzquierdaActivo) {
        moverGogeta(velocidadMovimientoGogeta, ":/images/sprites/gogeta_derecha.png");
    }

    actualizarSaltoGogeta(16);
    actualizarKamehamehaGogeta(16);
    actualizarRafagaFinalKamehameha();

    if (nivel2DificilActivo) {
        actualizarBlackDificil(16);
    }

    for (int i = bolasBlack.size() - 1; i >= 0; i--) {
        Proyectil *bola = bolasBlack.at(i);

        if (bola == nullptr || !bola->estaActiva()) {
            continue;
        }

        const bool esAtaqueEspecial = nivel2DificilActivo && bola->getDañoAtaque() >= 3.0f;
        const float factorVelocidadBase = bola->getVelocidadAtaque() / 3.6f;
        const float factorDificultadCaida = (nivel2DificilActivo && !esAtaqueEspecial) ? 1.28f : 1.0f;

        bola->actualizarBateo(dt * factorVelocidadBase * factorDificultadCaida);
        actualizarBonificacionAro(bola, esAtaqueEspecial);

        if (spriteColisionaConProyectil(gogeta, bola)) {
            if (esAtaqueEspecial) {
                aplicarDañoGogetaNivel2(18.0f);
            }
            else {
                pelotasAtrapadasNivel2 += bola->getValorPuntaje();

                if (nivel2DificilActivo &&
                    cargaKamehamehaNivel2 < cargaMaximaKamehamehaNivel2) {
                    cargaKamehamehaNivel2 = qMin(cargaMaximaKamehamehaNivel2,
                                                 cargaKamehamehaNivel2 + bola->getValorPuntaje());
                }

                while (pelotasAtrapadasNivel2 >= 3) {
                    pelotasAtrapadasNivel2 -= 3;
                    outsNivel2++;

                    if (nivel2DificilActivo) {
                        aplicarDañoBlackNivel2(12.0f);
                    }
                }
            }

            eliminarBolaBlack(bola);
            actualizarHUD();
            actualizarTextoNivel2();

            if (outsNivel2 >= metaNivel2) {
                finalizarNivel2("Victoria de Gogeta",
                                "Gogeta gano con " + QString::number(metaNivel2) + " outs.\n\nPuedes volver al selector del Nivel 2 o jugar otra vez con la misma dificultad.",
                                true);
                return;
            }

            if (nivel2DificilActivo) {
                if (vidaGogetaNivel2 <= 0.0f) {
                    finalizarNivel2("Goku Black gana",
                                    "Goku Black vacio la vida de Gogeta.\n\nConviene reintentar de inmediato para mantener el ritmo del combate.",
                                    true);
                    return;
                }

                if (vidaBlackNivel2 <= 0.0f) {
                    finalizarNivel2("Gogeta derrota a Black",
                                    "Goku Black fue derrotado en el modo dificil.\n\nPuedes repetir el duelo o volver al selector del Nivel 2.",
                                    true);
                    return;
                }
            }

            continue;
        }

        if (bola->terminoVuelo()) {
            eliminarBolaBlack(bola);

            if (!esAtaqueEspecial) {
                pelotasAtrapadasNivel2 = 0;
                pelotasPerdidasNivel2 += bola->getValorPuntaje();
                while (pelotasPerdidasNivel2 >= 3) {
                    pelotasPerdidasNivel2 -= 3;
                    carrerasBlackNivel2++;

                    if (nivel2DificilActivo) {
                        aplicarDañoGogetaNivel2(12.0f);
                    }
                }
            }

            actualizarHUD();
            actualizarTextoNivel2();

            if (carrerasBlackNivel2 >= metaNivel2) {
                finalizarNivel2("Goku Black gana",
                                "Black completo " + QString::number(metaNivel2) + " carreras antes que Gogeta.\n\nPuedes reintentar el nivel desde esta misma pantalla.",
                                true);
                return;
            }

            if (nivel2DificilActivo) {
                if (vidaGogetaNivel2 <= 0.0f) {
                    finalizarNivel2("Goku Black gana",
                                    "Goku Black vacio la vida de Gogeta.\n\nConviene reintentar de inmediato para mantener el ritmo del combate.",
                                    true);
                    return;
                }

                if (vidaBlackNivel2 <= 0.0f) {
                    finalizarNivel2("Gogeta derrota a Black",
                                    "Goku Black fue derrotado en el modo dificil.\n\nPuedes repetir el duelo o volver al selector del Nivel 2.",
                                    true);
                    return;
                }
            }
        }
    }

    if (nivel2Activo && !hayBolasBlackActivas() && !gogetaKamehamehaActivo) {
        programarSiguienteLanzamientoBlack();
    }
}
catch (const std::exception &error)
{
    manejarErrorJuego("Ocurrio un error durante la actualizacion del Nivel 2.", error);
}

void MainWindow::actualizarBonificacionAro(Proyectil *bola, bool esAtaqueEspecial)
{
    if (bola == nullptr || esAtaqueEspecial || bola->tieneBonificacionAro()) {
        return;
    }

    if (aroNivel2.colisionaCon(bola->getItem())) {
        bola->activarBonificacionAro();
    }
}

void MainWindow::actualizarBlackDificil(int dtMs)
{
    if (!nivel2DificilActivo || gokuBlack == nullptr || gogeta == nullptr || scene == nullptr) {
        return;
    }

    tiempoMovimientoBlackDificilMs += dtMs;
    tiempoAtaqueEspecialBlackMs += dtMs;

    const QRectF recta = scene->sceneRect();
    const qreal posicionX = recta.right() - 160.0;
    const qreal posicionActualYBlack = gokuBlack->getSprite()->pos().y();

    const qreal variacion = 24.0 * qSin(tiempoMovimientoBlackDificilMs / 350.0);
    const qreal destinoY = qBound(recta.top() + 120.0,
                                  posicionActualGogetaY - 40.0 + variacion,
                                  recta.bottom() - 220.0);

    const qreal nuevaY = posicionActualYBlack + (destinoY - posicionActualYBlack) * 0.08;
    gokuBlack->setPos(posicionX, nuevaY);

    if (tiempoMovimientoBlackDificilMs >= 700) {
        tiempoMovimientoBlackDificilMs = 0;

        if (destinoY < posicionActualYBlack) {
            gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_vuelo.png", 1);
        }
        else {
            gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_bajada.png", 1);
        }

        gokuBlack->getSprite()->reiniciarAnimacion();

        QTimer::singleShot(220, this, [this]() {
            try {
                if (!nivel2Activo || !nivel2DificilActivo || gokuBlack == nullptr) {
                    return;
                }

                gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_quieto.png", 1);
                gokuBlack->getSprite()->reiniciarAnimacion();
            }
            catch (const std::exception &error) {
                manejarErrorJuego("No se pudo restaurar el sprite de Goku Black.", error);
            }
        });
    }

    const int tiempoEspecial = 4200 + QRandomGenerator::global()->bounded(0, 1600);

    if (tiempoAtaqueEspecialBlackMs >= tiempoEspecial &&
        !hayBolasBlackActivas() &&
        !gogetaKamehamehaActivo) {

        tiempoAtaqueEspecialBlackMs = 0;
        lanzarAtaqueEspecialBlack();
    }
}

void MainWindow::lanzarAtaqueEspecialBlack() try
{
    if (!nivel2Activo ||
        !nivel2DificilActivo ||
        nivel2EnPausa ||
        gokuBlack == nullptr ||
        gogeta == nullptr ||
        scene == nullptr ||
        hayBolasBlackActivas() ||
        gogetaKamehamehaActivo) {
        return;
    }

    gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_ataque.png", 10);
    gokuBlack->getSprite()->reiniciarAnimacion();

    for (int frame = 1; frame < 10; frame++) {
        QTimer::singleShot(frame * 55, this, [this]() {
            if (!nivel2Activo || !nivel2DificilActivo || gokuBlack == nullptr) {
                return;
            }

            gokuBlack->getSprite()->avanzarFrame();
        });
    }

    QTimer::singleShot(180, this, [this]() {
        try {
            if (!nivel2Activo ||
                !nivel2DificilActivo ||
                nivel2EnPausa ||
                scene == nullptr ||
                gokuBlack == nullptr ||
                gogeta == nullptr ||
                hayBolasBlackActivas()) {
                return;
            }

            QPointF posicionInicial = gokuBlack->getSprite()->sceneBoundingRect().center();
            posicionInicial.setX(posicionInicial.x() - 45);

            QPointF destino = gogeta->sceneBoundingRect().center();
            destino.setY(destino.y() + QRandomGenerator::global()->bounded(-35, 36));

            Proyectil *rafaga = obtenerBolaBlack(":/images/sprites/rafaga_black_dificil.png",
                                                 posicionInicial);

            if (rafaga == nullptr) {
                return;
            }

            rafaga->configurarAtaque(6.8f, 4.0f);
            rafaga->iniciarBateo(destino.x(), destino.y());
        }
        catch (const std::exception &error) {
            manejarErrorJuego("No se pudo crear la rafaga especial de Goku Black.", error);
        }
    });

    QTimer::singleShot(620, this, [this]() {
        try {
            if (!nivel2Activo || !nivel2DificilActivo || gokuBlack == nullptr) {
                return;
            }

            gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_quieto.png", 1);
            gokuBlack->getSprite()->reiniciarAnimacion();
        }
        catch (const std::exception &error) {
            manejarErrorJuego("No se pudo restaurar el sprite de Goku Black despues del ataque.", error);
        }
    });
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo lanzar el ataque especial de Black.", error);
}

void MainWindow::actualizarKamehamehaGogeta(int dtMs)
{
    if (!gogetaKamehamehaActivo || gogeta == nullptr) {
        return;
    }

    tiempoAnimacionGogetaMs += dtMs;

    if (gogeta->getFrameActual() < gogeta->getTotalFrames() - 1) {
        while (tiempoAnimacionGogetaMs >= 90 && gogeta->getFrameActual() < gogeta->getTotalFrames() - 1) {
            tiempoAnimacionGogetaMs -= 90;
            gogeta->avanzarFrame();
        }
    }
    else {
        if (!rafagaFinalKamehamehaLanzada && scene != nullptr) {
            QPixmap pixmap(":/images/sprites/rafaga_final_kamehameha.png");

            if (pixmap.isNull()) {
                throw std::runtime_error("No se pudo cargar el sprite de la rafaga Final Kamehameha.");
            }

            pixmap = pixmap.scaled(300, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            if (pixmap.isNull()) {
                throw std::runtime_error("No se pudo escalar el sprite de la rafaga Final Kamehameha.");
            }

            rafagaFinalKamehameha = scene->addPixmap(pixmap);
            rafagaFinalKamehameha->setOffset(-pixmap.width() * 0.15, -pixmap.height() / 2.0);
            rafagaFinalKamehameha->setZValue(17);
            rafagaFinalKamehameha->setPos(posicionGogetaX + 136.0, posicionActualGogetaY - 10.0);
            rafagaFinalKamehamehaLanzada = true;
        }

        if (rafagaFinalKamehameha != nullptr) {
            return;
        }

        tiempoRetencionKamehamehaMs += dtMs;
        if (tiempoRetencionKamehamehaMs >= 420) {
            gogetaKamehamehaActivo = false;
            tiempoRetencionKamehamehaMs = 0;
            tiempoAnimacionGogetaMs = 0;
            dañoKamehamehaAplicado = false;
            rutaSpriteGogetaActual = rutaSpriteGogetaDireccion;
            gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
            gogeta->fijarTamañoVisual(anchoVisualGogeta, altoVisualGogeta);
            gogeta->reiniciarAnimacion();
            gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
        }
    }
}

void MainWindow::revisarCaidaBola(Proyectil *bola)
{
    QPointF posicionCaida = bola->centro();
    float dañoFreezer = calcularDañoFreezer(posicionCaida);

    if (dañoFreezer > 0.0f) {
        freezer->recibirDaño(dañoFreezer);
        freezer->reproducirImpacto();
        rachaDaño += dañoFreezer;
    }
    else {
        vegito->recibirDaño(0.5f);
        vegito->reproducirImpacto();
        rachaDaño = 0.0f;
    }

    actualizarDificultad();
}

float MainWindow::calcularDañoFreezer(const QPointF &posicionCaida) const
{
    if (zonaDañoAlto.contienePunto(posicionCaida)) {
        return 30.0f;
    }

    if (zonaDañoMedio.contienePunto(posicionCaida)) {
        return 20.0f;
    }

    if (zonaDañoBajo.contienePunto(posicionCaida)) {
        return 10.0f;
    }

    return 0.0f;
}

void MainWindow::actualizarDificultad()
{
    if (freezer == nullptr) {
        return;
    }

    float porcentajeVida = freezer->getVidaActual() / freezer->getVidaMaxima();

    if (porcentajeVida > 0.70f) {
        velocidadFreezer = 3.6f;
        lanzamientoOscilatorio = false;
    }
    else if (porcentajeVida > 0.45f) {
        velocidadFreezer = 3.9f;
        lanzamientoOscilatorio = true;
    }
    else if (porcentajeVida > 0.20f) {
        velocidadFreezer = 4.2f;
        lanzamientoOscilatorio = false;
    }
    else {
        velocidadFreezer = 4.5f;
        lanzamientoOscilatorio = true;
    }
}

void MainWindow::revisarEuforiaVegito()
{
    if (vegito == nullptr) {
        return;
    }

    float limiteKaioken = vegito->getVidaMaxima() * 0.30f;

    if (vegito->getVidaActual() <= limiteKaioken) {
        vegito->activarEuforia();
    }
}

void MainWindow::eliminarBola(Proyectil *bola)
{
    if (bola == nullptr) {
        return;
    }

    if (bolaControlada == bola) {
        bolaControlada = nullptr;
    }

    bola->desactivar();
}

void MainWindow::actualizarHUD()
{
    if (nivel2DificilActivo) {
        barraVidaJugador.actualizarBarraVida(vidaGogetaNivel2,vidaMaximaGogetaNivel2);
        barraVidaEnemigo.actualizarBarraVida(vidaBlackNivel2,vidaMaximaBlackNivel2);
        return;
    }

    if (vegito != nullptr) {
        barraVidaJugador.actualizarBarraVida(vegito->getVidaActual(),vegito->getVidaMaxima());
    }

    if (freezer != nullptr) {
        barraVidaEnemigo.actualizarBarraVida(freezer->getVidaActual(),freezer->getVidaMaxima());
    }

    if (textoEstadoKaioken != nullptr) {
        if (vegito != nullptr && vegito->estaEnEuforia()) {
            textoEstadoKaioken->setPlainText("Kaioken activo: los bateos conectados tambien golpean a Freezer.");
        }
        else {
            textoEstadoKaioken->setPlainText("");
        }
    }
}

void MainWindow::ganarNivel1()
{
    nivel1Activo = false;
    esperandoLanzamientos = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    desactivarBolasFreezer();
    setVisibleHUDVida(false);

    if (textoEstadoKaioken != nullptr) {
        textoEstadoKaioken->setVisible(false);
    }

    mostrarOverlayEstado("Nivel 1 completado",
                         "Freezer fue derrotado.\n\nToma un respiro antes del siguiente combate y elige la dificultad del Nivel 2 cuando quieras continuar.",
                         "Ir al Nivel 2",
                         [this]() {
                             ocultarOverlayEstado();
                             mostrarPantallaNivel2();
                         },
                         "Jugar Nivel 1 otra vez",
                         [this]() {
                             ocultarOverlayEstado();
                             iniciarNivel1();
                         },
                         "Volver al inicio",
                         [this]() {
                             volverAlMenuPrincipal();
                         });
}

void MainWindow::perderNivel1()
{
    nivel1Activo = false;
    esperandoLanzamientos = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    desactivarBolasFreezer();
    setVisibleHUDVida(false);

    if (textoEstadoKaioken != nullptr) {
        textoEstadoKaioken->setVisible(false);
    }

    mostrarOverlayEstado("Nivel 1 perdido",
                         "Vegito se quedo sin vida.\n\nPuedes reintentar el nivel de inmediato o volver al menu principal.",
                         "Reintentar Nivel 1",
                         [this]() {
                             ocultarOverlayEstado();
                             iniciarNivel1();
                         },
                         "Volver al inicio",
                         [this]() {
                             volverAlMenuPrincipal();
                         });
}

void MainWindow::iniciarVariablesNivel2()
{
    posicionActualGogetaY = posicionGogetaY;
    velocidadVerticalGogeta = 0.0;
    saltosUsadosGogeta = 0;
    tiempoAnimacionGogetaMs = 0;
    tiempoRetencionKamehamehaMs = 0;
    tiempoMovimientoBlackDificilMs = 0;
    tiempoAtaqueEspecialBlackMs = 0;
    pelotasAtrapadasNivel2 = 0;
    pelotasPerdidasNivel2 = 0;
    outsNivel2 = 0;
    carrerasBlackNivel2 = 0;
    cargaKamehamehaNivel2 = 0;
    gogetaSaltando = false;
    gogetaKamehamehaActivo = false;
    dañoKamehamehaAplicado = false;
    rafagaFinalKamehamehaLanzada = false;
    moverGogetaIzquierdaActivo = false;
    moverGogetaDerechaActivo = false;
    limpiarRafagaFinalKamehameha();
    esperandoLanzamientoBlack = false;
    vidaGogetaNivel2 = vidaMaximaGogetaNivel2;
    vidaBlackNivel2 = vidaMaximaBlackNivel2;

    const QRectF rectaNivel2 = scene != nullptr ? scene->sceneRect() : QRectF();
    const QRectF rectaAro(rectaNivel2.left() + rectaNivel2.width() * 0.455,rectaNivel2.top() + rectaNivel2.height() * 0.30,rectaNivel2.width() * 0.048,rectaNivel2.height() * 0.19);
    aroNivel2.crearHitboxEliptica(scene, rectaAro);

    desactivarBolasBlack();

    if (nivel2DificilActivo) {
        Proyectil *bolaNormal = obtenerBolaBlack(":/images/sprites/bolaBlack_actualizada.png", QPointF(-300, -300));
        if (bolaNormal != nullptr) {
            bolaNormal->desactivar();
        }

        Proyectil *rafagaBlack = obtenerBolaBlack(":/images/sprites/rafaga_black_dificil.png", QPointF(-300, -300));
        if (rafagaBlack != nullptr) {
            rafagaBlack->desactivar();
        }
    }
    else {
        Proyectil *bolaNormal = obtenerBolaBlack(":/images/sprites/bolaBlack_actualizada.png", QPointF(-300, -300));
        if (bolaNormal != nullptr) {
            bolaNormal->desactivar();
        }
    }

    if (textoEstadoNivel2 == nullptr) {
        textoEstadoNivel2 = scene->addText("");
        textoEstadoNivel2->setDefaultTextColor(Qt::white);
        textoEstadoNivel2->setFont(QFont("Arial", 12, QFont::Bold));
        textoEstadoNivel2->setPos(scene->sceneRect().center().x() - 180,
                                  scene->sceneRect().bottom() - 110);
        textoEstadoNivel2->setZValue(24);
    }

    textoEstadoNivel2->setVisible(true);

    if (nivel2DificilActivo) {
        crearHUDNivel2Dificil();
        setVisibleHUDVida(true);
        actualizarHUD();
    }
    else {
        destruirHUDVida();
        crearMarcadoresNivel2();
        marcadorNivel2Jugador.setVisible(true);
        marcadorNivel2Enemigo.setVisible(true);
    }

    actualizarTextoNivel2();
}

void MainWindow::programarSiguienteLanzamientoBlack()
{
    if (!nivel2Activo ||
        nivel2EnPausa ||
        esperandoLanzamientoBlack ||
        scene == nullptr ||
        gokuBlack == nullptr ||
        hayBolasBlackActivas() ||
        gogetaKamehamehaActivo) {
        return;
    }

    esperandoLanzamientoBlack = true;


    const int esperaMs = nivel2DificilActivo ? QRandomGenerator::global()->bounded(900, 1500): QRandomGenerator::global()->bounded(900, 1700);

    QTimer::singleShot(esperaMs, this, [this]() {
        esperandoLanzamientoBlack = false;

        if (!nivel2Activo ||
            nivel2EnPausa ||
            hayBolasBlackActivas() ||
            gogetaKamehamehaActivo) {
            return;
        }

        lanzarBolaBlack();
    });
}

void MainWindow::lanzarBolaBlack() try
{
    if (!nivel2Activo || nivel2EnPausa || scene == nullptr || gokuBlack == nullptr || gogeta == nullptr) {
        return;
    }

    esperandoLanzamientoBlack = false;

    const QRectF recta = scene->sceneRect();
    const qreal posicionBaseX = recta.right() - 160.0;
    const qreal posicionBaseY = nivel2DificilActivo
                                    ? gokuBlack->getSprite()->pos().y()
                                    : posicionGogetaY - 40.0;

    gokuBlack->setPos(posicionBaseX, posicionBaseY);

    gokuBlack->percibirPosicionJugador(gogeta->sceneBoundingRect().center());
    const Villano::Ataque ataque = gokuBlack->elegirAtaque();
    const QPointF destino = gokuBlack->razonarDestinoLejano(scene->sceneRect());
    gokuBlack->reproducirAtaqueActual();

    QTimer::singleShot(220, this, [this, ataque, destino, posicionBaseX, posicionBaseY]() {
        try {
            if (!nivel2Activo || nivel2EnPausa || scene == nullptr || gokuBlack == nullptr) {
                return;
            }

            gokuBlack->setPos(posicionBaseX, posicionBaseY);

            QPointF posicionInicial = gokuBlack->getSprite()->sceneBoundingRect().center();
            posicionInicial.setX(posicionInicial.x() - 45);

            Proyectil *bola = obtenerBolaBlack(ataque.spriteProyectil, posicionInicial);

            if (bola == nullptr) {
                return;
            }

            const float velocidadLanzamiento = qMax(4.0f,
                                                    ataque.velocidad +
                                                    static_cast<float>(QRandomGenerator::global()->bounded(-6, 9)) * 0.12f);
            bola->configurarAtaque(velocidadLanzamiento, ataque.daño);
            bola->iniciarBateo(destino.x(), destino.y());
        }
        catch (const std::exception &error) {
            manejarErrorJuego("No se pudo crear el proyectil de Goku Black.", error);
        }
    });
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo lanzar la bola de Goku Black.", error);
}

void MainWindow::eliminarBolaBlack(Proyectil *bola)
{
    if (bola == nullptr) {
        return;
    }

    bola->desactivar();
}

void MainWindow::desactivarBolasBlack()
{
    for (int i = 0; i < bolasBlack.size(); i++) {
        if (bolasBlack.at(i) != nullptr) {
            bolasBlack.at(i)->desactivar();
        }
    }
}

bool MainWindow::hayBolasBlackActivas() const
{
    for (int i = 0; i < bolasBlack.size(); i++) {
        if (bolasBlack.at(i) != nullptr && bolasBlack.at(i)->estaActiva()) {
            return true;
        }
    }

    return false;
}

Proyectil *MainWindow::obtenerBolaBlack(const QString &rutaSprite, QPointF posicionInicial)
{
    for (int i = 0; i < bolasBlack.size(); i++) {
        Proyectil *bola = bolasBlack.at(i);

        if (bola != nullptr &&
            !bola->estaActiva() &&
            bola->getRutaSprite() == rutaSprite) {
            bola->reiniciar(posicionInicial, rutaSprite);
            return bola;
        }
    }

    for (int i = 0; i < bolasBlack.size(); i++) {
        Proyectil *bola = bolasBlack.at(i);

        if (bola != nullptr && bola->getRutaSprite() == rutaSprite) {
            return nullptr;
        }
    }

    Proyectil *bola = new Proyectil(scene, rutaSprite, posicionInicial);
    bolasBlack.append(bola);
    return bola;
}

void MainWindow::actualizarSaltoGogeta(int dtMs)
{
    if (gogeta == nullptr) {
        return;
    }

    if (!gogetaSaltando) {
        if (!gogetaKamehamehaActivo) {
            posicionActualGogetaY = posicionGogetaY;
            gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
        }
        return;
    }

    const qreal gravedad = 0.85;
    velocidadVerticalGogeta += gravedad;
    posicionActualGogetaY += velocidadVerticalGogeta;

    if (posicionActualGogetaY >= posicionGogetaY) {
        posicionActualGogetaY = posicionGogetaY;
        velocidadVerticalGogeta = 0.0;
        gogetaSaltando = false;
        saltosUsadosGogeta = 0;
        tiempoAnimacionGogetaMs = 0;
        rutaSpriteGogetaActual = rutaSpriteGogetaDireccion;
        gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
        gogeta->fijarTamañoVisual(anchoVisualGogeta, altoVisualGogeta);
        gogeta->reiniciarAnimacion();
        gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
        return;
    }

    tiempoAnimacionGogetaMs += dtMs;

    while (tiempoAnimacionGogetaMs >= tiempoEntreFramesGogetaMs &&
           gogeta->getFrameActual() < gogeta->getTotalFrames() - 1) {
        tiempoAnimacionGogetaMs -= tiempoEntreFramesGogetaMs;
        gogeta->avanzarFrame();
    }

    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
}

void MainWindow::actualizarTextoNivel2()
{
    if (textoEstadoNivel2 == nullptr) {
        return;
    }

    const int metaNivel2Dificil = 8;
    const int metaNivel2Actual = nivel2DificilActivo ? metaNivel2Dificil : 3;

    marcadorNivel2Jugador.setTexto("Outs: " +
                                   QString::number(outsNivel2) +
                                   "/" +
                                   QString::number(metaNivel2Actual) +
                                   "\nPuntos: " +
                                   QString::number(pelotasAtrapadasNivel2) +
                                   "/3");

    marcadorNivel2Enemigo.setTexto("Carreras: " +
                                   QString::number(carrerasBlackNivel2) +
                                   "/" +
                                   QString::number(metaNivel2Actual) +
                                   "\nPuntos: " +
                                   QString::number(pelotasPerdidasNivel2) +
                                   "/3");

    if (nivel2DificilActivo) {
        QString estadoKamehameha;

        if (cargaKamehamehaNivel2 >= cargaMaximaKamehamehaNivel2) {
            estadoKamehameha = "LISTO - Presiona Q";
        }
        else {
            estadoKamehameha = QString::number(cargaKamehamehaNivel2) +
                               "/" +
                               QString::number(cargaMaximaKamehamehaNivel2);
        }

        textoEstadoNivel2->setPlainText(
            "NIVEL 2 DIFICIL"
            "\nFinal Kamehameha: " + estadoKamehameha +
            "\nControles: A/D mover | W quintuple salto | Q ataque especial"
            );
        return;
    }

    textoEstadoNivel2->setPlainText(
        "NIVEL 2 FACIL\n"
        "Controles: A/D mover | W quintuple salto\n"
        "Cruza el aro amarillo para duplicar la pelota y su valor"
        );
}

void MainWindow::aplicarDañoGogetaNivel2(qreal daño)
{
    vidaGogetaNivel2 -= daño;

    if (vidaGogetaNivel2 < 0.0) {
        vidaGogetaNivel2 = 0.0;
    }
}

void MainWindow::aplicarDañoBlackNivel2(qreal daño)
{
    vidaBlackNivel2 -= daño;

    if (vidaBlackNivel2 < 0.0) {
        vidaBlackNivel2 = 0.0;
    }
}

void MainWindow::dispararFinalKamehameha() try
{
    if (!nivel2Activo ||
        !nivel2DificilActivo ||
        gogeta == nullptr ||
        cargaKamehamehaNivel2 < cargaMaximaKamehamehaNivel2 ||
        gogetaKamehamehaActivo) {
        return;
    }

    gogetaKamehamehaActivo = true;
    reproducirAudioFinalKamehameha();
    dañoKamehamehaAplicado = false;
    rafagaFinalKamehamehaLanzada = false;
    tiempoAnimacionGogetaMs = 0;
    tiempoRetencionKamehamehaMs = 0;
    limpiarRafagaFinalKamehameha();
    cargaKamehamehaNivel2 = 0;
    rutaSpriteGogetaActual = ":/images/sprites/final_kamehameha.png";
    gogeta->cambiarSprite(rutaSpriteGogetaActual, 7);
    gogeta->fijarTamañoVisual(108.0, 142.0);

    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX + 22.0, posicionActualGogetaY);
    actualizarTextoNivel2();
}
catch (const std::exception &error)
{
    manejarErrorJuego("No se pudo ejecutar el Final Kamehameha.", error);
}

void MainWindow::actualizarRafagaFinalKamehameha()
{
    if (!gogetaKamehamehaActivo || rafagaFinalKamehameha == nullptr || scene == nullptr) {
        return;
    }

    rafagaFinalKamehameha->moveBy(velocidadRafagaFinalKamehameha, 0.0);

    if (!dañoKamehamehaAplicado &&
        gokuBlack != nullptr &&
        rafagaFinalKamehameha->sceneBoundingRect().intersects(gokuBlack->getSprite()->sceneBoundingRect())) {
        aplicarDañoBlackNivel2(18.0f);
        dañoKamehamehaAplicado = true;
        actualizarHUD();
        actualizarTextoNivel2();
        limpiarRafagaFinalKamehameha();

        if (vidaBlackNivel2 <= 0.0f) {
            finalizarNivel2("Gogeta derrota a Black",
                            "El Final Kamehameha cerro el combate.\n\nPuedes repetir el duelo o volver al selector del Nivel 2.",
                            true);
            return;
        }
    }

    if (rafagaFinalKamehameha != nullptr &&
        rafagaFinalKamehameha->sceneBoundingRect().left() > scene->sceneRect().right() + 40.0) {
        limpiarRafagaFinalKamehameha();
    }
}

void MainWindow::limpiarRafagaFinalKamehameha()
{
    if (rafagaFinalKamehameha == nullptr) {
        return;
    }

    if (rafagaFinalKamehameha->scene() != nullptr) {
        rafagaFinalKamehameha->scene()->removeItem(rafagaFinalKamehameha);
    }

    delete rafagaFinalKamehameha;
    rafagaFinalKamehameha = nullptr;
}

void MainWindow::finalizarNivel2(const QString &titulo, const QString &mensaje, bool permitirReintento)
{
    nivel2EnPausa = true;
    nivel2Activo = false;
    esperandoLanzamientoBlack = false;
    gogetaKamehamehaActivo = false;
    limpiarRafagaFinalKamehameha();

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    desactivarBolasBlack();
    setVisibleHUDVida(false);
    aroNivel2.setVisible(false);

    if (textoEstadoNivel2 != nullptr) {
        textoEstadoNivel2->setVisible(false);
    }

    actualizarHUD();

    const QString textoBotonReintento = permitirReintento ? QString("Reintentar Nivel 2"): QString();

    mostrarOverlayEstado(titulo,
                         mensaje,
                         "Selector Nivel 2",
                         [this]() {
                             ocultarOverlayEstado();
                             mostrarPantallaNivel2();
                         },
                         textoBotonReintento,
                         [this]() {
                             ocultarOverlayEstado();

                             if (dificultadSeleccionada == Dificil) {
                                 iniciarNivel2Dificil();
                             }
                             else {
                                 iniciarNivel2Facil();
                             }
                         },
                         "Volver al inicio",
                         [this]() {
                             volverAlMenuPrincipal();
                         });
}

bool MainWindow::spriteColisionaConProyectil(Sprite *sprite, Proyectil *bola) const
{
    if (sprite == nullptr || bola == nullptr || bola->getItem() == nullptr) {
        return false;
    }

    return sprite->collidesWithItem(bola->getItem(), Qt::IntersectsItemBoundingRect);
}

bool MainWindow::bolaEnZonaBateo(Proyectil *bola)
{
    if (vegito == nullptr) {
        return false;
    }

    return vegito->bolaEnZonaBateo(bola);
}

QPointF MainWindow::destinoAleatorioBateo()
{
    int tipoDestino = QRandomGenerator::global()->bounded(0, 5);
    qreal destinoX = 650;
    qreal destinoY = 320;

    switch (tipoDestino) {
    case 0:
        destinoX = QRandomGenerator::global()->bounded(170, 260);
        destinoY = QRandomGenerator::global()->bounded(220, 500);
        break;

    case 1:
        destinoX = QRandomGenerator::global()->bounded(1000, 1090);
        destinoY = QRandomGenerator::global()->bounded(220, 500);
        break;

    case 2:
        destinoX = QRandomGenerator::global()->bounded(470, 810);
        destinoY = QRandomGenerator::global()->bounded(465, 560);
        break;

    case 3:
        destinoX = QRandomGenerator::global()->bounded(360, 900);
        destinoY = QRandomGenerator::global()->bounded(40, 95);
        break;

    case 4:
        destinoX = QRandomGenerator::global()->bounded(320, 940);
        destinoY = QRandomGenerator::global()->bounded(140, 420);
        break;
    }

    return QPointF(destinoX, destinoY);
}

void MainWindow::keyPressEvent(QKeyEvent *e) try
{
    if (nivel2Activo) {
        if (e->isAutoRepeat()) {
            e->ignore();
            return;
        }

        if (nivel2DificilActivo && e->key() == Qt::Key_Q) {
            dispararFinalKamehameha();
            return;
        }

        if (e->key() == Qt::Key_A) {
            moverGogetaIzquierdaActivo = true;
            moverGogetaDerechaActivo = false;
            moverGogeta(-velocidadMovimientoGogeta, ":/images/sprites/gogeta_izquierda.png");
            return;
        }

        if (e->key() == Qt::Key_D) {
            moverGogetaDerechaActivo = true;
            moverGogetaIzquierdaActivo = false;
            moverGogeta(velocidadMovimientoGogeta, ":/images/sprites/gogeta_derecha.png");
            return;
        }

        if (e->key() == Qt::Key_W) {
            saltarGogeta();
            return;
        }
    }

    if (e->key() == Qt::Key_P &&
        vegito != nullptr &&
        freezer != nullptr &&
        puedeBatear &&
        nivel1Activo) {

        puedeBatear = false;

        QTimer::singleShot(1000, this, [this]() {
            puedeBatear = true;
        });

        vegito->reproducirAtaque();

        bool golpeo = false;

        for (int i = 0; i < bolasFreezer.size(); i++) {
            Proyectil *bola = bolasFreezer.at(i);

            if (bola != nullptr &&
                bola->estaActiva() &&
                bola->getEstado() == Proyectil::LanzadaPorFreezer &&
                bolaEnZonaBateo(bola)) {

                QPointF destino = destinoAleatorioBateo();
                bola->iniciarBateo(destino.x(), destino.y());
                freezer->aprender(true);
                reproducirAudiosBatazo();

                if (vegito->estaEnEuforia()) {
                    freezer->recibirDaño(8.0f);
                    freezer->reproducirImpacto();
                    mostrarEfectoTajoEspada();
                    rachaDaño += 8.0f;
                    actualizarDificultad();
                    actualizarHUD();

                    if (freezer->getVidaActual() <= 0.0f) {
                        ganarNivel1();
                        return;
                    }
                }

                bolaControlada = bola;
                golpeo = true;
                break;
            }
        }

        if (!golpeo) {
            qDebug() << "Vegito bateo, pero no golpeo la bola";
        }
    }

    if (bolaControlada != nullptr &&
        bolaControlada->getEstado() == Proyectil::BateadaPorVegito &&
        nivel1Activo) {

        vegito->controlarProyectil(bolaControlada, e->key());
    }
}
catch (const std::exception &error)
{
    manejarErrorJuego("Ocurrio un error al procesar la entrada del teclado.", error);
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (nivel2Activo) {
        if (e->isAutoRepeat()) {
            e->ignore();
            return;
        }

        if (e->key() == Qt::Key_A) {
            moverGogetaIzquierdaActivo = false;
            return;
        }

        if (e->key() == Qt::Key_D) {
            moverGogetaDerechaActivo = false;
            return;
        }
    }

    QMainWindow::keyReleaseEvent(e);
}

void MainWindow::moverGogeta(qreal deltaX, const QString &rutaSprite)
{
    if (!nivel2Activo || gogeta == nullptr || scene == nullptr || gogetaKamehamehaActivo) {
        return;
    }

    if (!gogetaSaltando) {
        rutaSpriteGogetaDireccion = rutaSprite;
    }

    if (!gogetaSaltando) {
        if (rutaSpriteGogetaActual != rutaSprite) {
            rutaSpriteGogetaActual = rutaSprite;
            gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
            gogeta->fijarTamañoVisual(anchoVisualGogeta, altoVisualGogeta);
            gogeta->reiniciarAnimacion();
            if (gogeta->getFrameActual() < gogeta->getTotalFrames() - 1) {
                gogeta->avanzarFrame();
            }
        }
        else if (gogeta->getFrameActual() < gogeta->getTotalFrames() - 1) {
            gogeta->avanzarFrame();
        }
    }

    qreal nuevaX = posicionGogetaX + deltaX;
    const qreal limiteIzquierdo = 70;
    const qreal limiteDerecho = scene->sceneRect().right() - 115;

    if (nuevaX < limiteIzquierdo) {
        nuevaX = limiteIzquierdo;
    }

    if (nuevaX > limiteDerecho) {
        nuevaX = limiteDerecho;
    }

    posicionGogetaX = nuevaX;
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
}

void MainWindow::saltarGogeta()
{
    if (!nivel2Activo || gogeta == nullptr || saltosUsadosGogeta >= 5 || gogetaKamehamehaActivo) {
        return;
    }

    gogetaSaltando = true;
    saltosUsadosGogeta++;
    velocidadVerticalGogeta = -12.5;
    tiempoAnimacionGogetaMs = 0;
    rutaSpriteGogetaActual = ":/images/sprites/gogeta_salto.png";
    const QVector<QRectF> framesSalto = {
        QRectF(5, 0, 58, 100),
        QRectF(90, 0, 55, 100),
        QRectF(164, 0, 52, 100),
        QRectF(237, 0, 48, 100),
        QRectF(304, 0, 40, 100),
        QRectF(356, 0, 52, 100)
    };
    const qreal anchoSaltoGogeta = 72.0;
    const qreal altoSaltoGogeta = 120.0;
    gogeta->cambiarSprite(rutaSpriteGogetaActual, framesSalto);
    gogeta->fijarTamañoVisual(anchoSaltoGogeta, altoSaltoGogeta);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
}

void MainWindow::ajustarFondo()
{
    if (!rutaFondoActual.isEmpty()) {
        ponerFondo(rutaFondoActual);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    ui->graphicsView->setGeometry(0, 0, width(), height());

    if (scene != nullptr) {
        scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    }

    ui->gridLayoutWidget->setGeometry(width() / 8,height() / 10,width() * 3 / 4,height() * 2 / 3);

    ui->labelTitulo->setMinimumHeight(height() / 6);
    ui->botonInicio->setMinimumSize(width() / 6, height() / 14);

    if (botonFacil != nullptr) {
        botonFacil->setGeometry(width() / 2 - 150, height() * 3 / 4, 120, 45);
    }

    if (botonDificil != nullptr) {
        botonDificil->setGeometry(width() / 2 + 30, height() * 3 / 4, 120, 45);
    }

    if (labelReglasTitulo != nullptr) {
        labelReglasTitulo->setGeometry(width() / 2 - 220, height() / 8, 440, 55);
    }

    if (labelReglasTexto != nullptr) {
        labelReglasTexto->setGeometry(width() / 2 - 260, height() / 4, 520, height() / 3);
    }

    if (labelNivel2Titulo != nullptr) {
        labelNivel2Titulo->setGeometry(width() / 2 - 220, height() / 8, 440, 55);
    }

    if (labelNivel2Texto != nullptr) {
        labelNivel2Texto->setGeometry(width() / 2 - 260, height() / 4, 520, height() / 3);
    }

    if (botonNivel1 != nullptr) {
        botonNivel1->setGeometry(width() / 2 - 90, height() * 3 / 4 + 62, 180, 45);
    }

    if (botonNivel2Facil != nullptr) {
        botonNivel2Facil->setGeometry(width() / 2 - 110, height() * 3 / 4, 95, 45);
    }

    if (botonNivel2Dificil != nullptr) {
        botonNivel2Dificil->setGeometry(width() / 2 + 15, height() * 3 / 4, 95, 45);
    }

    if (labelOverlayTitulo != nullptr) {
        labelOverlayTitulo->setGeometry(width() / 2 - 260, height() / 7, 520, 60);
    }

    if (labelOverlayTexto != nullptr) {
        labelOverlayTexto->setGeometry(width() / 2 - 280, height() / 4, 560, 185);
    }

    if (botonOverlayPrimario != nullptr) {
        botonOverlayPrimario->setGeometry(width() / 2 - 235, height() * 3 / 4, 150, 45);
    }

    if (botonOverlaySecundario != nullptr) {
        botonOverlaySecundario->setGeometry(width() / 2 - 75, height() * 3 / 4, 150, 45);
    }

    if (botonOverlayTerciario != nullptr) {
        botonOverlayTerciario->setGeometry(width() / 2 + 85, height() * 3 / 4, 150, 45);
    }

    if (nivel2Activo && gogeta != nullptr) {
        posicionGogetaY = scene->sceneRect().height() - 170;

        if (!gogetaSaltando) {
            posicionActualGogetaY = posicionGogetaY;
        }

        gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    }

    if (nivel2Activo && gokuBlack != nullptr) {
        const qreal nuevaPosicionBlackY = nivel2DificilActivo
                                              ? qMin(gokuBlack->getSprite()->pos().y(), scene->sceneRect().bottom() - 220.0)
                                              : posicionGogetaY - 40.0;
        gokuBlack->setPos(scene->sceneRect().right() - 160.0, nuevaPosicionBlackY);
    }

    if (nivel1Activo) {
        configurarZonasBateo();
    }

    if (nivel2Activo) {
        if (nivel2DificilActivo) {
            crearHUDNivel2Dificil();
            setVisibleHUDVida(true);
        }
        else {
            crearMarcadoresNivel2();
            marcadorNivel2Jugador.setVisible(true);
            marcadorNivel2Enemigo.setVisible(true);
        }

        const QRectF rectaNivel2 = scene->sceneRect();
        const QRectF rectaAro(rectaNivel2.left() + rectaNivel2.width() * 0.455,rectaNivel2.top() + rectaNivel2.height() * 0.30,rectaNivel2.width() * 0.048,rectaNivel2.height() * 0.19);
        aroNivel2.crearHitboxEliptica(scene, rectaAro);

        if (textoEstadoNivel2 != nullptr) {
            textoEstadoNivel2->setPos(scene->sceneRect().center().x() - 180,scene->sceneRect().bottom() - 110);
        }

        actualizarHUD();
        actualizarTextoNivel2();
    }

    ajustarFondo();
}
