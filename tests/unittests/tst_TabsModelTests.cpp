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

// Qt
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlProperty>
#include <QtQuick/QQuickItem>
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

// local
#include "tabs-model.h"

class TabsModelTests : public QObject
{
    Q_OBJECT

private:
    TabsModel* model;

    QQuickItem* createWebView()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        QByteArray data("import QtQuick 2.0\nItem {\nproperty url url\n"
                        "property string title\nproperty url icon\n}");
        component.setData(data, QUrl());
        QObject* object = component.create();
        object->setParent(this);
        QQuickItem* item = qobject_cast<QQuickItem*>(object);
        return item;
    }

private Q_SLOTS:
    void init()
    {
        model = new TabsModel;
    }

    void cleanup()
    {
        delete model;
    }

    void shouldBeInitiallyEmpty()
    {
        QCOMPARE(model->rowCount(), 0);
        QCOMPARE(model->currentIndex(), -1);
        QCOMPARE(model->currentWebview(), (QQuickItem*) 0);
    }

    void shouldNotAllowSettingTheIndexToAnInvalidValue()
    {
        model->setCurrentIndex(0);
        QCOMPARE(model->currentIndex(), -1);
        model->setCurrentIndex(2);
        QCOMPARE(model->currentIndex(), -1);
        model->setCurrentIndex(-2);
        QCOMPARE(model->currentIndex(), -1);
    }

    void shouldNotAddNullWebView()
    {
        QCOMPARE(model->add(0), -1);
        QCOMPARE(model->rowCount(), 0);
    }

    void shouldReturnIndexWhenAddingWebView()
    {
        for(int i = 0; i < 3; ++i) {
            QCOMPARE(model->add(createWebView()), i);
        }
    }

    void shouldUpdateCountWhenAddingWebView()
    {
        QSignalSpy spy(model, SIGNAL(countChanged()));
        model->add(createWebView());
        QCOMPARE(spy.count(), 1);
        QCOMPARE(model->rowCount(), 1);
    }

    void shouldUpdateCountWhenRemovingWebView()
    {
        model->add(createWebView());
        QSignalSpy spy(model, SIGNAL(countChanged()));
        model->remove(0);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(model->rowCount(), 0);
    }

    void shouldNotAllowRemovingAtInvalidIndex()
    {
        QCOMPARE(model->remove(0), (QQuickItem*) 0);
        QCOMPARE(model->remove(2), (QQuickItem*) 0);
        QCOMPARE(model->remove(-2), (QQuickItem*) 0);
    }

    void shouldReturnWebViewWhenRemoving()
    {
        QQuickItem* webview = createWebView();
        model->add(webview);
        QQuickItem* removed = model->remove(0);
        QCOMPARE(removed, webview);
    }

    void shouldNotChangeCurrentIndexWhenAddingWebView()
    {
        QSignalSpy spyIndex(model, SIGNAL(currentIndexChanged()));
        QSignalSpy spyWebview(model, SIGNAL(currentWebviewChanged()));
        model->add(createWebView());
        QCOMPARE(spyIndex.count(), 0);
        QCOMPARE(model->currentIndex(), -1);
        QCOMPARE(spyWebview.count(), 0);
        QCOMPARE(model->currentWebview(), (QQuickItem*) 0);
    }

    void shouldNotDeleteWebViewWhenRemoving()
    {
        QQuickItem* webview = createWebView();
        model->add(webview);
        model->remove(0);
        QCOMPARE(webview->parent(), this);
    }

    void shouldNotifyWhenAddingWebView()
    {
        QSignalSpy spy(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)));
        for(int i = 0; i < 3; ++i) {
            model->add(createWebView());
            QCOMPARE(spy.count(), 1);
            QList<QVariant> args = spy.takeFirst();
            QCOMPARE(args.at(1).toInt(), i);
            QCOMPARE(args.at(2).toInt(), i);
        }
    }

    void shouldNotifyWhenRemovingWebView()
    {
        QSignalSpy spy(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
        for(int i = 0; i < 5; ++i) {
            model->add(createWebView());
        }
        model->remove(3);
        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QCOMPARE(args.at(1).toInt(), 3);
        QCOMPARE(args.at(2).toInt(), 3);
        for(int i = 3; i >= 0; --i) {
            model->remove(i);
            QCOMPARE(spy.count(), 1);
            args = spy.takeFirst();
            QCOMPARE(args.at(1).toInt(), i);
            QCOMPARE(args.at(2).toInt(), i);
        }
    }

    void shouldNotifyWhenWebViewPropertiesChange()
    {
        qRegisterMetaType<QVector<int> >();
        QSignalSpy spy(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
        QQuickItem* webview = createWebView();
        model->add(webview);

        QQmlProperty(webview, "url").write(QUrl("http://ubuntu.com"));
        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QCOMPARE(args.at(0).toModelIndex().row(), 0);
        QCOMPARE(args.at(1).toModelIndex().row(), 0);
        QVector<int> roles = args.at(2).value<QVector<int> >();
        QCOMPARE(roles.size(), 1);
        QVERIFY(roles.contains(TabsModel::Url));

        QQmlProperty(webview, "title").write(QString("Lorem Ipsum"));
        QCOMPARE(spy.count(), 1);
        args = spy.takeFirst();
        QCOMPARE(args.at(0).toModelIndex().row(), 0);
        QCOMPARE(args.at(1).toModelIndex().row(), 0);
        roles = args.at(2).value<QVector<int> >();
        QCOMPARE(roles.size(), 1);
        QVERIFY(roles.contains(TabsModel::Title));

        QQmlProperty(webview, "icon").write(QUrl("image://webicon/123"));
        QCOMPARE(spy.count(), 1);
        args = spy.takeFirst();
        QCOMPARE(args.at(0).toModelIndex().row(), 0);
        QCOMPARE(args.at(1).toModelIndex().row(), 0);
        roles = args.at(2).value<QVector<int> >();
        QCOMPARE(roles.size(), 1);
        QVERIFY(roles.contains(TabsModel::Icon));
    }

    void shouldUpdateCurrentWebviewWhenSettingCurrentIndex()
    {
        QSignalSpy spyIndex(model, SIGNAL(currentIndexChanged()));
        QSignalSpy spyWebview(model, SIGNAL(currentWebviewChanged()));
        QQuickItem* webview = createWebView();
        model->add(webview);
        model->setCurrentIndex(0);
        QCOMPARE(spyIndex.count(), 1);
        QCOMPARE(spyWebview.count(), 1);
        QCOMPARE(model->currentWebview(), webview);
    }

    void shouldUpdateCurrentIndexAndWebviewWhenRemoving()
    {
        QSignalSpy spyIndex(model, SIGNAL(currentIndexChanged()));
        QSignalSpy spyWebview(model, SIGNAL(currentWebviewChanged()));

        // When the current index hasn’t been set,
        // it shouldn’t be updated upon removal.
        model->add(createWebView());
        model->remove(0);
        QCOMPARE(spyIndex.count(), 0);
        QCOMPARE(spyWebview.count(), 0);

        // When removing a webview after the current one,
        // the current index shouldn’t change.
        model->add(createWebView());
        model->add(createWebView());
        model->setCurrentIndex(0);
        spyIndex.clear();
        spyWebview.clear();
        model->remove(1);
        QCOMPARE(model->currentIndex(), 0);
        QCOMPARE(spyIndex.count(), 0);
        QCOMPARE(spyWebview.count(), 0);

        // When removing a webview before the current one,
        // the current index should be updated.
        model->add(createWebView());
        model->setCurrentIndex(1);
        spyIndex.clear();
        spyWebview.clear();
        model->remove(0);
        QCOMPARE(spyIndex.count(), 1);
        QCOMPARE(model->currentIndex(), 0);
        QCOMPARE(spyWebview.count(), 0);

        // When removing the current webview, if there is a webview after it,
        // it becomes the current one, and the current index remains valid.
        QQuickItem* webview1 = createWebView();
        model->add(webview1);
        spyIndex.clear();
        spyWebview.clear();
        model->remove(0);
        QCOMPARE(spyIndex.count(), 0);
        QCOMPARE(spyWebview.count(), 1);
        QCOMPARE(model->currentWebview(), webview1);

        // When removing the current webview, if there was no other webview
        // after it, but there was at least one before it, the current index
        // and current webview should be updated.
        model->add(createWebView());
        model->setCurrentIndex(1);
        spyIndex.clear();
        spyWebview.clear();
        model->remove(1);
        QCOMPARE(spyIndex.count(), 1);
        QCOMPARE(model->currentIndex(), 0);
        QCOMPARE(spyWebview.count(), 1);
        QCOMPARE(model->currentWebview(), webview1);

        // When removing the current webview, if it was the last one, the
        // current index should be reset to -1 and the current webview to 0.
        spyIndex.clear();
        spyWebview.clear();
        model->remove(0);
        QCOMPARE(spyIndex.count(), 1);
        QCOMPARE(model->currentIndex(), -1);
        QCOMPARE(spyWebview.count(), 1);
        QCOMPARE(model->currentWebview(), (QQuickItem*) 0);
    }
};

QTEST_MAIN(TabsModelTests)
#include "tst_TabsModelTests.moc"
