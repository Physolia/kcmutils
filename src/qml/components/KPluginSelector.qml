/*
    SPDX-FileCopyrightText: 2022 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.19 as Kirigami

import org.kde.kcmutils.private 1.0 as KCMUtilsPrivate
import "private" as Private

ListView {
    id: pluginSelector;
    property QtObject sourceModel;
    property Kirigami.SearchField searchField;

    clip: true

    model: KCMUtilsPrivate.ProxyModel {
        id: proxyModel
        model: pluginSelector.sourceModel
        query: searchField.text
    }

    delegate: KPluginDelegate {
        aboutDialog: pluginSelector.aboutDialog
    }

    section.property: "category"
    section.delegate: Kirigami.ListSectionHeader {
        width: pluginSelector.width
        text: section
    }

    Kirigami.OverlaySheet {
        id: internalAboutDialog
        parent: pluginSelector.parent
        property var metaDataInfo
        background: Rectangle {
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            color: Kirigami.Theme.backgroundColor
        }

        contentItem: Loader {
            active: aboutDialog.metaDataInfo !== undefined
            sourceComponent: Private.AboutPlugin {
                metaData: aboutDialog.metaDataInfo
            }
        }
    }
    property var aboutDialog: internalAboutDialog

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.gridUnit * 8)
        icon.name: "edit-none"
        text: pluginSelector.searchField && pluginSelector.searchField.text.length > 0 ? i18n("No matches") : i18n("No plugins found")
        visible: pluginSelector.count === 0
    }
}
