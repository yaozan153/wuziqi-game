# 五子棋游戏

使用 C++11 和 EGE 图形库开发的 Windows 五子棋课程项目。

## 功能

- 本地双人对战、悔棋、重新开始与胜负统计。
- 人机对战，支持选择棋色。
- TCP 网络联机对战。
- 棋谱保存、历史复盘与本地续玩。
- 木纹棋盘、棋子素材与落点准星。

## 编译与运行

需要 Windows、64 位 MinGW C++ 编译器以及已配置的 EGE 图形库。

### Dev-C++

打开 `wuziqi.dev`，检查编译器和 EGE 库配置，编译运行。
项目使用 C++11，链接参数已写入工程文件。

### VS Code

安装 C/C++ 扩展，将 `.vscode/tasks.json` 和调试配置中的
`D:/Dev-Cpp/MinGW64/bin/` 修改为本机编译器目录。
运行默认构建任务，或在项目目录执行：

```powershell
powershell -ExecutionPolicy Bypass -File .vscode/run-project.ps1 -Run
```

运行程序时，请保留 `assets/` 素材目录并以项目目录作为工作目录。
棋谱保存在本地 `records/` 中，该目录不提交到仓库。

## 项目结构

| 路径 | 用途 |
| --- | --- |
| `main.cpp` | 程序入口与页面切换 |
| `game.*` | 落子、判胜和悔棋规则 |
| `board.*`、`assets/` | 棋盘绘图与素材 |
| `ui/` | 菜单与共用对局界面 |
| `modes/local/` | 本地双人模式 |
| `modes/human_ai/` | 人机模式与 AI |
| `modes/online/` | 网络联机模式 |
| `history/` | 棋谱存储与复盘 |
| `tests/` | 模块测试源码 |

详细设计见 [项目架构说明](项目架构说明.md)，联机说明见
[联机模块文档](modes/online/README.md)。
