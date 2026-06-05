#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsScene>
#include <QString>

#include "sprite.h"

/*
    Clase Personaje

    Clase base de cualquier personaje del juego.
    Conserva exactamente la lógica que ya tenían: sprite, animación,
    posición, vida y euforia/kaioken. Jugador y Villano heredan de aquí
    para no cargar toda la lógica en una sola clase.
*/
class Personaje
{
public:
    struct DatosSprite {
        QString ruta;
        int frames;
    };

    enum TipoAnimacion {
        SinAnimacion,
        AnimacionAtaque,
        AnimacionTransformacion
    };

    Personaje(const QString &rutaQuieto,
              int framesQuieto,
              const QString &rutaAtaque,
              int framesAtaque,
              qreal posX,
              qreal posY,
              float vidaMaxima);

    virtual ~Personaje();

    void configurarSpritesEuforia(const QString &rutaTransformacion,
                                  int framesTransformacion,
                                  const QString &rutaAtaqueKaioken,
                                  int framesAtaqueKaioken);

    void agregarAEscena(QGraphicsScene *scene);
    Sprite *getSprite() const;

    void setPos(qreal posX, qreal posY);
    void setVisible(bool visible);

    virtual void actualizar(int dtMs);
    virtual void reproducirAtaque();
    virtual void activarEuforia();

    bool estaEnEuforia() const;
    bool estaAnimando() const;

    float getVidaActual() const;
    float getVidaMaxima() const;
    void reiniciarVida();
    void recibirDanio(float dano);

protected:
    void mostrarSpriteQuieto();
    void iniciarAnimacion(const DatosSprite &datos, TipoAnimacion tipo);

    Sprite *spritePersonaje;

    DatosSprite spriteQuietoNormal;
    DatosSprite spriteAtaqueNormal;
    DatosSprite spriteTransformacion;
    DatosSprite spriteAtaqueKaioken;

    qreal posicionX;
    qreal posicionY;

    int tiempoPorFrameMs;
    int tiempoAcumuladoMs;
    bool euforiaDisponible;
    bool tieneSpritesEuforia;
    TipoAnimacion animacionActual;

    float vidaMaxima;
    float vidaActual;
};

#endif // PERSONAJE_H
