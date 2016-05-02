H264BSAnalyzer -- H.264/AVC H.265/HEVC码流分析工具
=======================

工程说明
=======================
VS2010 MFC工程，使用h264bitstream开源项目实现对H.264码流分析。<br>
H.265分析以h264bitstream为参考基准代码，同时参考HM16.6代码。

功能
=======================
* 支持不同NAL的显示，包括VPS、SPS、PPS、SEI、AUD、Slice的解析。
* 支持显示NAL十六进制数据。
* 支持上下光标移动显示详细信息。
* 支持不同slice的着色显示，显示帧序号。
* 自动解析文件名。
* 支持文件名后缀：
    * H.264文件后缀名为.h264、.h264、.avc。
    * H.265文件后缀名为.h265、.h265、.hevc。
    * 如无上述后缀名，则根据内容自动识别。
* 支持播放H.264、H.265裸码流文件。
* 具备暂停、停止、逐帧播放功能。
* 支持保存为RGB(24bit)、YUV(yuv420p)原始文件，支持保存为BMP、JPEG图片。支持文件名含%d字符。
* 支持保存为AVI、MP4、MOV格式视频文件。

用法
=======================
菜单File->Open选项；或者直接拖曳文件至工具界面。<br>
工具会自动解析。双击某一项即可查看具体的NAL信息。<br>
点击“Play”菜单出现播放子窗口。

界面
=======================
V1.2版本界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v1.2.png)

V2.0版本H264分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.0_h264.png)

V2.0版本H265分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.0_h265.png)

V2.1版本H264分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.1_h264.png)

V2.1版本H265分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.1_h265.png)

V3.0版本H264分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v3.0_h264.png)

V3.0版本H265分析界面：<br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v3.0_h265.png)


声明
=======================
本程序仅限于学习交流。版本所有。<br>
    
版本
=======================
编译好的工具位于release目录中。<br>
* v1.x <br>
H264码流分析功能完成。
 
* v2.0 <br>
去掉v1.x版本手动点击开始分析功能。<br>
添加H.265/HEVC码流分析功能。<br>
其它界面及显示信息完善。<br>

* v2.1 <br>
使用树形控件显示码流字段。显示参考来源为H264Visa、H264VideoESViewer工具。<br>
增加缩放功能。<br>
其它界面及显示信息完善。<br>
遗留问题：<br>
部分SEI信息未做解析；H264帧率计算可能不准确(是真实帧率的2倍)。<br>

* v3.0 <br>
使用cppcheck进行检测，修复个别错误语句。
支持播放H.264、H.265裸码流文件。
具备暂停、停止、逐帧播放功能。
支持保存为RGB(24bit)、YUV(yuv420p)原始文件，支持保存为BMP、JPEG图片。
支持保存为AVI、MP4、MOV格式视频文件。

测试
=======================
本工程使用H264Visa、CodecVisa及HM工具对比测试。<br>
所用视频文件为x264/x265编码生成，另外使用H.265测试序列。<br>
本工具仅在Windows 7 64bit操作系统中运行测试通过。

bug
=======================
分析大文件较慢，可能会崩溃。<br>
本工具虽使用众多文件、工具对比分析，但无法满足所有条件，个别语法可能分析有误。<br>
H.265保存为AVI格式视频无法播放。注：使用ffmpeg转换，用ffplay也无法正常播放。<br>
可自行修正，也可反馈给作者。<br>

其它
=======================
* 修正h264bitstream个别bug。详见代码。
* 基于h264bitstream适应性修改的代码，遵从LGPL协议。
* 其它部分源码开放。由于作者能力有限，难免有错误，切勿用于商业目的。

作者
=======================
思堂工作室 李迟 <br>
[迟思堂工作室](http://www.latelee.org)

[欢迎捐赠支持作者](http://www.latelee.org/donate)
