/*
    SPDX-FileCopyrightText: 2022 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kcm 1.2
import org.kde.kirigami 2.10 as Kirigami

import org.kde.kcmutils.private as KCMUtilsPrivate
import "private" as Private

ScrollViewKCM {
    id: pluginSelector;
    property QtObject model;

    header: Kirigami.SearchField {
        id: searchField
        Layout.fillWidth: true
        onTextChanged: function (text) {
            pluginSelector.model.query = searchField.text;
        }
    }

    view: ListView {
        id: pluginSelectorListView

        clip: true

        model: KCMUtilsPrivate {
            model: pluginSelector.model
        }

        delegate: Private.KPluginDelegate {
            width: pluginSelectorListView.width
        }

        section.property: "category"
        section.delegate: Kirigami.ListSectionHeader {
            width: pluginSelectorListView.width
            text: section
        }
    }
}
