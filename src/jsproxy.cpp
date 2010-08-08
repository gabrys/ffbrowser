
#include "jsproxy.h"

#include <QList>
#include <QUrl>

JSProxy::JSProxy(QObject *parent, HotPages *hotPages):
    QObject(parent),
    hotPages(hotPages)
{
}

QString JSProxy::HotPagesHTML() {
    if (! hotPages) {
        return "";
    }
    
    QString ret = "";
    QString tpl =
        "<a class='hot-page' href='javascript:;'>"
            "<div class='title' onclick=\"HomePage.goToHotPage('%1');\">%2&nbsp;</div>"
            "<div class='url' onclick=\"HomePage.goToHotPage('%1');\">%1&nbsp;</div>"
            "<div class='star %3' onclick=\"HomePage.toggleStar(this, '%1')\"></div>"
        "</a>"
    ;
    
    QList<QUrl> pages = hotPages->getPages();
    for (int i = 0; i < pages.count(); i++) {
        QUrl url = pages[i];
        ret += tpl.arg(
            htmlEscape(url.toString()),
            htmlEscape(hotPages->title(url) == "" ? "<no title>" : hotPages->title(url)),
            hotPages->stared(url) ? "yes" : "no"
        );
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

QString JSProxy::htmlEscape(QString text) {
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
}
