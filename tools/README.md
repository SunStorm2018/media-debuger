# Media Debuger 国际化工具使用说明

## 🎯 概述

本目录包含Media Debuger项目的完整国际化支持工具，提供统一的翻译管理功能。

## 📁 工具文件

### 核心工具
- **`i18n_manager.py`** - 统一的国际化管理工具
  - 自动更新翻译文件
  - 补全中文翻译
  - 生成.qm文件
  - 打开Qt Linguist编辑器
  - 语言状态检查

### 文档
- **`translations/README.md`** - 详细的翻译文档
- **`translations/templates/translation_guide.md`** - 翻译指南

## 🚀 快速开始

### 1. 完整工作流程
```bash
# 1. 更新翻译文件（提取源代码中的tr()字符串）
python3 tools/i18n_manager.py update

# 2. 补全中文翻译
python3 tools/i18n_manager.py complete-cn

# 3. 生成.qm文件
python3 tools/i18n_manager.py release

# 4. 重新编译项目
qmake && make
```

### 2. 单独操作
```bash
# 列出支持的语言
python3 tools/i18n_manager.py list

# 打开Qt Linguist编辑中文翻译
python3 tools/i18n_manager.py linguist zh_CN

# 只生成中文.qm文件
lrelease translations/media-debuger_zh_CN.ts

# 生成所有语言的.qm文件
python3 tools/i18n_manager.py release
```

## 📊 支持的语言

| 语言代码 | 语言名称 | 状态 |
|---------|---------|------|
| zh_CN | 简体中文 | ✅ 已完成翻译 |
| en_US | English | ⏳ 待翻译 |
| ja_JP | 日本語 | ⏳ 待翻译 |
| ko_KR | 한국어 | ⏳ 待翻译 |
| fr_FR | Français | ⏳ 待翻译 |
| de_DE | Deutsch | ⏳ 待翻译 |
| es_ES | Español | ⏳ 待翻译 |
| ru_RU | Русский | ⏳ 待翻译 |
| ar_SA | العربية | ⏳ 待翻译 |
| pt_BR | Português (Brasil) | ⏳ 待翻译 |

## 🔧 功能特性

### 自动翻译更新
- 自动扫描所有源代码文件
- 智能提取tr()包装的字符串
- 过滤不需要翻译的内容（URL、路径等）
- 批量生成所有语言文件

### 中文翻译补全
- 内置200+个常用翻译映射
- 覆盖所有界面元素和功能
- 自动替换未翻译的字符串

### 翻译管理
- 统一的管理界面
- 集成Qt标准工具（lupdate、lrelease、linguist）
- 实时状态检查

## 📝 代码中的tr()使用

### ✅ 正确方式
```cpp
// 基本字符串
QString text = tr("Hello World");

// 带占位符
QString text = tr("File: %1").arg(fileName);

// 带上下文
QString text = tr("Menu", "File Menu");
```

### ❌ 错误方式
```cpp
// 直接硬编码 - 不会被翻译
QString text = "Hello World";

// 忘记tr()包装
QString text = QString("Hello World");
```

## 🛠️ 故障排除

### 常见问题

1. **lupdate命令找不到**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install qttools5-dev-tools
   
   # CentOS/RHEL
   sudo yum install qt5-linguist
   ```

2. **翻译不生效**
   ```bash
   # 检查.qm文件是否生成
   ls -la translations/*.qm
   
   # 检查翻译加载
   # 在main.cpp中添加调试信息
   qDebug() << "Translation loaded:" << translator.load(...);
   ```

3. **中文字符显示异常**
   - 确保源文件使用UTF-8编码
   - 检查翻译文件编码
   - 在.pro文件中添加：`CODECFORTR = UTF-8`

## 📋 命令参考

### i18n_manager.py 命令
```bash
python3 tools/i18n_manager.py [命令] [参数]

update          更新翻译文件
release         生成.qm翻译文件
complete-cn     补全中文翻译
list            列出支持的语言
linguist [语言] 打开Qt Linguist编辑器
help            显示帮助信息
```

### Qt标准命令
```bash
# 更新翻译文件
lupdate media-debuger.pro -ts -no-obsolete

# 生成.qm文件
lrelease translations/*.ts

# 编辑翻译文件
linguist translations/media-debuger_zh_CN.ts
```

## 🎯 翻译流程

### 对于开发者
1. 在代码中使用tr()包装字符串
2. 运行`python3 tools/i18n_manager.py update`更新翻译文件
3. 运行`python3 tools/i18n_manager.py linguist zh_CN`编辑翻译
4. 运行`python3 tools/i18n_manager.py release`生成.qm文件
5. 重新编译项目

### 对于翻译者
1. 运行`python3 tools/i18n_manager.py linguist zh_CN`打开翻译编辑器
2. 使用Qt Linguist进行翻译
3. 保存翻译文件
4. 运行`python3 tools/i18n_manager.py release`生成最终文件

## 📈 项目状态

### ✅ 已完成
- [x] 10种语言支持
- [x] 统一翻译管理工具
- [x] 完整中文翻译（224个字符串）
- [x] 自动化更新脚本
- [x] 项目配置更新
- [x] 完整文档系统

### 📁 文件结构
```
tools/
└── i18n_manager.py              # 统一翻译管理工具

translations/
├── media-debuger_zh_CN.ts/qm   # 中文翻译
├── media-debuger_en_US.ts/qm   # 英文翻译
├── media-debuger_*.ts/qm        # 其他8种语言
├── templates/
│   └── translation_guide.md   # 翻译指南
└── README.md                    # 详细文档
```

## 🤝 贡献指南

### 添加新语言
1. 编辑`tools/i18n_manager.py`中的`languages`字典
2. 运行`python3 tools/i18n_manager.py update`
3. 进行翻译
4. 运行`python3 tools/i18n_manager.py release`

### 改进翻译
1. 使用Qt Linguist编辑翻译文件
2. 确保术语一致性
3. 测试界面显示效果

### 报告问题
- 检查工具是否正常运行
- 确认Qt工具已正确安装
- 查看相关文档获取帮助

---

*最后更新: 2025年11月20日*
*状态: ✅ 多语言国际化支持完成*