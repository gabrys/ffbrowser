#include <QSettings>

#ifndef BROWSERSETTINGS_H
#define BROWSERSETTINGS_H

class BrowserSettings : public QSettings {
    
public:
    BrowserSettings();
    void setDefault(QString key, QVariant defaultValue);
};

#endif
