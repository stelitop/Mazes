#include<iostream>
#include<SFML/Graphics.hpp>
#include<fstream>
#include<stack>

using namespace std;
using namespace sf;

#include"loadTextures.h"
#include"level.h"

int texSz = 48;
int plSz = 40;
int tileSz = 70;
int ScreenX, ScreenY;
int WindowX, WindowY;
int TotalX, TotalY;
pair<double,double> adit;
string selectedIcon = "Icon2";
Color selectedColor1(0,100,155), selectedColor2(0,100,155);

enum Direction{UP,DOWN,LEFT,RIGHT,VOID} animDir=VOID;

enum Menu{MAIN,GAME,LOADING_LEVEL,LEVEL_BROWSE} menu = LOADING_LEVEL;
int currentLevel = 1;

level dynamicLevel;

struct level_memory
{
    int row_sz,column_sz;
    vector<char> row, column;
    bool gem;
};

stack<level_memory> memory;

void loadSettings()
{
    ifstream file;
    file.open("settings.txt");
    string x;
    texSz = 0;
    plSz = 0;
    tileSz = 0;
    while (getline(file,x))
    {
        int i=0;
        for (;x[i]!=';';i++)
        {
            tileSz = tileSz*10+x[i]-'0';
        }
        i++;
        for (;x[i]!=';';i++)
        {
            texSz = texSz*10+x[i]-'0';
        }
        i++;
        for (;x[i]!=';';i++)
        {
            plSz = plSz*10+x[i]-'0';
        }
    }
}//

void addToMemory(bool gem)
{
    level_memory add;
    add.gem = gem;
    add.row_sz = dynamicLevel.starting_position.first;
    add.column_sz = dynamicLevel.starting_position.second;
    for (int i=0; i<dynamicLevel.size.first;i++)
        add.row.push_back(dynamicLevel.level[i][add.column_sz]);
    for (int i=0; i<dynamicLevel.size.second;i++)
        add.column.push_back(dynamicLevel.level[add.row_sz][i]);
    memory.push(add);
}

struct cameraRect
{
    pair<double, double> corner;
    pair<double, double> sz;
} mainCamera;

void drawLevel (RenderWindow * window, char lvl[100][100], pair<int,int> mazeSize, pair<double,double> playerPos)
{
    mainCamera.corner = make_pair(WindowX/2, WindowY/2);

    swap(mazeSize.first, mazeSize.second);
    window->clear();

    pair<int,int> screenSize = make_pair(mazeSize.first*tileSz, mazeSize.second*tileSz);
    pair<double,double> origin = make_pair(0,0);
    if (screenSize.first <= WindowX*99/100) {origin.first = WindowX/2-screenSize.first/2; TotalX = WindowX;}
    else {origin.first = WindowX*1/200; TotalX = mazeSize.first*tileSz+WindowX*1/100;}
    if (screenSize.second <= WindowY*99/100) {origin.second = WindowY/2-screenSize.second/2; TotalY = WindowY;}
    else {origin.second = WindowY*1/200; TotalY = mazeSize.second*tileSz+WindowY*1/100;}

    mainCamera.sz = make_pair(TotalX - WindowX, TotalY - WindowY);

    origin.first += adit.first;
    origin.second += adit.second;

    for (int i=0; i<mazeSize.second; i++)
    {
        for (int j=0; j<mazeSize.first; j++)
        {
            Sprite s;
            s.setPosition(Vector2f(j*tileSz + origin.first ,i*tileSz + origin.second));
            s.setScale(Vector2f(double(tileSz)/texSz, double(tileSz)/texSz));

            switch (lvl[i][j])
            {
            case 'S':
                s.setTexture(textures["Start"]);
                break;
            case '0':
                s.setTexture(textures["Empty"]);
                break;
            case '1':
                s.setTexture(textures["Wall"]);
                break;
            case '2':
                s.setTexture(textures["TunnelUD"]);
                break;
            case '3':
                s.setTexture(textures["TunnelLR"]);
                break;
            case '4':
                s.setTexture(textures["TunnelAll"]);
                break;
            case '5':
                s.setTexture(textures["Rotating1"]);
                break;
            case '6':
                s.setTexture(textures["Rotating2"]);
                break;
            case 'A':
                s.setTexture(textures["Item"]);
                break;
            case 'M':
                s.setTexture(textures["Movable"]);
                break;
            case 'I':
                s.setTexture(textures["Sliding"]);
                break;
            }

            window->draw(s);
        }
    }

    pair<int,int> space;
    space.first = origin.first/double(tileSz);
    space.second = origin.second/double(tileSz);
    if (int(origin.first)%tileSz != 0) space.first++;
    if (int(origin.second)%tileSz != 0) space.second++;

    for (int i=-space.first;i<=space.first+mazeSize.first;i++)
    {
        for (int j=-space.second;j<=-1;j++)
        {
            Sprite s;
            s.setPosition(Vector2f(i*tileSz + origin.first, j*tileSz + origin.second));
            s.setScale(Vector2f(double(tileSz)/texSz, double(tileSz)/texSz));
            s.setTexture(textures["Wall"]);
            window->draw(s);
            s.setPosition(Vector2f(i*tileSz + origin.first, (-j+mazeSize.second-1)*tileSz + origin.second));
            window->draw(s);
        }
    }
    for (int i=-space.first;i<=-1;i++)
    {
        for (int j=0;j<mazeSize.second;j++)
        {
            Sprite s;
            s.setPosition(Vector2f(i*tileSz + origin.first, j*tileSz + origin.second));
            s.setScale(Vector2f(double(tileSz)/texSz, double(tileSz)/texSz));
            s.setTexture(textures["Wall"]);
            s.setColor(Color(255,255,255));
            window->draw(s);
            s.setPosition(Vector2f((-i+mazeSize.first-1)*tileSz + origin.first , j*tileSz + origin.second));
            window->draw(s);
        }
    }

    Sprite player;
    IntRect r;
    r.left = 0;
    r.top = 0;
    r.width = plSz;
    r.height = plSz;

    player.setPosition((playerPos.second)*double(tileSz)+origin.first+(texSz-plSz)/2*tileSz/plSz, (playerPos.first)*double(tileSz)+origin.second+(texSz-plSz)*tileSz/plSz);
    player.setTexture(playerIcons[selectedIcon]);
    player.setScale(Vector2f(tileSz/plSz, tileSz/plSz));

    player.setTextureRect(r);
    window->draw(player);

    r.left = plSz;
    player.setTextureRect(r);
    player.setColor(selectedColor1);
    window->draw(player);

    r.left = 2*plSz;
    player.setTextureRect(r);
    player.setColor(selectedColor2);
    window->draw(player);
}

void updateRotations ()
{
    for (int i=0;i<dynamicLevel.size.first;i++)
    {
        for (int j=0;j<dynamicLevel.size.second;j++)
        {
            if (dynamicLevel.level[i][j] == '5')
                dynamicLevel.level[i][j] = '6';
            else if (dynamicLevel.level[i][j] == '6')
                dynamicLevel.level[i][j] = '5';
        }
    }
}

int main()
{
    loadTextures();
    loadLevels();
    loadSettings();

    ScreenX = VideoMode::getDesktopMode().width;
    ScreenY = VideoMode::getDesktopMode().height;
    WindowX = ScreenX;
    WindowY = ScreenY*7/10;

    RenderWindow window(VideoMode(WindowX, WindowY), "Click R to restart, Z to undo", Style::Close);

    //window.setVerticalSyncEnabled(true);
    Image icon;
    icon.loadFromFile("textures/window_icon.png");

    window.setIcon(256, 256, icon.getPixelsPtr());

    window.setSize(Vector2u (WindowX, WindowY));
    window.setPosition(Vector2i(-5, ScreenY*3/20));
    View vw;
    vw.setSize(Vector2f(WindowX, WindowY));
    vw.setCenter(Vector2f(WindowX/2, WindowY/2));
    window.setView(vw);

    const int animTime = 100;

    bool update = true;
    int wait=0; /// Milliseconds

    while (window.isOpen())
    {
        Clock c;
        c.restart();
        while (c.getElapsedTime().asMilliseconds() <= wait);
        wait = 0;

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                break;
            }

            if (event.type == Event::GainedFocus && event.type == Event::LostFocus)
                update = true;

            if (event.type == Event::MouseWheelScrolled)
            {
                if (Keyboard::isKeyPressed(Keyboard::LControl))
                {
                    tileSz += event.mouseWheelScroll.delta;
                    tileSz = max(tileSz, 20);
                    update = true;
                }
            }

            if (menu == GAME)
            {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z)
                {
                    if (!memory.empty())
                    {
                        updateRotations();
                        for (int i=0; i<dynamicLevel.size.first;i++)
                            dynamicLevel.level[i][memory.top().column_sz] = memory.top().row[i];
                        for (int i=0; i<dynamicLevel.size.second;i++)
                            dynamicLevel.level[memory.top().row_sz][i] = memory.top().column[i];
                        dynamicLevel.starting_position = make_pair(memory.top().row_sz, memory.top().column_sz);
                        update = true;
                        if (memory.top().gem == true) dynamicLevel.itemAmount++;
                        memory.pop();
                        break;
                    }
                }
                if (event.type == Event::KeyPressed)
                {
                    int curX = dynamicLevel.starting_position.first;
                    int curY = dynamicLevel.starting_position.second;

                    if (Keyboard::isKeyPressed(Keyboard::R))
                        menu = LOADING_LEVEL;

                    if (Keyboard::isKeyPressed(Keyboard::LControl))
                    {
                        if (Keyboard::isKeyPressed(Keyboard::Up))
                        {
                            adit.second += 5;
                            adit.second = min (adit.second, 0.0);
                            update = true;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Down))
                        {
                            adit.second -= 5;
                            adit.second = max (adit.second, -mainCamera.sz.second);
                            update = true;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Left))
                        {
                            adit.first += 5;
                            adit.first = min (adit.first, 0.0);
                            update = true;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Right))
                        {
                            adit.first -= 5;
                            adit.first = max (adit.first, -mainCamera.sz.first);
                            update = true;
                        }
                        continue;
                    }
//
                    int doBut=0;

                    if (animDir == VOID || animDir == UP)
                    {
                        if (event.key.code == Keyboard::Up) doBut=1;
                        if (event.key.code == Keyboard::Down) doBut=2;
                        if (event.key.code == Keyboard::Left) doBut=3;
                        if (event.key.code == Keyboard::Right) doBut=4;
                    }
                    else if (animDir == DOWN)
                    {
                        if (event.key.code == Keyboard::Down) doBut=2;
                        if (event.key.code == Keyboard::Up) doBut=1;
                        if (event.key.code == Keyboard::Left) doBut=3;
                        if (event.key.code == Keyboard::Right) doBut=4;
                    }
                    else if (animDir == LEFT)
                    {
                        if (event.key.code == Keyboard::Down) doBut=2;
                        if (event.key.code == Keyboard::Left) doBut=3;
                        if (event.key.code == Keyboard::Up) doBut=1;
                        if (event.key.code == Keyboard::Right) doBut=4;
                    }
                    else
                    {
                        if (event.key.code == Keyboard::Down) doBut=2;
                        if (event.key.code == Keyboard::Left) doBut=3;
                        if (event.key.code == Keyboard::Right) doBut=4;
                        if (event.key.code == Keyboard::Up) doBut=1;
                    }


                    if (doBut == 1)
                    {
                        animDir = UP;
                        while (1)
                        {
                            curX--;
                            if (curX < 0) break;

                            if (dynamicLevel.level[curX][curY] == '0')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'A')
                            {
                                addToMemory(true);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                dynamicLevel.level[curX][curY] = '0';
                                dynamicLevel.itemAmount--;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'S')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                if (dynamicLevel.itemAmount == 0)
                                {
                                    window.close();
                                }
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == '2' || dynamicLevel.level[curX][curY] == '4' || dynamicLevel.level[curX][curY] == '5')
                            {
                                if (dynamicLevel.level[curX-1][curY] == 'M') break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'M')
                            {
                                while (dynamicLevel.level[curX][curY] == 'M' && curX >= 0)
                                    curX--;

                                if (curX == -1) break;
                                else if (dynamicLevel.level[curX][curY] == '0')
                                {
                                    addToMemory(false);
                                    dynamicLevel.level[curX][curY] = 'M';
                                    dynamicLevel.starting_position.first--;
                                    dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                    update=true;
                                    updateRotations();
                                }
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'I')
                            {
                                if (dynamicLevel.level[curX-1][curY] != '0') break;
                                while (dynamicLevel.level[curX-1][curY] == '0' && curX-1 >= 0)
                                    curX--;
                                addToMemory(false);
                                dynamicLevel.level[curX][curY] = 'I';
                                dynamicLevel.starting_position.first--;
                                dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                update = true;
                                updateRotations();
                                break;
                            }
                            else break;
                        }
                    }
                    else if (doBut == 2)
                    {
                        animDir = DOWN;
                        while (1)
                        {
                            curX++;
                            if (curX >= dynamicLevel.size.first) break;

                            if (dynamicLevel.level[curX][curY] == '0')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'A')
                            {
                                addToMemory(true);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                dynamicLevel.level[curX][curY] = '0';
                                dynamicLevel.itemAmount--;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'S')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                if (dynamicLevel.itemAmount == 0)
                                {
                                    window.close();
                                }
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == '2' || dynamicLevel.level[curX][curY] == '4' || dynamicLevel.level[curX][curY] == '5')
                            {
                                if (dynamicLevel.level[curX+1][curY] == 'M') break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'M')
                            {
                                while (dynamicLevel.level[curX][curY] == 'M' && curX < dynamicLevel.size.first)
                                    curX++;

                                if (curX == dynamicLevel.size.first) break;
                                else if (dynamicLevel.level[curX][curY] == '0')
                                {
                                    addToMemory(false);
                                    dynamicLevel.level[curX][curY] = 'M';
                                    dynamicLevel.starting_position.first++;
                                    dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                    update=true;
                                    updateRotations();
                                }
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'I')
                            {
                                if (dynamicLevel.level[curX+1][curY] != '0') break;
                                while (dynamicLevel.level[curX+1][curY] == '0' && curX+1 < dynamicLevel.size.first)
                                    curX++;
                                addToMemory(false);
                                dynamicLevel.level[curX][curY] = 'I';
                                dynamicLevel.starting_position.first++;
                                dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                update = true;
                                updateRotations();
                                break;
                            }
                            else break;
                        }
                    }
                    else if (doBut == 3)
                    {
                        animDir = LEFT;
                        while (1)
                        {
                            curY--;
                            if (curY < 0) break;

                            if (dynamicLevel.level[curX][curY] == '0')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'A')
                            {
                                addToMemory(true);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                dynamicLevel.level[curX][curY] = '0';
                                dynamicLevel.itemAmount--;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'S')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                if (dynamicLevel.itemAmount == 0)
                                {
                                    window.close();
                                }
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == '3' || dynamicLevel.level[curX][curY] == '4' || dynamicLevel.level[curX][curY] == '6')
                            {
                                if (dynamicLevel.level[curX][curY-1] == 'M') break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'M')
                            {
                                while (dynamicLevel.level[curX][curY] == 'M' && curY >= 0)
                                    curY--;

                                if (curY == -1) break;
                                else if (dynamicLevel.level[curX][curY] == '0')
                                {
                                    addToMemory(false);
                                    dynamicLevel.level[curX][curY] = 'M';
                                    dynamicLevel.starting_position.second--;
                                    dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                    updateRotations();
                                    update=true;
                                }
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'I')
                            {
                                if (dynamicLevel.level[curX][curY-1] != '0') break;
                                while (dynamicLevel.level[curX][curY-1] == '0' && curY-1 >= 0)
                                    curY--;
                                addToMemory(false);
                                dynamicLevel.level[curX][curY] = 'I';
                                dynamicLevel.starting_position.second--;
                                dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                update = true;
                                updateRotations();
                                break;
                            }
                            else break;
                        }
                    }
                    else if (doBut == 4)
                    {
                        animDir = RIGHT;
                        while (1)
                        {
                            curY++;
                            if (curY >= dynamicLevel.size.second) break;

                            if (dynamicLevel.level[curX][curY] == '0')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'A')
                            {
                                addToMemory(true);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                dynamicLevel.level[curX][curY] = '0';
                                dynamicLevel.itemAmount--;
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'S')
                            {
                                addToMemory(false);
                                dynamicLevel.starting_position = make_pair(curX, curY);
                                wait = animTime;
                                if (dynamicLevel.itemAmount == 0)
                                {
                                    window.close();
                                }
                                updateRotations();
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == '3' || dynamicLevel.level[curX][curY] == '4' || dynamicLevel.level[curX][curY] == '6')
                            {
                                if (dynamicLevel.level[curX][curY+1] == 'M') break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'M')
                            {
                                while (dynamicLevel.level[curX][curY] == 'M' && curY < dynamicLevel.size.second)
                                    curY++;

                                if (curY == dynamicLevel.size.second) break;
                                else if (dynamicLevel.level[curX][curY] == '0')
                                {
                                    addToMemory(false);
                                    dynamicLevel.level[curX][curY] = 'M';
                                    dynamicLevel.starting_position.second++;
                                    dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                    updateRotations();
                                    update=true;
                                }
                                break;
                            }
                            else if (dynamicLevel.level[curX][curY] == 'I')
                            {
                                if (dynamicLevel.level[curX][curY+1] != '0') break;
                                while (dynamicLevel.level[curX][curY+1] == '0' && curY+1 < dynamicLevel.size.second)
                                    curY++;
                                addToMemory(false);
                                dynamicLevel.level[curX][curY] = 'I';
                                dynamicLevel.starting_position.second++;
                                dynamicLevel.level[int(dynamicLevel.starting_position.first)][int(dynamicLevel.starting_position.second)] = '0';
                                update = true;
                                updateRotations();
                                break;
                            }
                            else break;
                        }
                    }
                    update = true;
                }
            }
        }
        if (update == true)
        {
            update = false;
            window.clear();
            if (menu == LOADING_LEVEL)
            {
                dynamicLevel = levels[currentLevel];
                while (!memory.empty()) memory.pop();
                menu = GAME;
                update = true;
            }
            else if (menu == GAME)
            {
                drawLevel(&window, dynamicLevel.level, dynamicLevel.size, dynamicLevel.starting_position);

                window.display();
            }
            else if (menu == LEVEL_BROWSE)
            {

            }
        }
    }

    cleanTextures();
    cleanLevels();
}
