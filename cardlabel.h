#ifndef CARDLABEL_H
#define CARDLABEL_H
#include <QLabel>
#include <QMouseEvent>

class CardLabel: public QLabel{

    Q_OBJECT

public:
    //CardLabel(int figure, int pattern, QWidget* parent); // joker = 0
    //CardLabel(std::pair<int, int>, QWidget* parent);
    CardLabel(int id, QWidget* parent);
    //void setUp();
    //void setDown();
    int getCardId();//0~53, 52 = BJ, 53 = RJ, the rest divided by 4, 0 = 3, ..., 12 = 2, mod 4, 0 = Diamond, 1 = Club, 2 = Heart, 3 = Spade
    static std::pair<int, int> id2Info(int id); //return (key, suit);
    static QPixmap getPixmap(int id);

private:
    int key; //card key: 0=3, 1=4, ... , 10=K, 11=A, 12=2, 13=BJ, 14=RJ
    int suit; //dia=0, clb=1, hrt=2, spd=3;
    int id;
    QPixmap image;
};

class HandLabel:public CardLabel{

    Q_OBJECT

public:
    //HandLabel(int figure, int pattern, QWidget* parent); // joker = 0
    //HandLabel(std::pair<int, int>, QWidget* parent);
    HandLabel(int id, QWidget* parent);
    void setUp();
    void setDown();
    void setUncovered(); //是手牌中最上层的，鼠标事件范围扩大
    bool isUp();
    static void isMyTurn(bool);

protected:
    void mousePressEvent(QMouseEvent *);

private:
    static bool my_turn;
    bool covered;
    bool is_up;

};

#endif // CARDLABEL_H
