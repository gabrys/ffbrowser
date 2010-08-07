#include <QWidget>
#include <QUrl>

#include "browserwidget.h"
#include "menuwidget.h"
#include <QProgressBar>
#include "hotpages.h"
#include "jsproxy.h"

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

class BrowserWindow : public QWidget {
    
    Q_OBJECT
    
public:
    BrowserWindow(QWidget *parent = 0);

private slots:
    void zoomChanging(float);
    void urlChanging(QUrl);
    void titleChanging(QString);
    void loadProgress(int percent);
    void loadingHomePage();
    void unloadingHomePage();
    void updateStyleSheet();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    BrowserWidget *browserWidget;
    MenuWidget *menuWidget;
    QProgressBar *progressBar;
    HotPages hotPages;
    JSProxy jsProxy;
    float zoomFactor;
    QUrl url;
};

#endif
