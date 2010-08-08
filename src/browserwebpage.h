#include <QWebPage>

#ifndef BROWSERWEBPAGE_H
#define BROWSERWEBPAGE_H

class BrowserWebPage : public QWebPage {
    
    Q_OBJECT
    
public:
    BrowserWebPage(QObject *parent = 0);

protected:
    QWebPage *createWindow(WebWindowType type);
};

#endif
