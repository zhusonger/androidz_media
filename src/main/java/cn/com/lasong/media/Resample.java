package cn.com.lasong.media;

import java.nio.ByteBuffer;

public class Resample {
    // 禁止修改, 由native赋值
    private long nativeSwrContext = 0;
    static {
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("resample");
    }

    /**
     * 双声道转单声道ja
     * 单声道直接读取src的值
     * 双声道转换一下
     * @param src   双声道解码字节数据
     * @param dst   单声道结果容器
     * @param channel 声道数
     */
    public static native void stereo2mono(/*DirectByteBuffer*/ByteBuffer src, byte[] dst, int channel);

    /**
     * 将dst数据更新为混音后的数据
     * @param dst  原音频
     * @param mix  混音音频
     */
    public static native void mix(/*DirectByteBuffer*/ByteBuffer dst, byte[] mix);

    /**
     * 重采样
     * @param in_data   解码源音频字节数据
     * @param out_data  采样后的字节数据容器
     * @param volume    采样音量调节
     * @return  返回实际重采样后的长度
     */
    private native int resample(long nativeSwrContext, byte[] in_data, byte[] out_data, float volume);
    public int resample(byte[] in_data, byte[] out_data) {
        return resample(nativeSwrContext, in_data, out_data, 2);
    }
    /**
     * 初始化重采样工具类
     * @param src_channel_layout 源音频声道排布 {@link cn.com.lasong.media.AVChannelLayout}
     * @param src_fmt   源音频格式 {@link cn.com.lasong.media.AVSampleFormat}
     * @param src_rate  源音频采样率
     * @param dst_channel_layout 目标音频声道排布
     * @param dst_fmt   目标音频格式
     * @param dst_rate 目标音频采样率
     * @return
     */
    private native int init(long nativeSwrContext, long src_channel_layout, int src_fmt, int src_rate,
                           long dst_channel_layout, int dst_fmt, int dst_rate);
    public int init(long src_channel_layout, int src_fmt, int src_rate,
                    long dst_channel_layout, int dst_fmt, int dst_rate) {
        return init(nativeSwrContext, src_channel_layout, src_fmt, src_rate, dst_channel_layout, dst_fmt, dst_rate);
    }
    /**
     * 销毁重采样工具类
     */
    private native void destroy(long nativeSwrContext);
    public void release() {
        destroy(nativeSwrContext);
    }
}