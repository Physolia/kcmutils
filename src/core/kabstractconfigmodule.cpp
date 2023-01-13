#include "kabstractconfigmodule.h"
#include <kpluginmetadata.h>

class KAbstractConfigModulePrivate
{
public:
    KAbstractConfigModulePrivate(const KPluginMetaData &data)
        : m_data(data)
    {
    }
    const KPluginMetaData m_data;

    QString m_rootOnlyMessage;
    QString m_quickHelp;
    QString m_errorString;

    bool m_useRootOnlyMessage = false;
    bool m_needsAuthorization = false;
    bool m_needsSave = false;
    bool m_representsDefaults = false;
    bool m_defaultsIndicatorVisible = false;
    QString m_authActionName;
    KAbstractConfigModule::Buttons m_buttons = KAbstractConfigModule::Help | KAbstractConfigModule::Default | KAbstractConfigModule::Apply;
};

KAbstractConfigModule::KAbstractConfigModule(QObject *parent, const KPluginMetaData &metaData, const QVariantList & /*args*/)
    : QObject(parent)
    , d(new KAbstractConfigModulePrivate(metaData))
{
}

KAbstractConfigModule::~KAbstractConfigModule() = default;

KAbstractConfigModule::Buttons KAbstractConfigModule::buttons() const
{
    return d->m_buttons;
}

void KAbstractConfigModule::setButtons(const KAbstractConfigModule::Buttons buttons)
{
    if (d->m_buttons == buttons) {
        return;
    }

    d->m_buttons = buttons;
    Q_EMIT buttonsChanged();
}

void KAbstractConfigModule::setNeedsAuthorization(bool needsAuth)
{
    if (d->m_needsAuthorization == needsAuth) {
        return;
    }

    d->m_needsAuthorization = needsAuth;
    if (needsAuth) {
        d->m_authActionName = QLatin1String("org.kde.kcontrol.") + d->m_data.pluginId() + QLatin1String(".save");
        d->m_needsAuthorization = true;

    } else {
        d->m_authActionName = QString();
    }

    Q_EMIT needsAuthorizationChanged();
    Q_EMIT authActionNameChanged();
}

bool KAbstractConfigModule::needsAuthorization() const
{
    return d->m_needsAuthorization;
}

QString KAbstractConfigModule::name() const
{
    return d->m_data.name();
}

QString KAbstractConfigModule::description() const
{
    return d->m_data.description();
}

void KAbstractConfigModule::setAuthActionName(const QString &name)
{
    if (d->m_authActionName == name) {
        return;
    }

    d->m_authActionName = name;
    d->m_needsAuthorization = true;

    Q_EMIT needsAuthorizationChanged();
    Q_EMIT authActionNameChanged();
}

QString KAbstractConfigModule::authActionName() const
{
    return d->m_authActionName;
}

void KAbstractConfigModule::load()
{
    setNeedsSave(false);
}

void KAbstractConfigModule::save()
{
    setNeedsSave(false);
}

void KAbstractConfigModule::defaults()
{
}

void KAbstractConfigModule::setRootOnlyMessage(const QString &message)
{
    if (d->m_rootOnlyMessage == message) {
        return;
    }

    d->m_rootOnlyMessage = message;
    Q_EMIT rootOnlyMessageChanged();
}

QString KAbstractConfigModule::rootOnlyMessage() const
{
    return d->m_rootOnlyMessage;
}

void KAbstractConfigModule::setUseRootOnlyMessage(bool on)
{
    if (d->m_useRootOnlyMessage == on) {
        return;
    }

    d->m_useRootOnlyMessage = on;

    Q_EMIT useRootOnlyMessageChanged();
}

bool KAbstractConfigModule::useRootOnlyMessage() const
{
    return d->m_useRootOnlyMessage;
}

void KAbstractConfigModule::setQuickHelp(const QString &help)
{
    if (d->m_quickHelp == help) {
        return;
    }

    d->m_quickHelp = help;

    Q_EMIT quickHelpChanged();
}

QString KAbstractConfigModule::quickHelp() const
{
    return d->m_quickHelp;
}

void KAbstractConfigModule::setNeedsSave(bool needs)
{
    if (needs == d->m_needsSave) {
        return;
    }

    d->m_needsSave = needs;
    Q_EMIT needsSaveChanged();
}

bool KAbstractConfigModule::needsSave() const
{
    return d->m_needsSave;
}

void KAbstractConfigModule::setRepresentsDefaults(bool defaults)
{
    if (defaults == d->m_representsDefaults) {
        return;
    }

    d->m_representsDefaults = defaults;
    Q_EMIT representsDefaultsChanged();
}

bool KAbstractConfigModule::representsDefaults() const
{
    return d->m_representsDefaults;
}

bool KAbstractConfigModule::defaultsIndicatorsVisible() const
{
    return d->m_defaultsIndicatorVisible;
}

void KAbstractConfigModule::setDefaultsIndicatorsVisible(bool visible)
{
    if (d->m_defaultsIndicatorVisible == visible) {
        return;
    }
    d->m_defaultsIndicatorVisible = visible;
    Q_EMIT defaultsIndicatorsVisibleChanged();
}

KPluginMetaData KAbstractConfigModule::metaData() const
{
    return d->m_data;
}

#include "moc_kabstractconfigmodule.cpp"