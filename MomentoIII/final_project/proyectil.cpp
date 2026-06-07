#include "proyectil.h"

#include <QtMath>
#include <QRandomGenerator>

Proyectil::Proyectil(QGraphicsScene *scene,
                     const QString &rutaImagen,
                     QPointF posicionInicial)
{
    rutaSprite = rutaImagen;

    // MainWindow suministra la imagen del proyectil y el punto desde el cual
    // debe aparecer visualmente en la escena.
    QPixmap pixmap(rutaSprite);
    int tamanoBase = 35;

    if (rutaSprite.contains("rafaga_black_dificil")) {
        tamanoBase = 90;
    }

    /*
        Se escala la bola a un tamaño base de 35x35.

        Este será el tamaño normal de referencia.
        Durante el vuelo se puede agrandar, pero al caer volverá casi a este tamaño.
    */
    pixmap = pixmap.scaled(tamanoBase, tamanoBase,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);

    proyectil = scene->addPixmap(pixmap);

    /*
        Importante:

        Con setOffset negativo, la posición de la bola representa su centro,
        no la esquina superior izquierda.

        Esto hace que las colisiones y la revisión de zonas sean más coherentes.
    */
    proyectil->setOffset(-pixmap.width() / 2.0,
                         -pixmap.height() / 2.0);

    proyectil->setPos(posicionInicial);
    proyectil->setZValue(5);
    proyectil->setVisible(true);

    // La escala debe crecer desde el centro para simular altura sin desplazar
    // la bola hacia un lado cuando cambia de tamaño.
    proyectil->setTransformOriginPoint(proyectil->boundingRect().center());

    activa = true;

    estado = LanzadaPorFreezer;

    tiempo = 0;
    tiempoVuelo = 0;

    /*
        La bola tarda 3 segundos desde que Vegito la batea
        hasta que cae en el campo.
    */
    duracionVuelo = 3.0;
    duracionVueloBase = duracionVuelo;

    inicioX = posicionInicial.x();
    inicioY = posicionInicial.y();

    destinoX = inicioX;
    destinoY = inicioY;
    limiteSueloVuelo = posicionInicial.y();

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
    amplitudArcoVuelo = 90.0;
    desvioHorizontalVuelo = 0.0;

    ataqueActual.velocidad = 3.6f;
    ataqueActual.dano = 1.0f;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;

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
    limiteSueloVuelo = destinoY;

    if (proyectil->scene() != nullptr) {
        limiteSueloVuelo = qMax(destinoY, proyectil->scene()->sceneRect().bottom() - 150.0);
    }

    const bool esRafagaBlack = rutaSprite.contains("rafaga_black_dificil");
    const bool esProyectilBlack = rutaSprite.contains("bolaBlack") || esRafagaBlack;
    const qreal factorDuracion = esRafagaBlack
                                     ? 0.94 + QRandomGenerator::global()->bounded(12) / 100.0
                                     : 0.82 + QRandomGenerator::global()->bounded(37) / 100.0;

    duracionVuelo = duracionVueloBase * factorDuracion;
    amplitudArcoVuelo = esRafagaBlack ? 0.0 : QRandomGenerator::global()->bounded(65, 121);
    desvioHorizontalVuelo = esRafagaBlack ? 0.0 : QRandomGenerator::global()->bounded(-42, 43);
}

void Proyectil::reiniciar(QPointF posicionInicial, const QString &rutaImagen)
{
    rutaSprite = rutaImagen;

    QPixmap pixmap(rutaSprite);
    int tamanoBase = 35;

    if (rutaSprite.contains("rafaga_black_dificil")) {
        tamanoBase = 90;
    }

    pixmap = pixmap.scaled(tamanoBase, tamanoBase,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);

    proyectil->setPixmap(pixmap);
    proyectil->setOffset(-pixmap.width() / 2.0,
                         -pixmap.height() / 2.0);
    proyectil->setTransformOriginPoint(proyectil->boundingRect().center());
    proyectil->setPos(posicionInicial);
    proyectil->setScale(1.0);
    proyectil->setVisible(true);

    activa = true;
    estado = LanzadaPorFreezer;

    tiempo = 0;
    tiempoVuelo = 0;
    duracionVuelo = duracionVueloBase;
    inicioX = posicionInicial.x();
    inicioY = posicionInicial.y();
    destinoX = inicioX;
    destinoY = inicioY;
    limiteSueloVuelo = posicionInicial.y();
    xBaseOscilacion = posicionInicial.x();

    int lado = QRandomGenerator::global()->bounded(0, 2);

    if (lado == 0) {
        direccionCurva = -1.0;
    }
    else {
        direccionCurva = 1.0;
    }

    fuerzaCurva = QRandomGenerator::global()->bounded(15, 36);
    amplitudArcoVuelo = 90.0;
    desvioHorizontalVuelo = 0.0;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;
}

void Proyectil::desactivar()
{
    activa = false;
    estado = Caida;
    tiempo = 0;
    tiempoVuelo = 0;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;
    proyectil->setScale(1.0);
    proyectil->setVisible(false);
}

void Proyectil::configurarAtaque(float velocidad, float dano)
{
    ataqueActual.velocidad = velocidad;
    ataqueActual.dano = dano;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;

    /*
        Los ataques especiales de Black deben sentirse más rápidos
        que una pelota normal. La duración del vuelo se ajusta según el daño
        y el sprite usado.
    */
    if (dano >= 3.0f || rutaSprite.contains("rafaga_black_dificil")) {
        duracionVueloBase = qMax(0.82f, 1.30f - velocidad * 0.05f);
    }
    else {
        duracionVueloBase = qBound(1.75f, 3.15f - velocidad * 0.18f, 2.65f);
    }

    duracionVuelo = duracionVueloBase;
}

void Proyectil::actualizarBateo(float dt)
{
    if (estado != BateadaPorVegito) {
        return;
    }

    const bool esRafagaBlack = rutaSprite.contains("rafaga_black_dificil");
    const bool esProyectilBlack = rutaSprite.contains("bolaBlack") || esRafagaBlack;

    if (esRafagaBlack) {
        const qreal avanceHorizontal = qMax<qreal>(9.0, ataqueActual.velocidad * 3.0);
        const qreal nuevaX = proyectil->pos().x() - avanceHorizontal;
        const qreal nuevaY = inicioY;

        proyectil->setPos(nuevaX, nuevaY);
        proyectil->setScale(multiplicadorEscala);

        if (proyectil->scene() != nullptr &&
            proyectil->sceneBoundingRect().right() <= proyectil->scene()->sceneRect().left()) {
            estado = Caida;
        }

        return;
    }

    tiempoVuelo += dt;

    float t = tiempoVuelo / duracionVuelo;

    if (t > 1.0) {
        t = 1.0;
    }

    const qreal senoVuelo = qSin(3.1416 * t);
    qreal nuevaX = inicioX + (destinoX - inicioX) * t;
    nuevaX += desvioHorizontalVuelo * senoVuelo * (1.0 - 0.30 * t);
    qreal nuevaY = inicioY + (destinoY - inicioY) * t;

    /*
        Las pelotas normales mantienen un arco más visible.
        La ráfaga especial usa un arco menor para sentirse más directa.
    */
    qreal arcoParabolico = amplitudArcoVuelo * senoVuelo;

    nuevaY -= arcoParabolico;

    qreal escala = 1.0;

    if (esRafagaBlack) {
        escala = 1.0;
    }
    else {
        escala = 1.0 + 0.75 * qSin(3.1416 * t) - 0.10 * t;

        if (escala < 0.90) {
            escala = 0.90;
        }
    }

    if (esProyectilBlack && t >= 1.0f) {
        nuevaX = destinoX;
        nuevaY = qMin(limiteSueloVuelo,
                      proyectil->pos().y() + qMax<qreal>(4.8, ataqueActual.velocidad * 1.35));
        escala = 1.0;
    }

    proyectil->setPos(nuevaX, nuevaY);
    proyectil->setScale(escala * multiplicadorEscala);

    if (!esProyectilBlack && t >= 1.0f) {
        estado = Caida;
    }
    else if (esProyectilBlack && t >= 1.0f && centro().y() >= limiteSueloVuelo - 1.0) {
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

int Proyectil::getValorPuntaje() const
{
    return valorPuntaje;
}

bool Proyectil::tieneBonificacionAro() const
{
    return bonificacionAroActiva;
}

void Proyectil::activarBonificacionAro()
{
    bonificacionAroActiva = true;
    valorPuntaje = 2;
    multiplicadorEscala = 2.0;
}
