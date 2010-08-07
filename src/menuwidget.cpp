
#include "menuwidget.h"

#include <QHBoxLayout>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent)
{
    
    backButton = new QPushButton(QPixmap(":/res/back.png"), "");
    zoomInButton = new QPushButton(QPixmap(":/res/zoom-in.png"), "");
    zoomOutButton = new QPushButton(QPixmap(":/res/zoom-out.png"), "");
    goHomeButton = new QPushButton(QPixmap(":/res/go-home.png"), "");
    
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(backClicked()));
    connect(zoomInButton, SIGNAL(clicked()), this, SIGNAL(zoomInClicked()));
    connect(zoomOutButton, SIGNAL(clicked()), this, SIGNAL(zoomOutClicked()));
    connect(goHomeButton, SIGNAL(clicked()), this, SIGNAL(goHomeClicked()));

    backButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zoomInButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zoomOutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    goHomeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    backButton->setIconSize(QSize(48, 48));
    zoomInButton->setIconSize(QSize(48, 48));
    zoomOutButton->setIconSize(QSize(48, 48));
    goHomeButton->setIconSize(QSize(48, 48));

    backButton->setFlat(true);
    zoomInButton->setFlat(true);
    zoomOutButton->setFlat(true);
    goHomeButton->setFlat(true);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(backButton);
    layout->addWidget(zoomInButton);
    layout->addWidget(zoomOutButton);
    layout->addWidget(goHomeButton);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize MenuWidget::minimumSizeHint() const {
    return QSize(80, 80);
}

void MenuWidget::setBackButtonEnabled(bool enabled) {
    backButton->setEnabled(enabled);
}

