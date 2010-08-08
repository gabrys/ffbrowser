#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QTimer>
#include <QSslCertificate>
#include <QSettings>
#include "browserwebpage.h"

#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

class BrowserWidget : public QGraphicsView {
    
    Q_OBJECT
    
public:
    BrowserWidget(QWidget *parent, QSettings *settings, QObject *jsProxy = 0);
    void freezeTilesFor(int msecs);
    void wideEmulationMode(bool wide);
    QVariant evaluateJavaScript(QString source);
    void setJsProxy(QObject *jsProxy);

public slots:
    void back();
    void loadUrl(QUrl);
    void loadUrlTryingHistory(QUrl);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void goHome();
    void unfreezeTiles();

signals:
    void canGoBack(bool);
    void loadProgress(int percent);
    void loadingHomePage();
    void unloadingHomePage();
    void zoomChanged(float);
    void urlChanged(QUrl);
    void titleChanged(QString);

private slots:
    void urlChanging(QUrl url);
    void addJavaScriptBinding();
    void unsupportedContent(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &sslErrors);
    void newScene();

private:
    QGraphicsScene *scene;
    QGraphicsWebView *webView;
    QTimer unfreezeTimer;
    QObject *jsProxy;
    BrowserWebPage *webPage;

// configuration
    float zoomStepFactor;
    QUrl homePageUrl;
    QUrl errorPageUrl;
    int pagesInFastHistory;
    int freezeForMsecsWhenZooming;
    int freezeForMsecsWhenDragging;
    int maxDragDistanceToEmitClick;
    int dragDistance;

// state
    bool wideEmulation;
    QUrl lastUrl;

// methods
    void loadUrlBackend(QUrl url, bool tryHistory);
    void changeZoom(float zoomChangeFactor);
    QList<QSslCertificate> acceptedSslCerts;

protected:
    void resizeEvent(QResizeEvent *event);

// finger scrolling
    QPoint lastMousePos;
    QPoint lastScrollPos;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
// end of finger scrolling

};

#endif
