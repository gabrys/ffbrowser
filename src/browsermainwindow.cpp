/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
 * Copyright 2008 Ariya Hidayat <ariya.hidayat@gmail.com>
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

#include "browsermainwindow.h"

#include "aboutdialog.h"
#include "adblockmanager.h"
#include "addbookmarkdialog.h"
#include "autosaver.h"
#include "bookmarksdialog.h"
#include "bookmarksmanager.h"
#include "bookmarksmenu.h"
#include "bookmarksmodel.h"
#include "bookmarkstoolbar.h"
#include "browserapplication.h"
#include "clearprivatedata.h"
#include "downloadmanager.h"
#include "history.h"
#include "languagemanager.h"
#include "networkaccessmanager.h"
#include "opensearchdialog.h"
#include "settings.h"
#include "sourceviewer.h"
#include "tabbar.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "useragentmenu.h"
#include "webview.h"
#include "webviewsearch.h"

#include <qdesktopwidget.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qprintdialog.h>
#include <qprintpreviewdialog.h>
#include <qprinter.h>
#include <qsettings.h>
#include <qtextcodec.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qinputdialog.h>
#include <qsplitter.h>

#include <qurl.h>
#include <qwebframe.h>
#include <qwebhistory.h>

#include <qdebug.h>

BrowserMainWindow::BrowserMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_navigationBar(0)
    , m_navigationSplitter(0)
    , m_toolbarSearch(0)
#if defined(Q_WS_MAC)
    , m_bookmarksToolbarFrame(0)
#endif
    , m_bookmarksToolbar(0)
    , m_tabWidget(new TabWidget(this))
    , m_autoSaver(new AutoSaver(this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setCentralWidget(m_tabWidget);

    /*connect(m_tabWidget, SIGNAL(setCurrentTitle(const QString &)),
            this, SLOT(updateWindowTitle(const QString &)));
    connect(m_tabWidget, SIGNAL(showStatusBarMessage(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(linkHovered(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgress(int)));
    connect(m_tabWidget, SIGNAL(tabsChanged()),
            m_autoSaver, SLOT(changeOccurred()));
    connect(m_tabWidget, SIGNAL(geometryChangeRequested(const QRect &)),
            this, SLOT(geometryChangeRequested(const QRect &)));
    connect(m_tabWidget, SIGNAL(printRequested(QWebFrame *)),
            this, SLOT(printRequested(QWebFrame *)));
    connect(m_tabWidget, SIGNAL(menuBarVisibilityChangeRequested(bool)),
            menuBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(statusBarVisibilityChangeRequested(bool)),
            statusBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_navigationBar, SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_bookmarksToolbar, SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(lastTabClosed()),
            this, SLOT(lastTabClosed()));*/

    updateWindowTitle();
    m_tabWidget->newTab();
    qDebug() << __FUNCTION__;
#if defined(Q_WS_MAC)
    m_navigationBar->setIconSize(QSize(18, 18));
    setWindowIcon(QIcon());
#endif
#if defined(Q_WS_X11)
    setWindowRole(QLatin1String("browser"));
#endif
    qDebug() << __FUNCTION__;
}

BrowserMainWindow::~BrowserMainWindow()
{
    m_autoSaver->changeOccurred();
    m_autoSaver->saveIfNeccessary();
}

void BrowserMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_HomePage:
        m_historyHomeAction->trigger();
        event->accept();
        break;
    case Qt::Key_Favorites:
        m_bookmarksShowAllAction->trigger();
        event->accept();
        break;
    case Qt::Key_Search:
        m_toolsWebSearchAction->trigger();
        event->accept();
        break;
    case Qt::Key_OpenUrl:
        m_fileOpenLocationAction->trigger();
        event->accept();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

BrowserMainWindow *BrowserMainWindow::parentWindow(QWidget *widget)
{
    while (widget) {
        if (BrowserMainWindow *parent = qobject_cast<BrowserMainWindow*>(widget))
            return parent;

        widget = widget->parentWidget();
    }

    qWarning() << "BrowserMainWindow::" << __FUNCTION__ << " used with a widget none of whose parents is a main window.";
    return BrowserApplication::instance()->mainWindow();
}

void BrowserMainWindow::loadDefaultState()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = settings.value(QLatin1String("defaultState")).toByteArray();
    restoreState(data);
    settings.endGroup();
}

QSize BrowserMainWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * 0.9;
    return size;
}

void BrowserMainWindow::save()
{
    BrowserApplication::instance()->saveSession();

    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = saveState(false);
    settings.setValue(QLatin1String("defaultState"), data);
    settings.endGroup();
}

static const qint32 BrowserMainWindowMagic = 0xba;

QByteArray BrowserMainWindow::saveState(bool withTabs) const
{
    int version = 3;
    QByteArray data;
/*
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(BrowserMainWindowMagic);
    stream << qint32(version);

    // save the normal size so exiting fullscreen/maximize will work reasonably
    stream << normalGeometry().size();
    stream << !m_navigationBar->isHidden(); // DEAD
    stream << !m_bookmarksToolbar->isHidden(); // DEAD
    stream << !statusBar()->isHidden();
    if (withTabs)
        stream << tabWidget()->saveState();
    else
        stream << QByteArray();
    stream << m_navigationSplitter->saveState();
    stream << m_tabWidget->tabBar()->showTabBarWhenOneTab();

    stream << qint32(toolBarArea(m_navigationBar));
    stream << qint32(toolBarArea(m_bookmarksToolbar));

    // version 3
    stream << isMaximized();
    stream << isFullScreen();
    stream << menuBar()->isVisible();
    stream << m_menuBarVisible; // DEAD
    stream << m_statusBarVisible;

    stream << QMainWindow::saveState();
*/
    return data;
}

bool BrowserMainWindow::restoreState(const QByteArray &state)
{
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    if (stream.atEnd())
        return false;

    qint32 marker;
    qint32 version;
    stream >> marker;
    stream >> version;
    if (marker != BrowserMainWindowMagic || !(version == 2 || version == 3))
        return false;

    QSize size;
    bool showToolbarDEAD;
    bool showBookmarksBarDEAD;
    bool showStatusbar;
    QByteArray tabState;
    QByteArray splitterState;
    bool showTabBarWhenOneTab;
    qint32 navigationBarLocation;
    qint32 bookmarkBarLocation;
    bool maximized;
    bool fullScreen;
    bool showMenuBar;
    QByteArray qMainWindowState;

    stream >> size;
    stream >> showToolbarDEAD;
    stream >> showBookmarksBarDEAD;
    stream >> showStatusbar;
    stream >> tabState;
    stream >> splitterState;
    stream >> showTabBarWhenOneTab;
    stream >> navigationBarLocation;
    stream >> bookmarkBarLocation;

    if (version >= 3) {
        stream >> maximized;
        stream >> fullScreen;
        stream >> showMenuBar;
        stream >> showMenuBar; // m_menuBarVisible DEAD
        stream >> m_statusBarVisible;
        stream >> qMainWindowState;
    } else {
        maximized = false;
        fullScreen = false;
        showMenuBar = true;
        m_statusBarVisible = showStatusbar;
    }

    if (size.isValid())
        resize(size);

    if (maximized)
        setWindowState(windowState() | Qt::WindowMaximized);
    if (fullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);
        m_viewFullScreenAction->setChecked(true);
    }

    menuBar()->setVisible(showMenuBar);
    m_menuBarVisible = showMenuBar;

    statusBar()->setVisible(showStatusbar);

    m_navigationSplitter->restoreState(splitterState);

    if (qMainWindowState.isEmpty()) {
        m_navigationBar->setVisible(showToolbarDEAD);
        m_bookmarksToolbar->setVisible(showBookmarksBarDEAD);
        Qt::ToolBarArea navigationArea = Qt::ToolBarArea(navigationBarLocation);
        if (navigationArea != Qt::TopToolBarArea && navigationArea != Qt::NoToolBarArea)
            addToolBar(navigationArea, m_navigationBar);
        Qt::ToolBarArea bookmarkArea = Qt::ToolBarArea(bookmarkBarLocation);
        if (bookmarkArea != Qt::TopToolBarArea && bookmarkArea != Qt::NoToolBarArea)
            addToolBar(bookmarkArea, m_bookmarksToolbar);
    } else {
        QMainWindow::restoreState(qMainWindowState);
    }

#if defined(Q_WS_MAC)
    m_bookmarksToolbarFrame->setVisible(m_bookmarksToolbar->isVisible());
#endif

    return true;
}

void BrowserMainWindow::lastTabClosed()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("tabs"));
    bool quit = settings.value(QLatin1String("quitAsLastTabClosed"), true).toBool();

    if (quit)
        close();
    else
        m_tabWidget->makeNewTab(true);
}

QAction *BrowserMainWindow::showMenuBarAction() const
{
    return m_viewShowMenuBarAction;
}

void BrowserMainWindow::setupMenu()
{
}

void BrowserMainWindow::viewTextEncoding(QAction *action)
{
    Q_UNUSED(action);
#if QT_VERSION >= 0x040600 || defined(WEBKIT_TRUNK)
    Q_ASSERT(action);
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    int offset = action->data().toInt();
    if (offset < 0 || offset >= codecs.count())
        QWebSettings::globalSettings()->setDefaultTextEncoding(QString());
    else
        QWebSettings::globalSettings()->setDefaultTextEncoding(QLatin1String(codecs[offset]));
#endif
}

void BrowserMainWindow::retranslate()
{
    m_fileMenu->setTitle(tr("&File"));
    m_fileNewWindowAction->setText(tr("&New Window"));
    m_fileOpenFileAction->setText(tr("&Open File..."));
    m_fileOpenLocationAction->setText(tr("Open &Location..."));
    m_fileSaveAsAction->setText(tr("&Save As..."));
    m_fileImportBookmarksAction->setText(tr("&Import Bookmarks..."));
    m_fileExportBookmarksAction->setText(tr("&Export Bookmarks..."));
    m_filePrintPreviewAction->setText(tr("P&rint Preview..."));
    m_filePrintAction->setText(tr("&Print..."));
    m_filePrivateBrowsingAction->setText(tr("Private &Browsing..."));
    m_fileCloseWindow->setText(tr("Close Window"));
    m_fileQuit->setText(tr("&Quit"));

    m_editMenu->setTitle(tr("&Edit"));
    m_editUndoAction->setText(tr("&Undo"));
    m_editRedoAction->setText(tr("&Redo"));
    m_editCutAction->setText(tr("Cu&t"));
    m_editCopyAction->setText(tr("&Copy"));
    m_editPasteAction->setText(tr("&Paste"));
    m_editSelectAllAction->setText(tr("Select &All"));
    m_editFindAction->setText(tr("&Find"));
    m_editFindNextAction->setText(tr("Find Nex&t"));
    m_editFindPreviousAction->setText(tr("Find P&revious"));

    m_viewMenu->setTitle(tr("&View"));
    m_viewToolbarAction->setShortcut(tr("Ctrl+|"));
    m_viewBookmarkBarAction->setShortcut(tr("Alt+Ctrl+B"));
    m_viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    m_viewShowMenuBarAction->setText(tr("Show Menu Bar"));
    m_viewReloadAction->setText(tr("&Reload Page"));
    m_viewStopAction->setText(tr("&Stop"));
    m_viewZoomInAction->setText(tr("Zoom &In"));
    m_viewZoomNormalAction->setText(tr("Zoom &Normal"));
    m_viewZoomOutAction->setText(tr("Zoom &Out"));
    m_viewZoomTextOnlyAction->setText(tr("Zoom &Text Only"));
    m_viewSourceAction->setText(tr("Page S&ource"));
    m_viewSourceAction->setShortcut(tr("Ctrl+Alt+U"));
    m_viewFullScreenAction->setText(tr("&Full Screen"));
#if QT_VERSION >= 0x040600 || defined(WEBKIT_TRUNK)
    m_viewTextEncodingAction->setText(tr("Text Encoding"));
#endif

    m_historyMenu->setTitle(tr("Hi&story"));
    m_historyBackAction->setText(tr("Back"));
    m_historyForwardAction->setText(tr("Forward"));
    m_historyHomeAction->setText(tr("Home"));
    m_historyRestoreLastSessionAction->setText(tr("Restore Last Session"));

    m_bookmarksMenu->setTitle(tr("&Bookmarks"));
    m_bookmarksShowAllAction->setText(tr("Show All Bookmarks..."));
    m_bookmarksAddAction->setText(tr("Add Bookmark..."));
    m_bookmarksAddFolderAction->setText(tr("Add Folder..."));

    m_windowMenu->setTitle(tr("&Window"));

    m_toolsMenu->setTitle(tr("&Tools"));
    m_toolsWebSearchAction->setText(tr("Web &Search"));
    m_toolsWebSearchAction->setShortcut(QKeySequence(tr("Ctrl+K", "Web Search")));
    m_toolsClearPrivateDataAction->setText(tr("&Clear Private Data"));
    m_toolsClearPrivateDataAction->setShortcut(QKeySequence(tr("Ctrl+Shift+Delete", "Clear Private Data")));
    m_toolsEnableInspectorAction->setText(tr("Enable Web &Inspector"));
    m_toolsPreferencesAction->setText(tr("Options..."));
    m_toolsPreferencesAction->setShortcut(tr("Ctrl+,"));
    m_toolsSearchManagerAction->setText(tr("Configure Search Engines..."));
    m_toolsUserAgentMenu->setTitle(tr("User Agent"));
    m_adBlockDialogAction->setText(tr("&Ad Block..."));

    m_helpMenu->setTitle(tr("&Help"));
    m_helpChangeLanguageAction->setText(tr("Switch application language "));
    m_helpAboutQtAction->setText(tr("About &Qt"));
    m_helpAboutApplicationAction->setText(tr("About &%1", "About Browser").arg(QApplication::applicationName()));

    // Toolbar
    m_navigationBar->setWindowTitle(tr("Navigation"));
    m_bookmarksToolbar->setWindowTitle(tr("&Bookmarks"));

    m_stopReloadAction->setText(tr("Reload / Stop"));
    updateStopReloadActionText(false);
}

void BrowserMainWindow::setupToolBar()
{
}

void BrowserMainWindow::showBookmarksDialog()
{
    BookmarksDialog *dialog = new BookmarksDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString &)),
            m_tabWidget, SLOT(loadUrl(const QUrl&, TabWidget::OpenUrlIn, const QString &)));
    dialog->show();
}

void BrowserMainWindow::addBookmark()
{
    WebView *webView = currentTab();
    QString url = QLatin1String(webView->url().toEncoded());
    QString title = webView->title();

    AddBookmarkDialog dialog;
    dialog.setUrl(url);
    dialog.setTitle(title);
    BookmarkNode *menu = BrowserApplication::bookmarksManager()->menu();
    QModelIndex index = BrowserApplication::bookmarksManager()->bookmarksModel()->index(menu);
    dialog.setCurrentIndex(index);
    dialog.exec();
}

void BrowserMainWindow::addBookmarkFolder()
{
    AddBookmarkDialog dialog;
    BookmarksManager *bookmarksManager = BrowserApplication::bookmarksManager();
    BookmarkNode *menu = bookmarksManager->menu();
    QModelIndex index = bookmarksManager->bookmarksModel()->index(menu);
    dialog.setCurrentIndex(index);
    dialog.setFolder(true);
    dialog.exec();
}

void BrowserMainWindow::viewMenuBar()
{
    menuBar()->setVisible(!menuBar()->isVisible());

    m_menuBarVisible = menuBar()->isVisible();
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::viewToolbar()
{
    if (m_navigationBar->isVisible()) {
        m_navigationBar->close();
    } else {
        m_navigationBar->show();
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::viewBookmarksBar()
{
    if (m_bookmarksToolbar->isVisible()) {
        m_bookmarksToolbar->hide();
#if defined(Q_WS_MAC)
        m_bookmarksToolbarFrame->hide();
#endif
    } else {
        m_bookmarksToolbar->show();
#if defined(Q_WS_MAC)
        m_bookmarksToolbarFrame->show();
#endif
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::viewStatusbar()
{
    if (statusBar()->isVisible()) {
        statusBar()->close();
    } else {
        statusBar()->show();
    }

    m_statusBarVisible = statusBar()->isVisible();

    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::downloadManager()
{
    BrowserApplication::downloadManager()->show();
}

void BrowserMainWindow::fileSaveAs()
{
    BrowserApplication::downloadManager()->download(currentTab()->url(), true);
}

void BrowserMainWindow::preferences()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}

void BrowserMainWindow::updateStatusbar(const QString &string)
{
    statusBar()->showMessage(string, 2000);
}

void BrowserMainWindow::updateWindowTitle(const QString &title)
{
    if (title.isEmpty()) {
        setWindowTitle(QApplication::applicationName());
    } else {
#if defined(Q_WS_MAC)
        setWindowTitle(title);
#else
        setWindowTitle(tr("%1 - Arora", "Page title and Browser name").arg(title));
#endif
    }
}

void BrowserMainWindow::aboutApplication()
{
    AboutDialog *aboutDialog = new AboutDialog(this);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog->show();
}

void BrowserMainWindow::fileNew()
{
    BrowserMainWindow *window = BrowserApplication::instance()->newMainWindow();

    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    int startup = settings.value(QLatin1String("startupBehavior")).toInt();

    if (startup == 0)
        window->goHome();
}

void BrowserMainWindow::fileOpen()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                   tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

    if (file.isEmpty())
        return;

    tabWidget()->loadUrl(QUrl::fromLocalFile(file));
}

void BrowserMainWindow::filePrintPreview()
{
    if (!currentTab())
        return;
    QPrintPreviewDialog dialog(this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter *)),
            currentTab(), SLOT(print(QPrinter *)));
    dialog.exec();
}

void BrowserMainWindow::filePrint()
{
    if (!currentTab())
        return;
    printRequested(currentTab()->page()->mainFrame());
}

void BrowserMainWindow::printRequested(QWebFrame *frame)
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted)
        return;
    frame->print(&printer);
}

void BrowserMainWindow::privateBrowsing()
{
    if (!BrowserApplication::isPrivate()) {
        QString title = tr("Are you sure you want to turn on private browsing?");
        QString text1 = tr("When private browsing is turned on, some actions concerning your privacy will be disabled:");

        QStringList actions;
        actions.append(tr("Webpages are not added to the history."));
        actions.append(tr("Items are automatically removed from the Downloads window."));
        actions.append(tr("New cookies are not stored, current cookies can't be accessed."));
        actions.append(tr("Site icons won't be stored."));
        actions.append(tr("Session won't be saved."));
        actions.append(tr("Searches are not added to the pop-up menu in the search box."));
        actions.append(tr("No new network cache is written to disk."));

        QString text2 = tr("Until you close the window, you can still click the Back and Forward "
                           "buttons to return to the webpages you have opened.");

        QString message = QString(QLatin1String("<b>%1</b><p>%2</p><ul><li>%3</li></ul><p>%4</p>"))
                          .arg(title, text1, actions.join(QLatin1String("</li><li>")), text2);

        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Private Browsing"), message,
                                                                   QMessageBox::Ok | QMessageBox::Cancel,
                                                                   QMessageBox::Ok);
        if (button == QMessageBox::Ok)
            BrowserApplication::setPrivate(true);
        else
            m_filePrivateBrowsingAction->setChecked(false);
    } else {
        BrowserApplication::setPrivate(false);
    }
}

void BrowserMainWindow::zoomTextOnlyChanged(bool textOnly)
{
    m_viewZoomTextOnlyAction->setChecked(textOnly);
}

void BrowserMainWindow::privacyChanged(bool isPrivate)
{
    m_filePrivateBrowsingAction->setChecked(isPrivate);
    if (!isPrivate)
        tabWidget()->clear();
}

void BrowserMainWindow::closeEvent(QCloseEvent *event)
{
    if (!BrowserApplication::instance()->allowToCloseWindow(this)) {
        event->ignore();
        if (m_tabWidget->count() == 0)
            m_tabWidget->newTab();
        return;
    }

    if (m_tabWidget->count() > 1) {
        QSettings settings;
        settings.beginGroup(QLatin1String("tabs"));
        bool confirm = settings.value(QLatin1String("confirmClosingMultipleTabs"), true).toBool();
        if (confirm) {
            QApplication::alert(this);
            int ret = QMessageBox::warning(this, QString(),
                                           tr("Are you sure you want to close the window?"
                                              "  There are %1 tabs open").arg(m_tabWidget->count()),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if (ret == QMessageBox::No) {
                event->ignore();
                return;
            }
        }
    }

    event->accept();
}

void BrowserMainWindow::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::XButton1:
        m_historyBackAction->activate(QAction::Trigger);
        break;
    case Qt::XButton2:
        m_historyForwardAction->activate(QAction::Trigger);
        break;
    default:
        QMainWindow::mousePressEvent(event);
        break;
    }
}

void BrowserMainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QMainWindow::changeEvent(event);
}

void BrowserMainWindow::editFind()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->showFind();
}

void BrowserMainWindow::editFindNext()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->findNext();
}

void BrowserMainWindow::editFindPrevious()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->findPrevious();
}

void BrowserMainWindow::zoomIn()
{
    if (!currentTab())
        return;
    currentTab()->zoomIn();
}

void BrowserMainWindow::zoomNormal()
{
    if (!currentTab())
        return;
    currentTab()->resetZoom();
}

void BrowserMainWindow::zoomOut()
{
    if (!currentTab())
        return;
    currentTab()->zoomOut();
}

void BrowserMainWindow::viewFullScreen(bool makeFullScreen)
{
    if (makeFullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);

        menuBar()->hide();
        statusBar()->hide();
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);

        menuBar()->setVisible(m_menuBarVisible);
        statusBar()->setVisible(m_statusBarVisible);
    }
}

void BrowserMainWindow::viewPageSource()
{
    if (!currentTab())
        return;

    QString title = currentTab()->title();
    QString markup = currentTab()->page()->mainFrame()->toHtml();
    QUrl url = currentTab()->url();
    SourceViewer *viewer = new SourceViewer(markup, title, url);
    viewer->setAttribute(Qt::WA_DeleteOnClose);
    viewer->show();
}

void BrowserMainWindow::goHome()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    QString home = settings.value(QLatin1String("home"), QLatin1String("about:home")).toString();
    tabWidget()->loadString(home);
}

void BrowserMainWindow::webSearch()
{
    m_toolbarSearch->selectAll();
    m_toolbarSearch->setFocus();
}

void BrowserMainWindow::clearPrivateData()
{
    ClearPrivateData dialog;
    dialog.exec();
}

TabWidget *BrowserMainWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserMainWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

ToolbarSearch *BrowserMainWindow::toolbarSearch() const
{
    return m_toolbarSearch;
}

void BrowserMainWindow::updateStopReloadActionText(bool loading)
{
    if (loading) {
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_stopReloadAction->setIconText(tr("Stop"));
    } else {
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_stopReloadAction->setIconText(tr("Reload"));
    }
}

void BrowserMainWindow::loadProgress(int progress)
{
    if (progress < 100 && progress > 0) {
        disconnect(m_stopReloadAction, SIGNAL(triggered()), m_viewReloadAction, SLOT(trigger()));
        m_stopReloadAction->setIcon(m_stopIcon);
        connect(m_stopReloadAction, SIGNAL(triggered()), m_viewStopAction, SLOT(trigger()));
        updateStopReloadActionText(true);
    } else {
        disconnect(m_stopReloadAction, SIGNAL(triggered()), m_viewStopAction, SLOT(trigger()));
        m_stopReloadAction->setIcon(m_reloadIcon);
        connect(m_stopReloadAction, SIGNAL(triggered()), m_viewReloadAction, SLOT(trigger()));
        updateStopReloadActionText(false);
    }
}

void BrowserMainWindow::showSearchDialog()
{
    OpenSearchDialog dialog(this);
    dialog.exec();
}

void BrowserMainWindow::showWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QVariant v = action->data();
        if (v.canConvert<int>()) {
            int offset = qvariant_cast<int>(v);
            QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
            windows.at(offset)->activateWindow();
            windows.at(offset)->raise();
            windows.at(offset)->currentTab()->setFocus();
        }
    }
}

void BrowserMainWindow::openActionUrl(QAction *action)
{
    int offset = action->data().toInt();
    QWebHistory *history = currentTab()->history();
    if (offset < 0)
        history->goToItem(history->backItems(-1*offset).first()); // back
    else if (offset > 0)
        history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
}

void BrowserMainWindow::geometryChangeRequested(const QRect &geometry)
{
    setGeometry(geometry);
}

