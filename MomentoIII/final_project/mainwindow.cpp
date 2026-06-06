#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , fondo(nullptr)
    , text(nullptr)
    , l1(nullptr)
    , e1(nullptr)
    , figEn(nullptr)
    , fig(nullptr)
    , timer(nullptr)
    , cronometro(nullptr)
    , timerFr(nullptr)
    , timerNivel1(nullptr)
    , musicaInicio(nullptr)
    , audioVegitoYosha(nullptr)
    , audioBatazoMuchedumbre(nullptr)
    , botonFacil(nullptr)
    , botonDificil(nullptr)
    , labelReglasTitulo(nullptr)
    , labelReglasTexto(nullptr)
    , botonNivel1(nullptr)
    , botonNivel2(nullptr)
    , botonNivel2Facil(nullptr)
    , botonNivel2Dificil(nullptr)
    , labelNivel2Titulo(nullptr)
    , labelNivel2Texto(nullptr)
    , vegito(nullptr)
    , freezer(nullptr)
    , gokuBlack(nullptr)
    , gogeta(nullptr)
    , fondoVidaVegito(nullptr)
    , rellenoVidaVegito(nullptr)
    , bordeVidaVegito(nullptr)
    , textoVidaVegito(nullptr)
    , fondoVidaFreezer(nullptr)
    , rellenoVidaFreezer(nullptr)
    , bordeVidaFreezer(nullptr)
    , textoVidaFreezer(nullptr)
    , textoEstadoKaioken(nullptr)
    , textoEstadoNivel2(nullptr)
    , bolaFreezer(nullptr)
    , bolaControlada(nullptr)
    , timerNivel2(nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , salidaAudioInicio(nullptr)
    , salidaAudioVegitoYosha(nullptr)
    , salidaAudioBatazoMuchedumbre(nullptr)
#endif
{
    ui->setupUi(this);

    connect(ui->botonInicio,
            &QPushButton::clicked,
            this,
            &MainWindow::on_botonInicio_clicked);

    QRect Desktop = QGuiApplication::primaryScreen()->geometry();

    x = Desktop.x();
    y = Desktop.y();
    ancho = Desktop.width() - 100;
    alto = Desktop.height() - 100;

    rachaDanio = 0.0f;
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
    danioKamehamehaAplicado = false;
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
    anchoBarraVida = 280;
    altoBarraVida = 24;

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    ponerFondo("/home/tavo/Escritorio/UNIVERSIDAD GUSAVAMER/U ANTIOQUIA/2026-1/Materias/Informática 2/Proyecto Final/Videojuego_ProyectoFinal/MomentoIII/final_project/images/backgrounds/background_main.png");

    scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

    ui->botonInicio->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/images/backgrounds/botonInicio.png) 0 0 0 0 stretch stretch;"
        "border: none;"
        "background: transparent;"
        "}"
        );

    QPixmap pixmap(":/images/backgrounds/titulo.png");
    ui->labelTitulo->setPixmap(pixmap.scaled(400,
                                             150,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
    ui->labelTitulo->setAlignment(Qt::AlignCenter);

    iniciarAudioInicio();
    iniciarAudiosBatazo();
    crearBotonesDificultad();
    crearTextosReglas();
    crearPantallaNivel2();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
}

MainWindow::~MainWindow()
{
    while (!bolasFreezer.isEmpty()) {
        delete bolasFreezer.takeLast();
    }

    while (!bolasDisponibles.isEmpty()) {
        delete bolasDisponibles.takeLast();
    }

    while (!bolasBlack.isEmpty()) {
        delete bolasBlack.takeLast();
    }

    while (!bolasBlackDisponibles.isEmpty()) {
        delete bolasBlackDisponibles.takeLast();
    }

    destruirHUDVida();

    while (!obstaculos.isEmpty()) {
        delete obstaculos.takeLast();
    }

    delete gogeta;
    delete vegito;
    delete freezer;
    delete gokuBlack;
    delete ui;
}

void MainWindow::setEscena(short numEscena)
{
    Q_UNUSED(numEscena);
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

    imagenFondo = imagenFondo.scaled(ui->graphicsView->width(),
                                     ui->graphicsView->height(),
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);

    fondo = scene->addPixmap(imagenFondo);
    fondo->setPos(0, 0);
    fondo->setZValue(-10);
    fondo->setOpacity(opacity);
}

void MainWindow::mostrarMenuInicio()
{
    ocultarElementosNivel1();
    ocultarElementosNivel2();
    ponerFondo(":/images/backgrounds/background_main.jpg");
    iniciarAudioInicio();
}

void MainWindow::on_botonInicio_clicked()
{
    detenerAudioInicio();
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
    ponerFondo(":/images/backgrounds/background_main.jpg");
    ui->labelTitulo->show();
    ocultarPantallaNivel2();
    ocultarElementosNivel2();
    ui->botonInicio->hide();
    mostrarPantallaReglas();
}

void MainWindow::abrirNivel2DesdeSelector()
{
    mostrarPantallaNivel2();
}

void MainWindow::iniciarNivel2Facil()
{
    detenerAudioInicio();
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
        gogeta->setMantenerTamanoVisual(true);
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
    gogeta->fijarTamanoVisual(anchoVisualGogeta, altoVisualGogeta);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    gogeta->setVisible(true);

    if (gokuBlack != nullptr) {
        delete gokuBlack;
        gokuBlack = nullptr;
    }

    gokuBlack = new Villano(":/images/sprites/black_quieto.png",1,":/images/sprites/black_lanzamiento.png",5,650,135,10.0f);
    gokuBlack->getSprite()->setMantenerTamanoVisual(true);
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

void MainWindow::iniciarNivel2Dificil()
{
    detenerAudioInicio();
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
        gogeta->setMantenerTamanoVisual(true);
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
    gogeta->fijarTamanoVisual(anchoVisualGogeta, altoVisualGogeta);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    gogeta->setVisible(true);

    if (gokuBlack != nullptr) {
        delete gokuBlack;
        gokuBlack = nullptr;
    }

    gokuBlack = new Villano(":/images/sprites/black_dificil_quieto.png",1,":/images/sprites/black_lanzamiento_dificil.png",4,650,135,100.0f);
    gokuBlack->getSprite()->setMantenerTamanoVisual(true);
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

void MainWindow::iniciarNivel1()
{
    detenerAudioInicio();
    nivel2Activo = false;
    ui->labelTitulo->hide();
    ocultarPantallaReglas();
    ocultarOpcionesDificultad();
    ocultarPantallaNivel2();
    ocultarElementosNivel2();

    ponerFondo(":/images/backgrounds/nivel1.png", 1.0);

    if (vegito == nullptr) {
        vegito = new Jugador(":/images/sprites/vegitoBateoQuieto.png",1,":/images/sprites/vegitoBateo.png",5,650,540,20.0f);

        vegito->configurarSpritesEuforia(":/images/sprites/transformacion_vegito.png",4,":/images/sprites/vegito_bateo_kaioken.png",5);
        vegito->configurarSpriteImpacto(":/images/sprites/vegito_impacto.png", 1);
    }

    vegito->agregarAEscena(scene);
    vegito->setVisible(true);
    vegito->setPos(650, 540);

    if (freezer == nullptr) {
        freezer = new Villano(":/images/sprites/freezerPitcher.png",6,":/images/sprites/freezerPitcher.png",6,650,270,100.0f);
        freezer->configurarSpriteImpacto(":/images/sprites/freezer_impacto.png", 1);
    }

    freezer->getSprite()->setMantenerTamanoVisual(true);
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

void MainWindow::iniciarVariablesNivel1()
{
    rachaDanio = 0.0f;
    velocidadFreezer = 3.6f;
    lanzamientoOscilatorio = false;
    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;
    nivel1Activo = true;
    puedeBatear = true;
    bolaFreezer = nullptr;
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

    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    for (int i = 0; i < bolasDisponibles.size(); i++) {
        bolasDisponibles.at(i)->desactivar();
    }

    crearHUDNivel1();
    actualizarHUD();
}

void MainWindow::crearHUDNivel1()
{
    destruirHUDVida();

    crearBarraVida(fondoVidaVegito,rellenoVidaVegito,bordeVidaVegito,textoVidaVegito,20,48,"Vegito",QColor(30, 160, 255));

    crearBarraVida(fondoVidaFreezer,rellenoVidaFreezer,bordeVidaFreezer,textoVidaFreezer,20,108,"Freezer",QColor(220, 40, 40));

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
                             ? scene->sceneRect().width() - anchoBarraVida - 20
                             : 20;

    crearBarraVida(fondoVidaVegito,rellenoVidaVegito,bordeVidaVegito,textoVidaVegito,20,48,"Gogeta",QColor(60, 170, 255));
    crearBarraVida(fondoVidaFreezer,rellenoVidaFreezer,bordeVidaFreezer,textoVidaFreezer,xBlack,48,"Black",QColor(220, 50, 70));
}


void MainWindow::crearBarraVida(QGraphicsRectItem *&fondoBarra,QGraphicsRectItem *&rellenoBarra,QGraphicsRectItem *&bordeBarra,QGraphicsTextItem *&textoBarra,qreal x,qreal y,const QString &etiqueta,const QColor &colorRelleno)
{
    if (scene == nullptr) {
        return;
    }

    fondoBarra = scene->addRect(x,y,anchoBarraVida,altoBarraVida,QPen(Qt::black, 2),QBrush(QColor(35, 35, 35, 190)));

    rellenoBarra = scene->addRect(x + 2,y + 2,anchoBarraVida - 4,altoBarraVida - 4,QPen(Qt::NoPen),QBrush(colorRelleno));

    bordeBarra = scene->addRect(x,y,anchoBarraVida,altoBarraVida,QPen(Qt::white, 2),QBrush(Qt::NoBrush));

    textoBarra = scene->addText(etiqueta);
    textoBarra->setDefaultTextColor(Qt::white);
    textoBarra->setFont(QFont("Arial", 12, QFont::Bold));
    textoBarra->setPos(x, y - 24);

    fondoBarra->setZValue(20);
    rellenoBarra->setZValue(21);
    bordeBarra->setZValue(22);
    textoBarra->setZValue(23);
}

void MainWindow::actualizarBarraVida(QGraphicsRectItem *rellenoBarra,QGraphicsTextItem *textoBarra,qreal x,qreal y,const QString &etiqueta,float vidaActual,float vidaMaxima)
{
    if (rellenoBarra == nullptr || textoBarra == nullptr) {
        return;
    }

    float vidaNormalizada = 0.0f;

    if (vidaMaxima > 0.0f) {
        vidaNormalizada = vidaActual / vidaMaxima;
    }

    if (vidaNormalizada < 0.0f) {
        vidaNormalizada = 0.0f;
    }

    if (vidaNormalizada > 1.0f) {
        vidaNormalizada = 1.0f;
    }

    qreal anchoRelleno = (anchoBarraVida - 4) * vidaNormalizada;
    rellenoBarra->setRect(x + 2,y + 2,anchoRelleno,altoBarraVida - 4);

    textoBarra->setPlainText(etiqueta +": " +QString::number(vidaActual, 'f', 1) +" / " +QString::number(vidaMaxima, 'f', 1));
}

void MainWindow::setVisibleHUDVida(bool visible)
{
    QList<QGraphicsItem*> items;
    items << fondoVidaVegito << rellenoVidaVegito << bordeVidaVegito << textoVidaVegito
          << fondoVidaFreezer << rellenoVidaFreezer << bordeVidaFreezer << textoVidaFreezer;

    for (QGraphicsItem *item : items) {
        if (item != nullptr) {
            item->setVisible(visible);
        }
    }
}

void MainWindow::destruirHUDVida()
{
    QList<QGraphicsItem*> items;
    items << fondoVidaVegito << rellenoVidaVegito << bordeVidaVegito << textoVidaVegito
          << fondoVidaFreezer << rellenoVidaFreezer << bordeVidaFreezer << textoVidaFreezer;

    for (QGraphicsItem *item : items) {
        if (item != nullptr) {
            if (item->scene() != nullptr) {
                item->scene()->removeItem(item);
            }

            delete item;
        }
    }

    fondoVidaVegito = nullptr;
    rellenoVidaVegito = nullptr;
    bordeVidaVegito = nullptr;
    textoVidaVegito = nullptr;
    fondoVidaFreezer = nullptr;
    rellenoVidaFreezer = nullptr;
    bordeVidaFreezer = nullptr;
    textoVidaFreezer = nullptr;
}

void MainWindow::configurarZonasBateo()
{
    limpiarZonasDebug();

    zonaDanioBajo = QPainterPath();
    zonaDanioMedio = QPainterPath();
    zonaDanioAlto = QPainterPath();
    const QRectF rectaEscena = scene != nullptr ? scene->sceneRect() : QRectF(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    const QPointF centroDiamante(rectaEscena.width() * 0.5, rectaEscena.height() * 0.83);
    const qreal anguloInicio = 232.0;
    const qreal anguloFin = 308.0;

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

    const qreal radioInicio = rectaEscena.width() * 0.12;
    const qreal radio10 = rectaEscena.width() * 0.275;
    const qreal radio20 = rectaEscena.width() * 0.35;
    const qreal radio30 = rectaEscena.width() * 0.425;

    zonaDanioBajo = crearSector(radioInicio, radio10);
    zonaDanioMedio = crearSector(radio10, radio20);
    zonaDanioAlto = crearSector(radio20, radio30);
}

void MainWindow::limpiarZonasDebug()
{
    while (!debugZonas.isEmpty()) {
        QGraphicsPathItem *item = debugZonas.takeLast();

        if (item->scene() != nullptr) {
            item->scene()->removeItem(item);
        }

        delete item;
    }
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

    labelReglasTitulo->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 22px;"
        "font-weight: bold;"
        "background-color: rgba(0, 0, 0, 190);"
        "border: 2px solid white;"
        "border-radius: 12px;"
        "padding: 8px;"
        "}"
        );

    labelReglasTexto->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 15px;"
        "font-weight: bold;"
        "background-color: rgba(0, 0, 0, 190);"
        "border: 2px solid white;"
        "border-radius: 12px;"
        "padding: 12px;"
        "}"
        );

    labelReglasTexto->setText(
        "->Golpea la bola con la tecla P cuando llegue a Vegito.\n"
        "->Despues del bateo usa A, D, W y S para corregir su caida.\n"
        "->Cada batazo resta vida a Freezer segun la zona donde caiga.\n"
        "->Si dejas pasar la bola, Vegito pierde vida.\n"
        "->Cuando Vegito baje del 30% de vida entrara en kaioken.\n"
        "->En kaioken, cada bateo conectado tambien lastima directamente a Freezer.\n"
        "->Facil: un solo tipo de ataque.\n"
        "->Dificil: Freezer puede lanzar 3 ataques con distinto dano y velocidad."
        );
}

void MainWindow::crearPantallaNivel2()
{
    botonNivel1 = new QPushButton("Volver a Nivel 1", ui->centralwidget);
    botonNivel2 = new QPushButton("Volver al menu Nivel 2", ui->centralwidget);
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

    botonNivel2->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(170, 90, 20, 220);"
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

    labelNivel2Titulo->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 22px;"
        "font-weight: bold;"
        "background-color: rgba(0, 0, 0, 150);"
        "border: 2px solid white;"
        "border-radius: 12px;"
        "padding: 8px;"
        "}"
        );

    labelNivel2Texto->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 15px;"
        "font-weight: bold;"
        "background-color: rgba(0, 0, 0, 150);"
        "border: 2px solid white;"
        "border-radius: 12px;"
        "padding: 12px;"
        "}"
        );

    labelNivel2Texto->setText(
        "Escoge la dificultad del Nivel 2.\n"
        "En modo facil, Gogeta atrapa las pelotas de Goku Black.\n"
        "En modo dificil, las atrapadas cargan el Final Kamehameha con Q.\n"
        "Controles del modo facil:\n"
        "-> A mueve a Gogeta hacia la izquierda.\n"
        "-> D mueve a Gogeta hacia la derecha.\n"
        "-> W permite hasta triple salto.\n"
        "-> Cada 3 pelotas atrapadas suman 1 out para Gogeta.\n"
        "-> Cada 3 pelotas perdidas suman 1 carrera para Black.\n"
        "En dificil, las 3 atrapadas para out deben ser consecutivas."
        );

    connect(botonNivel1, &QPushButton::clicked, this, &MainWindow::abrirNivel1DesdeSelector);
    connect(botonNivel2, &QPushButton::clicked, this, &MainWindow::abrirNivel2DesdeSelector);
    connect(botonNivel2Facil, &QPushButton::clicked, this, &MainWindow::iniciarNivel2Facil);
    connect(botonNivel2Dificil, &QPushButton::clicked, this, &MainWindow::iniciarNivel2Dificil);
}

void MainWindow::iniciarAudioInicio()
{
    if (musicaInicio == nullptr) {
        musicaInicio = new QMediaPlayer(this);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        salidaAudioInicio = new QAudioOutput(this);
        salidaAudioInicio->setVolume(0.35);
        musicaInicio->setAudioOutput(salidaAudioInicio);
        musicaInicio->setSource(QUrl("qrc:/images/audios/gt-dragonball-intro.mp3"));

        connect(musicaInicio,
                &QMediaPlayer::mediaStatusChanged,
                this,
                [this](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia && musicaInicio != nullptr) {
                        musicaInicio->setPosition(0);
                        musicaInicio->play();
                    }
                });
#else
        musicaInicio->setMedia(QUrl("qrc:/images/audios/gt-dragonball-intro.mp3"));
        musicaInicio->setVolume(35);

        connect(musicaInicio,
                &QMediaPlayer::mediaStatusChanged,
                this,
                [this](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia && musicaInicio != nullptr) {
                        musicaInicio->setPosition(0);
                        musicaInicio->play();
                    }
                });
#endif
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (musicaInicio->playbackState() != QMediaPlayer::PlayingState) {
        musicaInicio->play();
    }
#else
    if (musicaInicio->state() != QMediaPlayer::PlayingState) {
        musicaInicio->play();
    }
#endif
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        salidaAudioVegitoYosha = new QAudioOutput(this);
        salidaAudioVegitoYosha->setVolume(1.0);
        audioVegitoYosha->setAudioOutput(salidaAudioVegitoYosha);
        audioVegitoYosha->setSource(QUrl("qrc:/images/audios/vegito-yosha.mp3"));
#else
        audioVegitoYosha->setMedia(QUrl("qrc:/images/audios/vegito-yosha.mp3"));
        audioVegitoYosha->setVolume(100);
#endif
    }

    if (audioBatazoMuchedumbre == nullptr) {
        audioBatazoMuchedumbre = new QMediaPlayer(this);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        salidaAudioBatazoMuchedumbre = new QAudioOutput(this);
        salidaAudioBatazoMuchedumbre->setVolume(0.95);
        audioBatazoMuchedumbre->setAudioOutput(salidaAudioBatazoMuchedumbre);
        audioBatazoMuchedumbre->setSource(QUrl("qrc:/images/audios/sonido_bateo_muchedumbre.wav"));
#else
        audioBatazoMuchedumbre->setMedia(QUrl("qrc:/images/audios/sonido_bateo_muchedumbre.wav"));
        audioBatazoMuchedumbre->setVolume(95);
#endif
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
    nivel1Activo = false;
    nivel2Activo = false;
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
            "-> W permite hasta triple salto.\n"
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

    if (botonNivel2 != nullptr) {
        botonNivel2->show();
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

    if (botonNivel2 != nullptr) {
        botonNivel2->hide();
    }

    if (botonNivel2Facil != nullptr) {
        botonNivel2Facil->hide();
    }

    if (botonNivel2Dificil != nullptr) {
        botonNivel2Dificil->hide();
    }
}

void MainWindow::ocultarElementosNivel1()
{
    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    if (vegito != nullptr) {
        vegito->setVisible(false);
    }

    if (freezer != nullptr) {
        freezer->setVisible(false);
    }

    setVisibleHUDVida(false);

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
    danioKamehamehaAplicado = false;

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    while (!bolasBlack.isEmpty()) {
        eliminarBolaBlack(bolasBlack.last());
    }

    for (int i = 0; i < bolasBlackDisponibles.size(); i++) {
        bolasBlackDisponibles.at(i)->desactivar();
    }

    if (gogeta != nullptr) {
        gogeta->setVisible(false);
    }

    if (gokuBlack != nullptr) {
        gokuBlack->setVisible(false);
    }

    setVisibleHUDVida(false);

    if (textoEstadoNivel2 != nullptr) {
        textoEstadoNivel2->setVisible(false);
    }
}

void MainWindow::configurarAtaquesFreezer()
{
    ataquesFreezer.clear();
    ataquesFreezer.append({3.6f,
                           1.0f,
                           ":/images/sprites/bolaFreezer.png",
                           ":/images/sprites/freezerPitcher.png",
                           6});

    if (dificultadSeleccionada == Dificil) {
        ataquesFreezer.clear();
        ataquesFreezer.append({3.6f,
                               1.0f,
                               ":/images/sprites/bolaFreezer.png",
                               ":/images/sprites/freezerPitcher.png",
                               6});
        ataquesFreezer.append({4.3f,
                               1.5f,
                               ":/images/sprites/bolaFreezer2.png",
                               ":/images/sprites/freezerPitcher2.png",
                               5});
        ataquesFreezer.append({5.0f,
                               2.0f,
                               ":/images/sprites/bolafreezer3.png",
                               ":/images/sprites/freezerPitcher3.png",
                               3});
    }

    if (freezer != nullptr) {
        freezer->configurarAtaques(ataquesFreezer);
    }
}

Villano::Ataque MainWindow::obtenerAtaqueActual() const
{
    if (ataquesFreezer.isEmpty()) {
        return {3.6f,
                1.0f,
                ":/images/sprites/bolaFreezer.png",
                ":/images/sprites/freezerPitcher.png",
                6};
    }

    if (dificultadSeleccionada == Facil) {
        return ataquesFreezer.first();
    }

    int indiceAtaque = QRandomGenerator::global()->bounded(ataquesFreezer.size());
    return ataquesFreezer.at(indiceAtaque);
}

Proyectil *MainWindow::obtenerBolaDisponible(const Villano::Ataque &ataque, QPointF posicionInicial)
{
    Proyectil *bola = nullptr;

    if (!bolasDisponibles.isEmpty()) {
        bola = bolasDisponibles.takeLast();
        bola->reiniciar(posicionInicial, ataque.spriteProyectil);
    }
    else {
        bola = new Proyectil(scene, ataque.spriteProyectil, posicionInicial);
    }

    bola->configurarAtaque(ataque.velocidad, ataque.dano);
    return bola;
}

void MainWindow::programarSiguienteLanzamiento()
{
    if (!nivel1Activo || esperandoLanzamientos) {
        return;
    }

    esperandoLanzamientos = true;

    if (rachaDanio >= 12.0f) {
        bolasPendientesPorLanzar = 2;
        QTimer::singleShot(2000, this, &MainWindow::lanzarBolaFreezer);
        QTimer::singleShot(4000, this, &MainWindow::lanzarBolaFreezer);
    }
    else {
        bolasPendientesPorLanzar = 1;
        QTimer::singleShot(2000, this, &MainWindow::lanzarBolaFreezer);
    }
}

void MainWindow::lanzarBolaFreezer()
{
    if (!nivel1Activo || scene == nullptr || freezer == nullptr) {
        return;
    }

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
        return;
    }

    Villano::Ataque ataque = freezer->elegirAtaque();
    freezer->reproducirAtaqueActual();
    bolasPendientesPorLanzar--;

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
    }

    QTimer::singleShot(220, this, [this, ataque]() {
        if (!nivel1Activo || scene == nullptr || freezer == nullptr) {
            return;
        }

        QPointF posicionInicial = freezer->getSprite()->sceneBoundingRect().center();
        posicionInicial.setY(posicionInicial.y() + 45);

        Proyectil *nuevaBola = obtenerBolaDisponible(ataque, posicionInicial);
        bolasFreezer.append(nuevaBola);
        bolaFreezer = nuevaBola;
    });
}

void MainWindow::actualizarNivel1()
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

        if (bola->getEstado() == Proyectil::LanzadaPorFreezer) {
            float velocidadAtaque = velocidadFreezer;

            if (dificultadSeleccionada == Dificil) {
                velocidadAtaque += bola->getVelocidadAtaque() - 3.6f;
            }

            bola->moverLanzamiento(velocidadAtaque, lanzamientoOscilatorio);

            if (bola->pasoLinea(vegito->getSprite()->y() + 70)) {
                vegito->recibirDanio(bola->getDanoAtaque());
                vegito->reproducirImpacto();
                freezer->percibir(false);
                freezer->aprender(false);
                rachaDanio = 0.0f;

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

    actualizarHUD();

    if (bolasFreezer.isEmpty() && !esperandoLanzamientos) {
        programarSiguienteLanzamiento();
    }
}

void MainWindow::actualizarNivel2()
{
    if (!nivel2Activo || nivel2EnPausa || gogeta == nullptr || gokuBlack == nullptr) {
        return;
    }

    const float dt = 0.016f;

    gokuBlack->actualizar(16);
    actualizarSaltoGogeta(16);
    actualizarKamehamehaGogeta(16);

    if (nivel2DificilActivo) {
        actualizarBlackDificil(16);
    }

    for (int i = bolasBlack.size() - 1; i >= 0; i--) {
        Proyectil *bola = bolasBlack.at(i);
        const float factorVelocidad = bola->getVelocidadAtaque() / 3.6f;
        const bool esAtaqueEspecial = nivel2DificilActivo && bola->getDanoAtaque() >= 3.0f;
        bola->actualizarBateo(dt * factorVelocidad);

        if (spriteColisionaConProyectil(gogeta, bola)) {
            if (esAtaqueEspecial) {
                aplicarDanioGogetaNivel2(18.0f);
            }
            else {
                pelotasAtrapadasNivel2++;
                gokuBlack->aprenderTrayectoria(true);

                if (nivel2DificilActivo) {
                    if (cargaKamehamehaNivel2 < cargaMaximaKamehamehaNivel2) {
                        cargaKamehamehaNivel2++;
                    }
                }
            }

            eliminarBolaBlack(bola);

            if (!esAtaqueEspecial && pelotasAtrapadasNivel2 >= 3) {
                pelotasAtrapadasNivel2 = 0;
                outsNivel2++;

                if (nivel2DificilActivo) {
                    aplicarDanioBlackNivel2(12.0f);
                }
            }

            actualizarHUD();
            actualizarTextoNivel2();

            if (outsNivel2 >= 3) {
                finalizarNivel2("Gogeta gana con 3 outs. Vuelve a entrar al nivel 2 para jugar otra vez.");
                return;
            }

            if (nivel2DificilActivo) {
                if (vidaGogetaNivel2 <= 0.0f) {
                    finalizarNivel2("Goku Black vacio la vida de Gogeta. Vuelve a entrar al nivel 2 para reintentar.");
                    return;
                }

                if (vidaBlackNivel2 <= 0.0f) {
                    finalizarNivel2("Gogeta derroto a Goku Black. Vuelve a entrar al nivel 2 para jugar otra vez.");
                    return;
                }
            }

            continue;
        }

        if (bola->terminoVuelo()) {
            eliminarBolaBlack(bola);

            if (!esAtaqueEspecial) {
                pelotasPerdidasNivel2++;
                gokuBlack->aprenderTrayectoria(false);

                if (nivel2DificilActivo && pelotasAtrapadasNivel2 > 0) {
                    pelotasAtrapadasNivel2 = 0;
                }

                if (pelotasPerdidasNivel2 >= 3) {
                    pelotasPerdidasNivel2 = 0;
                    carrerasBlackNivel2++;

                    if (nivel2DificilActivo) {
                        aplicarDanioGogetaNivel2(12.0f);
                    }
                }
            }

            actualizarHUD();
            actualizarTextoNivel2();

            if (carrerasBlackNivel2 >= 3) {
                finalizarNivel2("Goku Black gana con 3 carreras. Vuelve a entrar al nivel 2 para reintentar.");
                return;
            }

            if (nivel2DificilActivo) {
                if (vidaGogetaNivel2 <= 0.0f) {
                    finalizarNivel2("Goku Black vacio la vida de Gogeta. Vuelve a entrar al nivel 2 para reintentar.");
                    return;
                }

                if (vidaBlackNivel2 <= 0.0f) {
                    finalizarNivel2("Gogeta derroto a Goku Black. Vuelve a entrar al nivel 2 para jugar otra vez.");
                    return;
                }
            }
        }
    }

    if (nivel2Activo && bolasBlack.isEmpty()) {
        programarSiguienteLanzamientoBlack();
    }
}

void MainWindow::actualizarBlackDificil(int dtMs)
{
    if (!nivel2DificilActivo || gokuBlack == nullptr || gogeta == nullptr || scene == nullptr) {
        return;
    }

    tiempoMovimientoBlackDificilMs += dtMs;
    tiempoAtaqueEspecialBlackMs += dtMs;

    if (tiempoMovimientoBlackDificilMs >= 850) {
        tiempoMovimientoBlackDificilMs = 0;

        if (QRandomGenerator::global()->bounded(100) < 55) {
            const qreal posicionX = scene->sceneRect().right() - 160.0;
            const qreal posicionActualYBlack = gokuBlack->getSprite()->pos().y();
            const qreal destinoY = qBound(scene->sceneRect().top() + 120.0,
                                          posicionActualGogetaY - 40.0,
                                          scene->sceneRect().bottom() - 220.0);
            qreal nuevaY = posicionActualYBlack + (destinoY - posicionActualYBlack) * 0.45;

            if (nuevaY < posicionActualYBlack) {
                gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_vuelo.png", 1);
            }
            else {
                gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_bajada.png", 1);
            }

            gokuBlack->setPos(posicionX, nuevaY);

            QTimer::singleShot(220, this, [this]() {
                if (!nivel2Activo || !nivel2DificilActivo || gokuBlack == nullptr) {
                    return;
                }

                gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_quieto.png", 1);
                gokuBlack->getSprite()->reiniciarAnimacion();
            });
        }
    }

    if (tiempoAtaqueEspecialBlackMs >= 4200) {
        tiempoAtaqueEspecialBlackMs = 0;
        lanzarAtaqueEspecialBlack();
    }
}

void MainWindow::lanzarAtaqueEspecialBlack()
{
    if (!nivel2Activo || !nivel2DificilActivo || nivel2EnPausa || gokuBlack == nullptr || gogeta == nullptr || scene == nullptr) {
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
        if (!nivel2Activo || !nivel2DificilActivo || nivel2EnPausa || scene == nullptr || gokuBlack == nullptr || gogeta == nullptr) {
            return;
        }

        QPointF posicionInicial = gokuBlack->getSprite()->sceneBoundingRect().center();
        QPointF destino = gogeta->sceneBoundingRect().center();
        Proyectil *rafaga = nullptr;

        if (!bolasBlackDisponibles.isEmpty()) {
            rafaga = bolasBlackDisponibles.takeLast();
            rafaga->reiniciar(posicionInicial, ":/images/sprites/rafaga_black_dificil.png");
        }
        else {
            rafaga = new Proyectil(scene, ":/images/sprites/rafaga_black_dificil.png", posicionInicial);
        }

        rafaga->configurarAtaque(6.8f, 4.0f);
        rafaga->iniciarBateo(destino.x(), destino.y());
        bolasBlack.append(rafaga);
    });

    QTimer::singleShot(620, this, [this]() {
        if (!nivel2Activo || !nivel2DificilActivo || gokuBlack == nullptr) {
            return;
        }

        gokuBlack->getSprite()->cambiarSprite(":/images/sprites/black_dificil_quieto.png", 1);
        gokuBlack->getSprite()->reiniciarAnimacion();
    });
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
        if (!danioKamehamehaAplicado) {
            aplicarDanioBlackNivel2(38.0f);
            danioKamehamehaAplicado = true;
            actualizarHUD();
            actualizarTextoNivel2();

            if (vidaBlackNivel2 <= 0.0f) {
                finalizarNivel2("Gogeta derroto a Goku Black. Vuelve a entrar al nivel 2 para jugar otra vez.");
                return;
            }
        }

        tiempoRetencionKamehamehaMs += dtMs;

        if (tiempoRetencionKamehamehaMs >= 420) {
            gogetaKamehamehaActivo = false;
            tiempoRetencionKamehamehaMs = 0;
            tiempoAnimacionGogetaMs = 0;
            danioKamehamehaAplicado = false;
            rutaSpriteGogetaActual = rutaSpriteGogetaDireccion;
            gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
            gogeta->fijarTamanoVisual(anchoVisualGogeta, altoVisualGogeta);
            gogeta->reiniciarAnimacion();
            gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
        }
    }
}

void MainWindow::revisarCaidaBola(Proyectil *bola)
{
    QPointF posicionCaida = bola->centro();
    float danoFreezer = calcularDanioFreezer(posicionCaida);

    if (danoFreezer > 0.0f) {
        freezer->recibirDanio(danoFreezer);
        freezer->reproducirImpacto();
        rachaDanio += danoFreezer;
    }
    else {
        vegito->recibirDanio(0.5f);
        vegito->reproducirImpacto();
        rachaDanio = 0.0f;
    }

    actualizarDificultad();
}

float MainWindow::calcularDanioFreezer(const QPointF &posicionCaida) const
{
    if (zonaDanioAlto.contains(posicionCaida)) {
        return 30.0f;
    }

    if (zonaDanioMedio.contains(posicionCaida)) {
        return 20.0f;
    }

    if (zonaDanioBajo.contains(posicionCaida)) {
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

    bolasFreezer.removeOne(bola);

    if (bolaControlada == bola) {
        bolaControlada = nullptr;
    }

    if (bolaFreezer == bola) {
        bolaFreezer = nullptr;
    }

    bola->desactivar();

    if (!bolasDisponibles.contains(bola)) {
        bolasDisponibles.append(bola);
    }
}

void MainWindow::actualizarHUD()
{
    if (nivel2DificilActivo) {
        const qreal xBlack = scene != nullptr
                                 ? scene->sceneRect().width() - anchoBarraVida - 20
                                 : 20;

        actualizarBarraVida(rellenoVidaVegito,
                            textoVidaVegito,
                            20,
                            48,
                            "Gogeta",
                            vidaGogetaNivel2,
                            vidaMaximaGogetaNivel2);

        actualizarBarraVida(rellenoVidaFreezer,
                            textoVidaFreezer,
                            xBlack,
                            48,
                            "Black",
                            vidaBlackNivel2,
                            vidaMaximaBlackNivel2);
        return;
    }

    if (vegito != nullptr) {
        actualizarBarraVida(rellenoVidaVegito,
                            textoVidaVegito,
                            20,
                            48,
                            "Vegito",
                            vegito->getVidaActual(),
                            vegito->getVidaMaxima());
    }

    if (freezer != nullptr) {
        actualizarBarraVida(rellenoVidaFreezer,
                            textoVidaFreezer,
                            20,
                            108,
                            "Freezer",
                            freezer->getVidaActual(),
                            freezer->getVidaMaxima());
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

    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    mostrarPantallaNivel2();
}

void MainWindow::perderNivel1()
{
    nivel1Activo = false;
    esperandoLanzamientos = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    QGraphicsTextItem *mensaje = scene->addText("PERDISTE");
    mensaje->setDefaultTextColor(Qt::red);
    mensaje->setScale(3);
    mensaje->setPos(540, 330);
    mensaje->setZValue(20);
}

void MainWindow::moveFig()
{
    lanzarBolaFreezer();
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
    danioKamehamehaAplicado = false;
    esperandoLanzamientoBlack = false;
    vidaGogetaNivel2 = vidaMaximaGogetaNivel2;
    vidaBlackNivel2 = vidaMaximaBlackNivel2;

    while (!bolasBlack.isEmpty()) {
        eliminarBolaBlack(bolasBlack.last());
    }

    for (int i = 0; i < bolasBlackDisponibles.size(); i++) {
        bolasBlackDisponibles.at(i)->desactivar();
    }

    if (textoEstadoNivel2 == nullptr) {
        textoEstadoNivel2 = scene->addText("");
        textoEstadoNivel2->setDefaultTextColor(Qt::white);
        textoEstadoNivel2->setFont(QFont("Arial", 12, QFont::Bold));
        textoEstadoNivel2->setPos(20, 22);
        textoEstadoNivel2->setZValue(24);
    }

    textoEstadoNivel2->setVisible(true);

    if (nivel2DificilActivo) {
        crearHUDNivel2Dificil();
        setVisibleHUDVida(true);
        actualizarHUD();
    }
    else {
        setVisibleHUDVida(false);
    }

    actualizarTextoNivel2();
}

void MainWindow::programarSiguienteLanzamientoBlack()
{
    if (!nivel2Activo || nivel2EnPausa || esperandoLanzamientoBlack || scene == nullptr || gokuBlack == nullptr || !bolasBlack.isEmpty()) {
        return;
    }

    esperandoLanzamientoBlack = true;
    const int esperaMs = nivel2DificilActivo
                             ? QRandomGenerator::global()->bounded(520, 900)
                             : QRandomGenerator::global()->bounded(650, 1100);

    QTimer::singleShot(esperaMs, this, [this]() {
        esperandoLanzamientoBlack = false;

        if (!nivel2Activo || nivel2EnPausa || !bolasBlack.isEmpty()) {
            return;
        }

        lanzarBolaBlack();
    });
}

void MainWindow::lanzarBolaBlack()
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
        if (!nivel2Activo || nivel2EnPausa || scene == nullptr || gokuBlack == nullptr) {
            return;
        }

        gokuBlack->setPos(posicionBaseX, posicionBaseY);

        QPointF posicionInicial = gokuBlack->getSprite()->sceneBoundingRect().center();
        posicionInicial.setX(posicionInicial.x() - 45);

        Proyectil *bola = nullptr;

        if (!bolasBlackDisponibles.isEmpty()) {
            bola = bolasBlackDisponibles.takeLast();
            bola->reiniciar(posicionInicial, ataque.spriteProyectil);
        }
        else {
            bola = new Proyectil(scene, ataque.spriteProyectil, posicionInicial);
        }

        bola->configurarAtaque(ataque.velocidad, ataque.dano);
        bola->iniciarBateo(destino.x(), destino.y());
        bolasBlack.append(bola);
    });
}

void MainWindow::eliminarBolaBlack(Proyectil *bola)
{
    if (bola == nullptr) {
        return;
    }

    bolasBlack.removeOne(bola);
    bola->desactivar();

    if (!bolasBlackDisponibles.contains(bola)) {
        bolasBlackDisponibles.append(bola);
    }
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
        gogeta->fijarTamanoVisual(anchoVisualGogeta, altoVisualGogeta);
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

    if (nivel2DificilActivo) {
        textoEstadoNivel2->setPlainText(
            "Nivel 2 dificil | Outs: " + QString::number(outsNivel2) + "/3"
            " | Carreras Black: " + QString::number(carrerasBlackNivel2) + "/3"
            "\nRacha atrapadas: " + QString::number(pelotasAtrapadasNivel2) + "/3"
            " | Carga Final Kamehameha: " + QString::number(cargaKamehamehaNivel2) + "/" + QString::number(cargaMaximaKamehamehaNivel2) +
            "\nA y D para moverte. W para triple salto. Q para disparar."
            );
        return;
    }

    textoEstadoNivel2->setPlainText(
        "Nivel 2 | Outs Gogeta: " + QString::number(outsNivel2) + "/3"
        " | Carreras Black: " + QString::number(carrerasBlackNivel2) + "/3"
        "\nAtrapadas: " + QString::number(pelotasAtrapadasNivel2) + "/3"
        " | Perdidas: " + QString::number(pelotasPerdidasNivel2) + "/3"
        "\nA y D para moverte. W para triple salto."
        );
}

void MainWindow::aplicarDanioGogetaNivel2(qreal dano)
{
    vidaGogetaNivel2 -= dano;

    if (vidaGogetaNivel2 < 0.0) {
        vidaGogetaNivel2 = 0.0;
    }
}

void MainWindow::aplicarDanioBlackNivel2(qreal dano)
{
    vidaBlackNivel2 -= dano;

    if (vidaBlackNivel2 < 0.0) {
        vidaBlackNivel2 = 0.0;
    }
}

void MainWindow::dispararFinalKamehameha()
{
    if (!nivel2Activo || !nivel2DificilActivo || gogeta == nullptr || cargaKamehamehaNivel2 < cargaMaximaKamehamehaNivel2 || gogetaKamehamehaActivo) {
        return;
    }

    gogetaKamehamehaActivo = true;
    danioKamehamehaAplicado = false;
    tiempoAnimacionGogetaMs = 0;
    tiempoRetencionKamehamehaMs = 0;
    cargaKamehamehaNivel2 = 0;
    rutaSpriteGogetaActual = ":/images/sprites/final_kamehameha.png";

    const QVector<QRectF> framesKamehameha = {
        QRectF(0, 0, 90, 150),
        QRectF(90, 0, 90, 150),
        QRectF(180, 0, 90, 150),
        QRectF(270, 0, 90, 150),
        QRectF(360, 0, 90, 150),
        QRectF(450, 0, 415, 150)
    };

    gogeta->cambiarSprite(rutaSpriteGogetaActual, framesKamehameha);
    gogeta->fijarTamanoVisual(140.0, 150.0);
    gogeta->reiniciarAnimacion();
    gogeta->setPos(posicionGogetaX, posicionActualGogetaY);
    actualizarTextoNivel2();
}

void MainWindow::finalizarNivel2(const QString &mensaje)
{
    nivel2EnPausa = true;
    nivel2Activo = false;
    esperandoLanzamientoBlack = false;
    gogetaKamehamehaActivo = false;

    if (timerNivel2 != nullptr) {
        timerNivel2->stop();
    }

    while (!bolasBlack.isEmpty()) {
        eliminarBolaBlack(bolasBlack.last());
    }

    if (textoEstadoNivel2 != nullptr) {
        textoEstadoNivel2->setPlainText(mensaje);
    }

    actualizarHUD();
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

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (nivel2Activo) {
        if (nivel2DificilActivo && e->key() == Qt::Key_Q) {
            dispararFinalKamehameha();
            return;
        }

        if (e->key() == Qt::Key_A) {
            moverGogeta(-20, ":/images/sprites/gogeta_izquierda.png");
            return;
        }

        if (e->key() == Qt::Key_D) {
            moverGogeta(20, ":/images/sprites/gogeta_derecha.png");
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

            if (bola->getEstado() == Proyectil::LanzadaPorFreezer &&
                bolaEnZonaBateo(bola)) {

                QPointF destino = destinoAleatorioBateo();
                bola->iniciarBateo(destino.x(), destino.y());
                freezer->percibir(true);
                freezer->aprender(true);
                reproducirAudiosBatazo();

                if (vegito->estaEnEuforia()) {
                    freezer->recibirDanio(8.0f);
                    freezer->reproducirImpacto();
                    rachaDanio += 8.0f;
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

void MainWindow::moverGogeta(qreal deltaX, const QString &rutaSprite)
{
    if (!nivel2Activo || gogeta == nullptr || scene == nullptr || gogetaKamehamehaActivo) {
        return;
    }

    rutaSpriteGogetaDireccion = rutaSprite;

    if (!gogetaSaltando) {
        if (rutaSpriteGogetaActual != rutaSprite) {
            rutaSpriteGogetaActual = rutaSprite;
            gogeta->cambiarSprite(rutaSpriteGogetaActual, 3);
            gogeta->fijarTamanoVisual(anchoVisualGogeta, altoVisualGogeta);
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
    const qreal limiteDerecho = scene->sceneRect().center().x() - 70;

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
    if (!nivel2Activo || gogeta == nullptr || saltosUsadosGogeta >= 3 || gogetaKamehamehaActivo) {
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
    gogeta->fijarTamanoVisual(anchoSaltoGogeta, altoSaltoGogeta);
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

    ui->gridLayoutWidget->setGeometry(width() / 8,
                                      height() / 10,
                                      width() * 3 / 4,
                                      height() * 2 / 3);

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
        botonNivel1->setGeometry(width() / 2 - 250, height() * 3 / 4, 160, 45);
    }

    if (botonNivel2 != nullptr) {
        botonNivel2->setGeometry(width() / 2 + 90, height() * 3 / 4, 170, 45);
    }

    if (botonNivel2Facil != nullptr) {
        botonNivel2Facil->setGeometry(width() / 2 - 90, height() * 3 / 4, 80, 45);
    }

    if (botonNivel2Dificil != nullptr) {
        botonNivel2Dificil->setGeometry(width() / 2 + 20, height() * 3 / 4, 80, 45);
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

    if (nivel2Activo && nivel2DificilActivo) {
        crearHUDNivel2Dificil();
        setVisibleHUDVida(true);
        actualizarHUD();
    }

    ajustarFondo();
}
