

#include "resample.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "resample_helper.h"


JNIEXPORT void JNICALL Java_cn_com_lasong_media_Resample_stereo2mono
		(JNIEnv *env, jclass clz, jobject src, jbyteArray dst, jint channel) {

//	if (!src || !dst) {
//		return;
//	}
//
//	jbyte* src_buffer = (*env)->GetDirectBufferAddress(env, src);
//	jbyte* dst_buffer = (*env)->GetByteArrayElements(env, dst, 0);
//	jsize len = (*env)->GetArrayLength(env, dst);
//	if (channel == 2) {
//		// stereo2mono
//		for (int i = 0, j = 0; i < len; j+=2, i+=2) {
//			dst_buffer[i] = (jbyte) ((src_buffer[j * 2] + src_buffer[j * 2 + 2]) / 2);
//			dst_buffer[i+1] = (jbyte) ((src_buffer[j * 2 + 1] + src_buffer[j * 2 + 3]) / 2);
//		}
//		(*env)->SetByteArrayRegion(env, dst, 0, len, dst_buffer);
//	} else {
//		(*env)->SetByteArrayRegion(env, dst, 0, len, src_buffer);
//	}
//    (*env)->ReleaseByteArrayElements(env, dst, dst_buffer, JNI_ABORT);
}

int16_t TPMixSamples(int16_t a, int16_t b) {
	return
		// If both samples are negative, mixed signal must have an amplitude between the lesser of A and B, and the minimum permissible negative amplitude
			(int16_t) (a < 0 && b < 0 ?
					   ((int) a + (int) b) - (((int) a * (int) b) / INT16_MIN) :

					   // If both samples are positive, mixed signal must have an amplitude between the greater of A and B, and the maximum permissible positive amplitude
					   (a > 0 && b > 0 ?
						((int) a + (int) b) - (((int) a * (int) b) / INT16_MAX)

						// If samples are on opposite sides of the 0-crossing, mixed signal should reflect that samples cancel each other out somewhat
									   :
						a + b));
}

JNIEXPORT void JNICALL Java_cn_com_lasong_media_Resample_mix
        (JNIEnv *env, jclass clz, jobject dst, jbyteArray mix) {
//    if (!mix || !dst) {
//        return;
//    }
//
//    // Step1:准备更新的数组
//    jbyte *dst_buffer = (*env)->GetDirectBufferAddress(env, dst);
//    jlong dst_len = (*env)->GetDirectBufferCapacity(env, dst);
//
//	// 转换成short数组
//	int short_dst_len = (int) (dst_len / 2);
//	jshortArray *array_short_dst = (*env)->NewShortArray(env, short_dst_len);
//	jshort *short_dst = (*env)->GetShortArrayElements(env, array_short_dst, 0);
//	// 转换byte到short, 2个byte对应1个short
//	for (int i = 0; i < short_dst_len; i++) {
//		short low = dst_buffer[i * 2];
//		short high = dst_buffer[i * 2 + 1];
//		short_dst[i] = (short) ((low & 0xff) | (high << 8));
//	}
//
//	// Step2:需要混音的数据
//	jbyte *mix_buffer = (*env)->GetByteArrayElements(env, mix, 0);
//	jsize len_mix = (*env)->GetArrayLength(env, mix);
//	int short_mix_len = len_mix / 2;
//	jshortArray *array_short_mix = (*env)->NewShortArray(env, short_mix_len);
//	jshort *short_mix = (*env)->GetShortArrayElements(env, array_short_mix, 0);
//	// 转换byte到short, 2个byte对应1个short
//	for (int i = 0; i < short_mix_len; i++) {
//		short low = mix_buffer[i * 2];
//		short high = mix_buffer[i * 2 + 1];
//		short_mix[i] = (short) ((low & 0xff) | (high << 8));
//	}
//
//	// 转换byte到short, 2个byte对应1个short
//	for (int i = 0; i < short_dst_len; i++) {
//		if (i < short_mix_len) {
//			short dst_sample = short_dst[i];
//			short mix_sample = short_mix[i];
//			short final_sample = TPMixSamples(dst_sample, mix_sample);
//			short_dst[i] = final_sample;
//		}
//	}
//
//	// Step3:更新result
//	// 转换short为byte数组
//	for (int i = 0; i < short_dst_len; i++) {
//		short value = short_dst[i];
//		if (value > MAX_AUDIO_SIZE) {
//			value = MAX_AUDIO_SIZE;
//		} else if (value < MIN_AUDIO_SIZE) {
//			value = MAX_AUDIO_SIZE;
//		}
//		short low = (short) (value & 0xff);
//		short high = (short) ((value >> 8) & 0xff);
//		dst_buffer[i * 2] = (jbyte) low;
//		dst_buffer[i * 2 + 1] = (jbyte) high;
//	}
//	// 索引重置为0
//    jclass cls = (*env)->GetObjectClass(env, dst);
//    jmethodID mid = (*env)->GetMethodID(env, cls, "position", "(I)Ljava/nio/Buffer;");
//    (*env)->CallObjectMethod(env, dst, mid, 0);
//
//	// 释放dst相关
//	// 释放新建的short数组
//	(*env)->ReleaseShortArrayElements(env, array_short_dst, short_dst, 0);
//	(*env)->DeleteLocalRef(env, array_short_dst);
//    (*env)->DeleteLocalRef(env, cls);
//	// 释放获取的byte引用
////	(*env)->ReleaseByteArrayElements(env, dst, dst_buffer, JNI_ABORT);
//
//    // 释放mix相关
//	// 释放新建的short数组
//	(*env)->ReleaseShortArrayElements(env, array_short_mix, short_mix, 0);
//	(*env)->DeleteLocalRef(env, array_short_mix);
//	// 释放获取的byte引用
//	(*env)->ReleaseByteArrayElements(env, mix, mix_buffer, JNI_ABORT);
}

/*
 * Class:     net_sourceforge_resample_Resample
 * Method:    init
 * Signature: (II)V
 */
JNIEXPORT int JNICALL Java_cn_com_lasong_media_Resample_init
		(JNIEnv *env, jobject thiz,jlong nativeSwrContext, jlong src_channel_layout, jint src_fmt, jint src_rate,
		 jlong dst_channel_layout, jint dst_fmt, jint dst_rate) {
	SwrContext *swr_ctx;
	SwrContextExt *ctx;
    AVSampleFormat src_sample_fmt = (AVSampleFormat) src_fmt;
    AVSampleFormat dst_sample_fmt = (AVSampleFormat)dst_fmt;
	if (nativeSwrContext == 0) {
		ctx = (SwrContextExt *)(malloc(sizeof(SwrContextExt)));
        swr_ctx = swr_alloc_set_opts(NULL, dst_channel_layout, dst_sample_fmt, dst_rate, src_channel_layout,
                                     src_sample_fmt, src_rate, 0, NULL);
		ctx->swr_ctx = swr_ctx;
    } else {
		ctx = (SwrContextExt *) nativeSwrContext;
		swr_ctx = ctx->swr_ctx;
        /* set options */
        av_opt_set_int(swr_ctx, "in_channel_layout",    src_channel_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate",       src_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", src_sample_fmt, 0);

        av_opt_set_int(swr_ctx, "out_channel_layout",    dst_channel_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate",       dst_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);
	}
	ctx->src_sample_fmt = src_sample_fmt;
	ctx->src_sample_rate = src_rate;
	ctx->src_nb_channels = av_get_channel_layout_nb_channels(src_channel_layout);
	ctx->src_nb_buffers = av_sample_fmt_is_planar(src_sample_fmt) ? ctx->src_nb_channels : 1;

	ctx->dst_sample_fmt = dst_sample_fmt;
	ctx->dst_sample_rate = dst_rate;
	ctx->dst_nb_channels = av_get_channel_layout_nb_channels(dst_channel_layout);
	ctx->dst_nb_buffers = av_sample_fmt_is_planar(dst_sample_fmt) ? ctx->dst_nb_channels : 1;


	int ret = 0;
	if (!swr_ctx) {
        LOGE("Could not allocate resampler context\n");
        goto error;
	}

	/* initialize the resampling context */
	if ((ret = swr_init(swr_ctx)) < 0) {
        LOGE("Failed to initialize the resampling context\n");
        goto error;
	}

	if (nativeSwrContext == 0) {
        ret = av_samples_alloc_array_and_samples(&ctx->src_buffers, &ctx->src_linesize, ctx->src_nb_channels,
                                                 DEFAULT_NB_SAMPLES, ctx->src_sample_fmt, 0);
        if (ret < 0) {
            LOGE("Could not allocate source samples\n");
            goto error;
        }

        ret = av_samples_alloc_array_and_samples(&ctx->dst_buffers, &ctx->dst_linesize, ctx->dst_nb_channels,
                                                 DEFAULT_NB_SAMPLES, ctx->dst_sample_fmt, 0);
        if (ret < 0) {
            LOGE("Could not allocate destination samples\n");
            goto error;
        }

        jclass clz = (*env)->GetObjectClass(env, thiz);
        jfieldID fieldId = (*env)->GetFieldID(env, clz, "nativeSwrContext", "J");
        // 初始化成功
        (*env)->SetLongField(env, thiz, fieldId, (jlong) ctx);
	}
    return 0;

error:
    ret = AVERROR(ENOMEM);
    swr_ext_free(&ctx);
	return ret;
}

/*
 * Class:     net_sourceforge_resample_Resample
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_cn_com_lasong_media_Resample_destroy
		(JNIEnv *env, jobject thiz, jlong nativeSwrContext) {
    if (nativeSwrContext == 0) {
        return;
    }
    // 将nativeSWContext设置为0，防止重复调用close导致崩溃
    jclass clz = (*env)->GetObjectClass(env, thiz);
    jfieldID fieldId = (*env)->GetFieldID(env, clz, "nativeSwrContext", "J");
    (*env)->SetLongField(env, thiz, fieldId, (jlong) 0);

    SwrContextExt *ctx = (SwrContextExt *) nativeSwrContext;
    swr_ext_free(&ctx);
}

JNIEXPORT jint JNICALL Java_cn_com_lasong_media_Resample_resample
        (JNIEnv *env, jobject thiz, jlong nativeSwrContext , jbyteArray src_data, jint src_len) {
    if (nativeSwrContext == 0) {
        return -1;
    }
    SwrContextExt *ctx = (SwrContextExt *) nativeSwrContext;

    jbyte *data = (*env)->GetByteArrayElements(env, src_data, NULL);

    // 源数据大于预设的输入buffer大小, 重新分配
    if (src_len > ctx->src_linesize) {
        // 重新分配输出buffer
        if (ctx->src_buffers) {
            av_freep(&ctx->src_buffers[0]);
        }
        ret = av_samples_alloc_array_and_samples(&ctx->src_buffers, &ctx->src_linesize, ctx->src_nb_channels,
                                                 DEFAULT_NB_SAMPLES, ctx->src_sample_fmt, 0);
        if (ret < 0) {
            LOGE("Could not allocate source samples\n");
            goto error;
        }
        int ret = av_samples_alloc(ctx->src_buffers, &ctx->src_linesize, ctx->src_nb_channels,
                         src_len, ctx->src_sample_fmt, 1)
        if (ret < 0) {
            LOGE("Could not allocate source samples\n");
            goto error;
        }
    }
    // packed类型的音频数据只需要传入第一个即可, swr_convert参数注释有说明
    // 如果是planner, 这里就是分割成每个声道的数据
    int single_len = src_len / ctx->src_nb_buffers;
    for (int i = 0; i < ctx->src_nb_buffers; i++) {
        memcpy(ctx->src_buffers[i], (uint8_t *) data + single_len * i, single_len);
    }
    swr_convert()
    (*env)->ReleaseByteArrayElements(env, src_buffer, data, 0);
    /* compute destination number of samples */
    int dst_nb_samples = av_rescale_rnd(swr_get_delay(ctx->swr_ctx, ctx->src_sample_rate) +
                                    1024, ctx->dst_sample_rate, ctx->src_sample_rate, AV_ROUND_UP);
//    // in
//	jbyte *in_byte = (*env)->GetByteArrayElements(env, in, 0);
//    jsize in_len = (*env)->GetArrayLength(env, in);
//    int in_short_len = in_len / 2;
//    // out
//	jbyte *out_byte = (*env)->GetByteArrayElements(env, out, 0);
//	jsize out_len = (*env)->GetArrayLength(env, out);
//	int out_short_len = out_len / 2;
//
//    // in
//	jshortArray *array_short_in = (*env)->NewShortArray(env, in_short_len);
//	jshort * short_in = (*env)->GetShortArrayElements(env, array_short_in, 0);
//	// 转换byte到short, 2个byte对应1个short
//	for (int i = 0; i < in_short_len; i++) {
//		short low = in_byte[i * 2];
//		short high = in_byte[i * 2 + 1];
//        short_in[i] = (short) ((low & 0xff) | (high << 8));
//	}
//
//	// out
//    jshortArray *array_short_out = (*env)->NewShortArray(env, out_short_len);
//    jshort * short_out = (*env)->GetShortArrayElements(env, array_short_out, 0);
//    int ret = speex_resampler_process_int(resampler, 0,
//                                          short_in/*in*/, (spx_uint32_t *) &in_short_len,
//                                          short_out/*out*/, (spx_uint32_t *) &out_short_len);
//
//	// out_short_len会更改为最新的值
//	int out_resample_byte_length = out_short_len * 2;
//    if (ret == RESAMPLER_ERR_SUCCESS) {
//		// 重采样后的数据
//		// 转换short为byte数组
//		for (int i = 0; i < out_short_len; i++) {
//		    short value = (short) (short_out[i] * volume);
//		    if (value > MAX_AUDIO_SIZE) {
//		    	value = MAX_AUDIO_SIZE;
//		    } else if (value < MIN_AUDIO_SIZE) {
//		    	value = MAX_AUDIO_SIZE;
//		    }
//			short low = (short) (value & 0xff);
//			short high = (short) ((value >> 8) & 0xff);
//			out_byte[i * 2] = (jbyte) low;
//			out_byte[i * 2 + 1] = (jbyte) high;
//		}
//		(*env)->SetByteArrayRegion(env, out, 0, out_resample_byte_length, out_byte);
//    }
//
//    // in
//    (*env)->ReleaseShortArrayElements(env, array_short_in, short_in, 0);
//	(*env)->DeleteLocalRef(env, array_short_in);
//    (*env)->ReleaseByteArrayElements(env, in, in_byte, JNI_ABORT);
//    // out
//    (*env)->ReleaseShortArrayElements(env, array_short_out, short_out, 0);
//    (*env)->DeleteLocalRef(env, array_short_out);
//    (*env)->ReleaseByteArrayElements(env, out, out_byte, JNI_ABORT);

    return 0;
}




