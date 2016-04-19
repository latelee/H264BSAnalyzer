/**
 * @file   bmp_utils.h
 * @author Late Lee 
 * @date   2012-7-2 13:21:53
 * @brief  
 *          BMP相关工具函数，目前只针对24位图片测试
 *
 *        1、在VS003及GCC下编译测试通过；
 *        2、解决了BMP图片倒立、偏色、倾斜等问题。
 *        3、BMP图像每行数据需要4字节对齐，即一行数据不足4的倍数，以0补。
 *           解决此问题方法：设置2个变量：
 *           width_byte：实际的RGB每一行的字节数
 *           stride_byte：4字节对齐的每一行的字节数(已对齐时两者相等)
 *           保存时，另外开辟一个考虑了4字节对齐的缓冲区，每拷贝一行数据(width_byte)，
 *           跳过stride_byte个字节，即跳到4字节对齐的下一行。
 *           读取时，只读width_byte，并且跳过每行最后补的0。
 *        4、图像倒立：读取与保存BMP时，将数据倒过来:
 *           读取时，将读到的数据由下往上存放到缓冲区
 *           保存时，将数据由下往上拷贝到缓冲区
 *        5、偏色：BMP排序为BGR，将RGB数据的G、B调换位置即可。
 *        6、倾斜：读取BMP时，未跳过补充的0。
 *
 *       笔记：
            BMP图片结构，基中第1、第2部分占54字节，真彩色图没有第三部分
              _______________________________
             |        BITMAPFILEHEADER       |
             |_______________________________|
             |        BITMAPINFOHEADER       |
             |_______________________________|
             |          n * RGBQUAD          |
             |_______________________________|
             |          image  data          |
             |_______________________________|

 *
        对于2色位图，用1位表示该象素的颜色(一般0表示黑，1表示白)，一个字节可以表示8个象素。调色板：2*4=8
        对于16色位图，用4位表示一个象素的颜色，以一个字节可以表示2个象素。调色板：16*4=64
        对于256色位图，一个字节表示1个象素。调色板：256*4=1024
        对于真彩色图，三个字节表示1个象素。无调色板

 *      单色BMP图：调色板占8字节，故头部占用54+8=62字节，后面为像素字节，
        注意每行字节需要4字节对齐，
        举例：16*16像素单色位图，一行占16/8 = 2字节，需要补2字节。
        实际像素字节：16*16/2 = 32字节，补齐字节：2*16 = 32，共64字节
        头部共62字节，故该图片总大小为64+62=126字节
 */

#ifndef _BMP_UTILS_H
#define _BMP_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include <Windows.h>
#else
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;

#pragma pack(push)
// 2字节对齐，共14
#pragma pack(2)
typedef struct tagBITMAPFILEHEADER {
    WORD    bfType;             // 文件类型, 0x4d42
    DWORD   bfSize;             // 文件总大小
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;          // 实际位图数据偏移
} BITMAPFILEHEADER; //__attribute__ ((packed));

// 40
typedef struct tagBITMAPINFOHEADER{
    DWORD      biSize;          // 本结构体长度
    LONG       biWidth;         // 宽(单位像素)
    LONG       biHeight;        // 高(单位像素)
    WORD       biPlanes;        // 为1
    WORD       biBitCount;      // 像素占用位数 1(2^1=2黑白二色)， 4(2^4=16色)，8(2^8=256色)，24(真彩色)，32
    DWORD      biCompression;   // 压缩类型，不压缩：BI_RGB(0)
    DWORD      biSizeImage;     // 位图数据大小，如果是不压缩类型，可以为0
    LONG       biXPelsPerMeter; // 水平分辨率,单位是每米的象素个数
    LONG       biYPelsPerMeter; // 垂直分辨率
    DWORD      biClrUsed;       // 位图实际使用的颜色表中的颜色数
    DWORD      biClrImportant;  // 位图显示过程中重要的颜色数
} BITMAPINFOHEADER; //__attribute__ ((aligned(2)));

typedef struct tagRGBQUAD {
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO{
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;   // __attribute__ ((aligned(2)));

#pragma pack(pop)

#endif

#undef  ALIGN
#define ALIGN(x, n) (((x)+(n)-1)&~((n)-1))

/**
 * RGB互换R、B顺序
 * 
 * @param[IN]  rgb_buffer RGB缓冲区
 * @param[IN]  len        缓冲区大小
 * 
 * @return none
 *
 * @note
 *        缓冲区数据可以是RGB，也可以是BGR，该函数只是将B、G进行互换
 */
void swap_rgb(unsigned char* rgb_buffer, int len);

/**
 * 分析BMP文件头部
 * 
 * @param[IN]  bmp_file  BMP图片文件名称
 * 
 * @return 
 *         0:  成功
 *         -1: 文件不存在或不是BMP文件
 */
int analyse_bmp_file(const char* bmp_file);

/**
 * 读取BMP图片文件
 * 
 * @param[IN]   bmp_file    BMP图片文件名称
 * 
 * @param[OUT]  rgb_buffer RGB数据(实际为BGR)
 * @param[OUT]  size       RGB数据大小
 * @param[OUT]  width      图片宽
 * @param[OUT]  height     图片高
 *
 * @return 
 *         0：成功
 *         -1：读取文件失败，或不是BMP文件，或申请内存失败
 * @note
 *         rgb_buffer为二级指针，内存由该函数分配，需要自行释放
 *         rgb_buffer数据排列顺序为BGR，因此，处理时可能需要转换成RGB顺序
 */
int read_bmp_file(const char* bmp_file, unsigned char** rgb_buffer,
                  int* size, int* width, int* height);

int read_bmp_file_1(const char* bmp_file, unsigned char** rgb_buffer, int* rgb_size,
                    unsigned char** palette_buf, int* palette_len,
                    int* width, int* height);
/**
 * 保存BMP文件
 * 
 * @param[IN]  bmp_file   BMP图片文件名称
 * 
 * @param[IN]  rgb_buffer RGB数据(实际为BGR)
 * @param[IN]  width      图片宽
 * @param[IN]  height     图片高
 *
 * @return 
 *         0：成功
 *         -1：打开文件失败
 * @note
 *         BMP图片颜色分量实际为BGR，因此，需要事先将rgb_buffer数据排列顺序转换成BGR。
 */
int write_bmp_file(const char* bmp_file, unsigned char* rgb_buffer, int width, int height);

int write_bmp_file_1(const char* bmp_file, unsigned char* rgb_buffer,
                     unsigned char* palette_buf, int* palette_len,
                     int width, int height);
#ifdef __cplusplus
};
#endif

#endif /* _BMP_UTILS_H */