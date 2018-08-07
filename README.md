# H264BSAnalyzer -- H.264/AVC H.265/HEVC bitstream analyze tool

[![GitHub stars](https://img.shields.io/github/stars/latelee/H264BSAnalyzer.svg)](https://github.com/latelee/H264BSAnalyzer)[![GitHub forks](https://img.shields.io/github/forks/latelee/H264BSAnalyzer.svg)](https://github.com/latelee/H264BSAnalyzer)

Stargazers over time  
[![Stargazers over time](https://starcharts.herokuapp.com/latelee/H264BSAnalyzer.svg)](https://starcharts.herokuapp.com/latelee/H264BSAnalyzer)

## Project
VS2010 MFC project, using h264bitstream to implement H.264 bitstream analyze.<br>
The code for H.265 bitstream analyzing is based on h264bitstream code and HM16.6.

## Project Feature
* support different NAL display, including VPS, SPS, PPS, SEI, AUD, Slice.
* support hex data display for NAL.
* support displaying detail information using cursor up and down.
* support different color for different slice, with frame number.
* auto parse file name.
* support file name suffix:
    * H.264 format file: .h264,.h264, .avc
    * H.265 format file: .h26, .h265, .hevc
    * auto decide format acording file content if no name suffix specify listing above.
* support playing H.264、H.265 bitstream video file.
* pause, stop, play frame by frame for video file.
* support saving for RGB(24bit) and YUV(yuv420p) file, BMP, JPEG (picture) file.
* support  saving for AVI, MP4, MOV format file.

## Usage
Click menu File->OPen option, or drag file to the main window, <br>
and the tool will auto parse file cotent. <br>
double click the item in the main windows will show the detail NAL information.<br>
to play the file, click "Play".

## Window view
V1.2 main window: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v1.2.png)

V2.0 main window for h.264: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.0_h264.png)

V2.0 main window for h.265: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.0_h265.png)

V2.1 main window for h.264: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.1_h264.png)

V2.1 main window for h.265: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v2.1_h265.png)

V3.0 main window for h.264: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v3.0_h264.png)

V3.0 main window for h.265: <br>
![GUI](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/v3.0_h265.png)
  
## Changelog
The binary file will locate in release directory. <br>
* v1.x <br>
> Complete H.264 bitstream analyze.
 
* v2.0 <br>
> Delete manual start function in v1.x.<br>
> Add for H.265/HEVC bitstream analyze.<br>
> Other update.<br>

* v2.1 <br>
> Dispaly bitstream information using CTreeCtrl, ref project: H264Visa、H264VideoESViewer.<br>
> Add windows resize.<br>
> Other update.<br>
> Some problems: <br>
> Not decode some SEI information, the H.264 frame rate calcuration may be inaccurate(2x for real frame rate).<br>

* v3.0 <br>
> fix code using cppcheck.<br>
> support play H.264、H.265 bitstream, minimize compile ffmpeg, static link.<br>
> pause, stop, play frame by frame for video file.<br>
> support saving for RGB(24bit) and YUV(yuv420p) file, BMP, JPEG (picture) file.<br>
> support  saving for AVI, MP4, MOV format file.<br>

## Testing
The tool is testing width H264Visa, CodecVisa and HM tool.<br>
The testing file is generating by x264/x265 tool, also use some H.265 test sequence file<br>
Only test under Windows 7 64bit OS.<br>

## Some bug
Parsing big file will be slow, and may be crash.<br>
Same slice information may be wrong.<br>
The avi file saving for h.265 can't be play. Note: it can't be play by ffplay.<br>
You ca fix yourself, and let me know.<br>

## Protocol
* Copyright [CST studio Late Lee](http://www.latelee.org)
* Fix some bug for h264bitstream, see the code.
* The code comes from h264bitstream, is LGPL.
* Total code is LGPL.
* You can use the code for study, and commercial purposes, but give no guarantee.

## Thanks
This project started at Feb, 2014 for work need, and see the article written by Dr leixiaohua, <br>
and then rewrite the code, refactor the code, and make improve.<br>
Tanks to [雷霄骅](http://blog.csdn.net/leixiaohua1020) , He's gone, but will last spirit.

## Author
CST studio Late Lee<br>
[CST studio](http://www.latelee.org) <br>
Donate the author <br>
![Donate](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/latelee_pay_small.png)


# H264BSAnalyzer -- H.264/AVC H.265/HEVC码流分析工具

## 工程说明
VS2010 MFC工程，使用h264bitstream开源项目实现对H.264码流分析。<br>
H.265分析以h264bitstream为参考基准代码，同时参考HM16.6代码。

## 功能
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

## 用法
菜单File->Open选项；或者直接拖曳文件至工具界面。<br>
工具会自动解析。双击某一项即可查看具体的NAL信息。<br>
点击“Play”菜单出现播放子窗口。

## 界面
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
  
## 版本变更
编译好的工具位于release目录中。<br>
* v1.x <br>
> H264码流分析功能完成。
 
* v2.0 <br>
> 去掉v1.x版本手动点击开始分析功能。<br>
> 添加H.265/HEVC码流分析功能。<br>
> 其它界面及显示信息完善。<br>

* v2.1 <br>
> 使用树形控件显示码流字段。显示参考来源为H264Visa、H264VideoESViewer工具。<br>
> 增加缩放功能。<br>
> 其它界面及显示信息完善。<br>
> 遗留问题：<br>
> 部分SEI信息未做解析；H264帧率计算可能不准确(是真实帧率的2倍)。<br>

* v3.0 <br>
> 使用cppcheck进行检测，修复个别语句错误之处。<br>
> 支持播放H.264、H.265裸码流文件。ffmpeg最小编译，静态链接。<br>
> 具备暂停、停止、逐帧播放功能。<br>
> 支持保存为RGB(24bit)、YUV(yuv420p)原始文件，支持保存为BMP、JPEG图片。<br>
> 支持保存为AVI、MP4、MOV格式视频文件。<br>

## 测试
本工程使用H264Visa、CodecVisa及HM工具对比测试。<br>
所用视频文件为x264/x265编码生成，另外使用H.265测试序列。<br>
本工具仅在Windows 7 64bit操作系统中运行测试通过。<br>

## 可能潜在问题
分析大文件较慢，可能会崩溃。<br>
本工具虽使用众多文件、工具对比分析，但无法满足所有条件，个别语法可能分析有误。<br>
H.265保存为AVI格式视频无法播放。注：使用ffmpeg转换，用ffplay也无法正常播放。<br>
可自行修正，也可反馈给作者。<br>

## 协议
* 版权所有 [迟思堂工作室 李迟](http://www.latelee.org)
* 修正h264bitstream个别bug。详见代码。
* 基于h264bitstream适应性修改的代码，遵从LGPL协议。
* 本工程源码使用LGPL协议。
* 可用于学习研究之目的，也可用于商业目的，但无义务保证程序功能完全可靠。

## 致谢
本工程于2014年2月因工作需要，无意看到雷霄骅博士之文章，于其基础上修改、重构、不断完善。<br>
感谢 [雷霄骅](http://blog.csdn.net/leixiaohua1020) 博士！斯人已逝，精神长存！

## 作者
思堂工作室 李迟<br>
[迟思堂工作室](http://www.latelee.org) <br>
如果觉得本软件不错，欢迎捐赠支持作者 <br>
![捐赠](https://github.com/latelee/H264BSAnalyzer/blob/master/screenshots/latelee_pay_small.png)
