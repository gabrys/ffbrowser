/*

 settings needed for the browser:

  * browser/zoom_step_factor             - what factor to multiply/divide current zoom when zooming in/out
  * browser/home_page_url                - URL of the home page
  * browser/error_url                    - URL of the error page
  * browser/pages_in_fast_history        - how many pages to keep in fast history cache
  * browser/freeze_time_when_zoom_ms     - how many miliseconds to freeze rendering for right after you change zoom
  * browser/freeze_time_when_drag_ms     - how many miliseconds to freeze rendering for while dragging the webpage
  * browser/click_if_drag_at_most_px     - how many pixels drag is considered a click
  * hotpages/max_number_of_history_items - how many history pages (maximum) keep as Hot Pages
  * hotpages/dont_track_scheme           - scheme to not track as Hot Pages

*/

#include "browsersettings.h"

#include <QUrl>

BrowserSettings::BrowserSettings():
    QSettings("ffBrowser", "ffBrowser")
{
    setDefault("browser/zoom_step_factor", 1.25);
    setDefault("browser/home_page_url", "qrc:/res/home.html");
    setDefault("browser/error_url", "qrc:/res/error.html");
    setDefault("browser/pages_in_fast_history", 5);
    setDefault("browser/freeze_time_when_zoom_ms", 2500);
    setDefault("browser/freeze_time_when_drag_ms", 750);
    setDefault("browser/click_if_drag_at_most_px", 7);
    setDefault("hotpages/max_number_of_history_items", value("browser/pages_in_fast_history").toInt());
    setDefault("hotpages/dont_track_scheme", "qrc");
}

void BrowserSettings::setDefault(QString key, QVariant defaultValue) {
    if (! value(key).isValid()) {
        setValue(key, defaultValue);
    }
}

