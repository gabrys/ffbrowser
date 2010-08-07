
#include "browserwindow.h"

#include <QVBoxLayout>
#include "styleupdater.h"


BrowserWindow::BrowserWindow(QWidget *parent):
    QWidget(parent),
    zoomFactor(1)
{
    
    browserWidget = new BrowserWidget(this);
    menuWidget = new MenuWidget(this);
    progressBar = new QProgressBar(this);

    connect(menuWidget, SIGNAL(backClicked()), browserWidget, SLOT(back()));
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
    connect(browserWidget, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(browserWidget);
    layout->addWidget(menuWidget);
    layout->addWidget(progressBar);
    layout->setSpacing(0);
    layout->setMargin(0);

    setLayout(layout);
    browserWidget->goHome();
    
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

void BrowserWindow::urlChanging(QUrl url) {
    progressBar->setFormat(url.toString());
}

void BrowserWindow::resizeEvent(QResizeEvent *event) {
    updateStyleSheet();
}

void BrowserWindow::updateStyleSheet() {
    browserWidget->evaluateJavaScript(StyleUpdater::styleForWidth(geometry().width() / zoomFactor));
}

void BrowserWindow::loadProgress(int percent) {
    if (percent == 100) {
        progressBar->setValue(0);
        browserWidget->freezeTilesFor(0); // workaround
    } else {
        progressBar->setValue(percent);
    }
}
