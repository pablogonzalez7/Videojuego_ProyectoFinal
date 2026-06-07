#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPointF>

class Proyectil : public QObject
{
public:
    struct DatosAtaque {
        float velocidad;
        float daño;
    };


    enum EstadoProyectil {
        LanzadaPorFreezer,
        BateadaPorVegito,
        Caida
    };


    Proyectil(QGraphicsScene *scene, const QString &rutaImagen, QPointF posicionInicial = QPointF(650, 270));


    ~Proyectil();


    void moverLanzamiento(float velocidad, bool oscilatorio);


    void iniciarBateo(qreal destinoX, qreal destinoY);


    void actualizarBateo(float dt);


    void ajustarDestino(qreal dx, qreal dy);


    void reiniciar(QPointF posicionInicial, const QString &rutaImagen);


    void desactivar();


    void configurarAtaque(float velocidad, float daño);


    bool terminoVuelo() const;


    bool pasoLinea(qreal limiteY) const;


    EstadoProyectil getEstado() const;

    bool estaActiva() const;


    QGraphicsPixmapItem* getItem() const;


    QPointF centro() const;

    float getVelocidadAtaque() const;

    float getDañoAtaque() const;
    QString getRutaSprite() const;
    int getValorPuntaje() const;
    bool tieneBonificacionAro() const;
    void activarBonificacionAro();

private:
    QGraphicsPixmapItem *proyectil;

    EstadoProyectil estado;

    float tiempo;

    float tiempoVuelo;

    float duracionVuelo;
    float duracionVueloBase;

    qreal inicioX;
    qreal inicioY;

    qreal destinoX;
    qreal destinoY;
    qreal limiteSueloVuelo;

    qreal xBaseOscilacion;

    qreal direccionCurva;

    qreal fuerzaCurva;
    qreal amplitudArcoVuelo;
    qreal desvioHorizontalVuelo;

    bool activa;
    QString rutaSprite;
    DatosAtaque ataqueActual;

    int valorPuntaje;
    bool bonificacionAroActiva;
    qreal multiplicadorEscala;
};

#endif
