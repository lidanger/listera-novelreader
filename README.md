# Listera Reader——听雨小说阅读器

#### 介绍
听雨小说阅读器是一个 txt 小说阅读器，软件基于 Qt5 和 Dtk5 开发，目前只完成了基本功能。


#### 安装教程

1. 编译

   ```bash
   cd src
   qmake
   make
   ```
   
   或

   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=release
   make
   make package
   ```
   

2. 安装

   ```bash
   sudo make install
   ```

   或

   ```bash
   sudo apt install ./*.deb
   ```


#### 使用说明

1.  通过文件菜单添加书籍，通过左侧书库选择书籍；
2.  双击目录对话框中的条目可以跳转到对应的章节；
3.  目前只有三个快捷键：上一页 ←   下一页 →    滚屏 空格。

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 许可证

本软件基于 GPLv3 协议发布。