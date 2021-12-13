#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QDialog>
#include <QTcpSocket>
#include <QHostInfo>
#include <QPixmap>
#include <QtNetwork>
#include <QMessageBox>
#include "mydialog.h"
#include "cardlabel.h"
#include "serverthread.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    MyDialog *dialog;
    ServerThread *server;
    QTcpSocket *server_socket; //client connects with server

    QPushButton *start_button;
    QPushButton *deal_button;//server only
    QPushButton *seat_button;//client only
    QPushButton *pass_button;
    QPushButton *play_button;
    QPushButton *jiaodizhu_button;
    QPushButton *bujiao_button;
    QLabel *played_illegally_label;

    QLabel *player_image[3]; //myself is 0 right is 1 left is 2;
    QLabel *player_seated_label[3];
    QLabel *player_jiaodizhu_label[3];
    QLabel *player_pass_label[3];
    QLabel *player_hands_number_icon[3];
    QLabel *player_hands_number_label[3];
    int player_hands_number[3];
    QLabel *player_yaobuqi_label[3];
    QLabel *player_identity_label[3];

    QVector<HandLabel *> player_hands_labels;
    QVector<int> player_hands_array;
    CardLabel * bottom_labels[3];
    QVector<CardLabel *> player_played_labels[3];
    QVector<int> player_played_array[3];

    bool is_host = false;
    int player_number;

    QPixmap nongmin_image;
    QPixmap dizhu_image;

    void showDialog();
    void initScene(); //初始化游戏界面
    void clearScene();//回到主界面
    void updateHandsLabels(); //刷新手牌显示
    void updatePlayedLabels(int); //刷新int位置上的桌面
    int getPosition(int);

    void sendToServer(QString);

protected slots:
    void createServer(QString);
    void joinServer(QString, QString);
    void readFromServer();//client reads
    void readError(QAbstractSocket::SocketError);
    void seatButtonClicked();
    void dealButtonClicked();
    void jiaodizhuButtonClicked();
    void bujiaoButtonClicked();
    void playButtonClicked();
    void passButtonClicked();
};

#endif // MAINWINDOW_H
