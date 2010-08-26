
#include "browsersettings.h"
#include "browserdefaultsettings.h"

BrowserSettings::BrowserSettings():
    QSettings("ffbrowser", "ffbrowser")
{
    BrowserDefaultSettings s("ffbrowser"); // just create & delete
}

