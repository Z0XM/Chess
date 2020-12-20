#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

int BOARD[64];

const int size = 56, offsetr = size / 2, offsetd = size / 2 + 50;
const sf::Vector2f offset(size / 2, size / 2);
std::string gameStatus;

int turn, check;
sf::Time timeLeft[2];
sf::Clock timer;

sf::Sprite pieces[32];
int defeatedPiecesCount[2];
std::string allMoveNotes;
std::vector<sf::RectangleShape> highlights;
std::vector<sf::RectangleShape> previousMoveHighlight;
sf::RectangleShape checkHighlight(sf::Vector2f(size, size));

void setGame() {
    int board[64] =
    { -1,-2,-3,-4,-5,-3,-2,-1,
     -6,-6,-6,-6,-6,-6,-6,-6,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     6, 6, 6, 6, 6, 6, 6, 6,
     1, 2, 3, 4, 5, 3, 2, 1 };

    for (int i = 0; i < 64; i++)
        BOARD[i] = board[i];

    gameStatus = "running";
    turn = 1;
    check = 0;
    timeLeft[0] = timeLeft[1] = sf::seconds(5 * 60);
    defeatedPiecesCount[0] = defeatedPiecesCount[1] = 0;
    allMoveNotes.clear();
    highlights.clear();
    previousMoveHighlight.clear();



}

int posToIndex(sf::Vector2f p) {
    return int((p.x - offset.x) / size) + 8 * int((p.y - offset.y) / size);
}
sf::Vector2f indexToPos(int x) {
    return sf::Vector2f((x % 8) * size + offset.x, (x / 8) * size + offset.y);
}
std::string chessNote(sf::Vector2f p)
{
    std::string s = "";
    s += char(p.x / size + 97);
    s += char(7 - p.y / size + 49);
    return s;
}
bool inRange(int k, int n = 0, int x = 0) { if (k >= 0 && k < 64 && (n == 0 ? true : n * (k % 8) > n * (x % 8)))return true; return false; }
std::vector<int> validMove(sf::Vector2f p, int side = turn) {
    std::vector<int> valid;
    int x = posToIndex(p);
    /** n->(-1 or 1) k->any index x->selected piece index */
    int n = side;
    if (abs(BOARD[x]) == 6) {//pawn
        if ((n == 1 ? x >= 48 : x < 16) && !BOARD[x - 16 * n] && !BOARD[x - 8 * n] && inRange(x - 16 * n) && inRange(x - 8 * n))valid.push_back(x - 16 * n);
        if (!BOARD[x - 8 * n] && inRange(x - 8 * n))valid.push_back(x - 8 * n);
        if (n * BOARD[x - 7 * n] < 0 && inRange(x - 7 * n, 1 * n, x))valid.push_back(x - 7 * n);
        if (n * BOARD[x - 9 * n] < 0 && inRange(x - 9 * n, -1 * n, x))valid.push_back(x - 9 * n);
    }
    else if (abs(BOARD[x]) == 2) {//knight
        int k = x - 2 - 8; if (inRange(k, -1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x - 2 + 8; if (inRange(k, -1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x + 2 - 8; if (inRange(k, 1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x + 2 + 8; if (inRange(k, 1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x - 2 * 8 - 1; if (inRange(k, -1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x - 2 * 8 + 1; if (inRange(k, 1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x + 2 * 8 - 1; if (inRange(k, -1, x) && n * BOARD[k] <= 0)valid.push_back(k);
        k = x + 2 * 8 + 1; if (inRange(k, 1, x) && n * BOARD[k] <= 0)valid.push_back(k);
    }
    else if (abs(BOARD[x]) == 4) {//king
        if (inRange(x - 1, -1, x) && n * BOARD[x - 1] <= 0)valid.push_back(x - 1);
        if (inRange(x - 9, -1, x) && n * BOARD[x - 9] <= 0)valid.push_back(x - 9);
        if (inRange(x - 8) && n * BOARD[x - 8] <= 0)valid.push_back(x - 8);
        if (inRange(x - 7, 1, x) && n * BOARD[x - 7] <= 0)valid.push_back(x - 7);
        if (inRange(x + 1, 1, x) && n * BOARD[x + 1] <= 0)valid.push_back(x + 1);
        if (inRange(x + 9, 1, x) && n * BOARD[x + 9] <= 0)valid.push_back(x + 9);
        if (inRange(x + 8) && n * BOARD[x + 8] <= 0)valid.push_back(x + 8);
        if (inRange(x + 7, -1, x) && n * BOARD[x + 7] <= 0)valid.push_back(x + 7);
    }
    else if (abs(BOARD[x]) == 1 || abs(BOARD[x]) == 5) {//Rook - Queen
        for (int k = x - 1; inRange(k, -1, x) && n * BOARD[k] <= 0; k--) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x - 8; inRange(k) && n * BOARD[k] <= 0; k -= 8) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x + 1; inRange(k, 1, x) && n * BOARD[k] <= 0; k++) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x + 8; inRange(k) && n * BOARD[k] <= 0; k += 8) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
    }
    if (abs(BOARD[x]) == 3 || abs(BOARD[x]) == 5) {//Bishop - Queen
        for (int k = x - 9; inRange(k, -1, x) && n * BOARD[k] <= 0; k -= 9) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x - 7; inRange(k, 1, x) && n * BOARD[k] <= 0; k -= 7) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x + 9; inRange(k, 1, x) && n * BOARD[k] <= 0; k += 9) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
        for (int k = x + 7; inRange(k) && n * BOARD[k] <= 0; k += 7) { valid.push_back(k); if (n * BOARD[k] < 0)break; }
    }
    return valid;
}
sf::RectangleShape addHighlight(sf::Vector2f p, sf::Color color) {
    sf::RectangleShape highlight(sf::Vector2f(size, size));
    highlight.setFillColor(color);
    highlight.setPosition(p);
    return highlight;
}
void setHighlights(sf::Vector2f p) {
    highlights.clear();
    std::vector<int> valid = validMove(p);
    for (unsigned int i = 0; i < valid.size(); i++) {
        highlights.push_back(addHighlight(indexToPos(valid.at(i)), sf::Color(0, 255, 0, 100)));
    }
}
int checkCheck(int side) {
    //side->who is giving check
    int kingIndex = 0, check = 0;
    std::vector<int> valid;
    for (int i = 0; i < 64; i++)
        if (BOARD[i] == 4 * side * -1) { kingIndex = i; break; }

    for (int i = 0; i < 64; i++) {
        if (side * BOARD[i] <= 0)continue;
        valid = validMove(indexToPos(i), side);
        for (unsigned int j = 0; j < valid.size(); j++) {
            if (valid.at(j) == kingIndex)check = side * -1;
        }
        if (check)break;
    }
    //check->on whom check is
    return check;
}
bool checkCheckMate(int side) {
    //side->who is giving check
    std::vector<int> valid;
    for (int i = 0; i < 64; i++) {
        if (side * BOARD[i] >= 0)continue;
        valid = validMove(indexToPos(i), -1 * side);
        for (unsigned int j = 0; j < valid.size(); j++) {
            int oldboard = BOARD[valid.at(j)];
            BOARD[valid.at(j)] = BOARD[i];
            BOARD[i] = 0;
            int tcheck = checkCheck(side);
            BOARD[i] = BOARD[valid.at(j)];
            BOARD[valid.at(j)] = oldboard;
            if (!tcheck)return false;
        }
    }
    return true;
}
bool checkStaleMate(int side) {
    //side->who cant move
    std::vector<int> valid;
    for (int i = 0; i < 64; i++) {
        if (side * BOARD[i] <= 0)continue;
        valid = validMove(indexToPos(i), side);
        for (unsigned int j = 0; j < valid.size(); j++) {
            int oldboard = BOARD[valid.at(j)];
            BOARD[valid.at(j)] = BOARD[i];
            BOARD[i] = 0;
            int tcheck = checkCheck(side * -1);
            BOARD[i] = BOARD[valid.at(j)];
            BOARD[valid.at(j)] = oldboard;
            if (!tcheck)return false;
        }
    }
    return true;
}

void pawnPromotion(sf::Vector2f p, int spi) {
    sf::RenderWindow pawnPromo(sf::VideoMode(4 * size, size), "promote");
    sf::Sprite options[4];
    sf::Texture options_img[4];

    for (int k = 0, i = 0; k < 4; k++, i++) {
        if (k == 3)i++;
        options_img[k].loadFromFile("images/figures.png", sf::IntRect((i % 6) * size, (BOARD[posToIndex(p)] < 0 ? 0 : 1) * size, size, size));
    }//1235
    for (int i = 0; i < 4; i++) {
        options[i].setTexture(options_img[i]);
        options[i].setPosition(sf::Vector2f(size * i, 0));
    }
    int spn = 0;
    while (pawnPromo.isOpen()) {
        sf::Event e;
        sf::Vector2i pos = sf::Mouse::getPosition(pawnPromo);
        while (pawnPromo.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                pawnPromo.close();

            if (e.type == sf::Event::MouseButtonPressed) {
                for (int i = 0; i < 4; i++)
                    if (options[i].getGlobalBounds().contains(pos.x, pos.y)) { spn = i + 1; break; }
                pawnPromo.close();
            }
        }
        pawnPromo.clear(sf::Color::Black);
        for (int i = 0; i < 4; i++)pawnPromo.draw(options[i]);
        pawnPromo.display();
    }

    if (spn) {
        if (spn == 4)spn = 5;
        int k = 0;
        if (BOARD[posToIndex(p)] > 0)k = 23;
        pieces[spi] = pieces[k + spn];
        BOARD[posToIndex(p)] = spn;
    }

}

sf::Vector2f legalMove(sf::Vector2f op, sf::Vector2f np, int spi) {
    if ((BOARD[posToIndex(op)] * BOARD[posToIndex(np)]) > 0)//restrict same team piece
        return op;

    std::vector<int> valid = validMove(op);
    unsigned int i = 0;
    for (; i < valid.size(); i++)
        if (valid.at(i) == posToIndex(np))break;
    if (i == valid.size())return op;

    int oldboard = BOARD[posToIndex(np)];
    BOARD[posToIndex(np)] = BOARD[posToIndex(op)];
    BOARD[posToIndex(op)] = 0;
    if (abs(BOARD[posToIndex(np)]) == 6 && (posToIndex(np) < 8 || posToIndex(np) > 55))pawnPromotion(np, spi);

    int tcheck = checkCheck(turn * -1);
    if (tcheck) {
        BOARD[posToIndex(op)] = BOARD[posToIndex(np)];
        BOARD[posToIndex(np)] = oldboard;
        return op;
    }

    if (!check)check = checkCheck(turn);
    else {
        int tcheck = checkCheck(check * -1);
        if (tcheck == check) {
            BOARD[posToIndex(op)] = BOARD[posToIndex(np)];
            BOARD[posToIndex(np)] = oldboard;
            return op;
        }
        else check = 0;
    }
    if (check)if (checkCheckMate(check * -1)) {
        if (check == 1)gameStatus = "Black";
        else gameStatus = "White";
        gameStatus += "is Victorius";
    }
    if (!check)if (checkStaleMate(turn * -1))gameStatus = "Stalemate";

    if ((BOARD[posToIndex(np)] * oldboard) < 0) {//if landing on opposite team's piece
        int i = 0;
        for (; i < 32; i++) {
            if (i == spi)continue;
            if (posToIndex(np) == posToIndex(pieces[i].getPosition()))break;
        }
        if (i < 32) {
            int index = 0;
            if (oldboard < 0)index = 1;
            defeatedPiecesCount[index]++;
            int x = ((defeatedPiecesCount[index] - 1) % 4) * size / 2 + 8 * size + offsetr;
            int y = (index * 4 + (defeatedPiecesCount[index] - 1) / 4) * size / 2;
            pieces[i].scale(sf::Vector2f(0.5, 0.5));
            pieces[i].setPosition(sf::Vector2f(x, y));
            pieces[i].move(offset);
        }
    }

    allMoveNotes += "." + chessNote(op) + chessNote(np);
    previousMoveHighlight.clear();
    previousMoveHighlight.push_back(addHighlight(op, sf::Color(0, 0, 255, 100)));
    previousMoveHighlight.push_back(addHighlight(np, sf::Color(0, 0, 255, 100)));

    turn *= -1;
    highlights.clear();//clicking a new piece clears highlights
    if (allMoveNotes.length() >= 10) {
        int index = turn == 1 ? 1 : 0;
        timeLeft[index] = sf::seconds(timeLeft[index].asSeconds() - 1);
        timer.restart();
    }
    if (check) {
        for (int i = 0; i < 64; i++)
            if (BOARD[i] == 4 * check) { checkHighlight.setPosition(indexToPos(i)); break; }
    }

    return np;
}
std::string timeToString(sf::Time time) {
    int mins, secs;
    std::string str = "";
    mins = time.asSeconds() / 60;
    secs = int(time.asSeconds()) % 60;
    if (mins < 10)str += "0";
    str += std::to_string(mins) + ":";
    if (secs < 10)str += "0";
    str += std::to_string(secs);
    return str;
}

int main()
{
start:setGame();

    sf::RenderWindow win(sf::VideoMode(size * 8 + size * 4, size * 8 + 2 * size), "Z-Chess");

    /** declare */
    sf::Texture board_imgT, pieces_img[12];
    sf::Sprite board_imgS;

    /** load font and set text*/
    sf::Font font;
    font.loadFromFile("consola.ttf");
    sf::Text moveNote;
    moveNote.setFont(font);
    moveNote.setPosition(sf::Vector2f(10, 8 * size + size));
    moveNote.setCharacterSize(36);

    /** load background image */
    board_imgT.loadFromFile("images/board.png");
    board_imgS.setTexture(board_imgT);
    board_imgS.setPosition(sf::Vector2f());

    /** load images of pieces */
    for (int i = 0; i < 12; i++)
        pieces_img[i].loadFromFile("images/figures.png", sf::IntRect((i % 6) * size, (i / 6) * size, size, size));

    /** set piece sprites and their position */
    for (int i = 0, k = 0; i < 64; i++)
        if (BOARD[i]) {
            pieces[k].setTexture(pieces_img[(BOARD[i] > 0 ? 1 : -1) * BOARD[i] + (BOARD[i] > 0 ? 5 : -1)]);
            pieces[k].setPosition(sf::Vector2f((i % 8) * size, (i / 8) * size));
            pieces[k].move(offset);
            k++;
        }

    /**time display */
    timeLeft[0] = sf::seconds(5 * 60); timeLeft[1] = sf::seconds(5 * 60);
    sf::RectangleShape timerBox(sf::Vector2f(2 * size, 2 * size));
    timerBox.setFillColor(sf::Color::White);
    timerBox.setOutlineColor(sf::Color::Black);
    timerBox.setOutlineThickness(2);
    timerBox.setPosition(9.5 * size, 6 * size);
    sf::RectangleShape timerBoxShade(sf::Vector2f(2 * size, size));
    timerBoxShade.setFillColor(sf::Color(0, 0, 0, 100));

    checkHighlight.setFillColor(sf::Color(255, 0, 0, 100));

    bool isMoving = false;
    float dx = 0, dy = 0;
    sf::Vector2f oldPos, newPos;
    int spi = 0;//selected piece index
    timerBoxShade.setPosition(timerBox.getPosition());
    sf::Text timeText[2];
    timeText[0].setString(timeToString(timeLeft[0]));
    timeText[0].setFont(font);
    timeText[0].setPosition(timerBox.getPosition().x + size / 4, timerBox.getPosition().y);
    timeText[1].setString(timeToString(timeLeft[1]));
    timeText[1].setFont(font);
    timeText[1].setPosition(timerBox.getPosition().x + size / 4, timerBox.getPosition().y + size);
    timeText[1].setFillColor(sf::Color::Black);

    while (win.isOpen() && gameStatus == "running")
    {
        sf::Vector2i tpos = sf::Mouse::getPosition(win);
        sf::Vector2f pos = win.mapPixelToCoords(tpos);
        sf::Event e;
        while (win.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                win.close();

            if (!(board_imgS.getGlobalBounds().contains(pos.x, pos.y)) && isMoving) {
                isMoving = false;
                pieces[spi].setPosition(oldPos);
            }
            if (e.type == sf::Event::MouseButtonPressed)
                if (e.key.code == sf::Mouse::Left) {
                    for (unsigned int i = 0; i < highlights.size(); i++) {
                        if (highlights.at(i).getGlobalBounds().contains(pos.x, pos.y)) {
                            pieces[spi].setPosition(legalMove(pieces[spi].getPosition(), highlights.at(i).getPosition(), spi));
                            break;
                        }
                    }
                    for (int i = 0; i < 32; i++)
                        if (pieces[i].getGlobalBounds().contains(pos.x, pos.y) && board_imgS.getGlobalBounds().contains(pos.x, pos.y)) {
                            if (turn * BOARD[posToIndex(pieces[i].getPosition())] < 0)break;
                            isMoving = true; spi = i;
                            oldPos = pieces[i].getPosition();
                            dx = pos.x - pieces[i].getPosition().x;
                            dy = pos.y - pieces[i].getPosition().y;
                            setHighlights(oldPos);
                            break;
                        }
                }
            if (e.type == sf::Event::MouseButtonReleased)
                if (e.key.code == sf::Mouse::Left)
                    if (isMoving && pieces[spi].getGlobalBounds().contains(pos.x, pos.y) && board_imgS.getGlobalBounds().contains(pos.x, pos.y)) {
                        isMoving = false;
                        sf::Vector2f p = pieces[spi].getPosition() + sf::Vector2f(size / 2, size / 2);
                        newPos = sf::Vector2f(indexToPos(posToIndex(p)));
                        pieces[spi].setPosition(legalMove(oldPos, newPos, spi));
                    }
            if (isMoving)pieces[spi].setPosition(pos.x - dx, pos.y - dy);
        }

        win.clear(sf::Color::Black);
        win.draw(board_imgS);
        for (unsigned int i = 0; i < highlights.size(); i++) {
            win.draw(highlights.at(i));
        }
        for (unsigned int i = 0; i < previousMoveHighlight.size(); i++) {
            win.draw(previousMoveHighlight.at(i));
        }
        for (int i = 0; i < 32; i++) {
            if (spi == i)continue;
            win.draw(pieces[i]);
        }
        win.draw(pieces[spi]);
        if (allMoveNotes.length())moveNote.setString(allMoveNotes.substr(allMoveNotes.length() - 4, allMoveNotes.length() - 1));
        win.draw(moveNote);
        win.draw(timerBox);
        win.draw(timerBoxShade);
        if (check)win.draw(checkHighlight);

        if (timer.getElapsedTime().asSeconds() >= 1 && allMoveNotes.length() >= 10) {
            int index = turn == 1 ? 1 : 0;
            timeLeft[index] = sf::seconds(timeLeft[index].asSeconds() - 1);
            timer.restart();
        }
        if (timeLeft[0].asSeconds() <= 0 && gameStatus == "running")gameStatus = "White is Victorious";
        else if (timeLeft[1].asSeconds() <= 0 && gameStatus == "running")gameStatus = "Black is Victorious";
        if (timeLeft[0].asSeconds() < 60 && timeText[0].getFillColor() != sf::Color::Red)timeText[0].setFillColor(sf::Color::Red);
        if (timeLeft[1].asSeconds() < 60 && timeText[1].getFillColor() != sf::Color::Red)timeText[1].setFillColor(sf::Color::Red);

        timeText[turn == 1 ? 1 : 0].setString(timeToString(timeLeft[turn == 1 ? 1 : 0]));

        win.draw(timeText[0]);
        win.draw(timeText[1]);
        win.display();
    }
    sf::RenderWindow endScreen;
    sf::Text message;

    win.clear(sf::Color::Black);
    win.draw(board_imgS);
    for (int i = 0; i < 32; i++) {
        win.draw(pieces[i]);
    }
    if (win.isOpen()) {
        win.draw(timerBox);
        win.draw(timerBoxShade);
        endScreen.create(sf::VideoMode(500, 100), "Z-chess");
        message.setString(gameStatus);
        message.setCharacterSize(40);
        message.setFont(font);
        message.setPosition(25, 25);
    }

    while (endScreen.isOpen())
    {
        sf::Event e;
        while (endScreen.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) {
                endScreen.close();
                win.close();
            }
        }
        endScreen.clear(sf::Color::Black);
        endScreen.draw(message);
        endScreen.display();
    }
    char choice = 'a';
    std::cout << "*** Command ***" << std::endl;
    std::cout << "Enter h for help\n";
    while (choice != 'e') {
        std::cin >> choice;
        switch (choice) {
        case 'h':std::cout << "r - restart" << std::endl;
            std::cout << "e - exit" << std::endl;
            std::cout << "s - save" << std::endl;
            break;
        case 's':std::cout << "[error]" << std::endl;
            break;
        case 'r':goto start;

        }
    }

    return 0;
}
