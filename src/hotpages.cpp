
#include "hotpages.h"

HotPages::HotPages(QObject *parent, QSettings *settings):
    QObject(parent),
    settings(settings)
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
    if (url.scheme() == settings->value("hotpages/dont_track_scheme")) {
        return;
    }
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
    
    settings->remove("hotpages-store");
    settings->setValue("hotpages-store/number_of_items", all.count());
    QString tpl = "hotpages-store/item_%1_%2";
    for (int i = 0; i < all.count(); i++) {
        QUrl url = all[i];
        settings->setValue(tpl.arg(i).arg("url"), url.toString());
        settings->setValue(tpl.arg(i).arg("title"), title(url));
        settings->setValue(tpl.arg(i).arg("stared"), stared(url));
    }
}

void HotPages::load() {
    maxNonStaredItems = settings->value("hotpages/max_number_of_history_items").toInt();

    int count = settings->value("hotpages-store/number_of_items", 0).toInt();
    QString tpl = "hotpages-store/item_%1_%2";
    for (int i = count - 1; i >= 0; i--) {
        QUrl url = settings->value(tpl.arg(i).arg("url")).toUrl();
        add(url, settings->value(tpl.arg(i).arg("title")).toString());
        if (settings->value(tpl.arg(i).arg("stared")).toBool()) {
            star(url);
        }
    }
}

