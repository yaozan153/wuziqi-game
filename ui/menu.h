// 文件职责：菜单及共用按钮：只绘图或返回点击结果，页面切换由 main.cpp 完成。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_MENU_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_MENU_H // 定义本头文件的包含标记，防止同一编译单元重复声明。

enum class Screen { MENU, HUMAN_AI_SELECT, HUMAN_AI, LOCAL, ONLINE, HISTORY, EXIT }; // 定义离散状态取值：`enum class Screen { MENU, HUMAN_AI_SELECT, HUMAN_AI, LOCAL, ONLINE, HISTORY, EXIT }`。

// 判断坐标是否在按钮矩形内：x、y 是待检查的窗口坐标。
// left、top 是按钮左上角，width、height 是像素宽高；在范围内返回 true。
bool insideButton(int x, int y, int left, int top, int width, int height); // 声明接口：检查坐标是否落在按钮矩形内，采用左上含、右下不含的范围。

// 画一个带文字的圆角按钮；前四个参数指定位置和大小，text 是按钮文字。
// mouseX、mouseY 是当前鼠标位置，用于悬停变色；本函数不处理点击或切换页面。
void drawButton(int left, int top, int width, int height, // 函数入口：根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
                const wchar_t* text, int mouseX, int mouseY); // 声明 text：本行使用的局部数据；参数或长度由本行给出。

// 画首页标题和人机、本地、联机、历史棋谱、退出按钮。
void drawMenu(int mouseX, int mouseY); // 声明接口：绘制首页标题和五个入口按钮。

// 判断首页点击了哪个按钮，返回应进入的 Screen；未点中按钮返回 MENU。
// 只返回选择结果，不直接修改主函数中的 screen。
Screen menuChoice(int x, int y); // 声明接口：把被点击的菜单按钮映射成 Screen 页面值。

// 画尚未实现的模式提示页：title 是标题，description 是说明，另画返回菜单按钮。
// 当前主流程未调用此备用占位页；mouseX、mouseY 用于按钮悬停效果。
void drawModePlaceholder(const wchar_t* title, const wchar_t* description, // 函数入口：绘制备用占位页面，当前主流程没有调用它。
                         int mouseX, int mouseY); // 声明 mouseX：鼠标横坐标；参数或长度由本行给出。

// 判断是否点击了上述提示页的返回按钮，返回 true/false。
// 不用于棋盘页或黑白选择页，它们的返回按钮位置不同。
bool clickedBack(int x, int y); // 声明接口：检查备用占位页面的返回按钮，当前主流程没有调用它。

#endif // 结束头文件防重复包含的条件范围。
