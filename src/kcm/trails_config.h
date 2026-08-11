/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>
    SPDX-FileCopyrightText: 2010 Alexandre Pereira <pereira.alex@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ui_trails_config.h"
#include <KCModule>

namespace KWin
{

class TrailsEffectConfig : public KCModule
{
    Q_OBJECT

public:
    explicit TrailsEffectConfig(QObject *parent, const KPluginMetaData &data);
    ~TrailsEffectConfig() override;

    void save() override;

private:
    ::Ui::TrailsEffectConfig ui;
};

} // namespace KWin
