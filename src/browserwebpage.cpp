
#include "browserwebpage.h"

BrowserWebPage::BrowserWebPage(QObject *parent): QWebPage(parent) {}

QWebPage *BrowserWebPage::createWindow(WebWindowType type) {
    return this;
}
