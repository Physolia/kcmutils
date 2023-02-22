/**
 * SPDX-FileCopyrightText: Year Author <author@domain.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <KPluginFactory>

#include <KQuickManagedConfigModule>

class TimeSettings : public KQuickManagedConfigModule
{
    Q_OBJECT
public:
    TimeSettings(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : KQuickManagedConfigModule(parent, data, args)
    {
    }
};

K_PLUGIN_CLASS_WITH_JSON(TimeSettings, "kcm_time.json")

#include "timesettings.moc"
