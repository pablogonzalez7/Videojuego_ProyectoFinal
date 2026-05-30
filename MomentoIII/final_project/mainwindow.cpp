#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , fondo(nullptr)
{
    ui->setupUi(this);

    connect(ui->botonInicio, &QPushButton::clicked, this, &MainWindow::on_botonInicio_clicked);

    // timer para manejo de eventos
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizacion()));

    QRect Desktop = QGuiApplication::primaryScreen()->geometry();
    x = Desktop.x();
    y = Desktop.y();
    ancho = Desktop.width()-100;
    alto = Desktop.height()-100;

    scene = new QGraphicsScene();

    ui->graphicsView->setScene(scene);

    //QPen pen(Qt::yellow, 3, Qt::DashDotLine, Qt::RoundCap, Qt:: RoundJoin); //Para personalizar los items creados
    //QBrush brocha(Qt::red); //Para colorear los ítems

   // QImage imgB("/home/tavo/Escritorio/UNIVERSIDAD GUSAVAMER/U ANTIOQUIA/2026-1/Materias/Informática 2/Proyecto Final/Videojuego_ProyectoFinal/MomentoIII/final_project/images/backgrounds/background_main.jpg");
    //QBrush brochaB(imgB);

    //text = scene->addText("Bienvenidos al juego");
    //l1 = scene->addLine(80,-80,10,100,pen);
    //ui->graphicsView->setBackgroundBrush(brochaB);
    //setEscena(0); // se inicializa la primera escena
    ponerFondo("/home/tavo/Escritorio/UNIVERSIDAD GUSAVAMER/U ANTIOQUIA/2026-1/Materias/Informática 2/Proyecto Final/Videojuego_ProyectoFinal/MomentoIII/final_project/images/backgrounds/background_main.png");
    scene->setSceneRect(0, 0, ui->graphicsView->width(),ui->graphicsView->height());//Rectángulo de la escnea, encuadre de cámara
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
    //ui->botonInicio->setText("");

    //timer = new QTimer(this);
    //timer->start(50);

    //connect(timer, SIGNAL(timeout(),this,SLOT(moveFig()); Señal de movimiento autonomo

    //Creacion de obstáculos
    /*QBrush brocha(Qt::black);
    for(int i; i<4; i++){
        obst.append(scene->addRect(600+(i*150),360,40,40,QPen(Qt::black),brocha));
    }*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::setEscena(short int NewNumEscena)
{
    ui->botonInicio->show(); // se muestra el boton

    //int numEscena = NewNumEscena;

    if(NewNumEscena != 0)
        delete scene; // se elimina escena anterior

    scene = new QGraphicsScene(this); // se crea la nuweva escena

    scene->setSceneRect(0, 0, 1280, 720);
    ui->graphicsView->setScene(scene);
    //ui->graphicsView->setFixedSize(1280 + 2 * ui->graphicsView->frameWidth(), 720 + 2 * ui->graphicsView->frameWidth());//manejar la relación de aspecto

    switch(NewNumEscena)
    {
    case 0:
    { // escena de inicio
        ponerFondo(":/images/backgrounds/background_main.jpg");
        break;
    }
    }
}*/
sprite *MainWindow::mostrarSprite(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY)
{
    sprite *nuevoSprite = new sprite(rutaImagen, cantidadFrames);

    scene->addItem(nuevoSprite);
    nuevoSprite->setPos(posX, posY);

    return nuevoSprite;
}

void MainWindow::ponerFondo(QString ruta){
    QPixmap imagenFondo(ruta);
    rutaFondoActual = ruta;

    if(fondo != nullptr){
        scene->removeItem(fondo);
        delete fondo;
        fondo = nullptr;
    }

    imagenFondo = imagenFondo.scaled(ui->graphicsView->width(), ui->graphicsView->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    fondo = scene->addPixmap(imagenFondo);

    fondo->setPos(0,0);
    fondo->setZValue(-10);

    fondo->setOpacity(0.8);
}

void MainWindow::mostrarMenuInicio(){
    scene->clear();
    fondo = nullptr;

    ponerFondo(":/images/backgrounds/background_main.jpg");
}

void MainWindow::on_botonInicio_clicked()
{

}

void MainWindow::moveFig()
{
    if(figEn->pos().x()+3 < ui->graphicsView->width())
    figEn->setPos(figEn->pos().x()+3, figEn->pos().y());

    else
        figEn->setPos(640,435);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_A){
        fig->setPos(fig->pos().x()-10,fig->pos().y()-1);
    }
    if(e->key()==Qt::Key_S){
        fig->setPos(fig->pos().x()+10,fig->pos().y()+1);
    }
}

void MainWindow::ajustarFondo()
{
    if(!rutaFondoActual.isEmpty()){
        ponerFondo(rutaFondoActual);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    ui->graphicsView->setGeometry(0, 0, width(), height());
    if(scene != nullptr){
        scene->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    }
    ui->gridLayoutWidget->setGeometry(width() / 8, height() / 10, width() * 3 / 4, height() * 2 / 3);

    ui->labelTitulo->setMinimumHeight(height() / 6);
    ui->botonInicio->setMinimumSize(width() / 6, height() / 14);

    ajustarFondo();
}
