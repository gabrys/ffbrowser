/*

 settings needed for the browser:

  * browser/zoom_step_factor             - what factor to multiply/divide current zoom when zooming in/out
  * browser/home_page_url                - URL of the home page
  * browser/error_url                    - URL of the error page
  * browser/unsupported_url              - URL of the page to load for non-HTML documents
  * browser/pages_in_fast_history        - how many pages to keep in fast history cache
  * browser/freeze_time_when_zoom_ms     - how many miliseconds to freeze rendering for right after you change zoom
  * browser/freeze_time_when_drag_ms     - how many miliseconds to freeze rendering for while dragging the webpage
  * browser/click_if_drag_at_most_px     - how many pixels drag is considered a click
  * hotpages/max_number_of_history_items - how many history pages (maximum) keep as Hot Pages
  * hotpages/dont_track_scheme           - scheme to not track as Hot Pages

 a way to populate first-time bookmarks:

  * hotpages-store/number_of_items = 1
  * item_0_url="URL"
  * item_0_title="Page title"
  * item_0_stared=true

*/

#include "browserdefaultsettings.h"

#include <QUrl>

BrowserDefaultSettings::BrowserDefaultSettings(const QString &organization):
    QSettings(organization)
{
    setValue("--/info", "this file will be overwritten on each start of the browser");
    setValue("browser/zoom_step_factor", 1.40);
    setValue("browser/home_page_url", "qrc:/res/home.html");
    setValue("browser/error_url", "qrc:/res/error.html");
    setValue("browser/unsupported_url", "qrc:/res/unsupported.html");
    setValue("browser/pages_in_fast_history", 5);
    setValue("browser/freeze_time_when_zoom_ms", 500);
    setValue("browser/freeze_time_when_drag_ms", 500);
    setValue("browser/click_if_drag_at_most_px", 10);
    setValue("hotpages/max_number_of_history_items", value("browser/pages_in_fast_history").toInt());
    setValue("hotpages/dont_track_scheme", "qrc");
    setValue("hotpages-store/number_of_items", 1);
    setValue("hotpages-store/item_0_url", "http://ffbrowser.com/howto");
    setValue("hotpages-store/item_0_title", "How to use ffbrowser");
    setValue("hotpages-store/item_0_stared", true);
}

