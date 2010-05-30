/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "browserapplication.h"

#include "cookiejar.h"
#include "networkaccessmanager.h"

#include <qbuffer.h>
#include <qdesktopservices.h>
#include <qdir.h>
#include <qevent.h>
#include <qlibraryinfo.h>
#include <qlocalsocket.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qwebsettings.h>

#include <qdebug.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

NetworkAccessManager *BrowserApplication::s_networkAccessManager = 0;

BrowserApplication::BrowserApplication(int &argc, char **argv)
    : SingleApplication(argc, argv)
    , quitting(false)
{
    QCoreApplication::setOrganizationDomain(QLatin1String("ffbrowser.wikidot.com"));
    QCoreApplication::setApplicationName(QLatin1String("ffbrowser"));
    QCoreApplication::setApplicationVersion(QLatin1String("0.0.1"
#ifdef GITVERSION
    " (Git: " GITCHANGENUMBER " " GITVERSION ")"
#endif
    ));

    connect(this, SIGNAL(messageReceived(QLocalSocket *)),
            this, SLOT(messageReceived(QLocalSocket *)));

    QStringList args = QCoreApplication::arguments();
    if (args.count() > 1) {
        QString message = parseArgumentUrl(args.last());
        sendMessage(message.toUtf8());
    }
    // If we could connect to another Arora then exit
    QString message = QString(QLatin1String("aroramessage://getwinid"));
    if (sendMessage(message.toUtf8(), 500))
        return;

    // not sure what else to do...
    if (!startSingleServer())
        return;

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    // Until QtWebkit defaults to 16
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, 16);
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFixedFontSize, 16);

#if defined(Q_WS_MAC)
    connect(this, SIGNAL(lastWindowClosed()),
            this, SLOT(lastWindowClosed()));
#endif

    // setIconDatabasePath
    QDesktopServices::StandardLocation location;
    location = QDesktopServices::CacheLocation;
    QString directory = QDesktopServices::storageLocation(location);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    QWebSettings::setIconDatabasePath(directory);

    loadSettings();
}

BrowserApplication::~BrowserApplication()
{
    quitting = true;
    delete s_networkAccessManager;
}

#if defined(Q_WS_MAC)
void BrowserApplication::lastWindowClosed()
{
    clean();
    BrowserMainWindow *mw = new BrowserMainWindow;
    mw->goHome();
    m_mainWindows.prepend(mw);
}
#endif

BrowserApplication *BrowserApplication::instance()
{
    return (static_cast<BrowserApplication*>(QCoreApplication::instance()));
}

// The only special property of an argument url is that the file's
// can be local, they don't have to be absolute.
QString BrowserApplication::parseArgumentUrl(const QString &string) const
{
    if (QFile::exists(string)) {
        QFileInfo info(string);
        return info.canonicalFilePath();
    }
    return string;
}

void BrowserApplication::messageReceived(QLocalSocket *socket)
{
    QString message;
    QTextStream stream(socket);
    stream >> message;
#ifdef BROWSERAPPLICATION_DEBUG
    qDebug() << "BrowserApplication::" << __FUNCTION__ << message;
#endif
    if (message.isEmpty())
        return;

    // Got a normal url
    if (!message.startsWith(QLatin1String("ffmessage://"))) {
        QSettings settings;
        settings.beginGroup(QLatin1String("tabs"));
        TabWidget::OpenUrlIn tab = TabWidget::OpenUrlIn(settings.value(QLatin1String("openLinksFromAppsIn"), TabWidget::NewSelectedTab).toInt());
        settings.endGroup();
        if (QUrl(message) == m_lastAskedUrl
                && m_lastAskedUrlDateTime.addSecs(10) > QDateTime::currentDateTime()) {
            qWarning() << "Possible recursive openUrl called, ignoring url:" << m_lastAskedUrl;
            return;
        }
        mainWindow()->tabWidget()->loadString(message, tab);
        return;
    }

    if (message.startsWith(QLatin1String("ffmessage://getwinid"))) {
#ifdef Q_OS_WIN
        QString winid = QString(QLatin1String("%1")).arg((qlonglong)mainWindow()->winId());
#else
        mainWindow()->show();
        mainWindow()->setFocus();
        mainWindow()->raise();
        mainWindow()->activateWindow();
        alert(mainWindow());
        QString winid;
#endif
#ifdef BROWSERAPPLICATION_DEBUG
        qDebug() << "BrowserApplication::" << __FUNCTION__ << "sending win id" << winid << mainWindow()->winId();
#endif
        QString message = QLatin1String("ffmessage://winid/") + winid;
        socket->write(message.toUtf8());
        socket->waitForBytesWritten();
        return;
    }

    if (message.startsWith(QLatin1String("ffmessage://winid"))) {
        QString winid = message.mid(21);
#ifdef BROWSERAPPLICATION_DEBUG
        qDebug() << "BrowserApplication::" << __FUNCTION__ << "got win id:" << winid;
#endif
#ifdef Q_OS_WIN
        WId wid = (WId)winid.toLongLong();
        SetForegroundWindow(wid);
#endif
        return;
    }
}

void BrowserApplication::quitBrowser()
{
    exit(0);
}


#if defined(Q_WS_MAC)
bool BrowserApplication::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::ApplicationActivate: {
        clean();
        if (!m_mainWindows.isEmpty()) {
            BrowserMainWindow *mw = mainWindow();
            if (mw && !mw->isMinimized()) {
                mainWindow()->show();
            }
            return true;
        }
    }
    case QEvent::FileOpen:
        if (!m_mainWindows.isEmpty()) {
            QString file = static_cast<QFileOpenEvent*>(event)->file();
            mainWindow()->tabWidget()->loadUrl(QUrl::fromLocalFile(file));
            return true;
        }
    default:
        break;
    }
    return QApplication::event(event);
}
#endif

void BrowserApplication::askDesktopToOpenUrl(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

BrowserMainWindow *BrowserApplication::mainWindow()
{
    return activeWindow;
}

CookieJar *BrowserApplication::cookieJar()
{
    return (CookieJar*)networkAccessManager()->cookieJar();
}

NetworkAccessManager *BrowserApplication::networkAccessManager()
{
    if (!s_networkAccessManager)
        s_networkAccessManager = new NetworkAccessManager();
    return s_networkAccessManager;
}

