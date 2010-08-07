#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QTimer>
#include <QSslCertificate>

#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

class BrowserWidget : public QGraphicsView {
    
    Q_OBJECT
    
public:
    BrowserWidget(QWidget *parent = 0);
    void freezeTilesFor(int msecs);
    void wideEmulationMode(bool wide);
    QVariant evaluateJavaScript(QString source);

public slots:
    void back();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void goHome();
    void loadUrl(QUrl);
    void unfreezeTiles();

signals:
    void canGoBack(bool);
    void loadProgress(int percent);
    void loadingHomePage();
    void unloadingHomePage();
    void zoomChanged(float newZoomLevel);
    void urlChanged(QUrl url);

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

// configuration
    float zoomStepFactor;
    QUrl homePageUrl;
    QUrl errorPageUrl;
    int freezeForMsecsWhenZooming;
    int freezeForMsecsWhenDragging;
    int maxDragDistanceToEmitClick;
    int dragDistance;

// state
    bool wideEmulation;
    QUrl lastUrl;

// methods
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
