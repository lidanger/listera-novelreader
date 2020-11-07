## Listera NovelReader——听雨小说阅读器

听雨小说阅读器是一个 txt 中文小说阅读器，基于 Qt5 和 Dtk5 开发。
目前只有适用于 deepin 20 的版本。
项目地址：https://gitee.com/lidanger/listera-novelreader

按钮用文字看着也不错，就没有再找图标了。不知道是不是看得时间长了太熟悉的关系。

#### 安装教程

1. 准备

   编译此程序需要 qt5widgets qt5core qt5gui dtkcore dtkgui dtkwidget 等库的开发版本。
   
2. 编译

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

   部分已编译版本见[这里](https://gitee.com/lidanger/listera-novelreader/releases)

3. 安装

   ```bash
   sudo make install
   ```

   或

   ```bash
   sudo apt install ./*.deb
   ```


#### 使用说明

1.  通过左侧书库添加、选择书籍；
2.  双击目录对话框中的条目可以跳转到对应的章节；
3.  默认快捷键：上一页 ←   下一页 →    翻屏 空格。

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 许可协议

本软件基于 GPLv3 协议发布。
