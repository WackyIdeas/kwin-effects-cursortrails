/*
 * SPDX-FileCopyrightText: 2026 WackyIdeas
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <chrono>
#include <QTime>
#include <QTimer>

#include <QObject>
#include <QVector2D>
#include <QVector4D>
#include <QRandomGenerator>

#include "effect/timeline.h"
#include "effect/effecthandler.h"
#include "effect/effectwindow.h"
#include "opengl/glshadermanager.h"
#include "opengl/glshader.h"
#include "opengl/gltexture.h"
#include "core/renderviewport.h"
#include "core/pixelgrid.h"
#include "input_event.h"
#include "input_event_spy.h"
#include "scene/scene.h"


// Credit for instancing code: https://www.cosmiclearn.com/opengl/particle-system.php

namespace KWin
{
struct Particle
{
    QVector2D position;
    QVector2D velocity;
    QVector4D color;
    float life = 0.0f;
    float size = 1.0f;
};

class ParticleEmitter
{
public:
    ParticleEmitter(unsigned int amount);
    void update(unsigned int newParticles, QVector2D offset, bool spawnNew);
    void draw(QMatrix4x4 mvp, GLShader* shader, const RenderViewport &view, GLTexture* texture);
    unsigned int activeParticleCount() const;
    void updateDelta();

    void setSize(unsigned int mn, unsigned int mx);
    void setSpeed(double mn, double mx, double a);
    void setFrequency(unsigned int f);
    void setDuration(unsigned int d);
    void setAura(bool a);

    bool hasAura() const;

private:
    Particle *particles;
    Particle cursorParticle;
    unsigned int amount;

    bool aura = false;

    unsigned int VAO;
    unsigned int instanceVBO;
    unsigned int lastUsedParticle = 0;
    unsigned int activeParticles = 0;

    QTime oldDelta;
    QTime currentDelta;
    double dt = 0.0;

    unsigned int minSize = 2;
    unsigned int maxSize = 2;

    double minSpeed = 50.0;
    double maxSpeed = 50.0;
    double angle = 0.0;

    unsigned int frequency = 50;
    unsigned int duration = 1000;
    std::unique_ptr<GLTexture> m_texture;

    void init();
    unsigned int firstUnusedParticle();
    void respawnParticle(Particle &particle, QVector2D offset);
};

class CursorTrailEffect : public Effect, public InputEventSpy
{
    Q_OBJECT

public:
    CursorTrailEffect();
    ~CursorTrailEffect() override;

    void reconfigure(ReconfigureFlags flags) override;
    void prePaintScreen(ScreenPrePaintData &data) override;
    void paintScreen(const RenderTarget &renderTarget, const RenderViewport &viewport, int mask, const Region &region, LogicalOutput *screen) override;
    void postPaintScreen() override;
    void pointerMotion(PointerMotionEvent *event) override;

    static bool supported();

    bool isActive() const override
    {
        return true;
    }

    int requestedEffectChainPosition() const override
    {
        return 10;
    }

private:

    ParticleEmitter *m_emitter;

    unsigned int m_frequency = 50;
    QTimer m_frequencyTimer;
    bool m_cursorMoved = false;
    bool m_canCreate = false;

    std::unique_ptr<GLShader> m_shader;
    std::unique_ptr<GLTexture> m_cursorShape;
};

}
