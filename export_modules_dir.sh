#!/bin/bash
#
# Copyright 2012 - 2015 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

. `dirname ${BASH_SOURCE[0]}`/build_paths.inc || exit 1
export QML_IMPORT_PATH=$BUILD_DIR/qml
export QML2_IMPORT_PATH=$BUILD_DIR/qml
export UBUNTU_UI_TOOLKIT_THEMES_PATH=$BUILD_DIR/qml
export LD_LIBRARY_PATH=$BUILD_DIR/lib
/sbin/initctl set-env --global QML_IMPORT_PATH=$BUILD_DIR/qml
/sbin/initctl set-env --global QML2_IMPORT_PATH=$BUILD_DIR/qml
/sbin/initctl set-env --global UBUNTU_UI_TOOLKIT_THEMES_PATH=$BUILD_DIR/qml
/sbin/initctl set-env --global LD_LIBRARY_PATH=$BUILD_DIR/lib
