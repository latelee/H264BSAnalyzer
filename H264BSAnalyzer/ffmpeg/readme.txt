ffmpeg°æ±¾£º2.6.3
¶¯Ì¬¿â

mingw±àÒëÃüÁî£º
./configure --prefix=../ffmpeg-2.6.3-bin --enable-shared --disable-static --enable-w32threads --disable-debug \
--enable-decoder=h264,hevc,mpeg4,mjpeg  --enable-demuxer=h264,hevc,avi --enable-muxer=h264,hevc,avi,mp4,mjpeg \
--enable-parser=h264,hevc,mjpeg --enable-protocol=file \
--disable-vaapi --disable-vdpau  --disable-dxva2 \
--extra-libs=-lmsvcrt

