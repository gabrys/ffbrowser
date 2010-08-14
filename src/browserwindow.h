#include <QWidget>
#include <QUrl>

#include "browsersettings.h"
#include "browserwidget.h"
#include "menuwidget.h"
#include <QProgressBar>
#include "hotpages.h"
#include "jsproxy.h"
#include <QTimer>

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

class BrowserWindow : public QWidget {
    
    Q_OBJECT
    
public:
    BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

private slots:
    void zoomChanging(float);
    void urlChanging(QUrl);
    void titleChanging(QString);
    void loadProgress(int percent);
    void loadingHomePage();
    void unloadingHomePage();
    void updateStyleSheet();
    void saveSettings();
    void backClicked();
    void disableZoomButtons();
    void enableZoomButtons();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    BrowserSettings *settings;
    BrowserWidget *browserWidget;
    MenuWidget *menuWidget;
    QProgressBar *progressBar;
    HotPages *hotPages;
    JSProxy *jsProxy;
    float zoomFactor;
    QUrl url;
    QTimer saveSettingsTimer;
};

#endif
