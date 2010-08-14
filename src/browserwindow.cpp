
#include "browserwindow.h"

#include <QVBoxLayout>
#include <QFontMetrics>
#include "styleupdater.h"

BrowserWindow::BrowserWindow(QWidget *parent):
    QWidget(parent),
    zoomFactor(1)
{
    settings = new BrowserSettings();
    hotPages = new HotPages(this, settings);
    jsProxy = new JSProxy(this, hotPages);
    browserWidget = new BrowserWidget(this, settings, jsProxy);
    menuWidget = new MenuWidget(this);
    progressBar = new QProgressBar(this);

    connect(menuWidget, SIGNAL(backClicked()), this, SLOT(backClicked()));
    connect(menuWidget, SIGNAL(zoomInClicked()), this, SLOT(disableZoomButtons()));
    connect(menuWidget, SIGNAL(zoomOutClicked()), this, SLOT(disableZoomButtons()));
    connect(menuWidget, SIGNAL(zoomInClicked()), browserWidget, SLOT(zoomIn()));
    connect(menuWidget, SIGNAL(zoomOutClicked()), browserWidget, SLOT(zoomOut()));
    connect(menuWidget, SIGNAL(goHomeClicked()), browserWidget, SLOT(goHome()));

    connect(browserWidget, SIGNAL(canGoBack(bool)), menuWidget, SLOT(setBackButtonEnabled(bool)));
    connect(browserWidget, SIGNAL(loadingHomePage()), this, SLOT(loadingHomePage()));
    connect(browserWidget, SIGNAL(unloadingHomePage()), this, SLOT(unloadingHomePage()));
    connect(browserWidget, SIGNAL(zoomChanged(float)), this, SLOT(zoomChanging(float)));
    connect(browserWidget, SIGNAL(zoomChanged(float)), this, SLOT(updateStyleSheet()));
    connect(browserWidget, SIGNAL(urlChanged(QUrl)), this, SLOT(updateStyleSheet()));
    connect(browserWidget, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanging(QUrl)));
    connect(browserWidget, SIGNAL(titleChanged(QString)), this, SLOT(titleChanging(QString)));
    connect(browserWidget, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));

    connect(jsProxy, SIGNAL(go(QUrl)), browserWidget, SLOT(loadUrlTryingHistory(QUrl)));
    connect(jsProxy, SIGNAL(star(QUrl)), hotPages, SLOT(star(QUrl)));
    connect(jsProxy, SIGNAL(destar(QUrl)), hotPages, SLOT(destar(QUrl)));
    connect(jsProxy, SIGNAL(pressKey(QString)), browserWidget, SLOT(pressKey(QString)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(browserWidget);
    layout->addWidget(menuWidget);
    layout->addWidget(progressBar);
    layout->setSpacing(0);
    layout->setMargin(0);

    setLayout(layout);
    browserWidget->goHome();
    
    // save settings each 100 seconds
    saveSettingsTimer.start(100 * 1000);
    connect(&saveSettingsTimer, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

BrowserWindow::~BrowserWindow() {
    saveSettings();
}

void BrowserWindow::saveSettings() {
    hotPages->dump();
    settings->sync();
}

void BrowserWindow::loadingHomePage() {
    browserWidget->resetZoom();
    browserWidget->wideEmulationMode(false);
    menuWidget->hide();
    progressBar->hide();
}

void BrowserWindow::unloadingHomePage() {
    browserWidget->wideEmulationMode(true);
    menuWidget->show();
    progressBar->show();
}

void BrowserWindow::zoomChanging(float newZoom) {
    zoomFactor = newZoom;
}

void BrowserWindow::urlChanging(QUrl newUrl) {
    url = newUrl;
    QFontMetrics fm = progressBar->fontMetrics();
    progressBar->setFormat(fm.elidedText(url.toString(), Qt::ElideRight, progressBar->width() * 0.8));
}

void BrowserWindow::titleChanging(QString title) {
    hotPages->add(url, title);
}

void BrowserWindow::resizeEvent(QResizeEvent *event) {
    updateStyleSheet();
}

void BrowserWindow::updateStyleSheet() {
    browserWidget->evaluateJavaScript(StyleUpdater::styleForWidth(geometry().width() / zoomFactor));
}

void BrowserWindow::loadProgress(int percent) {
    if (percent == 100) {
        browserWidget->freezeTilesFor(0); // workaround
    }
    progressBar->setValue(percent);
}

void BrowserWindow::backClicked() {
    menuWidget->setBackButtonEnabled(false);
    browserWidget->back();
}

void BrowserWindow::disableZoomButtons() {
    menuWidget->setZoomInButtonEnabled(false);
    menuWidget->setZoomOutButtonEnabled(false);
    QTimer::singleShot(0, this, SLOT(enableZoomButtons()));
}

void BrowserWindow::enableZoomButtons() {
    menuWidget->setZoomInButtonEnabled(true);
    menuWidget->setZoomOutButtonEnabled(true);
}

