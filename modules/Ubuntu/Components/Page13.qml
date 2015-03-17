/*
 * Copyright 2012-2015 Canonical Ltd.
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

import QtQuick 2.4
//import Ubuntu.Components 1.2 as Toolkit

/*!
  \internal
  Documentation is in Page.qdoc
*/
PageTreeNode {
    id: page
    anchors {
        left: parent ? parent.left : undefined
        bottom: parent ? parent.bottom : undefined
    }
    // Set width and height so that a parent Loader can be automatically resized
    // to the size of the loaded Page.
    width: parentNode ? parentNode.width - page.x : undefined
    height: parentNode ? page.flickable ? parentNode.height : parentNode.height - internal.headerHeight : undefined

    isLeaf: true

    property string title: parentNode && parentNode.hasOwnProperty("title") ? parentNode.title : ""

    property Flickable flickable: internal.getFlickableChild(page)

    Object {
        id: internal

        property AppHeader header: page.__propagated && page.__propagated.header ? page.__propagated.header : null
        // Used to position the Page when there is no flickable.
        // When there is a flickable, the header will automatically position it.
        property real headerHeight: internal.header && internal.header.visible ? internal.header.height : 0

        function isVerticalFlickable(object) {
            if (object && object.hasOwnProperty("flickableDirection") && object.hasOwnProperty("contentHeight")) {
                var direction = object.flickableDirection;
                if ( ((direction === Flickable.AutoFlickDirection) && (object.contentHeight !== object.height) )
                        || direction === Flickable.VerticalFlick
                        || direction === Flickable.HorizontalAndVerticalFlick) {
                    return true;
                }
            }
            return false;
        }

        /*!
          Return the first flickable child of this page.
         */
        function getFlickableChild(item) {
            if (item && item.hasOwnProperty("children")) {
                for (var i=0; i < item.children.length; i++) {
                    var child = item.children[i];
                    if (internal.isVerticalFlickable(child)) {
                        if (child.anchors.top === page.top || child.anchors.fill === page) {
                            return item.children[i];
                        }
                    }
                }
            }
            return null;
        }
    }
}
