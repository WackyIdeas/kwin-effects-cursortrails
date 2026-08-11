/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>
    SPDX-FileCopyrightText: 2010 Alexandre Pereira <pereira.alex@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "trails_config.h"

//#include "config-kwin.h"

// KConfigSkeleton
#include "trailsconfig.h"

#include <klocalizedstring.h>
#include <KPluginFactory>
#include "kwineffects_interface.h"
#define KWIN_CONFIG "kwinrc"

namespace KWin
{

K_PLUGIN_CLASS(TrailsEffectConfig)

TrailsEffectConfig::TrailsEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    TrailsConfig::instance(QStringLiteral(KWIN_CONFIG));
    addConfig(TrailsConfig::self(), widget());
}

TrailsEffectConfig::~TrailsEffectConfig()
{
}

void TrailsEffectConfig::save()
{
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("libkwin_effect_cursortrails"));
}

} // namespace KWin

#include "trails_config.moc"

#include "moc_trails_config.cpp"
