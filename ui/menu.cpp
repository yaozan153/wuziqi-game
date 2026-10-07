// 文件职责：菜单及共用按钮：只绘图或返回点击结果，页面切换由 main.cpp 完成。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "menu.h" // 引入 "menu.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
using namespace ege; // 允许直接使用该命名空间中的名称，省略 ege:: 或 std:: 前缀。

namespace // 开启匿名命名空间，让内部辅助函数只在当前源文件可见。
{ // 开始上方函数、条件、循环或类型的作用域。
    void roundedFill(int x, int y, int width, int height, int radius, color_t color) // 函数入口：用矩形与四个圆角拼出按钮背景。
    { // 开始上方函数、条件、循环或类型的作用域。
        setcolor(color); // 设置后续线条和文字颜色；参数为 `color)`。
        setfillcolor(color); // 设置图形填充颜色；参数为 `color)`。
        bar(x + radius, y, x + width - radius, y + height); // 填充矩形区域；参数为 `x + radius, y, x + width - radius, y + height)`。
        bar(x, y + radius, x + width, y + height - radius); // 填充矩形区域；参数为 `x, y + radius, x + width, y + height - radius)`。
        fillellipse(x + radius, y + radius, radius, radius); // 绘制实心椭圆或圆形；参数为 `x + radius, y + radius, radius, radius)`。
        fillellipse(x + width - radius - 1, y + radius, radius, radius); // 绘制实心椭圆或圆形；参数为 `x + width - radius - 1, y + radius, radius, radius)`。
        fillellipse(x + radius, y + height - radius - 1, radius, radius); // 绘制实心椭圆或圆形；参数为 `x + radius, y + height - radius - 1, radius, radius)`。
        fillellipse(x + width - radius - 1, y + height - radius - 1, radius, radius); // 绘制实心椭圆或圆形；参数为 `x + width - radius - 1, y + height - radius - 1, radius, radius)`。
    }

    void centeredText(int centerX, int top, const wchar_t* text, int size) // 函数入口：设置字体和颜色，将文字以给定中心横坐标居中。
    { // 开始上方函数、条件、循环或类型的作用域。
        setfont(size, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `size, 0, L"微软雅黑")`。
        if (size >= 40) // 检查 `if (size >= 40)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            LOGFONTW font = LOGFONTW(); // 声明 font：Windows 字体描述；按右侧表达式初始化。
            font.lfHeight = size; // 更新数据：`font.lfHeight = size`；赋值后的状态供后续逻辑或绘图使用。
            font.lfWeight = FW_BOLD; // 更新数据：`font.lfWeight = FW_BOLD`；赋值后的状态供后续逻辑或绘图使用。
            font.lfQuality = ANTIALIASED_QUALITY; // 更新数据：`font.lfQuality = ANTIALIASED_QUALITY`；赋值后的状态供后续逻辑或绘图使用。
            lstrcpyW(font.lfFaceName, L"微软雅黑"); // 复制字体名称到 Windows 字体结构；参数为 `font.lfFaceName, L"微软雅黑")`。
            setfont(&font); // 首页标题使用粗体，按钮文字保持普通字重。
        }
        setbkmode(TRANSPARENT); // 设置文字背景透明方式；参数为 `TRANSPARENT)`。
        setcolor(EGERGB(245, 246, 248)); // 设置后续线条和文字颜色；参数为 `EGERGB(245, 246, 248))`。
        outtextxy(centerX - textwidth(text) / 2, top, text); // 在指定坐标绘制文字；参数为 `centerX - textwidth(text) / 2, top, text)`。
    }
}

bool insideButton(int x, int y, int left, int top, int width, int height) // 函数入口：检查坐标是否落在按钮矩形内，采用左上含、右下不含的范围。
{ // 开始上方函数、条件、循环或类型的作用域。
    return x >= left && x < left + width && y >= top && y < top + height; // 返回 `x >= left && x < left + width && y >= top && y < top + height`，将结果交给调用者。
}

void drawButton(int left, int top, int width, int height, // 函数入口：根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
                const wchar_t* text, int mouseX, int mouseY) // 声明 text：本行使用的局部数据；参数或长度由本行给出。
{ // 开始上方函数、条件、循环或类型的作用域。
    const bool hover = insideButton(mouseX, mouseY, left, top, width, height); // 声明 hover：鼠标是否悬停在按钮内；按右侧表达式初始化。
    roundedFill(left, top, width, height, 14, // 用矩形与四个圆角拼出按钮背景。
                hover ? EGERGB(107, 142, 170) : EGERGB(82, 89, 96)); // 条件表达式：`hover ? EGERGB(107, 142, 170) : EGERGB(82, 89, 96))`；根据条件选择两个值之一。
    roundedFill(left + 2, top + 2, width - 4, height - 4, 12, // 用矩形与四个圆角拼出按钮背景。
                hover ? EGERGB(70, 80, 90) : EGERGB(55, 61, 67)); // 条件表达式：`hover ? EGERGB(70, 80, 90) : EGERGB(55, 61, 67))`；根据条件选择两个值之一。
    setfont(24, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `24, 0, L"微软雅黑")`。
    setbkmode(TRANSPARENT); // 设置文字背景透明方式；参数为 `TRANSPARENT)`。
    setcolor(EGERGB(245, 246, 248)); // 设置后续线条和文字颜色；参数为 `EGERGB(245, 246, 248))`。
    outtextxy(left + (width - textwidth(text)) / 2, // 在指定坐标绘制文字；参数为 `left + (width - textwidth(text)) / 2,`。
              top + (height - textheight(text)) / 2, text); // 执行 `top + (height - textheight(text)) / 2, text)`；调用相应对象的方法完成本步骤。
}

void drawMenu(int mouseX, int mouseY) // 函数入口：绘制首页标题和五个入口按钮。
{ // 开始上方函数、条件、循环或类型的作用域。
    centeredText(400, 185, L"五子棋游戏", 44); // 设置字体和颜色，将文字以给定中心横坐标居中。
    const wchar_t* labels[] = {L"人机对决", L"本地对决", L"联机对决", L"历史棋谱", L"退出"}; // 声明 labels：菜单按钮文字数组；按右侧表达式初始化。
    for (int i = 0; i < 5; i++) // 循环推进：`int i = 0; i < 5; i++)`；逐项处理棋格、方向、字符或列表元素。
        drawButton(280, 266 + i * 58, 240, 48, labels[i], mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
}

Screen menuChoice(int x, int y) // 函数入口：把被点击的菜单按钮映射成 Screen 页面值。
{ // 开始上方函数、条件、循环或类型的作用域。
    const Screen choices[] = {Screen::HUMAN_AI_SELECT, Screen::LOCAL, Screen::ONLINE, Screen::HISTORY, Screen::EXIT}; // 更新数据：`const Screen choices[] = {Screen::HUMAN_AI_SELECT, Screen::LOCAL, Screen::ONLINE, Screen::HISTORY, Screen::EXIT}`；赋值后的状态供后续逻辑或绘图使用。
    for (int i = 0; i < 5; i++) // 循环推进：`int i = 0; i < 5; i++)`；逐项处理棋格、方向、字符或列表元素。
        if (insideButton(x, y, 280, 266 + i * 58, 240, 48)) // 检查 `if (insideButton(x, y, 280, 266 + i * 58, 240, 48))`；条件成立时执行括号之后或下一行的处理。
            return choices[i]; // 返回 `choices[i]`，将结果交给调用者。
    return Screen::MENU; // 返回 `Screen::MENU`，将结果交给调用者。
}

void drawModePlaceholder(const wchar_t* title, const wchar_t* description, // 函数入口：绘制备用占位页面，当前主流程没有调用它。
                         int mouseX, int mouseY) // 声明 mouseX：鼠标横坐标；参数或长度由本行给出。
{ // 开始上方函数、条件、循环或类型的作用域。
    centeredText(400, 195, title, 40); // 设置字体和颜色，将文字以给定中心横坐标居中。
    centeredText(400, 270, description, 22); // 设置字体和颜色，将文字以给定中心横坐标居中。
    drawButton(280, 360, 240, 48, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
}

bool clickedBack(int x, int y) // 函数入口：检查备用占位页面的返回按钮，当前主流程没有调用它。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 280, 360, 240, 48); // 返回 `insideButton(x, y, 280, 360, 240, 48)`，将结果交给调用者。
}
