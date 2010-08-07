
#include "styleupdater.h"

QString StyleUpdater::styleForWidth(int width) {
    return QString(


"__ffbrowser_updateStyleSheet__ = function() {"
"   var style = document.getElementById('arora-mini-style-css');"
"   if (document.getElementsByTagName('head').length) {"
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
"       setTimeout(__ffbrowser_updateStyleSheet__, 250);"
"   }"
"};"
"__ffbrowser_updateStyleSheet__();"


    ).arg(QString(


// white background
"html {"
"   background: #fff;"
"}\\n"

// remove fancy animations
"html, html > body, html > body > *, html > body > * * {"
"    -webkit-transform: none !important;"
"    -webkit-box-shadow: none !important;"
"    -webkit-border-radius: 0 !important;"
"    -webkit-transition-property: none !important;"
"    -webkit-transition: none !important;"
"    opacity: 1 !important;"
"    background-attachment: scroll !important;"
"}\\n"

// assure text does not overflow over screen
"* > p, * > ul, * > ol, * > li, * > td, * > h1, * > h2, * > h3, * > h4, * > h5, * > h6, * > span {"
"    max-width: %3px !important;"
"}\\n"

// mark clicked links aggressively
"* * > a:focus {"
"    background: orange !important;"
"    color: white !important;"
"    outline: orange 20px solid !important;"
"}\\n"

// remove rich-web objects, sorry
"canvas, object {"
"    display: none !important;"
"}\\n"


    )).arg(width).arg(width * 8/10);
}

