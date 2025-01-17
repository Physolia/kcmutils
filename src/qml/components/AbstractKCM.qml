/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.7
import QtQuick.Controls 2.2 as QQC2
import org.kde.kirigami 2.14 as Kirigami

/**
 * This component is intended to be used as root item for
 * KCMs with arbitrary content. Unlike SimpleKCM this does NOT
 * provide a scrollable view, The developer will have to manage
 * their own scrollviews.
 * Most of the times SimpleKCM should be used instead
 * @code
 * import org.kde.kcmutils as KCM
 * import org.kde.kirigami 2.3 as Kirigami
 * KCM.AbstractKCM {
 *     RowLayout {
 *         QQC2.ScrollView {
 *             ...
 *         }
 *         QQC2.ScrollView {
 *             ...
 *         }
 *     }
 *     footer: Item {...}
 * }
 * @endcode
 * @inherits org.kde.kirigami.Page
 * @since 6.0
 */
Kirigami.Page {
    id: root

    readonly property int margins: 6 // Layout_ChildMarginWidth from Breeze

    /**
     * framedView: bool
     * Whether to use this component as the base of a "framed" KCM with an
     * inner scrollview that draws its own frame.
     * Default: true
     */
    property bool framedView: true

    /**
     * extraFooterTopPadding: bool
     * Whether to add extra top padding to an empty footer when framedView is
     * false. Use the default value of true for KCMs in System Settings, because
     * otherwise the Apply, Help, and Defaults buttons provided by System
     * Settings won't have enough top padding and the button bar will look ugly.
     * When using this component outside of System Settings where there is no
     * such restriction, or in System Settings KCMs that don't show Apply, Help,
     * or Defaults buttons, set it to false.
     * Default: true
     */
    property bool extraFooterTopPadding: true

    property bool sidebarMode: false

    title: (typeof kcm !== "undefined") ? kcm.name : ""

    // Make pages fill the whole view by default
    Kirigami.ColumnView.fillWidth: sidebarMode
                                   ? Kirigami.ColumnView.view
                                         && (Kirigami.ColumnView.view.width < Kirigami.Units.gridUnit * 36
                                             || Kirigami.ColumnView.index >= Kirigami.ColumnView.view.count - 1)
                                   : true

    topPadding: root.framedView && !headerParent.contentVisible ? root.margins : 0
    leftPadding: root.framedView ? root.margins : 0
    rightPadding: root.framedView ? root.margins : 0
    bottomPadding: root.framedView && !footerParent.contentVisible ? root.margins : 0

    header: QQC2.Control {
        id: headerParent
        readonly property bool contentVisible: contentItem && contentItem.visible && contentItem.implicitHeight
        height: contentVisible ? implicitHeight : 0
        leftPadding: root.margins
        topPadding: root.margins
        rightPadding: root.margins
        bottomPadding: root.margins

        // When the scrollview isn't drawing its own frame, we need to add a
        // line below the header (when visible) to separate it from the view
        Kirigami.Separator {
            z: 999
            anchors {
                left: parent.left
                right: parent.right
                top: parent.bottom
            }
            visible: !root.framedView && headerParent.contentVisible
        }
    }

    // View background, shown when the scrollview isn't drawing its own frame
    Rectangle {
        anchors.fill: parent
        visible: !root.framedView
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
    }

    footer: QQC2.Control {
        id: footerParent
        readonly property bool contentVisible: contentItem && contentItem.visible && contentItem.implicitHeight

        height: contentVisible ? implicitHeight : 0
        leftPadding: root.margins
        topPadding: root.margins
        rightPadding: root.margins
        bottomPadding: root.margins

        // When the scrollview isn't drawing its own frame, we need to add a
        // line above the footer ourselves to separate it from the view
        Kirigami.Separator {
            z: 999
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.top
            }
            // Deliberately not checking for footerParent.contentVisible because
            // we always want the footer line to be visible when the scrollview
            // doesn't have a frame of its own, because System Settings always
            // adds its own footer for the Apply, Help, and Defaults buttons
            visible: !root.framedView
        }
    }

    Component.onCompleted: {
        if (footer && footer !== footerParent) {
            const f = footer

            footerParent.contentItem = f
            footer = footerParent
            footer.visible = true
            f.parent = footerParent
        }

        if (header && header !== headerParent) {
            const h = header

            // Revert the effect of repeated onHeaderChanged invocations
            // during initialization in Page super-type.
            h.anchors.top = undefined

            headerParent.contentItem = h
            header = headerParent
            header.visible = true
            h.parent = headerParent
        }

        //Search overlaysheets in contentItem, parent to root if found
        for (const i in contentItem.data) {
            const child = contentItem.data[i];
            if (child instanceof Kirigami.OverlaySheet) {
                if (!child.parent) {
                    child.parent = root;
                }
                root.data.push(child);
            }
        }
    }
}
