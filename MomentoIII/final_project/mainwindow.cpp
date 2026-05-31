#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "personaje.h"
#include "proyectil.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , fondo(nullptr)
    , vegito(nullptr)
    , freezer(nullptr)
    , bolaFreezer(nullptr)
    , bolaControlada(nullptr)
    , panelHUD(nullptr)
    , textoPuntaje(nullptr)
    , textoVidas(nullptr)
    , timerNivel1(nullptr)
{
    ui->setupUi(this);

    /*
        Conecta el botón de inicio con la función que inicia el nivel 1.
    */
    connect(ui->botonInicio,
            &QPushButton::clicked,
            this,
            &MainWindow::on_botonInicio_clicked);

    /*
        Se obtiene la geometría de la pantalla.
    */
    QRect Desktop = QGuiApplication::primaryScreen()->geometry();

    x = Desktop.x();
    y = Desktop.y();
    ancho = Desktop.width() - 100;
    alto = Desktop.height() - 100;

    /*
        Variables iniciales del juego.
    */
    puntaje = 0;
    rachaPuntos = 0;
    vidas = 20.0;

    velocidadFreezer = 3.6;
    lanzamientoOscilatorio = false;

    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;
    nivel1Activo = false;
    puedeBatear = true;

    /*
        Se crea la escena gráfica principal.
    */
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    /*
        Fondo del menú principal.

        Esta ruta absoluta es la que venías usando.
        Si luego quieres, se puede cambiar por ruta del .qrc.
    */
    ponerFondo("/home/tavo/Escritorio/UNIVERSIDAD GUSAVAMER/U ANTIOQUIA/2026-1/Materias/Informática 2/Proyecto Final/Videojuego_ProyectoFinal/MomentoIII/final_project/images/backgrounds/background_main.png");

    scene->setSceneRect(0,
                        0,
                        ui->graphicsView->width(),
                        ui->graphicsView->height());

    /*
        Imagen del botón de inicio.
    */
    ui->botonInicio->setStyleSheet(
        "QPushButton {"
        "border-image: url(:/images/backgrounds/botonInicio.png) 0 0 0 0 stretch stretch;"
        "border: none;"
        "background: transparent;"
        "}"
        );

    /*
        Imagen del título.
    */
    QPixmap pixmap(":/images/backgrounds/titulo.png");
    ui->labelTitulo->setPixmap(pixmap.scaled(400,
                                             150,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));

    ui->labelTitulo->setAlignment(Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEscena(short numEscena)
{
    /*
        Función reservada.

        Se deja para futuros cambios de escena.
    */
    Q_UNUSED(numEscena);
}

void MainWindow::ponerFondo(QString ruta, float opacity)
{
    /*
        Guarda la ruta para poder reajustar el fondo
        si cambia el tamaño de la ventana.
    */
    QPixmap imagenFondo(ruta);
    rutaFondoActual = ruta;

    /*
        Si ya había un fondo, se elimina para evitar que se superponga.
    */
    if (fondo != nullptr) {
        scene->removeItem(fondo);
        delete fondo;
        fondo = nullptr;
    }

    /*
        Escala el fondo al tamaño del graphicsView.
    */
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
    /*
        Limpia la escena y vuelve a colocar el fondo del menú.
    */
    scene->clear();
    fondo = nullptr;

    ponerFondo(":/images/backgrounds/background_main.jpg");
}

void MainWindow::on_botonInicio_clicked()
{
    /*
        Al presionar inicio:
        - se ocultan los elementos del menú,
        - se carga el fondo del nivel 1,
        - se crean Vegito y Freezer,
        - se inicia el timer principal del nivel.
    */
    ui->labelTitulo->hide();
    ui->botonInicio->hide();

    ponerFondo(":/images/backgrounds/nivel1.png", 1.0);

    vegito = new Personaje(":/images/sprites/vegitoBateo.png", 5, 650, 540);
    vegito->agregarAEscena(scene);

    freezer = new Personaje(":/images/sprites/freezerPitcher.png", 6, 650, 270);
    freezer->agregarAEscena(scene);

    iniciarVariablesNivel1();

    timerNivel1 = new QTimer(this);
    connect(timerNivel1,
            &QTimer::timeout,
            this,
            &MainWindow::actualizarNivel1);

    timerNivel1->start(16);

    programarSiguienteLanzamiento();
}

void MainWindow::iniciarVariablesNivel1()
{
    /*
        Reinicia todos los valores del nivel 1.
    */
    puntaje = 0;
    rachaPuntos = 0;
    vidas = 20.0;

    velocidadFreezer = 3.6;
    lanzamientoOscilatorio = false;

    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;

    nivel1Activo = true;
    puedeBatear = true;

    bolaFreezer = nullptr;
    bolaControlada = nullptr;

    bolasFreezer.clear();

    /*
        Zonas de puntaje.

        Estas coordenadas dependen del fondo del nivel.
    */
    zona10 = QRectF(330, 330, 640, 130);
    zona20 = QRectF(390, 230, 520, 100);
    zona30 = QRectF(460, 130, 380, 90);

    /*
        Si ya había HUD de una partida anterior, se elimina.
    */
    if (textoPuntaje != nullptr) {
        scene->removeItem(textoPuntaje);
        delete textoPuntaje;
        textoPuntaje = nullptr;
    }

    if (textoVidas != nullptr) {
        scene->removeItem(textoVidas);
        delete textoVidas;
        textoVidas = nullptr;
    }

    if (panelHUD != nullptr) {
        scene->removeItem(panelHUD);
        delete panelHUD;
        panelHUD = nullptr;
    }

    /*
        Panel negro transparente para que los textos se lean bien.
    */
    panelHUD = scene->addRect(18, 18, 260, 100,
                              QPen(Qt::black),
                              QBrush(QColor(0, 0, 0, 160)));

    panelHUD->setZValue(9);

    QFont fuenteHUD("Arial", 16, QFont::Bold);

    /*
        Texto de puntaje.

        Se crea vacío y luego actualizarHUD() pone el valor real.
    */
    textoPuntaje = scene->addText("");
    textoPuntaje->setFont(fuenteHUD);
    textoPuntaje->setDefaultTextColor(Qt::white);
    textoPuntaje->setPos(35, 30);
    textoPuntaje->setZValue(10);
    textoPuntaje->setTextWidth(220);

    /*
        Texto de vidas.

        También se crea vacío para evitar superposición de números.
    */
    textoVidas = scene->addText("");
    textoVidas->setFont(fuenteHUD);
    textoVidas->setDefaultTextColor(Qt::white);
    textoVidas->setPos(35, 75);
    textoVidas->setZValue(10);
    textoVidas->setTextWidth(220);

    actualizarHUD();

    /*
    // Activa esto solo si quieres ver las zonas de puntaje.
    scene->addRect(zona10, QPen(Qt::yellow));
    scene->addRect(zona20, QPen(Qt::green));
    scene->addRect(zona30, QPen(Qt::red));
    */
}

void MainWindow::programarSiguienteLanzamiento()
{
    /*
        Programa cuándo Freezer debe lanzar.

        Si el nivel ya terminó, no programa nada.
    */
    if (!nivel1Activo) {
        return;
    }

    if (esperandoLanzamientos) {
        return;
    }

    esperandoLanzamientos = true;

    /*
        Si hay racha de 50 puntos o más, Freezer lanza dos bolas
        con 2 segundos de diferencia.
    */
    if (rachaPuntos >= 50) {
        bolasPendientesPorLanzar = 2;

        QTimer::singleShot(2000,
                           this,
                           &MainWindow::lanzarBolaFreezer);

        QTimer::singleShot(4000,
                           this,
                           &MainWindow::lanzarBolaFreezer);
    }
    else {
        bolasPendientesPorLanzar = 1;

        QTimer::singleShot(2000,
                           this,
                           &MainWindow::lanzarBolaFreezer);
    }
}

void MainWindow::lanzarBolaFreezer()
{
    /*
        Crea una bola nueva desde la posición visual de Freezer.
    */
    if (!nivel1Activo) {
        return;
    }

    if (scene == nullptr || freezer == nullptr) {
        return;
    }

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
        return;
    }

    freezer->getSprite()->iniciarAnimacion();

    /*
        La bola sale del centro del sprite de Freezer
        y un poco más abajo para que parezca que sale desde su cuerpo.
    */
    QPointF posicionInicial = freezer->getSprite()->sceneBoundingRect().center();
    posicionInicial.setY(posicionInicial.y() + 45);

    Proyectil *nuevaBola = new Proyectil(scene,
                                         ":/images/sprites/bolaFreezer.png",
                                         posicionInicial);

    bolasFreezer.append(nuevaBola);
    bolaFreezer = nuevaBola;

    bolasPendientesPorLanzar--;

    if (bolasPendientesPorLanzar <= 0) {
        esperandoLanzamientos = false;
    }
}

void MainWindow::actualizarNivel1()
{
    /*
        Esta función se ejecuta cada 16 ms.

        Aquí se actualizan las bolas y se revisa:
        - si golpean a Vegito,
        - si fueron bateadas,
        - si cayeron en zona de puntos,
        - si el jugador ganó o perdió.
    */
    if (!nivel1Activo) {
        return;
    }

    const float dt = 0.016;

    for (int i = bolasFreezer.size() - 1; i >= 0; i--) {

        Proyectil *bola = bolasFreezer.at(i);

        if (bola->getEstado() == Proyectil::LanzadaPorFreezer) {

            bola->moverLanzamiento(velocidadFreezer,
                                   lanzamientoOscilatorio);

            /*
                Si la bola pasa más abajo de Vegito sin ser bateada,
                se considera que golpeó al jugador y pierde vida.
            */
            if (bola->pasoLinea(vegito->getSprite()->y() + 70)) {

                vidas -= 1.0;
                rachaPuntos = 0;

                eliminarBola(bola);
                actualizarHUD();

                if (vidas <= 0) {
                    perderNivel1();
                    return;
                }
            }
        }

        else if (bola->getEstado() == Proyectil::BateadaPorVegito) {

            bola->actualizarBateo(dt);

            /*
                Cuando la bola termina el vuelo, se revisa dónde cayó.
            */
            if (bola->terminoVuelo()) {

                revisarCaidaBola(bola);
                eliminarBola(bola);
                actualizarHUD();

                if (puntaje >= 1000) {
                    ganarNivel1();
                    return;
                }

                if (vidas <= 0) {
                    perderNivel1();
                    return;
                }
            }
        }
    }

    /*
        Si no hay bolas en pantalla y no hay lanzamiento pendiente,
        se programa el siguiente lanzamiento.
    */
    if (bolasFreezer.isEmpty() && !esperandoLanzamientos) {
        programarSiguienteLanzamiento();
    }
}

void MainWindow::revisarCaidaBola(Proyectil *bola)
{
    /*
        Revisa la posición final de la bola después del batazo.
    */
    QPointF posicionCaida = bola->centro();

    int puntosGanados = 0;

    if (zona30.contains(posicionCaida)) {
        puntosGanados = 30;
    }
    else if (zona20.contains(posicionCaida)) {
        puntosGanados = 20;
    }
    else if (zona10.contains(posicionCaida)) {
        puntosGanados = 10;
    }

    /*
        Si cae en una zona, suma puntos.
        Si cae afuera, pierde media vida.
    */
    if (puntosGanados > 0) {
        puntaje += puntosGanados;
        rachaPuntos += puntosGanados;
    }
    else {
        vidas -= 0.5;
        rachaPuntos = 0;
    }

    actualizarDificultad();
}

void MainWindow::actualizarDificultad()
{
    /*
        Cambia dificultad por rangos de puntaje:

        0-250:
        - lanzamiento parabólico
        - velocidad baja

        250-500:
        - lanzamiento oscilatorio
        - velocidad un poco mayor

        500-750:
        - vuelve a parabólico
        - velocidad mayor

        750-1000:
        - oscilatorio
        - velocidad final
    */
    if (puntaje < 250) {
        velocidadFreezer = 3.6;
        lanzamientoOscilatorio = false;
    }
    else if (puntaje < 500) {
        velocidadFreezer = 3.9;
        lanzamientoOscilatorio = true;
    }
    else if (puntaje < 750) {
        velocidadFreezer = 4.2;
        lanzamientoOscilatorio = false;
    }
    else {
        velocidadFreezer = 4.5;
        lanzamientoOscilatorio = true;
    }
}

void MainWindow::eliminarBola(Proyectil *bola)
{
    /*
        Elimina una bola de forma segura.
    */
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

    delete bola;
}

void MainWindow::actualizarHUD()
{
    /*
        Muestra solo los valores necesarios.

        Se quitó el "/1000" porque no hacía falta
        y ocupaba mucho espacio visual.
    */
    if (textoPuntaje != nullptr) {
        textoPuntaje->setPlainText("Puntaje: " + QString::number(puntaje));
    }

    if (textoVidas != nullptr) {
        textoVidas->setPlainText("Vidas: " + QString::number(vidas, 'f', 1));
    }
}

void MainWindow::ganarNivel1()
{
    /*
        Detiene el nivel al ganar.
    */
    nivel1Activo = false;
    esperandoLanzamientos = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    QGraphicsTextItem *mensaje = scene->addText("GANASTE EL NIVEL 1");
    mensaje->setDefaultTextColor(Qt::yellow);
    mensaje->setScale(3);
    mensaje->setPos(420, 330);
    mensaje->setZValue(20);
}

void MainWindow::perderNivel1()
{
    /*
        Detiene el nivel al perder.
    */
    nivel1Activo = false;
    esperandoLanzamientos = false;
    bolasPendientesPorLanzar = 0;

    if (timerNivel1 != nullptr) {
        timerNivel1->stop();
    }

    QGraphicsTextItem *mensaje = scene->addText("PERDISTE");
    mensaje->setDefaultTextColor(Qt::red);
    mensaje->setScale(3);
    mensaje->setPos(540, 330);
    mensaje->setZValue(20);
}

void MainWindow::moveFig()
{
    /*
        Función antigua conservada.

        Ahora simplemente lanza una bola de Freezer.
    */
    lanzarBolaFreezer();
}

bool MainWindow::bolaEnZonaBateo(Proyectil *bola)
{
    /*
        Revisa si la bola está tocando a Vegito.

        Esto hace que no se pueda batear demasiado antes.
        El golpe solo se acepta si realmente hay colisión.
    */
    if (bola == nullptr || vegito == nullptr) {
        return false;
    }

    return vegito->getSprite()->collidesWithItem(bola->getItem(),
                                                 Qt::IntersectsItemBoundingRect);
}

QPointF MainWindow::destinoAleatorioBateo()
{
    /*
        Genera un destino aleatorio para la bola.

        La idea es que si el jugador no usa A/D/W/S,
        no siempre caiga en zona de puntos.
    */
    int tipoDestino = QRandomGenerator::global()->bounded(0, 5);

    qreal destinoX = 650;
    qreal destinoY = 300;

    switch (tipoDestino) {

    case 0:
        // Sale por fuera a la izquierda.
        destinoX = zona10.left() - 120;
        destinoY = QRandomGenerator::global()->bounded(180, 430);
        break;

    case 1:
        // Sale por fuera a la derecha.
        destinoX = zona10.right() + 120;
        destinoY = QRandomGenerator::global()->bounded(180, 430);
        break;

    case 2:
        // No alcanza a llegar a la zona de puntos.
        // El usuario debe usar W para subirla.
        destinoX = QRandomGenerator::global()->bounded(int(zona10.left()),
                                                       int(zona10.right()));
        destinoY = zona10.bottom() + 130;
        break;

    case 3:
        // Se pasa demasiado hacia arriba.
        // El usuario debe usar S para bajarla.
        destinoX = QRandomGenerator::global()->bounded(int(zona10.left()),
                                                       int(zona10.right()));
        destinoY = zona30.top() - 90;
        break;

    case 4:
        // Algunas veces sí cae dentro sin corregir.
        destinoX = QRandomGenerator::global()->bounded(int(zona10.left() + 40),
                                                       int(zona10.right() - 40));
        destinoY = QRandomGenerator::global()->bounded(int(zona30.top()),
                                                       int(zona10.bottom()));
        break;
    }

    return QPointF(destinoX, destinoY);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    /*
        Tecla P:
        intenta batear.

        Se limita a una vez por segundo para que el usuario
        no pueda quedarse presionando P y ganar fácilmente.
    */
    if (e->key() == Qt::Key_P &&
        vegito != nullptr &&
        puedeBatear &&
        nivel1Activo) {

        puedeBatear = false;

        QTimer::singleShot(1000, this, [this]() {
            puedeBatear = true;
        });

        vegito->getSprite()->iniciarAnimacion();

        bool golpeo = false;

        for (int i = 0; i < bolasFreezer.size(); i++) {

            Proyectil *bola = bolasFreezer.at(i);

            if (bola->getEstado() == Proyectil::LanzadaPorFreezer &&
                bolaEnZonaBateo(bola)) {

                qDebug() << "Vegito golpeo la bola";

                QPointF destino = destinoAleatorioBateo();

                bola->iniciarBateo(destino.x(), destino.y());

                bolaControlada = bola;
                golpeo = true;

                break;
            }
        }

        if (!golpeo) {
            qDebug() << "Vegito bateo, pero no golpeo la bola";
        }
    }

    /*
        Después de batear, A/D/W/S ajustan el destino de caída.
    */
    if (bolaControlada != nullptr &&
        bolaControlada->getEstado() == Proyectil::BateadaPorVegito &&
        nivel1Activo) {

        if (e->key() == Qt::Key_A) {
            bolaControlada->ajustarDestino(-45, 0);
        }

        if (e->key() == Qt::Key_D) {
            bolaControlada->ajustarDestino(45, 0);
        }

        if (e->key() == Qt::Key_W) {
            bolaControlada->ajustarDestino(0, -45);
        }

        if (e->key() == Qt::Key_S) {
            bolaControlada->ajustarDestino(0, 45);
        }
    }
}

void MainWindow::ajustarFondo()
{
    /*
        Reescala el fondo cuando cambia la ventana.
    */
    if (!rutaFondoActual.isEmpty()) {
        ponerFondo(rutaFondoActual);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    /*
        Ajusta el graphicsView al tamaño de la ventana.
    */
    ui->graphicsView->setGeometry(0, 0, width(), height());

    if (scene != nullptr) {
        scene->setSceneRect(0,
                            0,
                            ui->graphicsView->width(),
                            ui->graphicsView->height());
    }

    /*
        Mantiene centrado el layout del menú.
    */
    ui->gridLayoutWidget->setGeometry(width() / 8,
                                      height() / 10,
                                      width() * 3 / 4,
                                      height() * 2 / 3);

    ui->labelTitulo->setMinimumHeight(height() / 6);
    ui->botonInicio->setMinimumSize(width() / 6,
                                    height() / 14);

    ajustarFondo();
}
