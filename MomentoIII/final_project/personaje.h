#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsScene>
#include <QString>

#include "sprite.h"

/*
    Clase Personaje

    Encapsula la parte gráfica y el estado básico de vida de un personaje.
    También centraliza la animación para evitar usar un timer por sprite.
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
    ~Personaje();

    void configurarSpritesEuforia(const QString &rutaTransformacion,
                                  int framesTransformacion,
                                  const QString &rutaAtaqueKaioken,
                                  int framesAtaqueKaioken);

    void agregarAEscena(QGraphicsScene *scene);
    Sprite *getSprite() const;

    void setPos(qreal posX, qreal posY);
    void setVisible(bool visible);

    void actualizar(int dtMs);
    void reproducirAtaque();
    void activarEuforia();

    bool estaEnEuforia() const;
    bool estaAnimando() const;

    float getVidaActual() const;
    float getVidaMaxima() const;
    void reiniciarVida();
    void recibirDanio(float dano);

private:
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
