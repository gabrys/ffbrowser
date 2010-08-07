#include <QObject>
#include <QUrl>
#include <QList>
#include <QMap>

#ifndef HOTPAGES_H
#define HOTPAGES_H

class HotPages : public QObject {
    
    Q_OBJECT
    
public:
    HotPages(QObject *parent = 0);
    QList<QUrl> getPages();
    QString title(QUrl url);
    bool stared(QUrl url);

public slots:
    void load();
    void dump();
    void add(QUrl url, QString title);
    void star(QUrl url);
    void destar(QUrl url);

private:
    QString storePath;
    int maxNonStaredItems;
    QMap<QUrl,QString> titles;
    QList<QUrl> all;
    QList<QUrl> stars;
    QList<QUrl> nonStars;
    void cleanUp();
};

#endif
