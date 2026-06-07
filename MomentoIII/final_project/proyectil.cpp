#include "proyectil.h"

#include <QtMath>
#include <QHash>
#include <QRandomGenerator>
#include <stdexcept>

namespace {
int tamañoBaseProyectil(const QString &ruta)
{
    return ruta.contains("rafaga_black_dificil") ? 90 : 35;
}

QPixmap obtenerPixmapProyectil(const QString &ruta)
{
    static QHash<QString, QPixmap> cacheProyectiles;

    if (cacheProyectiles.contains(ruta)) {
        return cacheProyectiles.value(ruta);
    }

    QPixmap pixmapOriginal(ruta);

    if (pixmapOriginal.isNull()) {
        throw std::runtime_error(("No se pudo cargar el sprite del proyectil: " + ruta).toStdString());
    }

    QPixmap pixmapEscalado = pixmapOriginal.scaled(tamañoBaseProyectil(ruta), tamañoBaseProyectil(ruta), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (pixmapEscalado.isNull()) {
        throw std::runtime_error(("El sprite del proyectil no pudo escalarse correctamente: " + ruta).toStdString());
    }

    cacheProyectiles.insert(ruta, pixmapEscalado);
    return pixmapEscalado;
}
}

Proyectil::Proyectil(QGraphicsScene *scene, const QString &rutaImagen, QPointF posicionInicial)
{
    if (scene == nullptr) {
        throw std::runtime_error("No se puede crear un proyectil sin una escena valida.");
    }

    rutaSprite = rutaImagen;
    proyectil = nullptr;

    const QPixmap pixmap = obtenerPixmapProyectil(rutaSprite);

    proyectil = scene->addPixmap(pixmap);


    proyectil->setOffset(-pixmap.width() / 2.0,
                         -pixmap.height() / 2.0);

    proyectil->setPos(posicionInicial);
    proyectil->setZValue(5);
    proyectil->setVisible(true);

    proyectil->setTransformOriginPoint(proyectil->boundingRect().center());

    activa = true;

    estado = LanzadaPorFreezer;

    tiempo = 0;
    tiempoVuelo = 0;


    duracionVuelo = 3.0;
    duracionVueloBase = duracionVuelo;

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

    ataqueActual.velocidad = 3.6f;
    ataqueActual.daño = 1.0f;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;

}

Proyectil::~Proyectil()
{

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

    if (estado != LanzadaPorFreezer) {
        return;
    }

    tiempo += 0.016;

    qreal nuevaX = proyectil->pos().x();
    qreal nuevaY = proyectil->pos().y() + velocidad;

    if (oscilatorio) {

        nuevaX = xBaseOscilacion + 55 * qSin(tiempo * 12);
    }
    else {

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
    const bool esProyectilFreezer = rutaSprite.contains("bolaFreezer");
    const qreal factorDuracion = esRafagaBlack
                                     ? 0.94 + QRandomGenerator::global()->bounded(12) / 100.0
                                     : (esProyectilFreezer
                                            ? 0.72 + QRandomGenerator::global()->bounded(16) / 100.0
                                            : 0.82 + QRandomGenerator::global()->bounded(37) / 100.0);

    duracionVuelo = duracionVueloBase * factorDuracion;
    amplitudArcoVuelo = esRafagaBlack
                            ? 0.0
                            : (esProyectilFreezer
                                   ? QRandomGenerator::global()->bounded(55, 96)
                                   : QRandomGenerator::global()->bounded(65, 121));
    desvioHorizontalVuelo = esRafagaBlack ? 0.0 : QRandomGenerator::global()->bounded(-42, 43);
}

void Proyectil::reiniciar(QPointF posicionInicial, const QString &rutaImagen)
{
    if (proyectil == nullptr) {
        throw std::runtime_error("No se puede reiniciar un proyectil sin item grafico.");
    }

    const bool cambioSprite = rutaSprite != rutaImagen || proyectil->pixmap().isNull();
    rutaSprite = rutaImagen;

    if (cambioSprite) {
        const QPixmap pixmap = obtenerPixmapProyectil(rutaSprite);
        proyectil->setPixmap(pixmap);
        proyectil->setOffset(-pixmap.width() / 2.0,
                             -pixmap.height() / 2.0);
    }

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

void Proyectil::configurarAtaque(float velocidad, float daño)
{
    ataqueActual.velocidad = velocidad;
    ataqueActual.daño = daño;
    valorPuntaje = 1;
    bonificacionAroActiva = false;
    multiplicadorEscala = 1.0;


    if (daño >= 3.0f || rutaSprite.contains("rafaga_black_dificil")) {
        duracionVueloBase = qMax(0.82f, 1.30f - velocidad * 0.05f);
    }
    else if (rutaSprite.contains("bolaFreezer")) {
        duracionVueloBase = qBound(1.05f, 1.78f - velocidad * 0.11f, 1.34f);
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
    const bool esProyectilFreezer = rutaSprite.contains("bolaFreezer");

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


    qreal arcoParabolico = amplitudArcoVuelo * senoVuelo;

    nuevaY -= arcoParabolico;

    if (esProyectilFreezer) {
        const qreal progresoCaida = qMax<qreal>(0.0, (t - 0.52) / 0.48);
        nuevaY += 82.0 * progresoCaida * progresoCaida;
    }

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

    if (estado != BateadaPorVegito) {
        return;
    }

    destinoX += dx;
    destinoY += dy;
}

bool Proyectil::terminoVuelo() const
{
    return estado == Caida;
}

bool Proyectil::pasoLinea(qreal limiteY) const
{

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


    return proyectil->sceneBoundingRect().center();
}

float Proyectil::getVelocidadAtaque() const
{
    return ataqueActual.velocidad;
}

float Proyectil::getDañoAtaque() const
{
    return ataqueActual.daño;
}

QString Proyectil::getRutaSprite() const
{
    return rutaSprite;
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
