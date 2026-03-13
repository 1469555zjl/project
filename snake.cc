#include <iostream>
#include <windows.h>
#include <deque>
#include <vector>
#include <ctime>
#include <conio.h>

using namespace std;

// 游戏配置
const int Size = 1;    // 每个格子大小
const int Width = 30;  // 横向格子数
const int Height = 20; // 纵向格子数

enum direction
{
    Up,
    Down,
    Left,
    Right,
    Stop
};

// 全局变量 (保持与窗口版一致)
deque<pair<int, int>> snake;
direction dir = Stop;
int foodX, foodY;
int score = 0;
bool gameover = false;

// 隐藏光标并设置光标位置 (用于模拟窗口刷新)
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// 重置游戏状态 (逻辑与窗口版完全相同)
void resetGame()
{
    snake.clear();
    snake.push_back({Width / 2, Height / 2});
    snake.push_back({Width / 2 - 1, Height / 2});
    snake.push_back({Width / 2 - 2, Height / 2});

    dir = Stop;
    score = 0;
    gameover = false;

    foodX = rand() % Width;
    foodY = rand() % Height;
    system("cls"); // 仅在重置时清屏
}

// 游戏更新
void update()
{
    if (dir == Stop || gameover)
        return;

    int newX = snake.front().first;
    int newY = snake.front().second;

    if (dir == Up)
        newY--;
    else if (dir == Down)
        newY++;
    else if (dir == Left)
        newX--;
    else if (dir == Right)
        newX++;

    // 撞墙或撞自己
    if (newX < 0 || newX >= Width || newY < 0 || newY >= Height)
    {
        gameover = true;
        return;
    }
    for (auto &s : snake)
    {
        if (s.first == newX && s.second == newY)
        {
            gameover = true;
            return;
        }
    }

    snake.push_front({newX, newY});

    if (newX == foodX && newY == foodY)
    {
        score += 10;
        foodX = rand() % Width;
        foodY = rand() % Height;
    }
    else
    {
        snake.pop_back();
    }
}

void draw()
{
    gotoxy(0, 0);
    for (int i = 0; i < Width + 2; i++)
        cout << "#";
    cout << endl;

    for (int y = 0; y < Height; y++)
    {
        cout << "#";
        for (int x = 0; x < Width; x++)
        {
            if (x == snake.front().first && y == snake.front().second)
                cout << "O"; // 蛇头
            else if (x == foodX && y == foodY)
                cout << "*"; // 食物
            else
            {
                bool isBody = false;
                for (size_t i = 1; i < snake.size(); i++)
                {
                    if (snake[i].first == x && snake[i].second == y)
                    {
                        cout << "o"; // 蛇身
                        isBody = true;
                        break;
                    }
                }
                if (!isBody)
                    cout << " ";
            }
        }
        cout << "#" << endl;
    }
    for (int i = 0; i < Width + 2; i++)
        cout << "#";
    cout << endl;

    // 显示分数和状态
    printf("Score: %-10d\n", score);
    if (gameover)
    {
        printf("GAME OVER! Press SPACE to Restart.    \n");
    }
    else
    {
        printf("WASD to Move, SPACE to Start.         \n");
    }
}

// 输入处理
void input()
{
    if (_kbhit())
    {
        int key = _getch();
        if (gameover && key == ' ')
        {
            resetGame();
        }

        switch (key)
        {
        case 'w':
        case 'W':
        case 72:
            if (dir != Down)
                dir = Up;
            break;
        case 's':
        case 'S':
        case 80:
            if (dir != Up)
                dir = Down;
            break;
        case 'a':
        case 'A':
        case 75:
            if (dir != Right)
                dir = Left;
            break;
        case 'd':
        case 'D':
        case 77:
            if (dir != Left)
                dir = Right;
            break;
        }
    }
}

int main()
{
    srand(time(0));
    hideCursor();
    resetGame();

    // 游戏主循环
    while (true)
    {
        draw();
        input();
        update();
        Sleep(150); // 控制速度，越大越慢
    }

    return 0;
}