#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , fondo(nullptr)
{
    ui->setupUi(this);

    // timer para manejo de eventos
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(actualizacion()));

    QRect Desktop = QGuiApplication::primaryScreen()->geometry();
    x = Desktop.x();
    y = Desktop.y();
    ancho = Desktop.width()-100;
    alto = Desktop.height()-100;

    scene = new QGraphicsScene(x,y,ancho, alto);

    ui->graphicsView->setScene(scene);

    setEscena(0); // se inicializa la primera escena
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEscena(short int NewNumEscena)
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
}
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

    imagenFondo = imagenFondo.scaled(ui->graphicsView->width(), ui->graphicsView->height(),
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

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

    ui->label->setMinimumHeight(height() / 6);
    ui->botonInicio->setMinimumSize(width() / 6, height() / 14);

    ajustarFondo();
}
