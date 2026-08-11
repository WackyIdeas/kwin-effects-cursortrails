/*
 * SPDX-FileCopyrightText: 2026 WackyIdeas
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cursortrails.h"

namespace KWin
{
    KWIN_EFFECT_FACTORY_SUPPORTED(
        CursorTrailEffect, "metadata.json",
        {
            return CursorTrailEffect::supported();
        }
    )
}

#include "plugin.moc"
