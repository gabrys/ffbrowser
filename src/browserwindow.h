#include <QWidget>
#include <QUrl>

#include "browserwidget.h"
#include "menuwidget.h"
#include <QProgressBar>

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

class BrowserWindow : public QWidget {
    
    Q_OBJECT
    
public:
    BrowserWindow(QWidget *parent = 0);

private slots:
    void zoomChanging(float newZoom);
    void updateStyleSheet();
    void loadProgress(int percent);
    void loadingHomePage();
    void unloadingHomePage();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    BrowserWidget *browserWidget;
    MenuWidget *menuWidget;
    QProgressBar *progressBar;
    float zoomFactor;
};

#endif
