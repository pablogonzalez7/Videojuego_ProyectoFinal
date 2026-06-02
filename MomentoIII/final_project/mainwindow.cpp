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
    , botonFacil(nullptr)
    , botonDificil(nullptr)
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

    // Se conserva una referencia del tamaño de pantalla disponible; esto ayuda
    // a mantener proporciones generales del juego respecto al monitor.
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
    dificultadSeleccionada = Facil;

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

    crearBotonesDificultad();
    ocultarOpcionesDificultad();
}

MainWindow::~MainWindow()
{
    while (!bolasFreezer.isEmpty()) {
        Proyectil *bola = bolasFreezer.takeLast();
        delete bola;
    }

    while (!bolasDisponibles.isEmpty()) {
        Proyectil *bola = bolasDisponibles.takeLast();
        delete bola;
    }

    // ui libera los widgets creados desde el archivo .ui y sus jerarquías hijas.
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
        El botón de inicio ahora muestra las dos dificultades.
    */
    ui->botonInicio->hide();
    mostrarOpcionesDificultad();
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

void MainWindow::iniciarNivel1()
{
    ui->labelTitulo->hide();
    ocultarOpcionesDificultad();

    ponerFondo(":/images/backgrounds/nivel1.png", 1.0);

    if (vegito == nullptr) {
        vegito = new Personaje(":/images/sprites/vegitoBateo.png", 5, 650, 540);
    }

    vegito->agregarAEscena(scene);
    vegito->getSprite()->setPos(650, 540);

    if (freezer == nullptr) {
        freezer = new Personaje(":/images/sprites/freezerPitcher.png", 6, 650, 270);
    }

    freezer->agregarAEscena(scene);
    freezer->getSprite()->setPos(650, 270);

    iniciarVariablesNivel1();

    if (timerNivel1 == nullptr) {
        timerNivel1 = new QTimer(this);
        connect(timerNivel1,
                &QTimer::timeout,
                this,
                &MainWindow::actualizarNivel1);
    }

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
    configurarAtaquesFreezer();

    bolasPendientesPorLanzar = 0;
    esperandoLanzamientos = false;

    nivel1Activo = true;
    puedeBatear = true;

    bolaFreezer = nullptr;
    bolaControlada = nullptr;

    bolasFreezer.clear();

    for (int i = 0; i < bolasDisponibles.size(); i++) {
        bolasDisponibles.at(i)->desactivar();
    }

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

void MainWindow::crearBotonesDificultad()
{
    botonFacil = new QPushButton("Facil", ui->centralwidget);
    botonDificil = new QPushButton("Dificil", ui->centralwidget);

    botonFacil->setGeometry(250, 290, 120, 45);
    botonDificil->setGeometry(420, 290, 120, 45);

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

    connect(botonFacil,
            &QPushButton::clicked,
            this,
            &MainWindow::iniciarModoFacil);

    connect(botonDificil,
            &QPushButton::clicked,
            this,
            &MainWindow::iniciarModoDificil);
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

void MainWindow::configurarAtaquesFreezer()
{
    ataquesFreezer.clear();

    ataquesFreezer.append({3.6f, 1.0f, ":/images/sprites/bolaFreezer.png"});

    if (dificultadSeleccionada == Dificil) {
        ataquesFreezer.clear();
        ataquesFreezer.append({3.6f, 1.0f, ":/images/sprites/bolaFreezer.png"});
        ataquesFreezer.append({4.3f, 1.5f, ":/images/sprites/bolaFreezer.png"});
        ataquesFreezer.append({5.0f, 2.0f, ":/images/sprites/bolaFreezer.png"});
    }
}

MainWindow::AtaqueFreezer MainWindow::obtenerAtaqueActual() const
{
    if (ataquesFreezer.isEmpty()) {
        return {3.6f, 1.0f, ":/images/sprites/bolaFreezer.png"};
    }

    if (dificultadSeleccionada == Facil) {
        return ataquesFreezer.first();
    }

    int indiceAtaque = QRandomGenerator::global()->bounded(ataquesFreezer.size());
    return ataquesFreezer.at(indiceAtaque);
}

Proyectil *MainWindow::obtenerBolaDisponible(const AtaqueFreezer &ataque,
                                             QPointF posicionInicial)
{
    Proyectil *bola = nullptr;

    if (!bolasDisponibles.isEmpty()) {
        bola = bolasDisponibles.takeLast();
        bola->reiniciar(posicionInicial, ataque.sprite);
    }
    else {
        bola = new Proyectil(scene, ataque.sprite, posicionInicial);
    }

    bola->configurarAtaque(ataque.velocidad, ataque.dano);
    return bola;
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

    // Esta bandera impide programar varias tandas al mismo tiempo si el método
    // vuelve a ser llamado antes de que salgan las bolas pendientes.
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
        Toma una bola del pool o crea una si todavía no existe ninguna.
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

    AtaqueFreezer ataque = obtenerAtaqueActual();
    Proyectil *nuevaBola = obtenerBolaDisponible(ataque, posicionInicial);

    // Se almacena en la lista global para actualizarla en cada tick del nivel.
    bolasFreezer.append(nuevaBola);

    // También se guarda como última bola creada por compatibilidad con código
    // previo que trabajaba con un solo proyectil activo.
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

        // Se recorre hacia atrás porque algunas iteraciones pueden eliminar la
        // bola actual, y así no se desordenan los índices pendientes.
        Proyectil *bola = bolasFreezer.at(i);

        if (bola->getEstado() == Proyectil::LanzadaPorFreezer) {
            float velocidadAtaque = velocidadFreezer;

            if (dificultadSeleccionada == Dificil) {
                velocidadAtaque += bola->getVelocidadAtaque() - 3.6f;
            }

            bola->moverLanzamiento(velocidadAtaque,
                                   lanzamientoOscilatorio);

            /*
                Si la bola pasa más abajo de Vegito sin ser bateada,
                se considera que golpeó al jugador y pierde vida.
            */
            if (bola->pasoLinea(vegito->getSprite()->y() + 70)) {

                vidas -= bola->getDanoAtaque();
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
        // La racha mide desempeño continuo y desbloquea dobles lanzamientos.
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
        Saca la bola de la lista activa y la devuelve al pool.
    */
    if (bola == nullptr) {
        return;
    }

    bolasFreezer.removeOne(bola);

    // Si la bola eliminada era la que estaba siendo corregida con A/D/W/S, se
    // anula la referencia para evitar acceso a memoria liberada.
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

    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    // El texto se coloca con Z alto para que quede por encima del fondo y HUD.
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

    while (!bolasFreezer.isEmpty()) {
        eliminarBola(bolasFreezer.last());
    }

    // Misma idea que en victoria: superponer el mensaje al resto de la escena.
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
            // Rehabilita el bateo tras un enfriamiento de 1 segundo.
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

                // El golpe solo cambia el estado de la bola y define su destino.
                // El movimiento real se seguirá calculando en actualizarNivel1().
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
            // A corre el punto de caída hacia la izquierda.
            bolaControlada->ajustarDestino(-45, 0);
        }

        if (e->key() == Qt::Key_D) {
            // D corre el punto de caída hacia la derecha.
            bolaControlada->ajustarDestino(45, 0);
        }

        if (e->key() == Qt::Key_W) {
            // W hace que la bola termine más arriba en el campo.
            bolaControlada->ajustarDestino(0, -45);
        }

        if (e->key() == Qt::Key_S) {
            // S hace que la bola termine más abajo en el campo.
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
    // Se conserva primero el comportamiento base del widget principal.
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

    if (botonFacil != nullptr) {
        botonFacil->setGeometry(width() / 2 - 150,
                                height() * 2 / 3,
                                120,
                                45);
    }

    if (botonDificil != nullptr) {
        botonDificil->setGeometry(width() / 2 + 30,
                                  height() * 2 / 3,
                                  120,
                                  45);
    }

    ajustarFondo();
}
