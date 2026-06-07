#include "personaje.h"

Personaje::Personaje(const QString &rutaQuieto, int framesQuieto, const QString &rutaAtaque, int framesAtaque, qreal posX, qreal posY, float vidaMaximaInicial)
{
    spriteQuietoNormal = {rutaQuieto, framesQuieto};
    spriteAtaqueNormal = {rutaAtaque, framesAtaque};
    spriteTransformacion = {"", 0};
    spriteAtaqueKaioken = {"", 0};
    spriteImpacto = {"", 0};

    spritePersonaje = new Sprite(rutaQuieto, framesQuieto);

    posicionX = posX;
    posicionY = posY;
    spritePersonaje->setPos(posicionX, posicionY);

    tiempoPorFrameMs = 100;
    tiempoAcumuladoMs = 0;
    tiempoRetencionAnimacionMs = 0;
    euforiaDisponible = false;
    tieneSpritesEuforia = false;
    animacionActual = SinAnimacion;

    vidaMaxima = vidaMaximaInicial;
    vidaActual = vidaMaximaInicial;
}

Personaje::~Personaje()
{
    delete spritePersonaje;
}

void Personaje::configurarSpritesEuforia(const QString &rutaTransformacion, int framesTransformacion, const QString &rutaAtaqueKaioken, int framesAtaqueKaioken)
{
    spriteTransformacion = {rutaTransformacion, framesTransformacion};
    spriteAtaqueKaioken = {rutaAtaqueKaioken, framesAtaqueKaioken};
    tieneSpritesEuforia = true;
}

void Personaje::configurarSpriteImpacto(const QString &rutaImpacto, int framesImpacto)
{
    spriteImpacto = {rutaImpacto, framesImpacto};
}

void Personaje::agregarAEscena(QGraphicsScene *scene)
{
    if (scene != nullptr && spritePersonaje->scene() != scene) {
        scene->addItem(spritePersonaje);
    }
}

Sprite *Personaje::getSprite() const
{
    return spritePersonaje;
}

void Personaje::setPos(qreal posX, qreal posY)
{
    posicionX = posX;
    posicionY = posY;
    spritePersonaje->setPos(posicionX, posicionY);
}

void Personaje::setVisible(bool visible)
{
    spritePersonaje->setVisible(visible);
}

void Personaje::actualizar(int dtMs)
{
    if (animacionActual == SinAnimacion) {
        return;
    }

    if (tiempoRetencionAnimacionMs > 0) {
        tiempoRetencionAnimacionMs -= dtMs;

        if (tiempoRetencionAnimacionMs > 0) {
            return;
        }

        tiempoRetencionAnimacionMs = 0;
    }

    tiempoAcumuladoMs += dtMs;

    while (tiempoAcumuladoMs >= tiempoPorFrameMs) {
        tiempoAcumuladoMs -= tiempoPorFrameMs;

        if (spritePersonaje->avanzarFrame()) {
            if (animacionActual == AnimacionTransformacion) {
                euforiaDisponible = true;
            }

            animacionActual = SinAnimacion;
            mostrarSpriteQuieto();
            break;
        }
    }
}

void Personaje::reproducirAtaque()
{
    if (euforiaDisponible && tieneSpritesEuforia) {
        iniciarAnimacion(spriteAtaqueKaioken, AnimacionAtaque);
        return;
    }

    iniciarAnimacion(spriteAtaqueNormal, AnimacionAtaque);
}

void Personaje::reproducirImpacto()
{
    iniciarAnimacion(spriteImpacto, AnimacionImpacto);
}

void Personaje::activarEuforia()
{
    if (!tieneSpritesEuforia || euforiaDisponible || animacionActual == AnimacionTransformacion) {
        return;
    }

    iniciarAnimacion(spriteTransformacion, AnimacionTransformacion);
}

bool Personaje::estaEnEuforia() const
{
    return euforiaDisponible;
}

bool Personaje::estaAnimando() const
{
    return animacionActual != SinAnimacion;
}

float Personaje::getVidaActual() const
{
    return vidaActual;
}

float Personaje::getVidaMaxima() const
{
    return vidaMaxima;
}

void Personaje::reiniciarVida()
{
    vidaActual = vidaMaxima;
    euforiaDisponible = false;
    tiempoAcumuladoMs = 0;
    tiempoRetencionAnimacionMs = 0;
    animacionActual = SinAnimacion;
    mostrarSpriteQuieto();
}

void Personaje::recibirDaño(float daño)
{
    vidaActual -= daño;

    if (vidaActual < 0.0f) {
        vidaActual = 0.0f;
    }
}

void Personaje::mostrarSpriteQuieto()
{
    if (euforiaDisponible && tieneSpritesEuforia) {
        spritePersonaje->cambiarSprite(spriteAtaqueKaioken.ruta, spriteAtaqueKaioken.frames);
        spritePersonaje->reiniciarAnimacion();
        return;
    }

    spritePersonaje->cambiarSprite(spriteQuietoNormal.ruta, spriteQuietoNormal.frames);
    spritePersonaje->reiniciarAnimacion();
}

void Personaje::iniciarAnimacion(const DatosSprite &datos, TipoAnimacion tipo)
{
    if (datos.ruta.isEmpty()) {
        return;
    }

    tiempoAcumuladoMs = 0;
    tiempoRetencionAnimacionMs = 0;

    if (tipo == AnimacionImpacto) {
        tiempoRetencionAnimacionMs = 320;
    }

    animacionActual = tipo;
    spritePersonaje->cambiarSprite(datos.ruta, datos.frames);
    spritePersonaje->reiniciarAnimacion();
}
