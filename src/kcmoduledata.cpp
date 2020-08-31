/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2020 Benjamin Port <benjamin.port@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kcmoduledata.h"

#include <QList>
#include <QPointer>

#include <KCoreConfigSkeleton>

class KCModuleDataPrivate
{
public:
    explicit KCModuleDataPrivate(KCModuleData *probe)
        : _q(probe)
    {
    }

    KCModuleData *_q;
    QList<QPointer<KCoreConfigSkeleton>> _skeletons;
};

KCModuleData::KCModuleData(QObject *parent, const QVariantList &)
    : QObject(parent), d(new KCModuleDataPrivate(this))
{
}

KCModuleData::~KCModuleData()
{
    delete d;
}

void KCModuleData::virtual_hook(int, void *)
{
}

void KCModuleData::registerSkeleton(KCoreConfigSkeleton *skeleton)
{
    if (!skeleton || d->_skeletons.contains(skeleton)) {
        return;
    }

    d->_skeletons.append(skeleton);
}

bool KCModuleData::isDefaults() const
{
    bool defaults = true;
    for (const auto &skeleton : qAsConst(d->_skeletons)) {
        defaults &= skeleton->isDefaults();
    }
    return defaults;
}

void KCModuleData::revertToDefaults()
{
    for (const auto &skeleton : qAsConst(d->_skeletons)) {
        skeleton->useDefaults(true);
        skeleton->save();
    }
}

void KCModuleData::autoRegisterSkeletons()
{
    const auto skeletons = findChildren<KCoreConfigSkeleton*>();
    for (auto *skeleton : skeletons) {
        registerSkeleton(skeleton);
    }
}

bool KCModuleData::matchesQuery(const QString &query) const
{
    // Currently not implemented, here for future use case
    Q_UNUSED(query)
    return false;
}

#include "moc_kcmoduledata.cpp"
