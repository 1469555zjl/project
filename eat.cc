#include <windows.h>
#include <deque>
#include <vector>
#include <ctime>

using namespace std;

// 游戏配置
const int Size = 20;   // 每个格子的大小
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

// 全局变量
deque<pair<int, int>> snake;
direction dir = Stop;
int foodX, foodY;
int score = 0;
bool gameover = false;

// 重置游戏状态
void resetGame()
{
    snake.clear();
    // 初始蛇位置
    snake.push_back({Width / 2, Height / 2});
    snake.push_back({Width / 2 - 1, Height / 2});
    snake.push_back({Width / 2 - 2, Height / 2});

    dir = Stop;
    score = 0;
    gameover = false;

    // 随机生成食物
    foodX = rand() % Width;
    foodY = rand() % Height;
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

// 窗口过程函数
LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
    {
        // 如果游戏结束，按空格重新开始
        if (gameover && (wParam == VK_SPACE))
        {
            resetGame();
        }

        switch (wParam)
        {
        case VK_UP:
            if (dir != Down)
                dir = Up;
            break;
        case VK_DOWN:
            if (dir != Up)
                dir = Down;
            break;
        case VK_LEFT:
            if (dir != Right)
                dir = Left;
            break;
        case VK_RIGHT:
            if (dir != Left)
                dir = Right;
            break;
        case 'W':
            if (dir != Down)
                dir = Up;
            break;
        case 'S':
            if (dir != Up)
                dir = Down;
            break;
        case 'A':
            if (dir != Right)
                dir = Left;
            break;
        case 'D':
            if (dir != Left)
                dir = Right;
            break;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 1. 画背景 (深灰色)
        HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 30));
        FillRect(hdc, &ps.rcPaint, bgBrush);
        DeleteObject(bgBrush);

        // 2. 画食物 (红色)
        HBRUSH foodBrush = CreateSolidBrush(RGB(255, 0, 0));
        RECT foodRect = {foodX * Size, foodY * Size, (foodX + 1) * Size, (foodY + 1) * Size};
        FillRect(hdc, &foodRect, foodBrush);
        DeleteObject(foodBrush);

        // 3. 画蛇 (绿色)
        HBRUSH headBrush = CreateSolidBrush(RGB(0, 255, 0));
        HBRUSH bodyBrush = CreateSolidBrush(RGB(0, 180, 0));
        for (size_t i = 0; i < snake.size(); i++)
        {
            RECT r = {snake[i].first * Size, snake[i].second * Size,
                      (snake[i].first + 1) * Size, (snake[i].second + 1) * Size};
            FillRect(hdc, &r, (i == 0) ? headBrush : bodyBrush);
        }
        DeleteObject(headBrush);
        DeleteObject(bodyBrush);

        // 4. 显示文字
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        char scoreStr[50];
        wsprintf(scoreStr, "Score: %d", score);
        TextOut(hdc, 10, 10, scoreStr, strlen(scoreStr));

        if (gameover)
        {
            HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(255, 50, 50));
            TextOut(hdc, Width * Size / 2 - 80, Height * Size / 2 - 20, " 游戏结束!   ", 10);

            SetTextColor(hdc, RGB(200, 200, 200));
            TextOut(hdc, Width * Size / 2 - 100, Height * Size / 2 + 20, "按下空格重新开始    ", 20);
            DeleteObject(hFont);
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(time(0));
    resetGame(); // 初始化游戏

    const char className[] = "SnakeWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    // 计算窗口实际大小
    HWND hwnd = CreateWindowEx(0, className, "Snake Game - Window Mode",
                               WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
                               CW_USEDEFAULT, CW_USEDEFAULT, Width * Size + 16, Height * Size + 39,
                               NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            update();
            InvalidateRect(hwnd, NULL, FALSE);
            Sleep(150); // 控制速度,越大越慢
        }
    }
    return 0;
}