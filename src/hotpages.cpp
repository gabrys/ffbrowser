
#include "hotpages.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

HotPages::HotPages(QObject *parent):
    QObject(parent),
    storePath("/tmp/ffbrowser-hotpages"),
    maxNonStaredItems(10)
{
    load();
}

QList<QUrl> HotPages::getPages() {
    return all;
}

QString HotPages::title(QUrl url) {
    return titles.value(url, QString(""));
}

bool HotPages::stared(QUrl url) {
    return stars.contains(url);
}

void HotPages::add(QUrl url, QString title) {
    titles.insert(url, title);
    if (all.contains(url)) {
        all.removeAt(all.indexOf(url));
    }
    all.prepend(url);
    if (! stars.contains(url)) {
        if (nonStars.contains(url)) {
            nonStars.removeAt(nonStars.indexOf(url));
        }
        nonStars.prepend(url);
    }
    cleanUp();
}

void HotPages::star(QUrl url) {
    if (nonStars.contains(url)) {
        stars.prepend(url);
        nonStars.removeAt(nonStars.indexOf(url));
    }
    // move it to the beginning in the global list
    if (all.contains(url)) {
        all.removeAt(all.indexOf(url));
        all.prepend(url);
    }
}

void HotPages::destar(QUrl url) {
    if (stars.contains(url)) {
        nonStars.prepend(url);
        stars.removeAt(stars.indexOf(url));
    }
}

void HotPages::cleanUp() {
    // remove last non stared elements
    while (nonStars.count() > maxNonStaredItems) {
        QUrl url = nonStars.last();
        all.removeAt(all.indexOf(url));
        nonStars.removeLast();
        titles.remove(url);
    }
}

void HotPages::dump() {
    cleanUp();
    QFile file(storePath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream stream(&file);
    for (int i = 0; i < all.count(); i++) {
        QUrl url = all[i];
        stream << url.toString() << "\n";
        stream << title(url) << "\n";
        stream << stared(url) << "\n\n";
    }
}

void HotPages::load() {
    QFile file(storePath);
    if (! file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream stream(&file);
    QUrl url;
    QString title;
    int stared;
    while (! stream.atEnd()) {
        url = QUrl(stream.readLine());
        title = stream.readLine();
        stream >> stared;
        stream.readLine(); // empty line
        stream.readLine(); // empty line
        add(url, title);
        if (stared) {
            star(url);
        }
    }
}

