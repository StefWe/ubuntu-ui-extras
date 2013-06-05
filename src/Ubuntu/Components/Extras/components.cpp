/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtQuick>
#include <QDir>
#include <QStandardPaths>

#include "components.h"
#include "Browser/history-model.h"
#include "Browser/history-matches-model.h"

void Components::registerTypes(const char *uri)
{
}

void Components::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dataLocation.exists()) {
        QDir::root().mkpath(dataLocation.absolutePath());
    }
    HistoryModel* history = new HistoryModel(dataLocation.filePath("history.sqlite"), this);
    engine->rootContext()->setContextProperty("historyModel", history);
    HistoryMatchesModel* historyMatches = new HistoryMatchesModel(history, this);
    engine->rootContext()->setContextProperty("historyMatches", historyMatches);
}

