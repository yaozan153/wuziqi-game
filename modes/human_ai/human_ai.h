// 文件职责：人机模式：选择玩家颜色、限制玩家回合、在电脑回合调用 AI 落子。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_HUMAN_AI_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_HUMAN_AI_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "../../game.h" // 引入 "../../game.h"，提供本文件使用的类型和函数声明。

// 画对战前的选色页：执黑（人先下）、执白（电脑先下）、返回菜单。
// mouseX、mouseY 用于按钮悬停效果；不保存玩家选择。
void drawHumanAiChoice(int mouseX, int mouseY); // 声明接口：绘制玩家选黑、选白及返回菜单按钮。

// 根据窗口点击坐标 x、y 返回选择的颜色；点中黑/白按钮返回对应 PieceColor。
// 其他位置返回 EMPTY；选择结果由主函数保存到 humanPiece。
PieceColor humanAiChoice(int x, int y); // 点击执黑或执白按钮，未选中时返回 EMPTY。

// 判断是否点中选色页的返回菜单按钮，返回 true/false。
bool humanAiChoiceBack(int x, int y); // 声明接口：检查选色页面的返回按钮位置。

// 调用共用 drawMatch，显示人机棋盘以及玩家和电脑各自的颜色。
// humanPiece 是玩家所选颜色；game 只读，鼠标坐标用于返回按钮悬停。
// 仅玩家回合显示落子准星；满盘无赢家时显示和棋。
void drawHumanAiScreen(const GameState& game, PieceColor humanPiece, int mouseX, int mouseY); // 声明接口：按玩家颜色与当前回合绘制人机状态，满盘无赢家显示和棋。

// 玩家点击落子：仅在玩家回合接受合法棋盘点击。
bool humanAiMove(GameState& game, PieceColor humanPiece, int x, int y); // 声明接口：仅允许玩家回合处理点击并通过共用规则落子。

// 电脑回合计算并落一子；玩家回合、游戏结束或满盘时返回 false。
bool computerAiMove(GameState& game, PieceColor humanPiece); // 声明接口：电脑回合选最佳空位并落子；结束、满盘或玩家回合不动作。
#endif // 结束头文件防重复包含的条件范围。
