#include <QObject>
#include "hotpages.h"

#ifndef JSPROXY_H
#define JSPROXY_H

class JSProxy : public QObject {
    
    Q_OBJECT
    
public:
    JSProxy(QObject *parent = 0);
    void setHotPages(HotPages *hotPages);

public slots:
    QString HotPagesHTML();
    void Go(QString url);
    void Star(QString url);
    void Destar(QString url);

signals:
    void go(QUrl url);
    void star(QUrl url);
    void destar(QUrl url);

private:
    HotPages *hotPages;
};

#endif
