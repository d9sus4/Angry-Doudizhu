#include "serverthread.h"

ServerThread::ServerThread(QString port, QObject* parent): QThread(parent){
    listen_socket = new QTcpServer(this);
    listen_socket->listen(QHostAddress::Any, port.toInt());
    for (int i = 0; i < 3; ++i){
        player_is_connected[i] = false;
    }
    initGame();
}

void ServerThread::run(){
    connect(listen_socket, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void ServerThread::acceptConnection(){
    //qDebug() << "newconnection incoming " << Qt::endl;
    int i = 0;
    while (player_is_connected[i]) {
        ++i;
        if (i >= 3){return;}
    }
    //qDebug() << "newconnection at " << i << Qt::endl;
    client_socket[i] = listen_socket->nextPendingConnection();
    connect(client_socket[i], SIGNAL(readyRead()), this, SLOT(handleRead()));
    connect(client_socket[i], SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
    player_is_connected[i] = true;

    //告诉玩家自己的编号，并令其初始化游戏画面
    broadcast(i, "you're " + QString::number(i));
    broadcast(i, "init");

    //向该玩家更新初始化信息
    broadcastReadyInfo(i);

    //告诉所有玩家有人加入
    broadcastAll("joined " + QString::number(i));
    //qDebug() << "newconnection finished "<< Qt::endl;
}

void ServerThread::handleDisconnected(){
    for (int i = 0; i < 3; ++i){
        if (!player_is_connected[i]) {continue;}
        if (client_socket[i]->state() == QAbstractSocket::UnconnectedState){
            client_socket[i]->deleteLater();
            player_is_connected[i] = false;
            broadcastAll("left " + QString::number(i));
            broadcast(0, "wait");
        }
    }
}

void ServerThread::handleRead(){
    for(int i = 0; i < 3; ++i){
        if (!player_is_connected[i]){continue;}
        while (client_socket[i]->canReadLine()){ //i号玩家发送请求
            QTextStream ts(client_socket[i]->readLine());
            QString keyword;
            ts >> keyword;

            if (keyword == "seat"){
                player_is_seated[i] = true;
                broadcastAll("seated " + QString::number(i));
                if (player_is_seated[0] && player_is_seated[1] && player_is_seated[2]){
                    broadcast(0, "dealable");
                }
            }

            if (keyword == "standup"){
                player_is_seated[i] = false;
                broadcastAll("stoodup " + QString::number(i));
                broadcast(0, "wait");
            }

            else if (keyword == "deal"){
                startDeal();
            }

            else if (keyword == "jiaodizhu"){
                current_dizhu = i;
                broadcastAll("jiaole " + QString::number(i));
                decideDizhu((i + 1) % 3);
            }

            else if (keyword == "bujiao"){
                broadcastAll("meijiao " + QString::number(i));
                decideDizhu((i + 1) % 3);
            }

            else if(keyword == "play"){
                int size, temp;
                ts >> size;
                QVector<int> to_be_judged;
                qDebug()<<"judging";
                while (size--){
                    ts >> temp;
                    to_be_judged.append(temp);
                    qDebug() << " " << temp;
                }
                qDebug() << Qt::endl;
                if (judgeLegality(to_be_judged)){
                    broadcast(i, "legal");
                    QString str;
                    str.clear();
                    for (int c: to_be_judged){
                        player_hands[i].erase(std::find(player_hands[i].begin(), player_hands[i].end(), c));
                        str.append(" " + QString::number(c));
                    }
                    str.prepend("played " + QString::number(i) + " " + QString::number(to_be_judged.size()));
                    broadcastAll(str);
                    pass_count = 0;
                    if (player_hands[i].empty()) {//游戏结束
                        gameOver(i);
                    }
                    else nextTurn((i + 1) % 3, true);
                }
                else {broadcast(i, "illegal");}
                to_be_judged.clear();
            }

            else if (keyword == "pass"){
                broadcastAll("passed " + QString::number(i));
                if (++pass_count == 2){
                    current_playing_attach_scale = 0;
                    current_playing_key = 0;
                    current_playing_scale = 0;
                    current_playing_pattern = Anything;
                    pass_count = 0;
                    nextTurn((i + 1) % 3, false);
                }
                else {nextTurn((i + 1) % 3, true);}
            }

            else if (keyword == "stay"){
                broadcast(i, "init");
                broadcastReadyInfo(i);
            }

            ts.flush();
        }
    }
}

void ServerThread::initGame(){
    card_pile.clear();
    for (int i = 0; i < 3; ++i){
        player_hands[i].clear();
        player_is_seated[i] = false;
    }
    current_playing_pattern = Anything;
    current_playing_key = 0;
    current_playing_scale = 0;
    current_playing_attach_scale = 0;
    current_dizhu = 0;
    pass_count = 0;
    decide_count = 0;
    for (int i = 0; i < 54; ++i){
        card_pile.append(i);
    }
}

void ServerThread::startDeal(){
    int ran, id;
    for (int i = 0; i < 51; ++i){
        ran = QRandomGenerator::global()->bounded(54 - i);
        id = card_pile.at(ran);
        card_pile.removeAt(ran);
        player_hands[i % 3].append(id);
    }
    QString str;
    for (int i = 0; i < 3; ++i){
        str.clear();
        std::sort(player_hands[i].begin(), player_hands[i].end(), std::greater<int>());
        str.append("dealed");
        for (int k = 0; k < 17; k++){
            str.append(" " + QString::number(player_hands[i].at(k)));
        }
        broadcast(i, str);
        /*qDebug() << "dealing player " << i << ":";
        for (int k = 0; k < 17; k++){
            qDebug() << " " << player_hands[i].at(k);
        }
        qDebug() << Qt::endl;*/
    }
    current_dizhu = QRandomGenerator::global()->bounded(3);
    decideDizhu(current_dizhu);
}

void ServerThread::decideDizhu(int i){
    if (decide_count == 3) {
        QString str;
        str.clear();
        str.append("congrats " + QString::number(current_dizhu));//congrats: current_dizhu号玩家是地主，获得的三张底牌分别是
        for (int k = 0; k < 3; ++k){
            str.append(" " + QString::number(card_pile.at(k)));
        }
        broadcastAll(str);
        while (!card_pile.empty()){
            player_hands[current_dizhu].append(card_pile.at(0));
            card_pile.removeAt(0);
        }
        std::sort(player_hands[current_dizhu].begin(), player_hands[current_dizhu].end(), std::greater<int>());
        nextTurn(current_dizhu, false);
        return;
    }
    broadcast(i, "decide");
    ++decide_count;
}

void ServerThread::nextTurn(int player, bool passable){
    broadcastAll("it's " + QString::number(player));
    if (passable) {broadcast(player, "passable");}
}

void ServerThread::gameOver(int player){
    if (current_dizhu == player){
        broadcast(player, "gameover won");
        broadcast((player+1)%3, "gameover lost");
        broadcast((player+2)%3, "gameover lost");
    }
    else {
        broadcast(current_dizhu, "gameover lost");
        broadcast(player, "gameover won");
        broadcast(3 - player - current_dizhu, "gameover won");
    }
    initGame();
}

void ServerThread::broadcastAll(QString info){
    QByteArray *array = new QByteArray;
    array->clear();
    array->append(info);
    array->append("\n");
    for (int i = 0; i < 3; ++i){
        if (player_is_connected[i])
        {client_socket[i]->write(array->data());}
    }
    array->clear();
    delete array;
}

void ServerThread::broadcast(int i, QString info){
    QByteArray *array = new QByteArray;
    array->clear();
    array->append(info);
    array->append("\n");
    client_socket[i]->write(array->data());
    array->clear();
    delete array;
}

void ServerThread::broadcastReadyInfo(int target){
    for (int i = 0; i < 3; ++i){
        if (player_is_connected[i]){
            broadcast(target, "joined " + QString::number(i));
        }
        if (player_is_seated[i]){
            broadcast(target, "seated " + QString::number(i));
        }
    }
}

bool ServerThread::judgeLegality(QVector<int> &array){
    if (current_playing_pattern == WangZha) {return false;}
    int count [15]; //count[key]为该牌的数量
    memset(count, 0, sizeof(count));
    for (int i: array){
        ++count[CardLabel::id2Info(i).first];
    }
    /*
    qDebug() << "player played";
    for (int i = 0; i < 15; ++i){
        qDebug() << " " << i << "x" << count[i];
    }
    qDebug() << Qt::endl;
    qDebug() << "playing pattern " << current_playing_pattern << Qt::endl;
    qDebug() << "now key " << current_playing_key << Qt::endl;
    */
    //王炸最优先判断
    if (judgeWangZha(count)){
        current_playing_pattern = WangZha;
        return true;
    }

    //炸弹比较特殊，先判断现在是否在出炸弹
    if (current_playing_pattern == ZhaDan){
        int result = judgeZhaDan(count);
        if (result > current_playing_key){
            current_playing_key = result;
            return true;
        }
        else return false;
    }
    //如果不是，则炸弹一定合法，先判断之
    if (judgeZhaDan(count) >= 0){
        current_playing_pattern = ZhaDan;
        current_playing_key = judgeZhaDan(count);
        current_playing_scale = 4;
        current_playing_attach_scale = 0;
        return true;
    }
    //再判断其他的
    if (current_playing_pattern == Anything) {
        //依次judge直到确定牌型及两个scale
        if (judgeDanZhang(count) >= 0){
            current_playing_pattern = DanZhang;
            current_playing_key = judgeDanZhang(count);
            current_playing_scale = 1;
            current_playing_attach_scale = 0;
        }
        else if (judgeYiDui(count) >= 0){
            current_playing_pattern = YiDui;
            current_playing_key = judgeYiDui(count);
            current_playing_scale = 2;
            current_playing_attach_scale = 0;
        }
        else if (judgeSanDai(count).first >=0){
            current_playing_pattern = SanDai;
            current_playing_key = judgeSanDai(count).first;
            current_playing_scale = 3;
            current_playing_attach_scale = judgeSanDai(count).second;
        }
        else if (judgeShunZi(count).first >= 0){
            current_playing_pattern = ShunZi;
            current_playing_key = judgeShunZi(count).first;
            current_playing_scale = judgeShunZi(count).second;
            current_playing_attach_scale = 0;
        }
        else if (judgeLianDui(count).first >= 0){
            current_playing_pattern = LianDui;
            current_playing_key = judgeLianDui(count).first;
            current_playing_scale = judgeLianDui(count).second;
            current_playing_attach_scale = 0;
        }
        else if (judgeSiDaiEr(count).first >= 0){
            current_playing_pattern = SiDaiEr;
            current_playing_key = judgeSiDaiEr(count).first;
            current_playing_scale = 4;
            current_playing_attach_scale = judgeSiDaiEr(count).second;
        }
        else if (std::get<0>(judgeFeiJi(count)) >= 0){
            current_playing_pattern = FeiJi;
            current_playing_key = std::get<0>(judgeFeiJi(count));
            current_playing_scale = std::get<1>(judgeFeiJi(count));
            current_playing_attach_scale = std::get<2>(judgeFeiJi(count));
        }
        if (current_playing_pattern != Anything) {return true;}
        else {return false;}
    }

    if (current_playing_pattern == DanZhang){
        int result = judgeDanZhang(count);
        if (result > current_playing_key){
            current_playing_key = result;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == YiDui){
        int result = judgeYiDui(count);
        //qDebug() << "pair " << result << Qt::endl;
        if (result > current_playing_key){
            current_playing_key = result;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == SanDai){
        std::pair<int,int> result = judgeSanDai(count);
        if (result.second == current_playing_attach_scale &&
                result.first > current_playing_key){
            current_playing_key = result.first;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == ShunZi){
        std::pair<int,int> result = judgeShunZi(count);
        if (result.second == current_playing_scale &&
                result.first > current_playing_key){
            current_playing_key = result.first;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == LianDui){
        std::pair<int,int> result = judgeLianDui(count);
        if (result.second == current_playing_scale &&
                result.first > current_playing_key){
            current_playing_key = result.first;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == SiDaiEr){
        std::pair<int, int> result = judgeSiDaiEr(count);
        if (result.second == current_playing_attach_scale &&
                result.first > current_playing_key){
            current_playing_key = result.first;
            return true;
        }
        else return false;
    }

    if (current_playing_pattern == FeiJi){
        std::tuple<int,int,int> result = judgeFeiJi(count);
        if (std::get<1>(result) == current_playing_scale &&
                std::get<2>(result) == current_playing_attach_scale &&
                std::get<0>(result) > current_playing_key){
            current_playing_key = std::get<0>(result);
            return true;
        }
        else return false;
    }

    return false;
}

bool ServerThread::judgeWangZha(int *count){
    if (count[13] == 1 && count[14] == 1){
        for (int i = 0; i < 13; ++i){
            if (count[i] > 0) {return false;}
        }
        return true;
    }
    return false;
}

int ServerThread::judgeZhaDan(int *count){
    for (int i = 0; i < 13; ++i){ //找四张
        if (count[i] == 4){ //i有四张了
            //不能有其他牌
            for (int j = 0; j < 15; ++j){
                if (count[j] > 0){
                    if (j == i) {continue;}
                    else return false;
                }
            }
            return i;
        }
    }
    //没找到
    return -1;
}

int ServerThread::judgeDanZhang(int *count){
    for (int i = 0; i < 15; ++i){//找单张
        if (count[i] == 1){ //i有了
            //不能有其他牌
            for (int j = 0; j < 15; ++j){
                if (count[j] > 0 && i != j){
                    return -1;
                }
            }
            return i;
        }
    }
    //没找到
    return -1;
}

int ServerThread::judgeYiDui(int *count){
    for (int i = 0; i < 13; ++i){//找一对
        if (count[i] == 2){ //i有一对了
            //不能有其他牌
            for (int j = 0; j < 15; ++j){
                if (count[j] > 0 && i != j){
                    return -1;
                }
            }
            return i;
        }
    }
    //没找到
    return -1;
}

std::pair<int, int> ServerThread::judgeSanDai(int *count){
    for (int i = 0; i < 13; ++i){ //先找三张的牌
        if (count[i] == 3){//找到了，找第一张其他牌
            for (int j = 0; j < 15; ++j){
                if (count[j] > 0 && j != i){ //找到了，判断其张数
                    if (count[j] == 1){ //只能是三带一，进一步确定没有其他牌
                        for (int k = j + 1; k < 15; ++k){
                            if (count[k] > 0 && i != k){
                                return std::make_pair(-1, 0);
                            }
                        }
                        return std::make_pair(i, 1);
                    }
                    else if (count[j] == 2){ //只能是三带一对，进一步确定没有其他牌
                        for (int k = j + 1; k < 15; ++k){
                            if (count[k] > 0 && i != k){
                                return std::make_pair(-1, 0);
                            }
                        }
                        return std::make_pair(i, 2);
                    }
                    else{ //找到了其他东西，不是三带
                        return std::make_pair(-1, 0);
                    }
                    break;
                }
            }
            //没找到其他的牌，是三不带
            return std::make_pair(i, 0);
            break;
        }
    }
    return std::make_pair(-1, 0);
}

std::pair<int, int> ServerThread::judgeShunZi(int *count){
    for (int i = 0; i < 13; ++i){//找第一张牌
        if (count[i] > 0){ //找到第一张牌了
            int j = 0;
            while ((i + j) < 12 && count[i + j] == 1) {++j;} //找连续的单张有多少张
            if (j >= 5){//有连续至少五张，才可能是顺子
                for (int k = i + j; k < 15; ++k){//后续的牌不能再有
                    if (count[k] > 0){
                        return std::make_pair(-1, 0);
                    }
                }
                //没有其他牌了
                return std::make_pair(i, j);
            }
            break;
        }
    }
    return std::make_pair(-1, 0);//其他情况一律不是
}

std::pair<int, int> ServerThread::judgeLianDui(int *count){
    for (int i = 0; i < 13; ++i){//找第一张牌
        if (count[i] > 0){ //找到第一张牌了
            int j = 0;
            while ((i + j) < 12 && count[i + j] == 2) {++j;} //找连续的对子有多少张
            if (j >= 3){//有连续至少三对，才可能是连对
                for (int k = i + j; k < 15; ++k){//后续的牌不能再有
                    if (count[k] > 0){
                        return std::make_pair(-1, 0);
                    }
                }
                //没有其他牌了
                return std::make_pair(i, j);
            }
            break;
        }
    }
    return std::make_pair(-1, 0);//其他情况一律不是
}

std::pair<int, int> ServerThread::judgeSiDaiEr(int *count){
    for (int i = 0; i < 13; ++i){//找四张
        if (count[i] == 4){//找到了
            for (int j = 0; j < 15; ++j){//从头找不是i的第一张牌
                if (count[j] > 0 && j != i){//找到了
                    if (count[j] == 1){//如果j是一张，只有可能是四带两张，进一步找寻第二张
                        for (int k = j + 1; k < 15; ++k){
                            if (count[k] > 0 && k != i){ //找到了，k只能是单张
                                if (count[k] == 1){ //确定k只有一张后，进一步保证后续没有其他牌
                                    for (int l = k + 1; l < 15; ++l){
                                        if (count[l] > 0 && l != i){
                                            return std::make_pair(-1, 0);
                                        }
                                    }
                                    return std::make_pair(i, 1);//确实是四带二
                                }
                                break;
                            }
                        }
                    }
                    if (count[j] == 2){//如果是j是两张，有可能是四带一对，也有可能是四带两对
                        for (int k = j + 1; k < 15; ++k){//找下一张牌
                            if (count[k] > 0 && k != i){
                                if (count[k] == 2){//找到的是一对,进一步确定没有其他牌
                                    for (int l = k + 1; l < 15; ++l){
                                        if (count[l] > 0 && l != i){
                                            return std::make_pair(-1, 0);
                                        }
                                    }
                                    return std::make_pair(i, 2);//确实是四带两对
                                }
                                else {//找到的不是一对
                                    return std::make_pair(-1, 0);
                                }
                            }
                        }
                        //什么也没找到，但仍然是四带二
                        return std::make_pair(i, 1);
                    }
                    break;
                }
            }
        }
    }
    return std::make_pair(-1, 0);
}

std::tuple<int, int, int> ServerThread::judgeFeiJi(int *count){
    for (int i = 0; i < 12; ++i){//找第一个三张（或四张）i（事实上，有极小概率三张i是飞机带的三张牌，它可以不与飞机连续，所以这个for循环不break，直到找到飞机的本体）
        if (count[i] >= 3){//找到了，其后必须至少跟随一个三张
            int j = 1;
            while (count[i+j] >= 3 && i+j < 12){++j;}//计数连续的三张
            if (j >= 2){//有可能是飞机，进一步统计飞机本体之外的牌数，只有能被飞机长度整除才可能为飞机
                for (int left = i+j-2; left >= i; --left){
                    for (int right = i+j; right >= left+2; --right){
                        int rest = 0;
                        for (int k = 0; k < 15; ++k){
                            if (k < right && k >= left){rest += count[k] - 3;}
                            else {rest += count[k];}
                        }
                        if (rest % (right - left) == 0){//可能是飞机
                            if (rest == 0){
                                return std::make_tuple(left, right - left, 0);
                            }
                            else if (rest == right - left){
                                return std::make_tuple(left, right - left, 1);
                            }
                            else if (rest == 2 * (right - left)){//判断count > 0的牌是否全部为2两张
                                for (int l = 0; l < 15; ++l){
                                    if ((l < left || l >= right) && (count[l] != 2 && count[l] != 0)){
                                        return std::make_tuple(-1, 0, 0);
                                    }
                                    else if (l >= left && l < right && count[l] != 3){
                                        return std::make_tuple(-1, 0, 0);
                                    }
                                }
                                return std::make_tuple(left, right - left, 2);
                            }
                        }
                    }
                }
            }
        }
    }
    return std::make_tuple(-1, 0, 0);
}
