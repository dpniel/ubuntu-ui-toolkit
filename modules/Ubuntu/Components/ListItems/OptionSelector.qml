/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import "." as ListItem

/*!
    \qmltype OptionSelector
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief List item displaying single selected value when not expanded,
    where expanding it opens a listing of all the possible values for selection
    with an additional option of always being expanded.

    \b{This component is under heavy development.}

    Examples:
    \qml
        import Ubuntu.Components 0.1
        Column {
            width: 250
            OptionSelector {
                text: "Standard"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            OptionSelector {
                text: "Disabled"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                enabled: false
            }
            OptionSelector {
                text: "Expanded"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                expanded: true
            }
            OptionSelector {
                text: "Icon"
                icon: Qt.resolvedUrl("icon.png")
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                selectedIndex: 2
            }
        }
    \endqml
*/
ListItem.Empty {
    id: optionSelector
    __height: column.height

    /*!
      \preliminary
      The list of values that will be shown under the label text
     */
    property var values

    /*!
      \preliminary
      The index of the currently selected element from the \l values array.
     */
    property alias selectedIndex: list.currentIndex

    /*!
      \preliminary
      Specifies whether the list is always expanded.
     */
    property bool expanded: false

    /*!
      Called when the optionSelector is either expanded or collapsed.
     */
    signal scroll(real selectorHeight, string currentState)

    showDivider: false

    Column {
        id: column

        spacing: units.gu(2)
        anchors {
            left: parent.left
            right: parent.right
        }

        ListItem.LabelVisual {
            text: optionSelector.text
            height: units.gu(2)
        }

        ListItem.Empty {
            id: listContainer

            property bool isExpanded: expanded
            property int itemHeight: units.gu(5)
            property url chevron: __styleInstance.chevron
            property url tick: __styleInstance.tick
            property color themeColour: Theme.palette.selected.fieldText
            property bool colourComponent: __styleInstance.colourComponent

            anchors {
                left: parent.left
                right: parent.right
            }
            style: Theme.createStyleComponent("ListItemOptionSelectorStyle.qml", listContainer)

            onStateChanged: scroll(list.contentHeight, state)

            states: [ State {
                    name: "expanded"
                    when: listContainer.isExpanded
                    PropertyChanges {
                        target: listContainer
                        height: list.contentHeight
                    }
                }, State {
                    name: "closed"
                    when: !listContainer.isExpanded
                    PropertyChanges {
                        target: listContainer
                        height: itemHeight
                    }
                }
            ]

            Behavior on height {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                }
            }

            ListView {
                id: list

                interactive: false
                clip: true
                currentIndex: 0
                model: optionSelector.values
                anchors.fill: parent

                delegate:
                ListItem.Standard {
                    id: option

                    property bool currentItem: ListView.isCurrentItem

                    width: parent.width + units.gu(2)
                    height: listContainer.itemHeight
                    showDivider: index === list.count - 1 || !listContainer.isExpanded ? false : true
                    highlightWhenPressed: false
                    selected: ListView.isCurrentItem
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(-2)
                    }
                    onClicked: {
                        if (listContainer.isExpanded) list.currentIndex = index
                        if (!optionSelector.expanded) listContainer.isExpanded = !listContainer.isExpanded
                    }

                    Image {
                        id: rightImage

                        width: units.gu(2)
                        height: units.gu(2)
                        opacity: enabled ? 1.0 : 0.5
                        visible: option.selected
                        anchors {
                            right: parent.right
                            rightMargin: units.gu(2)
                            verticalCenter: parent.verticalCenter
                        }
                        source: !listContainer.isExpanded && listContainer.height === listContainer.itemHeight ? listContainer.chevron : listContainer.tick

                        ShaderEffect {
                            property color colour: listContainer.themeColour
                            property var source: rightImage

                            width: source.width
                            height: source.height
                            visible: source.status === Image.Ready && listContainer.colourComponent

                            fragmentShader: "
                                    varying highp vec2 qt_TexCoord0;
                                    uniform sampler2D source;
                                    uniform lowp vec4 colour;
                                    uniform lowp float qt_Opacity;

                                    void main() {
                                        lowp vec4 sourceColour = texture2D(source, qt_TexCoord0);
                                        gl_FragColor = colour * sourceColour.a * qt_Opacity;
                                    }"
                        }
                    }

                    ListItem.LabelVisual {
                        text: modelData
                        anchors {
                        left: parent.left
                            leftMargin: units.gu(3)
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
