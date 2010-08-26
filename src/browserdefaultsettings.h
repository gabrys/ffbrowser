#include <QSettings>

#ifndef BROWSERDEFAULTSETTINGS_H
#define BROWSERDEFAULTSETTINGS_H

class BrowserDefaultSettings : public QSettings {
    
public:
    BrowserDefaultSettings(const QString &organization);
};

#endif
