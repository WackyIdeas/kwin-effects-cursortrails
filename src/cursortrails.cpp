/*
 * SPDX-FileCopyrightText: 2026 WackyIdeas
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cursortrails.h"
#include "trailsconfig.h"

#include "core/rendertarget.h"

#define PI 3.14159

namespace KWin
{


ParticleEmitter::ParticleEmitter(unsigned int am)
{
    amount = am;
    particles = new Particle[am];
    currentDelta = QTime::currentTime();
    init();
}

void ParticleEmitter::update(unsigned int newParticles, QVector2D offset, bool spawnNew)
{
    if(spawnNew)
    {
        for (unsigned int i = 0; i < newParticles; i++)
        {
            int unusedParticle = firstUnusedParticle();
            respawnParticle(particles[unusedParticle], offset);
        }
    }

    if(aura)
        cursorParticle.position = offset;

    if(activeParticles == 0) return;

    for (unsigned int i = 0; i < amount; ++i) {
        Particle &p = particles[i];
        p.life -= dt;
        if (p.life > 0.0f) {

            p.color.setW(p.color.w() - dt * duration);
            if(p.color.w() < 0.0) p.color.setW(0.0);

            p.position += p.velocity * dt;
        }
    }
}

void ParticleEmitter::respawnParticle(Particle &particle, QVector2D offset)
{
    particle.position = offset;
    particle.color = QVector4D(0.0, 0.0, 0.0, 1.0f); // Used to control the opacity, in the future the afterimages can have color
    particle.life = 1.0f;

    particle.velocity = QVector2D((aura ? 50.0 : 0.0), 0.0);
}


void ParticleEmitter::init()
{
    float particle_quad[] =
    {
        // position // UV
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    cursorParticle.velocity = QVector2D((0.0), 0.0);
    cursorParticle.color = QVector4D(0.0, 0.0, 0.0, 1.0);

    int VAO_old;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &VAO_old);

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(QVector4D) * 2, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector4D), (void*)0);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector4D), (void*)(sizeof(QVector4D)));
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(VAO_old);
}

void ParticleEmitter::draw(QMatrix4x4 mvp, GLShader *shader, const RenderViewport &viewport, GLTexture* texture, const RenderTarget &renderTarget)
{
    if(texture == nullptr) return;
    std::vector<QVector4D> instanceData;
    instanceData.reserve(amount * 2 + (aura ? 2 : 0));

    activeParticles = 0;
    for (unsigned int i = 0; i < amount; i++)
    {
        if (particles[i].life >= 0.0f)
        {
            auto newPos = particles[i].position * viewport.scale();
            instanceData.push_back(QVector4D(newPos.x(), newPos.y(), texture->width() * viewport.scale(), texture->height() * viewport.scale()));
            instanceData.push_back(particles[i].color);
            activeParticles++;
        }
    }

    if(aura)
    {
        auto newPos = cursorParticle.position * viewport.scale();
        instanceData.push_back(QVector4D(newPos.x(), newPos.y(), texture->width() * viewport.scale(), texture->height() * viewport.scale()));
        instanceData.push_back(cursorParticle.color);
        activeParticles++;
    }

    if (activeParticles == 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, activeParticles * sizeof(QVector4D) * 2 + (aura ? 2 : 0), &instanceData[0]);

    ShaderManager::instance()->pushShader(shader);
    shader->setUniform(GLShader::Mat4Uniform::ModelViewProjectionMatrix, mvp);
    const auto toXYZ = renderTarget.colorDescription()->containerColorimetry().toXYZ();
    shader->setUniform(GLShader::Vec3Uniform::PrimaryBrightness, QVector3D(toXYZ(1, 0), toXYZ(1, 1), toXYZ(1, 2)));
    shader->setColorspaceUniforms(ColorDescription::sRGB, renderTarget.colorDescription(), RenderingIntent::Perceptual);

    texture->bind();

    int VAO_old;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &VAO_old);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, activeParticles);
    glBindVertexArray(VAO_old);

    texture->unbind();
    ShaderManager::instance()->popShader();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
unsigned int ParticleEmitter::firstUnusedParticle() {
    for (unsigned int i = lastUsedParticle; i < amount; i++)
    {
        if (particles[i].life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (particles[i].life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    lastUsedParticle = 0;
    return 0;
}
void ParticleEmitter::updateDelta()
{
    oldDelta = currentDelta;
    currentDelta = QTime::currentTime();
    dt = (oldDelta.msecsTo(currentDelta) / 1000.0);
}
void ParticleEmitter::setFrequency(unsigned int f)
{
    frequency = f;
}
void ParticleEmitter::setDuration(unsigned int d)
{
    duration = 1000.0 / d;
}
void ParticleEmitter::setAura(bool a)
{
    aura = a;
}
bool ParticleEmitter::hasAura() const
{
    return aura;
}
unsigned int ParticleEmitter::activeParticleCount() const
{
    return activeParticles;
}

CursorTrailEffect::CursorTrailEffect()
{
    TrailsConfig::instance(effects->config());
    input()->installInputEventSpy(this);

    m_shader = ShaderManager::instance()->generateShaderFromFile(
        ShaderTrait::MapTexture | ShaderTrait::TransformColorspace,
        QStringLiteral(":/effects/cursortrails/shaders/vertex.vert"),
        QStringLiteral(":/effects/cursortrails/shaders/shader.frag")
    );

    m_emitter = new ParticleEmitter(200);
    reconfigure(ReconfigureAll);

    m_frequencyTimer.setInterval(m_frequency);
    m_frequencyTimer.callOnTimeout([&]() {
        m_canCreate = true;
    });
    m_frequencyTimer.start();

}

CursorTrailEffect::~CursorTrailEffect()
{
    if (m_emitter)
    {
        delete m_emitter;
    }
}

bool CursorTrailEffect::supported()
{
    return effects->isOpenGLCompositing();
}

void CursorTrailEffect::reconfigure(Effect::ReconfigureFlags flags)
{
    Q_UNUSED(flags)
	if(m_emitter == nullptr) return;
    TrailsConfig::self()->read();
    m_emitter->setDuration(TrailsConfig::duration());
    m_emitter->setAura(TrailsConfig::aura());
    m_frequency = TrailsConfig::frequency();
    m_frequencyTimer.setInterval(m_frequency);
    m_frequencyTimer.stop();
    m_frequencyTimer.start();

}

void CursorTrailEffect::pointerMotion(PointerMotionEvent *event)
{
    float magnitude = event->delta.manhattanLength();
    if(magnitude > 0.1)
    {
        m_cursorMoved = true;
    }
}

void CursorTrailEffect::prePaintScreen(ScreenPrePaintData &data)
{
    m_emitter->updateDelta();
    QPoint pos = (effects->cursorPos() - effects->cursorImage().hotSpot()).toPoint();
    QImage shape = effects->cursorImage().image();
    m_emitter->update(1, QVector2D(pos.x(), pos.y()), (m_cursorMoved || m_emitter->hasAura()) && m_canCreate);
    m_cursorShape = GLTexture::upload(shape);
    effects->prePaintScreen(data);
}

void CursorTrailEffect::paintScreen(const RenderTarget &renderTarget, const RenderViewport &viewport, int mask, const Region &region, LogicalOutput *screen)
{
    effects->paintScreen(renderTarget, viewport, mask, region, screen);
    m_emitter->draw(viewport.projectionMatrix(), m_shader.get(), viewport, m_cursorShape.get(), renderTarget);
}

void CursorTrailEffect::postPaintScreen()
{
    if(m_emitter->activeParticleCount() > 0)
    {
        effects->addRepaintFull();
    }

    effects->postPaintScreen();
    m_cursorMoved = false;
    m_canCreate = false;
}

}
