package cn.com.lasong.media;

import java.nio.ByteBuffer;

public class Resample {
    static {
        System.loadLibrary("resample");
    }

    /**
     * 双声道转单声道
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
    public static native int resample(byte[] in_data, byte[] out_data, float volume);

    /**
     * 初始化重采样工具类
     * 只做单声道重采样
     * @param inRate    输入采样率
     * @param outRate   输出采样率
     * @return
     */
    public static native int init(int inRate, int outRate);

    /**
     * 销毁重采样工具类
     */
    public static native void destroy();
}