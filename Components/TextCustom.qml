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

import QtQuick 1.1
import "fontUtils.js" as FontUtils

/*!
    \qmlclass TextCustom
    \inqmlmodule UbuntuUIToolkit
    \brief The TextCustom class is DOCME

    \b{This component is under heavy development.}

    The TextCustom class is part of the \l{UbuntuUIToolkit} module.
*/
Text {
    color: "black"
//    font.family: "UbuntuBeta"

    /*!
       \preliminary
       DOCME
    */
    property string fontSize: "medium"
    font.pixelSize: FontUtils.sizeToPixels(fontSize)
}
