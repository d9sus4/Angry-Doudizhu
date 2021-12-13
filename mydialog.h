#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
#include <QLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>

class MyDialog: public QDialog{

    Q_OBJECT

public:
    MyDialog(QWidget *);

private:
    QLineEdit *ip_input;
    QLineEdit *port_input;
    QRadioButton *create_a_room;
    QRadioButton *join_a_room;

protected slots:
    void setDone();
signals:
    void connectInfo(QString ,QString);
    void hostInfo(QString);

};

#endif // MYDIALOG_H
