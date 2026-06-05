#include "proyectil.h"

#include <QtMath>
#include <QRandomGenerator>

Proyectil::Proyectil(QGraphicsScene *scene,const QString &rutaImagen,QPointF posicionInicial)
{
    rutaSprite = rutaImagen;

    // MainWindow suministra la imagen del proyectil y el punto desde el cual
    // debe aparecer visualmente en la escena.
    QPixmap pixmap(rutaSprite);

    /*
        Se escala la bola a un tamaño base de 35x35.

        Este será el tamaño normal de referencia.
        Durante el vuelo se puede agrandar, pero al caer volverá casi a este tamaño.
    */
    pixmap = pixmap.scaled(35, 35,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);

    proyectil = scene->addPixmap(pixmap);

    /*
        Importante:

        Con setOffset negativo, la posición de la bola representa su centro,
        no la esquina superior izquierda.

        Esto hace que las colisiones y la revisión de zonas sean más coherentes.
    */
    proyectil->setOffset(-pixmap.width() / 2.0,-pixmap.height() / 2.0);

    proyectil->setPos(posicionInicial);
    proyectil->setZValue(5);
    proyectil->setVisible(true);

    // La escala debe crecer desde el centro para simular altura sin desplazar
    // la bola hacia un lado cuando cambia de tamaño.
    proyectil->setTransformOriginPoint(proyectil->boundingRect().center());

    vx = 0;
    vy = 5;
    activa = true;

    estado = LanzadaPorFreezer;

    tiempo = 0;
    tiempoVuelo = 0;

    /*
        La bola tarda 3 segundos desde que Vegito la batea
        hasta que cae en el campo.
    */
    duracionVuelo = 1.0;

    inicioX = posicionInicial.x();
    inicioY = posicionInicial.y();

    destinoX = inicioX;
    destinoY = inicioY;

    xBaseOscilacion = posicionInicial.x();

    /*
        Para el movimiento parabólico antes del bateo se escoge
        una dirección aleatoria: izquierda o derecha.
    */
    int lado = QRandomGenerator::global()->bounded(0, 2);

    if (lado == 0) {
        direccionCurva = -1.0;
    }
    else {
        direccionCurva = 1.0;
    }

    /*
        Fuerza aleatoria de la curva.

        Esto evita que todos los lanzamientos parabólicos sean idénticos.
    */
    fuerzaCurva = QRandomGenerator::global()->bounded(15, 36);

    ataqueActual.velocidad = 3.6f;
    ataqueActual.dano = 1.0f;

    timerP = new QTimer(this);

    // Este timer quedó preparado para mover el proyectil de forma autónoma si
    // en el futuro se desacopla más lógica de MainWindow.
    connect(timerP, &QTimer::timeout, this, &Proyectil::movProyectil);
}

Proyectil::~Proyectil()
{
    /*
        Si la bola existe, primero se quita de la escena
        y luego se libera la memoria.
    */
    if (proyectil != nullptr) {
        if (proyectil->scene() != nullptr) {
            proyectil->scene()->removeItem(proyectil);
        }

        delete proyectil;
        proyectil = nullptr;
    }
}

void Proyectil::iniciarTimerProyectil()
{
    // Frecuencia cercana a 60 FPS para movimiento fluido.
    timerP->start(16);
}

void Proyectil::detenerTimerProyectil()
{
    // Se puede pausar el movimiento interno si el control se hace externamente.
    timerP->stop();
}

void Proyectil::movProyectil()
{
    /*
        Movimiento por defecto.

        En la lógica actual del juego, MainWindow llama directamente
        a moverLanzamiento(), pero se conserva este método.
    */
    moverLanzamiento(4, false);
}

void Proyectil::moverLanzamiento(float velocidad, bool oscilatorio)
{
    /*
        Este movimiento solo se aplica mientras la bola viene desde Freezer.
    */
    if (estado != LanzadaPorFreezer) {
        return;
    }

    tiempo += 0.016;

    // La componente vertical siempre baja la bola hacia Vegito.
    qreal nuevaX = proyectil->pos().x();
    qreal nuevaY = proyectil->pos().y() + velocidad;

    if (oscilatorio) {
        /*
            Movimiento oscilatorio.

            La bola baja mientras se mueve varias veces de lado a lado.
            Se usa en ciertos rangos de puntaje para aumentar dificultad.
        */
        nuevaX = xBaseOscilacion + 55 * qSin(tiempo * 12);
    }
    else {
        /*
            Movimiento parabólico visto desde arriba.

            No representa altura real. Lo que hace es curvar la bola
            suavemente en X mientras baja hacia Vegito.
        */
        qreal tNormalizado = tiempo / 2.0;

        if (tNormalizado > 1.0) {
            tNormalizado = 1.0;
        }

        qreal curva = 4 * tNormalizado * (1 - tNormalizado);

        nuevaX = xBaseOscilacion + direccionCurva * fuerzaCurva * curva;
    }

    proyectil->setPos(nuevaX, nuevaY);
}

void Proyectil::iniciarBateo(qreal nuevoDestinoX, qreal nuevoDestinoY)
{
    /*
        Cuando Vegito golpea la bola, se cambia el estado del proyectil.

        Se guarda:
        - dónde empezó el batazo,
        - hacia dónde debe caer.
    */
    estado = BateadaPorVegito;

    tiempoVuelo = 0;

    inicioX = proyectil->pos().x();
    inicioY = proyectil->pos().y();

    destinoX = nuevoDestinoX;
    destinoY = nuevoDestinoY;
}

void Proyectil::reiniciar(QPointF posicionInicial, const QString &rutaImagen)
{
    rutaSprite = rutaImagen;

    QPixmap pixmap(rutaSprite);
    pixmap = pixmap.scaled(35, 35,Qt::KeepAspectRatio,Qt::SmoothTransformation);

    proyectil->setPixmap(pixmap);
    proyectil->setOffset(-pixmap.width() / 2.0,-pixmap.height() / 2.0);
    proyectil->setTransformOriginPoint(proyectil->boundingRect().center());
    proyectil->setPos(posicionInicial);
    proyectil->setScale(1.0);
    proyectil->setVisible(true);

    activa = true;
    estado = LanzadaPorFreezer;

    tiempo = 0;
    tiempoVuelo = 0;
    inicioX = posicionInicial.x();
    inicioY = posicionInicial.y();
    destinoX = inicioX;
    destinoY = inicioY;
    xBaseOscilacion = posicionInicial.x();

    int lado = QRandomGenerator::global()->bounded(0, 2);

    if (lado == 0) {
        direccionCurva = -1.0;
    }
    else {
        direccionCurva = 1.0;
    }

    fuerzaCurva = QRandomGenerator::global()->bounded(15, 36);
}

void Proyectil::desactivar()
{
    activa = false;
    estado = Caida;
    tiempo = 0;
    tiempoVuelo = 0;
    proyectil->setScale(1.0);
    proyectil->setVisible(false);
}

void Proyectil::configurarAtaque(float velocidad, float dano)
{
    ataqueActual.velocidad = velocidad;
    ataqueActual.dano = dano;
}

void Proyectil::actualizarBateo(float dt)
{
    if (estado != BateadaPorVegito) {
        return;
    }

    tiempoVuelo += dt;

    // Normaliza el avance del vuelo entre 0 y 1 para interpolar trayectorias.
    float t = tiempoVuelo / duracionVuelo;

    if (t > 1.0) {
        t = 1.0;
    }

    /*
        Interpolación lineal entre la posición inicial y el destino.
    */
    qreal nuevaX = inicioX + (destinoX - inicioX) * t;
    qreal nuevaY = inicioY + (destinoY - inicioY) * t;

    qreal escala = 1.0 + 0.75 * qSin(3.1416 * t) - 0.10 * t;

    /*
        Protección para que nunca se vuelva diminuta.
    */
    if (escala < 0.90) {
        escala = 0.90;
    }

    proyectil->setPos(nuevaX, nuevaY);
    proyectil->setScale(escala);

    if (tiempoVuelo >= duracionVuelo) {
        estado = Caida;
    }
}

void Proyectil::ajustarDestino(qreal dx, qreal dy)
{
    /*
        Solo se puede ajustar el destino mientras la bola ya fue bateada.
    */
    if (estado != BateadaPorVegito) {
        return;
    }

    // No mueve directamente la bola; cambia el punto final hacia el que se
    // seguirá interpolando en los próximos frames.
    destinoX += dx;
    destinoY += dy;
}

bool Proyectil::terminoVuelo() const
{
    return estado == Caida;
}

bool Proyectil::pasoLinea(qreal limiteY) const
{
    /*
        Se usa para saber si la bola pasó de largo y golpeó a Vegito.
    */
    return centro().y() > limiteY;
}

Proyectil::EstadoProyectil Proyectil::getEstado() const
{
    return estado;
}

bool Proyectil::estaActiva() const
{
    return activa;
}

QGraphicsPixmapItem* Proyectil::getItem() const
{
    return proyectil;
}

QPointF Proyectil::centro() const
{
    /*
        Retorna el centro visual real de la bola.
    */

    // Se usa sceneBoundingRect() porque incorpora escala y transformaciones,
    // así que el centro reportado coincide mejor con lo que ve el jugador.
    return proyectil->sceneBoundingRect().center();
}

float Proyectil::getVelocidadAtaque() const
{
    return ataqueActual.velocidad;
}

float Proyectil::getDanoAtaque() const
{
    return ataqueActual.dano;
}
