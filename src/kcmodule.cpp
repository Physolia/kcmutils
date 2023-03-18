/*
    This file is part of the KDE libraries

    SPDX-FileCopyrightText: 2001 Michael Goffioul <kdeprint@swing.be>
    SPDX-FileCopyrightText: 2004 Frans Englich <frans.englich@telia.com>
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcmodule.h"
#include "kabstractconfigmodule.h"
#include "kcmutils_debug.h"

#include <QVBoxLayout>

#include <KConfigDialogManager>
#include <KConfigSkeleton>
#include <KLocalizedString>
#include <KPluginMetaData>

class KCModuleProxyInternal : public QWidget
{
    Q_OBJECT
public:
    KCModuleProxyInternal(KCModule *parentModule, QWidget *parent)
        : QWidget(parent)
        , m_parentModule(parentModule)
    {
    }

protected:
    void showEvent(QShowEvent *ev) override
    {
        if (m_firstShow) {
            m_firstShow = false;
            QMetaObject::invokeMethod(m_parentModule, &KCModule::load, Qt::QueuedConnection);
            QMetaObject::invokeMethod(
                this,
                [this]() {
                    m_parentModule->setNeedsSave(false);
                },
                Qt::QueuedConnection);
        }

        QWidget::showEvent(ev);
    }

private:
    bool m_firstShow = true;
    KCModule *m_parentModule;
};

class KCModulePrivate
{
public:
    KCModulePrivate(QWidget *parentWidget)
        : _needsAuthorization(false)
        , _unmanagedWidgetChangeState(false)
        , _unmanagedWidgetDefaultState(false)
        , _unmanagedWidgetDefaultStateCalled(false)
        , parentWidget(parentWidget)
    {
    }

    void authStatusChanged(int status);

    QList<KConfigDialogManager *> managers;

    bool _needsAuthorization : 1;

    // this member is used to record the state on non-automatically
    // managed widgets, allowing for mixed KConfigXT-drive and manual
    // widgets to coexist peacefully and do the correct thing with
    // the changed(bool) signal
    bool _unmanagedWidgetChangeState : 1;
    bool _unmanagedWidgetDefaultState : 1;
    bool _unmanagedWidgetDefaultStateCalled : 1;
    QVBoxLayout *m_topLayout = nullptr; /* Contains QScrollView view, and root stuff */
    QWidget *parentWidget;
    KCModuleProxyInternal *m_proxyInternal = nullptr;
};

KCModule::KCModule(QWidget *parent, const KPluginMetaData &data, const QVariantList &)
    : KAbstractConfigModule(parent, data, {})
    , d(new KCModulePrivate(parent))
{
}

KConfigDialogManager *KCModule::addConfig(KCoreConfigSkeleton *config, QWidget *widget)
{
    KConfigDialogManager *manager = new KConfigDialogManager(widget, config);
    manager->setObjectName(objectName());
    connect(manager, &KConfigDialogManager::widgetModified, this, &KCModule::widgetChanged);
    connect(manager, &QObject::destroyed, this, [this, manager]() {
        d->managers.removeOne(manager);
    });
    d->managers.append(manager);
    return manager;
}

KCModule::~KCModule()
{
    qDeleteAll(d->managers);
    d->managers.clear();
}

void KCModule::load()
{
    for (KConfigDialogManager *manager : std::as_const(d->managers)) {
        manager->updateWidgets();
    }
    widgetChanged();
}

void KCModule::save()
{
    for (KConfigDialogManager *manager : std::as_const(d->managers)) {
        manager->updateSettings();
    }
    setNeedsSave(false);
}

void KCModule::defaults()
{
    for (KConfigDialogManager *manager : std::as_const(d->managers)) {
        manager->updateWidgetsDefault();
    }
}

QWidget *KCModule::widget()
{
    if (!d->m_proxyInternal) {
        d->m_topLayout = new QVBoxLayout(d->parentWidget);
        d->m_proxyInternal = new KCModuleProxyInternal(this, d->parentWidget);
        d->m_topLayout->addWidget(d->m_proxyInternal);
    }
    return d->m_proxyInternal;
}

void KCModule::widgetChanged()
{
    setNeedsSave(d->_unmanagedWidgetChangeState || managedWidgetChangeState());
    if (d->_unmanagedWidgetDefaultStateCalled) {
        setRepresentsDefaults(d->_unmanagedWidgetDefaultState && managedWidgetDefaultState());
    } else {
        setRepresentsDefaults(!d->managers.isEmpty() && managedWidgetDefaultState());
    }
}

bool KCModule::managedWidgetChangeState() const
{
    for (KConfigDialogManager *manager : std::as_const(d->managers)) {
        if (manager->hasChanged()) {
            return true;
        }
    }

    return false;
}

bool KCModule::managedWidgetDefaultState() const
{
    for (KConfigDialogManager *manager : std::as_const(d->managers)) {
        if (!manager->isDefault()) {
            return false;
        }
    }

    return true;
}

void KCModule::unmanagedWidgetChangeState(bool changed)
{
    d->_unmanagedWidgetChangeState = changed;
    widgetChanged();
}

void KCModule::unmanagedWidgetDefaultState(bool defaulted)
{
    d->_unmanagedWidgetDefaultStateCalled = true;
    d->_unmanagedWidgetDefaultState = defaulted;
    widgetChanged();
}

QList<KConfigDialogManager *> KCModule::configs() const
{
    return d->managers;
}

#include "kcmodule.moc"
