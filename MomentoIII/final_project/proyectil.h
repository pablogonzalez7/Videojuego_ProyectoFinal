#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QPointF>

/*
    Clase Proyectil

    Representa la bola de energía lanzada por Freezer.

    Esta clase maneja dos momentos principales:

    1. Cuando Freezer lanza la bola:
       - La bola baja hacia Vegito.
       - Puede tener movimiento parabólico o movimiento oscilatorio.

    2. Cuando Vegito batea la bola:
       - La bola se mueve hacia una zona del campo.
       - Se agranda un poco para simular altura.
       - Luego cae casi con el mismo tamaño inicial.
*/
class Proyectil : public QObject
{
public:
    struct DatosAtaque {
        float velocidad;
        float dano;
    };

    /*
        Estados posibles de la bola.

        LanzadaPorFreezer:
        La bola viene desde Freezer hacia Vegito.

        BateadaPorVegito:
        La bola ya fue golpeada y viaja hacia el campo.

        Caida:
        La bola ya terminó su vuelo y debe revisarse si cayó en zona de puntos.
    */
    enum EstadoProyectil {
        LanzadaPorFreezer,
        BateadaPorVegito,
        Caida
    };

    /*
        Constructor del proyectil.

        Recibe:
        - scene: escena donde se dibuja la bola.
        - rutaImagen: ruta del sprite de la bola.
        - posicionInicial: punto donde aparece la bola.
    */
    Proyectil(QGraphicsScene *scene,
              const QString &rutaImagen,
              QPointF posicionInicial = QPointF(650, 270));

    /*
        Destructor.

        Elimina la bola de la escena y libera memoria.
    */
    ~Proyectil();

    /*
        Inicia el timer interno del proyectil.

        En esta versión casi todo el movimiento se maneja desde MainWindow,
        pero se deja por si se necesita usar el timer propio del proyectil.
    */
    void iniciarTimerProyectil();

    /*
        Detiene el timer interno del proyectil.
    */
    void detenerTimerProyectil();

    /*
        Movimiento básico usado por el timer interno.

        Llama a moverLanzamiento con valores por defecto.
    */
    void movProyectil();

    /*
        Mueve la bola cuando viene desde Freezer.

        Recibe:
        - velocidad: velocidad vertical de la bola.
        - oscilatorio: si es true, la bola se mueve de lado a lado.
                       si es false, usa movimiento parabólico suave.
    */
    void moverLanzamiento(float velocidad, bool oscilatorio);

    /*
        Cambia la bola al estado de bateada.

        Recibe el punto final donde se supone que va a caer la bola.
    */
    void iniciarBateo(qreal destinoX, qreal destinoY);

    /*
        Actualiza el movimiento de la bola después del batazo.

        dt es el tiempo de actualización. En MainWindow se usa 0.016,
        que equivale aproximadamente a 60 FPS.
    */
    void actualizarBateo(float dt);

    /*
        Permite modificar el destino de caída usando A, D, W, S.

        Esto es lo que le da control al jugador mientras la bola está en vuelo.
    */
    void ajustarDestino(qreal dx, qreal dy);

    /*
        Prepara la bola para volver a usarla desde la posición de Freezer.
    */
    void reiniciar(QPointF posicionInicial, const QString &rutaImagen);

    /*
        Oculta la bola y la deja lista para volver al pool.
    */
    void desactivar();

    /*
        Asigna los datos del ataque actual.
    */
    void configurarAtaque(float velocidad, float dano);

    /*
        Retorna true cuando la bola terminó el vuelo después del batazo.
    */
    bool terminoVuelo() const;

    /*
        Retorna true si la bola pasó una línea en Y.

        Se usa para saber si la bola pasó a Vegito sin ser bateada.
    */
    bool pasoLinea(qreal limiteY) const;

    /*
        Retorna el estado actual del proyectil.
    */
    EstadoProyectil getEstado() const;

    bool estaActiva() const;

    /*
        Retorna el QGraphicsPixmapItem real de la bola.

        Se usa para colisiones y para acceder al item visual.
    */
    QGraphicsPixmapItem* getItem() const;

    /*
        Retorna el centro real de la bola en la escena.
    */
    QPointF centro() const;

    float getVelocidadAtaque() const;

    float getDanoAtaque() const;

private:
    // Item gráfico que representa visualmente la bola.
    QGraphicsPixmapItem *proyectil;

    // Timer propio del proyectil.
    QTimer *timerP;

    // Velocidades base. Se dejan por si se requieren después.
    float vx;
    float vy;

    // Estado actual de la bola.
    EstadoProyectil estado;

    // Tiempo usado para calcular la trayectoria antes del bateo.
    float tiempo;

    // Tiempo usado para calcular el vuelo después del bateo.
    float tiempoVuelo;

    // Duración del vuelo de la bola bateada.
    float duracionVuelo;

    // Posición inicial del vuelo después del batazo.
    qreal inicioX;
    qreal inicioY;

    // Posición final o destino donde caerá la bola.
    qreal destinoX;
    qreal destinoY;

    // Centro horizontal usado para movimientos parabólicos u oscilatorios.
    qreal xBaseOscilacion;

    // Dirección de curva para el lanzamiento parabólico.
    qreal direccionCurva;

    // Qué tanto se curva la bola en el lanzamiento parabólico.
    qreal fuerzaCurva;

    // Indica si la bola se está usando en este momento.
    bool activa;

    // Ruta del sprite actual para reutilizar el mismo proyectil.
    QString rutaSprite;

    // Datos del ataque asociado a esta bola.
    DatosAtaque ataqueActual;
};

#endif // PROYECTIL_H
