/*
 * Copyright 2013 Canonical Ltd.
 *
 * This file is part of webbrowser-app.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components.Extras 0.1

TestCase {
    name: "Chrome"

    // TODO: Change this to be a more useful test once there is some actual logic
    function test_enter_url() {
        addressBar.url = "canonical.com"
        signalSpy.signalName = "urlValidated";
        compare(signalSpy.count, 0,"urlValidated() not emitted when changing url")
    }

    Chrome {
        id: addressBar
        SignalSpy {
            id: signalSpy
            target: parent
        }
    }
}
