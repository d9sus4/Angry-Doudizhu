#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    this->setWindowTitle(tr("生气斗地主"));

    dialog = new MyDialog(this);

    connect(dialog, &MyDialog::hostInfo, this, &MainWindow::createServer);
    connect(dialog, &MyDialog::connectInfo, this, &MainWindow::joinServer);
    //buttons
    start_button = new QPushButton(tr("一键破产"), this);
    start_button->setGeometry(350, 285, 100, 30);
    start_button->setVisible(true);
    start_button->setEnabled(true);
    connect(start_button, &QPushButton::clicked, this, &MainWindow::showDialog);

    deal_button = new QPushButton(tr("发牌"), this);
    deal_button->setGeometry(350, 435, 100, 30);
    deal_button->setVisible(false);
    deal_button->setEnabled(false);
    connect(deal_button, &QPushButton::clicked, this, &MainWindow::dealButtonClicked);

    seat_button = new QPushButton(tr("坐下"), this);
    seat_button->setGeometry(350, 435, 100, 30);
    seat_button->setVisible(false);
    seat_button->setEnabled(false);
    connect(seat_button, &QPushButton::clicked, this, &MainWindow::seatButtonClicked);

    jiaodizhu_button = new QPushButton(tr("叫地主"), this);
    jiaodizhu_button->setGeometry(290, 435, 100, 30);
    jiaodizhu_button->setVisible(false);
    jiaodizhu_button->setEnabled(false);
    connect(jiaodizhu_button, &QPushButton::clicked, this, &MainWindow::jiaodizhuButtonClicked);

    bujiao_button = new QPushButton(tr("不叫"), this);
    bujiao_button->setGeometry(410, 435, 100, 30);
    bujiao_button->setVisible(false);
    bujiao_button->setEnabled(false);
    connect(bujiao_button, &QPushButton::clicked, this, &MainWindow::bujiaoButtonClicked);

    play_button = new QPushButton(tr("出牌"), this);
    play_button->setGeometry(290, 435, 100, 30);
    play_button->setEnabled(false);
    play_button->setVisible(false);
    connect(play_button, &QPushButton::clicked, this, &MainWindow::playButtonClicked);

    pass_button = new QPushButton(tr("不出"), this);
    pass_button->setGeometry(410, 435, 100, 30);
    pass_button->setEnabled(false);
    pass_button->setVisible(false);
    connect(pass_button, &QPushButton::clicked, this, &MainWindow::passButtonClicked);

    //player images
    for (int i = 0; i <= 2; ++i){
        player_image[i] = new QLabel(this);
        player_image[i]->setStyleSheet("border:1px solid grey;");
        player_image[i]->setScaledContents(true);
        player_image[i]->setVisible(false);
    }
    player_image[0]->setGeometry(710, 450, 80, 100);
    player_image[1]->setGeometry(710, 150, 80, 100);
    player_image[2]->setGeometry(10, 150, 80, 100);

    //state labels
    for (int i = 0; i <= 2; ++i){
        player_hands_number_icon[i] = new QLabel(this);
        player_hands_number_icon[i]->setPixmap(CardLabel::getPixmap(-1));
        player_hands_number_icon[i]->setScaledContents(true);
        player_hands_number_icon[i]->setVisible(false);
    }
    player_hands_number_icon[0] -> setGeometry(730, 560, 20, 30);
    player_hands_number_icon[1] -> setGeometry(730, 260, 20, 30);
    player_hands_number_icon[2] -> setGeometry(30, 260, 20, 30);

    for (int i = 0; i <= 2; ++i){
        player_hands_number_label[i] = new QLabel(this);
        player_hands_number_label[i]->setVisible(false);
    }
    player_hands_number_label[0]->setGeometry(760, 570, 20, 20);
    player_hands_number_label[1]->setGeometry(760, 270, 20, 20);
    player_hands_number_label[2]->setGeometry(60, 270, 20, 20);

    for (int i = 0; i <= 2; ++i){
        player_seated_label[i] = new QLabel(this);
        player_seated_label[i]->setText(tr("坐下了"));
        player_seated_label[i]->setVisible(false);
    }
    player_seated_label[0]->setGeometry(725, 415, 50, 20);
    player_seated_label[1]->setGeometry(725, 115, 50, 20);
    player_seated_label[2]->setGeometry(25, 115, 50, 20);

    for (int i = 0; i <= 2; ++i){
        player_jiaodizhu_label[i] = new QLabel(this);
        player_jiaodizhu_label[i]->setVisible(false);
    }
    player_jiaodizhu_label[0]->setGeometry(725, 415, 50, 20);
    player_jiaodizhu_label[1]->setGeometry(725, 115, 50, 20);
    player_jiaodizhu_label[2]->setGeometry(25, 115, 50, 20);

    for (int i = 0; i <= 2; ++i){
        player_yaobuqi_label[i] = new QLabel(this);
        player_yaobuqi_label[i]->setText(tr("要不起"));
        player_yaobuqi_label[i]->setVisible(false);
    }
    player_yaobuqi_label[0]->setGeometry(375, 380, 50, 20);
    player_yaobuqi_label[1]->setGeometry(600, 150, 50, 20);
    player_yaobuqi_label[2]->setGeometry(150, 150, 50, 20);

    for (int i = 0; i <= 2; ++i){
        player_identity_label[i] = new QLabel(this);
        player_identity_label[i]->setVisible(false);
    }
    player_identity_label[0]->setGeometry(725, 415, 50, 20);
    player_identity_label[1]->setGeometry(725, 115, 50, 20);
    player_identity_label[2]->setGeometry(25, 115, 50, 20);

    played_illegally_label = new QLabel(this);
    played_illegally_label->setGeometry(340, 275, 120, 30);
    played_illegally_label->setVisible(false);
    played_illegally_label->setText(tr("阿姨你会不会玩啊"));

    for (int i = 0; i < 3; ++i){
        bottom_labels[i] = new CardLabel(-1, this);
        bottom_labels[i]->setGeometry(305 + 65 * i, 10, 60, 90);
        bottom_labels[i]->setVisible(false);
    }

    nongmin_image.load(":/icons/images/nongmin.jpg");
    dizhu_image.load(":/icons/images/dizhu.jpg");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialog(){
    dialog->show();
}

void MainWindow::createServer(QString port){
    //qDebug() << "creating a server" << Qt::endl;
    server = new ServerThread(port, this);
    server->start();
    is_host = true;
    server_socket = new QTcpSocket(this);
    server_socket->connectToHost("127.0.0.1", port.toInt());
    if (server_socket->waitForConnected(1000)){
        connect(server_socket, SIGNAL(readyRead()), this, SLOT(readFromServer()));
        is_host = true;
    }
}

void MainWindow::joinServer(QString port, QString ip){
    //qDebug() << "connecting to a server" << Qt::endl;
    server_socket = new QTcpSocket(this);
    server_socket->connectToHost(ip, port.toInt());
    if (server_socket->waitForConnected(1000)){
        connect(server_socket, SIGNAL(readyRead()), this, SLOT(readFromServer()));
        connect(server_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readError(QAbstractSocket::SocketError)));
    }
    else {
        server_socket->deleteLater();
        //qDebug() << "connect failed" << Qt::endl;
    }
}

void MainWindow::readFromServer(){
    while (server_socket->canReadLine()){
        QTextStream ts(server_socket->readLine());
        QString keyword;
        ts >> keyword;

        if (keyword == "you're"){
            ts >> player_number;
        }

        else if (keyword == "init"){
            clearScene();
            initScene();
        }

        else if (keyword == "joined"){
            int player;
            ts >> player;
            player_image[getPosition(player)]->setPixmap(nongmin_image);
        }

        else if (keyword == "seated"){
            int player;
            ts >> player;
            player_seated_label[getPosition(player)]->setVisible(true);
        }

        else if (keyword == "stoodup"){
            int player;
            ts >> player;
            player_seated_label[getPosition(player)]->setVisible(false);
        }

        else if (keyword == "left"){
            int player;
            ts >> player;
            player_image[getPosition(player)]->setPixmap(QPixmap());
            player_seated_label[getPosition(player)]->setVisible(false);
        }

        else if (keyword == "dealable"){
            deal_button->setEnabled(true);
        }

        else if (keyword == "wait"){
            deal_button->setEnabled(false);
        }

        else if (keyword == "dealed"){
            int temp;
            for (int i = 0; i < 17; ++i){
                ts >> temp;
                player_hands_array.append(temp);
            }
            for (int i = 0; i < 3; ++i){
                bottom_labels[i]->setVisible(true);
            }
            for (int i = 0; i < 3; ++i){
                player_seated_label[i]->setVisible(false);
                player_hands_number[i] = 17;
                player_hands_number_icon[i]->setVisible(true);
                player_hands_number_label[i]->setText("17");
                player_hands_number_label[i]->setVisible(true);
            }
            if (is_host){
                deal_button->setEnabled(false);
                deal_button->setVisible(false);
            }
            else {
                seat_button->setVisible(false);
                seat_button->setText(tr("坐下"));
            }
            jiaodizhu_button->setVisible(true);
            bujiao_button->setVisible(true);
            updateHandsLabels();
        }

        else if (keyword == "decide"){

            jiaodizhu_button->setEnabled(true);
            bujiao_button->setEnabled(true);
        }

        else if (keyword == "jiaole"){
            int temp;
            ts >> temp;
            player_jiaodizhu_label[getPosition(temp)]->setText(tr("叫地主"));
            player_jiaodizhu_label[getPosition(temp)]->setVisible(true);
        }

        else if (keyword == "meijiao"){
            int temp;
            ts >> temp;
            player_jiaodizhu_label[getPosition(temp)]->setText(tr("不叫"));
            player_jiaodizhu_label[getPosition(temp)]->setVisible(true);
        }

        else if (keyword == "congrats"){
            int temp;
            int temp0, temp1, temp2;
            ts >> temp >> temp0 >> temp1 >> temp2;
            player_image[getPosition(temp)]->setPixmap(dizhu_image);
            bottom_labels[0]->setPixmap(CardLabel::getPixmap(temp0));
            bottom_labels[1]->setPixmap(CardLabel::getPixmap(temp1));
            bottom_labels[2]->setPixmap(CardLabel::getPixmap(temp2));
            player_hands_number[getPosition(temp)] = 20;
            player_hands_number_label[getPosition(temp)]->setText("20");
            if (player_number == temp){
                player_hands_array.append(temp0);
                player_hands_array.append(temp1);
                player_hands_array.append(temp2);
                std::sort(player_hands_array.begin(), player_hands_array.end(), std::greater<int>());
                updateHandsLabels();
            }
            jiaodizhu_button->setVisible(false);
            bujiao_button->setVisible(false);
            for (int i: {0, 1, 2}){
                player_jiaodizhu_label[i]->setVisible(false);
                player_identity_label[i]->setText(tr("农民"));
                player_identity_label[i]->setVisible(true);
            }
            player_identity_label[getPosition(temp)]->setText(tr("地主"));
            play_button->setVisible(true);
            pass_button->setVisible(true);
        }

        else if (keyword == "it's"){
            int temp;
            ts >> temp;
            player_yaobuqi_label[getPosition(temp)]->setVisible(false);
            player_played_array[getPosition(temp)].clear();
            updatePlayedLabels(getPosition(temp));
            if (temp == player_number){
                HandLabel::isMyTurn(true);
                play_button->setEnabled(true);
            }
        }

        else if (keyword == "passable"){
            pass_button->setEnabled(true);
        }

        else if (keyword == "legal"){
            for (auto i: player_hands_labels){
                if (i->isUp()){
                    player_hands_array.erase(std::find(player_hands_array.begin(), player_hands_array.end(), i->getCardId()));
                }
            }
            updateHandsLabels();
            play_button->setEnabled(false);
            if (pass_button->isEnabled()){
                pass_button->setEnabled(false);
            }
            HandLabel::isMyTurn(false);
        }

        else if (keyword == "illegal"){
            played_illegally_label->setVisible(true);
        }

        else if (keyword == "played"){
            int p, size, temp;
            ts >> p >> size;
            player_hands_number[getPosition(p)] -= size;
            while (size--){
                ts >> temp;
                player_played_array[getPosition(p)].append(temp);
            }
            updatePlayedLabels(getPosition(p));
            player_hands_number_label[getPosition(p)]->setText(QString::number(player_hands_number[getPosition(p)]));
        }

        else if (keyword == "passed"){
            int p;
            ts >> p;
            player_yaobuqi_label[getPosition(p)]->setVisible(true);
        }

        else if (keyword == "gameover"){
            QString won_or_lost;
            ts >> won_or_lost;
            QMessageBox msgBox;
            if (won_or_lost == "won") {msgBox.setText("全体起立!");}
            else {msgBox.setText("17张牌你能秒我...");}
            msgBox.setInformativeText("再来一局?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int choice = msgBox.exec();
            if(choice == QMessageBox::Ok){
                sendToServer("stay");
            }
            else{
                server_socket->disconnectFromHost();
                server_socket->deleteLater();
                clearScene();
            }
        }

        ts.flush();
    }
}

void MainWindow::readError(QAbstractSocket::SocketError){
    server_socket->disconnectFromHost();
    server_socket->deleteLater();
    QMessageBox msgBox;
    msgBox.setText("连接中断");
    msgBox.setInformativeText(tr("Error string: %1").arg(server_socket->errorString()));
    msgBox.exec();
    clearScene();
}

void MainWindow::initScene(){
    start_button->setVisible(false);
    start_button->setEnabled(false);

    if (is_host){
        deal_button->setVisible(true);
        deal_button->setEnabled(false);
    }

    else{
        seat_button->setVisible(true);
        seat_button->setEnabled(true);
    }

    for (int i = 0; i <= 2; ++i){
        player_image[i]->setVisible(true);
    }

    if (is_host) {seatButtonClicked();}
}

void MainWindow::clearScene(){ //回到主界面并清空所有游戏数据
    //buttons
    start_button->setVisible(true);
    start_button->setEnabled(true);

    deal_button->setVisible(false);
    deal_button->setEnabled(false);

    seat_button->setVisible(false);
    seat_button->setEnabled(false);

    jiaodizhu_button->setVisible(false);
    jiaodizhu_button->setEnabled(false);

    bujiao_button->setVisible(false);
    bujiao_button->setEnabled(false);

    play_button->setEnabled(false);
    play_button->setVisible(false);

    pass_button->setEnabled(false);
    pass_button->setVisible(false);

    for (int i = 0; i <= 2; ++i){
        player_image[i]->setVisible(false);
        player_image[i]->setPixmap(QPixmap());
        player_hands_number_icon[i]->setVisible(false);
        player_hands_number_label[i]->setVisible(false);
        player_seated_label[i]->setVisible(false);
        player_jiaodizhu_label[i]->setVisible(false);
        player_yaobuqi_label[i]->setVisible(false);
        player_identity_label[i]->setVisible(false);
    }

    for (int i = 0; i < 3; ++i){
        bottom_labels[i]->setVisible(false);
    }

    for (int i = 0; i < 3; ++i){
        player_played_array[i].clear();
        updatePlayedLabels(i);
        player_hands_array.clear();
        updateHandsLabels();
    }

    played_illegally_label->setVisible(false);
}

void MainWindow::updateHandsLabels(){
    for (auto hand: player_hands_labels){
        delete hand;
    }
    player_hands_labels.clear();
    if (player_hands_array.empty()){
        return;
    }
    for (int i: player_hands_array){
        player_hands_labels.append(new HandLabel(i, this));
    }
    int size = player_hands_labels.size();
    int start_x = 400; //中轴线
    if ((size % 2) == 0){
        start_x += 10;
    }
    start_x -= (size / 2) * 20; //找到最左那张的中轴线
    start_x -= 20; //找到最左那张的左边缘
    for (int i = 0; i < size; ++i){
        player_hands_labels.at(i)->setGeometry(start_x + i * 20, 500, 60, 90);
        player_hands_labels.at(i)->setVisible(true);
    }
    player_hands_labels.last()->setUncovered();
}

void MainWindow::updatePlayedLabels(int pos){
    for (auto card: player_played_labels[pos]){
        delete card;
    }
    player_played_labels[pos].clear();
    for (int i: player_played_array[pos]){
        player_played_labels[pos].append(new CardLabel(i, this));
    }
    switch (pos) {
    case 0:{
        int start_x = 400;
        int size = player_played_labels[0].size();
        if ((size % 2) == 0){
                start_x += 10;
            }
        start_x -= (size / 2) * 20; //找到最左那张的中轴线
        start_x -= 20; //找到最左那张的左边缘
        for (int i = 0; i < size; ++i){
            player_played_labels[0].at(i)->setGeometry(start_x + i * 20, 305, 60, 90);
            player_played_labels[0].at(i)->setVisible(true);
        }
        break;
    }
    case 1:{
        int start_x = 480, start_y = 140;
        int size = player_played_labels[1].size();
        for (int i = 0; i < size; ++i){
            player_played_labels[1].at(i)->setGeometry(start_x + (i % 11) * 20, start_y + (i / 11) * 30, 60, 90);
            player_played_labels[1].at(i)->setVisible(true);
        }
        break;
    }
    case 2:{
        int start_x = 110, start_y = 140;
        int size = player_played_labels[2].size();
        for (int i = 0; i < size; ++i){
            player_played_labels[2].at(i)->setGeometry(start_x + (i % 11) * 20, start_y + (i / 11) * 30, 60, 90);
            player_played_labels[2].at(i)->setVisible(true);
        }
        break;
    }
    }
}

void MainWindow::seatButtonClicked(){
    if (is_host){
        sendToServer("seat");
        return;
    }
    if (player_seated_label[0]->isVisible()){
        sendToServer("standup");
        seat_button->setText(tr("坐下"));
    }
    else{
        sendToServer("seat");
        seat_button->setText(tr("起立"));
    }
}

void MainWindow::dealButtonClicked(){
    sendToServer("deal");
}

void MainWindow::jiaodizhuButtonClicked(){
    sendToServer("jiaodizhu");
    jiaodizhu_button->setEnabled(false);
    bujiao_button->setEnabled(false);
}

void MainWindow::bujiaoButtonClicked(){
    sendToServer("bujiao");
}

void MainWindow::passButtonClicked(){
    played_illegally_label->setVisible(false);
    sendToServer("pass");
    play_button->setEnabled(false);
    pass_button->setEnabled(false);
    updateHandsLabels();
    HandLabel::isMyTurn(false);
}

void MainWindow::playButtonClicked(){
    played_illegally_label->setVisible(false);
    QString str;
    str.clear();
    int count = 0;
    for (auto hand: player_hands_labels){
        if (hand->isUp()){
            ++count;
            str.append(" " + QString::number(hand->getCardId()));
        }
    }
    str.prepend("play " + QString::number(count));
    sendToServer(str);
}

int MainWindow::getPosition(int i){ //输入i号玩家，输出i号玩家在屏幕上的位置，本人为0， 右侧为1， 左侧为2
    return (i + (3 - player_number)) % 3;
}

void MainWindow::sendToServer(QString str){
    QByteArray *array = new QByteArray;
    array->clear();
    array->append(str);
    array->append("\n");
    server_socket->write(array->data());
    array->clear();
    delete array;
}
