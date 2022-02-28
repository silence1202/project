#include <conio.h>
#include <iostream>
#include <cstdio>
#include <cstring>
// #define isKeyPressed(nVirtKey) (GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0
using namespace std;
const int little_second = 1000;
const int Maxsize = 20;
int count = 0, boom_count = 0;
int cd_A, cd_B, cd_A_boom, cd_B_boom, speed_A, speed_B;
bool A_life, B_life, R_life, r_life;
bool change;

void display();
class Map
{

public:
    char node[11][21];
    Map()
    {
        strcpy(this->node[0], "####################");
        strcpy(this->node[1], "#** ***#**#****** ##");
        strcpy(this->node[2], "#*   #******###*   #");
        strcpy(this->node[3], "#** ##**#   **### ##");
        strcpy(this->node[4], "##******### **#  **#");
        strcpy(this->node[5], "#**  #** ###******##");
        strcpy(this->node[6], "## ###**   #**## **#");
        strcpy(this->node[7], "#   *###******#   *#");
        strcpy(this->node[8], "## ******#**#*** **#");
        strcpy(this->node[9], "####################");
    }
};
Map map;
class Bomb
{
public:
    int power;
    std::pair<int, int> location;
    int time_boom;
    int boom_lasting;
    Bomb()
    {
        time_boom = 0;
        power = 1;
    }
    void placing(int x, int y)
    {
        time_boom = 3;
        location.first = x;
        location.second = y;
        map.node[x][y] = 'O';
        display();
    }
    bool check(int x, int y)
    {
        switch (map.node[x][y])
        {
        case 'A':
            A_life = false;
            return true;
            break;
        case 'B':
            B_life = false;
            return true;
            break;
        case 'R':
            R_life = false;
            return true;
            break;
        case 'r':
            r_life = false;
            return true;
            break;
        case ' ':
            return true;
            break;
        case '*':
            return true;
            break;
        default:
            break;
        }
        return false;
    }
    void boom()
    {
        int x = this->location.first;
        int y = this->location.second;
        map.node[x][y] = '@';
        check(x, y);
        for (int i = 1; i <= power; i++)
        {
            if (check(x - i, y))
            {
                map.node[x - i][y] = 'M';
            }
            if (check(x + i, y))
            {
                map.node[x + i][y] = 'W';
            }
            if (check(x, y - i))
            {
                map.node[x][y - i] = '<';
            }
            if (check(x, y + i))
            {
                map.node[x][y + i] = '>';
            }
        }
        display();
        boom_lasting = 1;
    }
    void disappear()
    {
        int x = this->location.first;
        int y = this->location.second;
        map.node[x][y] = ' ';
        if (map.node[x - 1][y] == 'M')
        {
            map.node[x - 1][y] = ' ';
        }
        if (map.node[x + 1][y] == 'W')
        {
            map.node[x + 1][y] = ' ';
        }
        if (map.node[x][y - 1] == '<')
        {
            map.node[x][y - 1] = ' ';
        }
        if (map.node[x][y + 1] == '>')
        {
            map.node[x][y + 1] = ' ';
        }
        display();
    }
};

class Player
{
private:
    // 现 在 的 位 置
    std::pair<int, int> location;
    // 代 表 该 玩 家 的 字 符
    char symbol;
    Bomb bomb;

public:
    // 初 始 化
    Player(int x, int y, char symbol)
    {
        this->location.first = x;
        this->location.second = y;
        this->symbol = symbol;
        map.node[x][y] = symbol;
    }

    // 更 新 位 置
    void update_location(int type)
    {
        int x = this->location.first;
        int y = this->location.second;
        int q = 0;
        if (map.node[x][y] == 'A' || map.node[x][y] == 'B' || map.node[x][y] == 'R')
            q = 1;
        switch (type)
        {
        case 1: //上
            x--;
            break;
        case 2: //左
            y--;
            break;
        case 3: //下
            x++;
            break;
        case 4: //右
            y++;
            break;
        default:
            break;
        }
        change = false;
        if (map.node[x][y] == ' ')
        {
            if (q)
                map.node[this->location.first][this->location.second] = ' ';
            this->location.first = x;
            this->location.second = y;
            map.node[x][y] = this->symbol;
            change = true;
        }

        if (change)
            display();
    }

    // 获 取 位 置
    std::pair<int, int> get_location()
    {
        return this->location;
    }
    //放炸弹
    void bomb_placing() //放置炸弹
    {
        std::pair<int, int> p = get_location();
        bomb.placing(p.first, p.second);
    }
    //炸弹倒计时
    void bomb_booming() //炸弹爆炸
    {
        if (bomb.time_boom > 0)
        {
            bomb.time_boom--;
            if (bomb.time_boom == 0)
                bomb.boom();
        }
    }
    void bomb_disappear() //炸弹爆炸特效消去
    {
        if (bomb.boom_lasting > 0)
        {
            bomb.boom_lasting--;
            if (bomb.boom_lasting == 0)
                bomb.disappear();
        }
    }
    bool bomb_exist() //炸弹存在
    {
        if (bomb.boom_lasting == 0 && bomb.time_boom == 0)
        {
            return false;
        }
        else
            return true;
    }
};
Player A(2, 3, 'A');
Player B(7, 16, 'B');
Player R(2, 17, 'R');
Player r(7, 2, 'r');
void init()
{
    cd_A = 6;
    cd_B = 6;
    speed_A = 5;
    speed_B = 5;
    cd_A_boom = 56;
    cd_B_boom = 56;
}
void display()
{
    system("cls");
    for (int i = 0; i < 10; i++)
    {
        puts(map.node[i]);
    }
}
void deal_with_input()
{
    char ch;
    if (_kbhit())
    {                  //如 果 有 按 键 按 下， 则_kbhit () 函 数 返 回 真
        ch = _getch(); //使 用_getch() 函 数 获 取 按 下 的 键 值
        switch (ch)
        {
        case 'w':
            if (cd_A > speed_A)
                cd_A = 0, A.update_location(1);
            break;
        case 'a':
            if (cd_A > speed_A)
                cd_A = 0, A.update_location(2);
            break;
        case 's':
            if (cd_A > speed_A)
                cd_A = 0, A.update_location(3);
            break;
        case 'd':
            if (cd_A > speed_A)
                cd_A = 0, A.update_location(4);
            break;
        case 'i':
            if (cd_B > speed_B)
                cd_B = 0, B.update_location(1);
            break;
        case 'j':
            if (cd_B > speed_B)
                cd_B = 0, B.update_location(2);
            break;
        case 'k':
            if (cd_B > speed_B)
                cd_B = 0, B.update_location(3);
            break;
        case 'l':
            if (cd_B > speed_B)
                cd_B = 0, B.update_location(4);
            break;
        case ' ':
            if (cd_A_boom > 55 && !A.bomb_exist())
                cd_A_boom = 0, A.bomb_placing();
            break;
        case '\r':
            if (cd_B_boom > 55 && !B.bomb_exist())
                cd_B_boom = 0, B.bomb_placing();
            break;
        default:
            break;
        }
    }
}
void deal_with_timer()
{
    A.bomb_booming();
    A.bomb_disappear();
    B.bomb_booming();
    B.bomb_disappear();
    R.bomb_booming();
    R.bomb_disappear();
    r.bomb_booming();
    r.bomb_disappear();
}
int main()
{
    init();
    display();
    while (1)
    {
        deal_with_input();
        count++;
        boom_count++;
        if (count == little_second)
        {
            cd_A++;
            cd_B++;
            cd_A_boom++;
            cd_B_boom++;
            count = 0;
            printf("!");
        }
        if (boom_count == little_second * 20)
        {
            deal_with_timer();
            boom_count = 0;
        }
    }

    return 0;
}