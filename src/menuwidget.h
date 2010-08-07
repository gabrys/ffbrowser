#include <QWidget>
#include <QPushButton>

#ifndef MENUWIDGET_H
#define MENUWIDGET_H

class MenuWidget : public QWidget {
    
    Q_OBJECT
    
public:
    MenuWidget(QWidget *parent = 0);
    QSize minimumSizeHint() const;

signals:
    void backClicked();
    void zoomInClicked();
    void zoomOutClicked();
    void goHomeClicked();

public slots:
    void setBackButtonEnabled(bool enabled);

private:
    QPushButton *backButton;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QPushButton *goHomeButton;

};

#endif
