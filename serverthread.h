#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QRandomGenerator>
#include "cardlabel.h"

class ServerThread: public QThread{

    Q_OBJECT

public:
    ServerThread(QString port ,QObject* parent);

protected:
    void run();

private:
    QTcpServer *listen_socket;
    QTcpSocket *client_socket[3];

    QVector<int> player_hands[3];
    QVector<int> card_pile;

    enum Pattern {Anything,
                  DanZhang,
                  YiDui,
                  SanDai,
                  ShunZi,
                  LianDui,
                  SiDaiEr,
                  FeiJi,
                  ZhaDan,
                  WangZha} current_playing_pattern;

    //card key: 0:3, 1:4, ... , 10:K, 11:A, 12:2, 13:BJ, 14:RJ
    //用来比大小
    //如果是王炸，设为15确保万无一失
    int current_playing_key;

    //scale:
    //单张、一对、三带、四带二、炸弹、王炸无意义
    //顺子为张数
    //连对为对数
    //飞机有多少个三张就是多少
    int current_playing_scale;

    //attach scale：
    //单张、一对、顺子、连对、炸弹、王炸无意义
    //三不带为0， 三带一为1， 三带一对为2
    //四带二为1，四带两对为2
    //飞机是三不带则为0，三带一则为1，三带二则为2
    int current_playing_attach_scale;
    int current_dizhu;

    bool player_is_connected[3];
    bool player_is_seated[3];

    int pass_count;
    int decide_count;

    void initGame();
    void startDeal();
    void decideDizhu(int);
    void nextTurn(int, bool);
    void gameOver(int);//game over at player i
    void broadcastAll(QString); //only valid
    void broadcast(int, QString);
    void broadcastReadyInfo(int);


    bool judgeLegality(QVector<int> &);

    bool judgeWangZha(int *);
    //int型的judge函数全部返回其key；若不是该牌型，key返回-1
    int judgeZhaDan(int *);
    int judgeDanZhang(int *);
    int judgeYiDui(int *);
    std::pair<int, int> judgeSanDai(int *); //返回(key, attach_scale)
    std::pair<int, int> judgeShunZi(int *); //返回(key, scale)
    std::pair<int, int> judgeLianDui(int *); //返回(key, scale)
    std::pair<int, int> judgeSiDaiEr(int *); //返回(key, attach_scale)
    std::tuple<int, int, int> judgeFeiJi(int *); //返回(key, scale, attach_scale)

protected slots:
    void acceptConnection();
    void handleDisconnected();
    void handleRead();
};

#endif // SERVERTHREAD_H
