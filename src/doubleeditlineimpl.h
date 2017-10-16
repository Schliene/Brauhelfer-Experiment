#ifndef DOUBLEEDITLINEIMPL_H
#define DOUBLEEDITLINEIMPL_H
//
#include "ui_doubleEditLine.h"
#include <QWidget>
//
class doubleEditLineImpl : public QWidget, public Ui::Form {
    Q_OBJECT
public:
    doubleEditLineImpl(QWidget* parent = 0, Qt::WindowFlags f = 0);
private slots:
};
#endif
