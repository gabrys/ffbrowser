
#include "jsproxy.h"

#include <QList>
#include <QUrl>

JSProxy::JSProxy(QObject *parent) : QObject(parent) {
    hotPages = 0;
}

void JSProxy::setHotPages(HotPages *newHotPages) {
    hotPages = newHotPages;
}

QString JSProxy::HotPagesHTML() {
    if (! hotPages) {
        return "";
    }
    
    QString ret = "";
    QString tpl =
        "<div class='hot-page'>"
            "<div class='title' onclick=\"HomePage.goToHotPage('%1');\">%2</div>"
            "<div class='url' onclick=\"HomePage.goToHotPage('%1');\">%1</div>"
            "<div class='star %3' onclick=\"HomePage.toggleStar(this, '%1')\"></div>"
        "</div>"
    ;
    
    QList<QUrl> pages = hotPages->getPages();
    for (int i = 0; i < pages.count(); i++) {
        QUrl url = pages[i];
        ret += tpl.arg(url.toString(), hotPages->title(url), hotPages->stared(url) ? "yes" : "no");
    }
    return ret;
}

void JSProxy::Go(QString url) {
    emit go(QUrl(url));
}

void JSProxy::Star(QString url) {
    emit star(QUrl(url));
}

void JSProxy::Destar(QString url) {
    emit destar(QUrl(url));
}

