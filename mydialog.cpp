#include "mydialog.h"

MyDialog::MyDialog(QWidget* parent): QDialog(parent){
    this->setWindowTitle(tr("寻找对手"));
    QVBoxLayout *vbox = new QVBoxLayout;

    QGroupBox *create_or_join = new QGroupBox(this);
    QHBoxLayout *hbox = new QHBoxLayout;
    create_a_room = new QRadioButton(tr("创建房间"));
    join_a_room = new QRadioButton(tr("加入房间"));
    create_a_room->setChecked(true);
    hbox->addWidget(create_a_room);
    hbox->addWidget(join_a_room);
    create_or_join->setLayout(hbox);
    vbox->addWidget(create_or_join);

    QGroupBox *inputs = new QGroupBox(this);
    QGridLayout *grid = new QGridLayout;
    QLabel *ip_label = new QLabel(tr("IP: "), this);
    QLabel *port_label = new QLabel(tr("端口: "), this);

    ip_input = new QLineEdit(this);
    port_input = new QLineEdit(this);
    ip_input->setText("127.0.0.1");
    port_input->setText("8888");

    QRegExp ip_rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExp port_rx("^([0-9]|[1-9]\\d{1,3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    ip_input->setValidator(new QRegExpValidator(ip_rx, this));
    port_input->setValidator(new QRegExpValidator(port_rx, this));
    grid->addWidget(ip_label, 0, 0);
    grid->addWidget(ip_input, 0, 1);
    grid->addWidget(port_label, 1, 0);
    grid->addWidget(port_input, 1, 1);
    inputs->setLayout(grid);
    vbox->addWidget(inputs);

    QPushButton *done_button = new QPushButton(tr("连接"), this);
    vbox->addWidget(done_button);
    connect(done_button, &QPushButton::clicked, this, &MyDialog::setDone);

    this->setLayout(vbox);
}

void MyDialog::setDone(){
    if (create_a_room->isChecked()){
        emit hostInfo(port_input->text());
    }
    else if (join_a_room->isChecked()){
        emit connectInfo(port_input->text(), ip_input->text());
    }
    this->close();
}

