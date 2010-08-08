
#include "browserwidget.h"

#include <QUrl>
#include <QWebSettings>
#include <QScrollBar>
#include <QWebFrame>
#include <QWebHistory>
#include <QMessageBox>
#include <QSslError>
#include <QNetworkReply>
#include <QPalette>

BrowserWidget::BrowserWidget(QWidget *parent, QSettings *settings, QObject *jsProxy):
    QGraphicsView(parent),
    scene(0),
    jsProxy(jsProxy),
    zoomStepFactor              (settings->value("browser/zoom_step_factor").toFloat()),
    homePageUrl                 (settings->value("browser/home_page_url").toString()),
    errorPageUrl                (settings->value("browser/error_url").toString()),
    unsupportedPageUrl          (settings->value("browser/unsupported_url").toString()),
    pagesInFastHistory          (settings->value("browser/pages_in_fast_history").toInt()),
    freezeForMsecsWhenZooming   (settings->value("browser/freeze_time_when_zoom_ms").toInt()),
    freezeForMsecsWhenDragging  (settings->value("browser/freeze_time_when_drag_ms").toInt()),
    maxDragDistanceToEmitClick  (settings->value("browser/click_if_drag_at_most_px").toInt()),
    dragDistance(maxDragDistanceToEmitClick)
{
    
    webView = new QGraphicsWebView();
    webPage = new BrowserWebPage();
    webView->setPage(webPage);
    
    newScene();
    
    // visual settings
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    webView->setResizesToContents(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QPalette pal(palette());
    pal.setColor(QPalette::Base, Qt::white);
    setPalette(pal);

    // web settings
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    globalSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    globalSettings->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);
    globalSettings->setMaximumPagesInCache(pagesInFastHistory);
    webView->page()->history()->setMaximumItemCount(pagesInFastHistory);
    webView->page()->setForwardUnsupportedContent(true);
    
    // connects
    connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanging(QUrl)));
    connect(webView, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(newScene()));
    connect(webView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    connect(webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(unsupportedContent(QNetworkReply*)));
    connect(webView->page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
                                                 this, SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&))
    );
    connect(&unfreezeTimer, SIGNAL(timeout()), this, SLOT(unfreezeTiles()));
    unfreezeTimer.setSingleShot(true);
    
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptBinding()));
    addJavaScriptBinding();
}

void BrowserWidget::back() {
    webView->back();
}

void BrowserWidget::changeZoom(float zoomChangeFactor) {
    freezeTilesFor(freezeForMsecsWhenZooming);
    webView->setScale(webView->scale() * zoomChangeFactor);
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() * zoomChangeFactor);
    verticalScrollBar()->setValue(verticalScrollBar()->value() * zoomChangeFactor);
    emit zoomChanged(webView->scale());
}

void BrowserWidget::zoomIn() {
    changeZoom(zoomStepFactor);
}

void BrowserWidget::zoomOut() {
    changeZoom(1 / zoomStepFactor);
    newScene();
}

void BrowserWidget::newScene() {
    if (scene) {
        scene->deleteLater();
    }
    scene = new QGraphicsScene();
    scene->addItem(webView);
    this->setScene(scene);
}

void BrowserWidget::resetZoom() {
    float scale = webView->scale();
    webView->setScale(1);
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() / scale);
    verticalScrollBar()->setValue(verticalScrollBar()->value() / scale);
    newScene();
    emit zoomChanged(1);
}

void BrowserWidget::goHome() {
    loadUrl(homePageUrl);
}

void BrowserWidget::loadUrlBackend(QUrl url, bool tryFromHistory) {
    if (url == homePageUrl) {
        emit loadingHomePage();
    } else if (tryFromHistory) {
        QWebHistory *history = webView->page()->history();
        for (int i = 0; i < history->count(); i++) {
            if (history->itemAt(i).url() == url) {
                history->goToItem(history->itemAt(i));
                urlChanging(history->itemAt(i).url()); // workaround
                return;
            }
        }
    }
    webView->load(url);
}

void BrowserWidget::loadUrl(QUrl url) {
    loadUrlBackend(url, false);
}

void BrowserWidget::loadUrlTryingHistory(QUrl url) {
    loadUrlBackend(url, true);
}

void BrowserWidget::urlChanging(QUrl url) {
    if (lastUrl == homePageUrl && url != homePageUrl) {
        emit unloadingHomePage();
    }
    
    QList<QWebHistoryItem> backItems = webView->page()->history()->backItems(1);
    if (! backItems.empty() && backItems.last().url() != homePageUrl) {
        emit canGoBack(true);
    } else {
        emit canGoBack(false);
    }
    
    lastUrl = url;
    centerOn(QPoint(0, 0));
    
    emit urlChanged(url);
}

void BrowserWidget::setJsProxy(QObject *newJsProxy) {
    jsProxy = newJsProxy;
    addJavaScriptBinding();
}

void BrowserWidget::addJavaScriptBinding() {
    if (jsProxy) {
        webView->page()->mainFrame()->addToJavaScriptWindowObject("__ffBrowser__", jsProxy);
    }
}

QVariant BrowserWidget::evaluateJavaScript(QString source) {
    return webView->page()->mainFrame()->evaluateJavaScript(source);
}

void BrowserWidget::freezeTilesFor(int msecs) {
    unfreezeTimer.stop();
    webView->setTiledBackingStoreFrozen(true);
    unfreezeTimer.start(msecs);
}

void BrowserWidget::unfreezeTiles() {
    webView->setTiledBackingStoreFrozen(false);
}

void BrowserWidget::unsupportedContent(QNetworkReply *reply) {
    if (! reply) {
        loadUrl(errorPageUrl);
    }

    if (reply->error() == QNetworkReply::NoError) {
        QString url = unsupportedPageUrl.toString();
        url += "?url=" + reply->url().toString();
        QVariant mimeType = reply->header(QNetworkRequest::ContentTypeHeader);
        if (mimeType.isValid()) {
            url += "&mime=" + mimeType.toString();
        }
        loadUrl(url);
        reply->abort();
        return;
    }
    
    loadUrl(errorPageUrl);
}

void BrowserWidget::sslErrors(QNetworkReply *reply, const QList<QSslError> &sslErrors) {
    QSslCertificate sslCert;
    if (sslErrors.count() == 1) {
        sslCert = sslErrors[0].certificate();
        if (! sslCert.isNull() && acceptedSslCerts.contains(sslCert)) {
            reply->ignoreSslErrors();
            return;
        }
    }
    QString msg = "<qt>There is a problem with the site's certificate:<ul>";
    for (int i = 0; i < sslErrors.count(); i++) {
        msg += "<li>" + sslErrors[i].errorString() + "</li>";
    }
    msg += "</ul>Do you want to ignore these errors?</qt>";
    if (QMessageBox::warning(this, "SSL Errors", msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        if (! sslCert.isNull() && ! acceptedSslCerts.contains(sslCert)) {
            acceptedSslCerts.append(QSslCertificate(sslCert));
        }
        reply->ignoreSslErrors();
    }
}

void BrowserWidget::wideEmulationMode(bool wide) {
    wideEmulation = wide;
    if (wide) {
        webView->page()->setPreferredContentsSize(QSize(960, geometry().height()));
    } else {
        webView->page()->setPreferredContentsSize(geometry().size());
    }
}

void BrowserWidget::resizeEvent(QResizeEvent *event) {
    wideEmulationMode(wideEmulation);
    newScene();
}

// finger scrolling
void BrowserWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        lastMousePos = event->pos();
        lastScrollPos = QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
        dragDistance = 0;
    }
}

void BrowserWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        //freezeTilesFor(freezeForMsecsWhenDragging);
        QPoint newScrollPos = lastScrollPos - event->pos() + lastMousePos;
        horizontalScrollBar()->setValue(newScrollPos.x());
        verticalScrollBar()->setValue(newScrollPos.y());
        
        dragDistance += abs(event->pos().x() - lastMousePos.x());
        dragDistance += abs(event->pos().y() - lastMousePos.y());
        
        // emit wheel event to let you scroll overflow:auto divs and similar things
        if (event->pos().y() - lastMousePos.y()) {
            wheelEvent(new QWheelEvent(event->pos(), event->pos().y() - lastMousePos.y(), 0, 0));
        }
        
        lastMousePos = event->pos();
        lastScrollPos = newScrollPos;
    }
}

void BrowserWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (dragDistance < maxDragDistanceToEmitClick) {
        QGraphicsView::mousePressEvent(new QMouseEvent(
            QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier
        ));
        QGraphicsView::mouseReleaseEvent(event);
        dragDistance = maxDragDistanceToEmitClick;
    }
}
// end of finger scrolling
