#include "sprite.h"
Sprite::Sprite(const QString &rutaImagen, int cantidadFrames, QObject *parent) : QObject{parent}
{
    // El timer permite animar el sprite sin bloquear el hilo de interfaz.
    timer = new QTimer(this);

    frameActual = 0;
    totalFrames = cantidadFrames;

    // La imagen recibida contiene todos los cuadros uno junto al otro.
    pixmap = new QPixmap(rutaImagen);

    if(totalFrames <= 0){
        // Se protege el cálculo del ancho ante un valor inválido de frames.
        totalFrames = 1;
    }

    // Cada frame ocupa una fracción horizontal idéntica de la imagen.
    ancho = pixmap->width() / totalFrames;
    alto = pixmap->height();

    // Cada tick del timer invoca el slot que avanza la animación.
    connect(timer, &QTimer::timeout, this, &Sprite::actualizacion);
}

void Sprite::iniciarAnimacion()
{
    // Siempre se reinicia para que la acción empiece visualmente desde el frame 0.
    frameActual = 0;
    update();

    // 100 ms por frame da una animación visible y corta para acciones del juego.
    timer->start(100);
}

void Sprite::actualizacion()
{
    // Se avanza al siguiente frame de la hoja.
    frameActual++;

    if(frameActual >= totalFrames){
        // Al terminar la secuencia se regresa al frame inicial y se detiene el
        // timer para que la animación no quede en bucle infinito.
        frameActual = 0;
        timer->stop();
    }

    // Se actualiza solo el rectángulo que ocupa el sprite dentro de la escena.
    this->update(-ancho/2, -alto/2, ancho, alto);
}

QRectF Sprite::boundingRect() const
{
    // El rectángulo se centra en el origen local para posicionar el personaje
    // desde su centro, lo que simplifica colisiones y alineación visual.
    return QRectF(-ancho/2, -alto/2, ancho, alto);
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // origenX indica la coordenada horizontal del frame actual dentro de la hoja.
    const int origenX = frameActual * static_cast<int>(ancho);

    // Se dibuja el recorte del frame actual en la posición local del item.
    painter->drawPixmap(QPointF(-ancho / 2, -alto / 2), *pixmap, QRectF(origenX, 0, ancho, alto));
}
