
#include "browserwebpage.h"

#include <QDebug>

BrowserWebPage::BrowserWebPage(QObject *parent): QWebPage(parent) {}

QWebPage *BrowserWebPage::createWindow(WebWindowType type) {
    return this;
}
