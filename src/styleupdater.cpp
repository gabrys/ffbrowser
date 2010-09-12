
#include "styleupdater.h"

QString StyleUpdater::styleForWidth(int width) {
    return QString(


"__ffbrowser_updateStyleSheet__ = function() {"
"   if (document.getElementsByTagName('head').length) {"
"       var style = document.getElementById('arora-mini-style-css');"
"       if (! style) {"
"           style = document.createElement('style');"
"           style.type = 'text/css';"
"           style.id = 'arora-mini-style-css';"
"           style.className = 'width-%2';"
"           style.innerHTML = '%1';"
"           document.getElementsByTagName('head')[0].appendChild(style);"
"       } else if (style.className != 'width-%2') {"
"           style.innerHTML = '%1';"
"           style.className = 'width-%2';"
"       }"
"   } else {"
"       document.addEventListener('DOMContentLoaded', __ffbrowser_updateStyleSheet__, false);"
"   }"
"};"
"__ffbrowser_updateStyleSheet__();"


    ).arg(QString(


// white background
"html {"
"   background-color: #fff;"
"}\\n"

// assure text does not overflow over screen
"p, ul, ol, li, td, "
"h1, h2, h3, h4, h5, h6, "
"span, dd, dt, label, "
"input, textarea {"
"    max-width: %3px !important;"
"}\\n"

// mark clicked links aggressively
"a:focus {"
"    background: orange !important;"
"    color: white !important;"
"    outline: orange 20px solid !important;"
"}\\n"

// remove rich-web objects, sorry
"canvas, object {"
"    display: none !important;"
"}\\n"


    )).arg(width).arg(width * 9/10);
}

