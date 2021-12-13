#include "cardlabel.h"
#include <QDebug>

//CardLabel::CardLabel(int fig, int pat, QWidget* parent): QLabel(parent), figure(fig), pattern(pat){}

//CardLabel::CardLabel(std::pair<int, int> info, QWidget* parent): QLabel(parent), figure(info.first), pattern(info.second){}

bool HandLabel::my_turn = false;

CardLabel::CardLabel(int i, QWidget* parent): QLabel(parent){
    id = i;
    key = id2Info(i).first;
    suit = id2Info(i).second;
    this->setPixmap(getPixmap(i));
    this->setScaledContents(true);
    this->setVisible(false);
}

std::pair<int, int> CardLabel::id2Info(int i){
    if (i == -1) {return std::make_pair(-1, 0);}
    else if (i == 52) {return std::make_pair(13, 0);}
    else if (i == 53) {return std::make_pair(14, 1);}
    else{
        int k = i / 4;
        int s = i % 4;
        return std::make_pair(k, s);
    }
}

QPixmap CardLabel::getPixmap(int i){
    QPixmap image;
    if (i == -1) {image.load(":/cards/cards/BACK.png");}
    else {
        int key = id2Info(i).first;
        int suit = id2Info(i).second;
        QString s, k;
        if (key == 13) {s = "B"; k = "J";}
        else if (key == 14) {s = "R"; k = "J";}
        else{
            if (key == 11) {k = "A";}
            else if (key == 12) {k = "2";}
            else if (key == 8) {k = "J";}
            else if (key == 9) {k = "Q";}
            else if (key == 10) {k = "K";}
            else {k = QString::number(key + 3);}
            switch (suit){
            case 0:
                s = "D";
                break;
            case 1:
                s = "C";
                break;
            case 2:
                s = "H";
                break;
            case 3:
                s = "S";
                break;
            }
        }
        image.load(":/cards/cards/" + s + k + ".png");
    }
    return image;
}

int CardLabel::getCardId(){
    return this->id;
}

HandLabel::HandLabel(int i, QWidget* parent): CardLabel(i, parent), covered(true), is_up(false){};

void HandLabel::isMyTurn(bool b){
    my_turn = b;
}

void HandLabel::setUncovered(){
    covered = false;
}

bool HandLabel::isUp(){
    return is_up;
}

void HandLabel::setUp(){
    this->setGeometry(this->pos().x(), 470, 60, 90);
    this->is_up = true;
}

void HandLabel::setDown(){
    this->setGeometry(this->pos().x(), 500, 60, 90);
    this->is_up = false;
}

void HandLabel::mousePressEvent(QMouseEvent *event){
    if (!my_turn) {return;}
    int valid_left = covered? 20: 60;
    int valid_above = is_up? 30: 0;
    if ((event->pos().x() <= valid_left) || event->pos().y() < valid_above){
        if (!is_up) {setUp();}
        else {setDown();}
    }
}
