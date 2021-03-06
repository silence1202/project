#include <conio.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <windows.h>
// #define isKeyPressed(nVirtKey) (GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0
using namespace std;
const int little_second = 1000;
const int Maxsize = 20;
int count = 0, boom_count = 0; //计时
int cd_A, cd_B, cd_A_boom, cd_B_boom;
int cd_R, cd_r, cd_R_boom, cd_r_boom; //前进的冷却cd，放炸弹的冷却cd。随count每次自增一，直到超过预设冷却时间，保证了每两次行进或放炸弹之间的时间间隔下限
bool A_life, B_life, R_life, r_life;  //生命
bool change;                          //判断人物位置是否有变化
int R_routine_x[21] = {1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 4, 5, 5, 5, 4, 4, 4, 3, 2};
int R_routine_y[21] = {17, 16, 15, 14, 13, 12, 11, 11, 11, 12, 13, 13, 13, 14, 15, 15, 16, 17, 17, 17};
int r_routine_x[21] = {8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 6, 5, 4, 4, 4, 5, 5, 5, 6, 7};
int r_routine_y[21] = {2, 3, 4, 5, 6, 7, 8, 8, 8, 7, 6, 6, 6, 5, 4, 3, 2, 2, 2, 2}; //机器人预设巡逻路线
int R_aim;
int r_aim;                                         //当前机器人行进目标，指向路线数组下标
bool be_stopped[4] = {false, false, false, false}; //判断炸弹爆炸有无被墙阻挡
bool over;                                         //判断游戏是否结束
void display();
bool safety(int x, int y);
void robot_walking();
class Map
{

public:
    char node[11][21];
    char inside[11][21];
    Map()
    { //地图
        strcpy(this->node[0], "####################");
        strcpy(this->node[1], "#** ***#**#** *** ##");
        strcpy(this->node[2], "#*   #** ***# #*   #");
        strcpy(this->node[3], "#** ##**#   **### ##");
        strcpy(this->node[4], "##******### **#  **#");
        strcpy(this->node[5], "#**  #** ###******##");
        strcpy(this->node[6], "## ###**   #**## **#");
        strcpy(this->node[7], "#   *# #*** **#   *#");
        strcpy(this->node[8], "## *** **#**#*** **#");
        strcpy(this->node[9], "####################");
        //地图软墙内部道具
        strcpy(this->inside[0], "####################");
        strcpy(this->inside[1], "#2* 1**#**#2* *1* ##");
        strcpy(this->inside[2], "#*   #** 1**# #*   #");
        strcpy(this->inside[3], "#** ##2*#   **### ##");
        strcpy(this->inside[4], "##*11***### *1#  **#");
        strcpy(this->inside[5], "#**  #1* ###***11*##");
        strcpy(this->inside[6], "## ###**   #*2## **#");
        strcpy(this->inside[7], "#   *# #**1 **#   *#");
        strcpy(this->inside[8], "## *1* *2#**#**1 *2#");
        strcpy(this->inside[9], "####################");
    }
};
Map map;
bool can_go(int x, int y) //判断能否通过
{
    return map.node[x][y] != '*' && map.node[x][y] != '#' && map.node[x][y] != 'O';
}
class Bomb
{ //炸弹
public:
    int power;                    //炸弹威力
    std::pair<int, int> location; //位置
    int time_boom;                //爆炸剩余时间
    int boom_lasting;             //爆炸特效持续时间
    int *master;                  //炸弹的放置者
    Bomb()
    {
        time_boom = 0;
        power = 1;
    }
    void placing(int x, int y) //放置炸弹
    {
        time_boom = 3;
        location.first = x;
        location.second = y;
        map.node[x][y] = 'O';
        display();
    }
    bool check(int x, int y, int i) //查看炸弹爆炸波及情况
    {
        switch (map.node[x][y])
        {
        case 'A':
            A_life = false;
            *master += 3000;
            return true;
            break;
        case 'B':
            B_life = false;
            *master += 3000;
            return true;
            break;
        case 'R':
            R_life = false;
            *master += 3000;
            return true;
            break;
        case 'r':
            r_life = false;
            *master += 3000;
            return true;
            break;
        case ' ':
            return true;
            break;
        case '*':
            *master += 30;
            return true;
            break;
        case '#':
            be_stopped[i] = true;
            return false;
            break;
        default:
            break;
        }
        return false;
    }
    void boom() //爆炸
    {
        int x = this->location.first;
        int y = this->location.second;

        map.node[x][y] = '@';
        for (int i = 1; i <= power; i++)
        {
            if (!be_stopped[0] && check(x - i, y, 0))
            {
                map.node[x - i][y] = 'M';
            }
            if (!be_stopped[1] && check(x + i, y, 1))
            {
                map.node[x + i][y] = 'W';
            }
            if (!be_stopped[2] && check(x, y - i, 2))
            {
                map.node[x][y - i] = '<';
            }
            if (!be_stopped[3] && check(x, y + i, 3))
            {
                map.node[x][y + i] = '>';
            }
        }
        be_stopped[0] = false;
        be_stopped[1] = false;
        be_stopped[2] = false;
        be_stopped[3] = false;
        display();
        boom_lasting = 1;
    }
    void disappear() //爆炸特效散去
    {
        int x = this->location.first;
        int y = this->location.second;
        map.node[x][y] = ' ';
        for (int i = 1; i <= power; i++)
        {
            if (map.node[x - i][y] == 'M')
            {
                map.node[x - i][y] = ' ';
                if (map.inside[x - i][y] > '0')
                    map.node[x - i][y] = map.inside[x - i][y];
            }
            if (map.node[x + i][y] == 'W')
            {
                map.node[x + i][y] = ' ';
                if (map.inside[x + i][y] > '0')
                    map.node[x + i][y] = map.inside[x + i][y];
            }
            if (map.node[x][y - i] == '<')
            {
                map.node[x][y - i] = ' ';
                if (map.inside[x][y - i] > '0')
                    map.node[x][y - i] = map.inside[x][y - i];
            }
            if (map.node[x][y + i] == '>')
            {
                map.node[x][y + i] = ' ';
                if (map.inside[x][y + i] > '0')
                    map.node[x][y + i] = map.inside[x][y + i];
            }
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
    int aim;   //机器人当前目标
    int speed; //速度
    int score; //分数
    // 初 始 化
    Player(int x, int y, char symbol)
    {
        this->location.first = x;
        this->location.second = y;
        this->symbol = symbol;
        map.node[x][y] = symbol;
        aim = 0;
        speed = 10;
        score = 0;
    }

    // 更 新 位 置
    void update_location(int type)
    {
        int x = this->location.first;
        int y = this->location.second;
        int q = 0;
        if (map.node[x][y] == this->symbol)
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
        if (can_go(x, y))
        {
            if (q)
                map.node[this->location.first][this->location.second] = ' ';
            this->location.first = x;
            this->location.second = y;
            if (map.inside[x][y] == '1')
            {
                map.inside[x][y] = '0';
                score += 100;
                this->speed--; // speed实为每次行走间隔下限，实际上的速度应为1/speed
            }
            if (map.inside[x][y] == '2')
            {
                map.inside[x][y] = '0';
                score += 100;
                this->bomb.power++; //炸弹威力增加
            }
            map.node[x][y] = this->symbol;
            change = true;
        }

        // if (change)
        // display();
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
        bomb.master = &score;
    }
    //炸弹倒计时
    void bomb_booming() //炸弹爆炸
    {
        if (bomb.time_boom > 0)
        {
            bomb.time_boom--;
            if (bomb.time_boom == 0)
            {
                bomb.boom();
                if (this->get_location() == bomb.location)
                {
                    if (this->symbol == 'A')
                        A_life = false;
                    else if (this->symbol == 'B')
                        B_life = false;
                    else if (this->symbol == 'R')
                        R_life = false;
                    else if (this->symbol == 'r')
                        r_life = false;
                }
            }
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
    bool bomb_exist() //判断炸弹存在
    {
        if (bomb.boom_lasting == 0 && bomb.time_boom == 0)
        {
            return false;
        }
        else
            return true;
    }

    bool bomb_danger(int x, int y) //判断是否处于炸弹范围内
    {
        std::pair<int, int> p = this->bomb.location; //炸弹可能位置
        if (this->bomb_exist())
        {
            if (p.first == x && abs(p.second - y) <= this->bomb.power && map.node[x][(p.second + y) / 2] != '#')
                return true;
            if (p.second == y && abs(p.first - x) <= this->bomb.power && map.node[(p.first + x) / 2][y] != '#')
                return true;
        }
        return false;
    }
    bool aim_to(int x0, int y0) //机器人目标位置
    {
        std::pair<int, int> cur = this->get_location();
        change = false;
        int x = cur.first, y = cur.second;
        //可行进且是向着自己的目标前进，则前进，若是软墙或者人，则放置炸弹
        if (!change && x > x0 && safety(x - 1, y))
        {
            if (map.node[x - 1][y] == ' ' || (map.node[x - 1][y] > '0' && map.node[x - 1][y] < '9'))
                this->update_location(1), change = true;
            else if (map.node[x - 1][y] != '#' && !this->bomb_exist())
            {
                this->bomb_placing();
                robot_walking();
                aim = (aim + 19) % 20;
                change = true;
            }
        }

        if (!change && x < x0 && safety(x + 1, y))
        {
            if (map.node[x + 1][y] == ' ' || (map.node[x + 1][y] > '0' && map.node[x + 1][y] < '9'))
                this->update_location(3), change = true;
            else if (map.node[x + 1][y] != '#' && !this->bomb_exist())
            {
                this->bomb_placing();
                robot_walking();
                aim = (aim + 19) % 20;
                change = true;
            }
        }
        if (!change && y > y0 && safety(x, y - 1))
        {
            if (map.node[x][y - 1] == ' ' || (map.node[x][y - 1] > '0' && map.node[x][y - 1] < '9'))
                this->update_location(2), change = true;
            else if (map.node[x][y - 1] != '#' && !this->bomb_exist())
            {
                this->bomb_placing();
                robot_walking();
                aim = (aim + 19) % 20;
                change = true;
            }
        }
        if (!change && y < y0 && safety(x, y + 1))
        {
            if (map.node[x][y + 1] == ' ' || (map.node[x][y + 1] > '0' && map.node[x][y + 1] < '9'))
                this->update_location(4), change = true;
            else if (map.node[x][y + 1] != '#' && !this->bomb_exist())
            {
                this->bomb_placing();
                robot_walking();
                aim = (aim + 19) % 20;
                change = true;
            }
        }
        return change;
    }
};
Player A(2, 3, 'A');
Player B(7, 16, 'B');
Player R(2, 17, 'R');
Player r(7, 2, 'r');

bool safety(int x, int y) //判断是否安全
{
    return !A.bomb_danger(x, y) && !B.bomb_danger(x, y) && !R.bomb_danger(x, y) && !r.bomb_danger(x, y);
}
void Robot_walking() // R机器人的行进
{
    if (!R_life)
        return;
    cd_R = 0;
    std::pair<int, int> cur = R.get_location();
    int x = cur.first, y = cur.second;
    // printf("%d", safety(x, y));
    if (!safety(x, y)) //不安全->逃
    {
        if (can_go(x - 1, y) && (safety(x - 1, y) || map.node[x - 2][y] == ' ' || map.node[x - 1][y - 1] == ' ' || map.node[x - 1][y + 1] == ' '))
            R.update_location(1); //往上走
        else if (can_go(x, y - 1) && (safety(x, y - 1) || map.node[x][y - 2] == ' ' || map.node[x - 1][y - 1] == ' ' || map.node[x + 1][y - 1] == ' '))
            R.update_location(2); //往左走
        else if (can_go(x + 1, y) && (safety(x + 1, y) || map.node[x + 2][y] == ' ' || map.node[x + 1][y - 1] == ' ' || map.node[x + 1][y + 1] == ' '))
            R.update_location(3); //往下走
        else if (can_go(x, y + 1) && (safety(x, y + 1) || map.node[x][y + 2] == ' ' || map.node[x - 1][y + 1] == ' ' || map.node[x + 1][y + 1] == ' '))
            R.update_location(4); //往右走
        else
            R.update_location((rand() % 4) + 1); //随机走
    }
    else //安全->向着目标行进
    {
        if (x == R_routine_x[R.aim] && y == R_routine_y[R.aim])
        {
            R.aim = (R.aim + 1) % 20;
        }
        int x0 = R_routine_x[R.aim];
        int y0 = R_routine_y[R.aim];
        //行进后刷新位置防止显示错误
        if (R_life && !R.aim_to(x0, y0))
            map.node[R.get_location().first][R.get_location().second] = 'R';
    }
}
void robot_walking() // r机器人的行进
{
    if (!r_life)
        return;
    cd_r = 0;
    std::pair<int, int> cur = r.get_location();
    int x = cur.first, y = cur.second;
    // printf("%d", safety(x, y));
    if (!safety(x, y)) //逃
    {
        if (can_go(x - 1, y) && (safety(x - 1, y) || map.node[x - 2][y] == ' ' || map.node[x - 1][y - 1] == ' ' || map.node[x - 1][y + 1] == ' '))
            r.update_location(1); //往上走
        else if (can_go(x, y - 1) && (safety(x, y - 1) || map.node[x][y - 2] == ' ' || map.node[x - 1][y - 1] == ' ' || map.node[x + 1][y - 1] == ' '))
            r.update_location(2); //往左走
        else if (can_go(x + 1, y) && (safety(x + 1, y) || map.node[x + 2][y] == ' ' || map.node[x + 1][y - 1] == ' ' || map.node[x + 1][y + 1] == ' '))
            r.update_location(3); //往下走
        else if (can_go(x, y + 1) && (safety(x, y + 1) || map.node[x][y + 2] == ' ' || map.node[x - 1][y + 1] == ' ' || map.node[x + 1][y + 1] == ' '))
            r.update_location(4); //往右走
        else
            r.update_location((rand() % 4) + 1); //随机走
    }
    else
    {
        if (x == r_routine_x[r.aim] && y == r_routine_y[r.aim])
        {
            r.aim = (r.aim + 1) % 20;
        }
        int x0 = r_routine_x[r.aim];
        int y0 = r_routine_y[r.aim];
        //行进后刷新位置防止显示错误
        if (r_life && !r.aim_to(x0, y0))
            map.node[r.get_location().first][r.get_location().second] = 'r';
    }
}

void init()
{
    cd_A = 6;
    cd_B = 6;
    cd_R = 6;
    cd_r = 6;
    cd_A_boom = 56;
    cd_B_boom = 56;
    cd_R_boom = 56;
    cd_r_boom = 56;
    A_life = true;
    B_life = true;
    R_life = true;
    r_life = true;
    over = false;
}
void display()
{
    system("cls");
    if (!r_life && !R_life)
    {
        over = true;
        strcpy(map.node[3], "/~~~~~~~~~~~~~~~~~~\\");
        strcpy(map.node[4], "|~~>>>YOU WIN!<<<~~|");
        strcpy(map.node[5], "\\~~~~~~~~~~~~~~~~~~/");
    }
    else if (!A_life && !B_life)
    {
        over = true;
        strcpy(map.node[3], "/==================\\");
        strcpy(map.node[4], "|--<<<YOU LOSE>>>--|");
        strcpy(map.node[5], "\\==================/");
    }
    for (int i = 0; i < 10; i++)
    {
        puts(map.node[i]);
    }
    if (A_life == false)
        A.score = 0;
    if (B_life == false)
        B.score = 0;
    if (R_life == false)
        R.score = 0;
    if (r_life == false)
        r.score = 0;
    printf("     --------SCORE--------\n");
    printf("Player A : %04d |Player B : %04d\n", A.score, B.score);
    printf("Robot R  : %04d |Robot r  : %04d", R.score, r.score);
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
            if (cd_A > A.speed && A_life)
                cd_A = 0, A.update_location(1), display();
            break;
        case 'a':
            if (cd_A > A.speed && A_life)
                cd_A = 0, A.update_location(2), display();
            break;
        case 's':
            if (cd_A > A.speed && A_life)
                cd_A = 0, A.update_location(3), display();
            break;
        case 'd':
            if (cd_A > A.speed && A_life)
                cd_A = 0, A.update_location(4), display();
            break;
        case 'i':
            if (cd_B > B.speed && B_life)
                cd_B = 0, B.update_location(1), display();
            break;
        case 'j':
            if (cd_B > B.speed && B_life)
                cd_B = 0, B.update_location(2), display();
            break;
        case 'k':
            if (cd_B > B.speed && B_life)
                cd_B = 0, B.update_location(3), display();
            break;
        case 'l':
            if (cd_B > B.speed && B_life)
                cd_B = 0, B.update_location(4), display();
            break;
        case ' ':
            if (A_life && cd_A_boom > 0 && !A.bomb_exist())
                cd_A_boom = 0, A.bomb_placing();
            break;
        case '\r':
            if (B_life && cd_B_boom > 0 && !B.bomb_exist())
                cd_B_boom = 0, B.bomb_placing();
            break;
        default:
            break;
        }
    }
}
void deal_with_timer() //炸弹时间
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
void deal_with_cd() //前进cd与放炸弹cd
{
    cd_A++;
    cd_B++;
    cd_A_boom++;
    cd_B_boom++;
    cd_R++;
    cd_r++;
    cd_R_boom++;
    cd_r_boom++;
    if (r_life && cd_r > r.speed)
    {
        robot_walking();
    }
    if (R_life && cd_R > R.speed)
    {
        Robot_walking();
    }
    if (cd_r * cd_R == 0)
        display();
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
        if (count == little_second) //大致0.1s一次
        {
            deal_with_cd();
            count = 0;
            // printf("!");
        }
        if (boom_count == little_second * 20) //大致两秒一次
        {
            deal_with_timer();
            boom_count = 0;
        }
        if (over)
            break;
    }
    return 0;
}