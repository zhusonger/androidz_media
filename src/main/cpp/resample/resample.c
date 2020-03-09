/* resample.c - sampling rate conversion subroutines
 *
 * Original version available at the
 * Digital Audio Resampling Home Page located at
 * http://ccrma.stanford.edu/~jos/resample/.
 *
 * Modified for use on Android by Ethan Chen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "resample.h"
#include "resample.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <android/log.h>

#include "../speexdst/speex/speex_resampler.h"
#include "../speexdst/speex/speexdsp_config_types.h"

#define MAX_AUDIO_SIZE 32767
#define MIN_AUDIO_SIZE -32768
static SpeexResamplerState *resampler = NULL;

JNIEXPORT void JNICALL Java_cn_com_lasong_media_Resample_stereo2mono
		(JNIEnv *env, jclass clz, jobject src, jbyteArray dst, jint channel) {

	if (!src || !dst) {
		return;
	}

	jbyte* src_buffer = (*env)->GetDirectBufferAddress(env, src);
	jbyte* dst_buffer = (*env)->GetByteArrayElements(env, dst, 0);
	jsize len = (*env)->GetArrayLength(env, dst);
	if (channel == 2) {
		// stereo2mono
		for (int i = 0, j = 0; i < len; j+=2, i+=2) {
			dst_buffer[i] = (jbyte) ((src_buffer[j * 2] + src_buffer[j * 2 + 2]) / 2);
			dst_buffer[i+1] = (jbyte) ((src_buffer[j * 2 + 1] + src_buffer[j * 2 + 3]) / 2);
		}
		(*env)->SetByteArrayRegion(env, dst, 0, len, dst_buffer);
	} else {
		(*env)->SetByteArrayRegion(env, dst, 0, len, src_buffer);
	}
    (*env)->ReleaseByteArrayElements(env, dst, dst_buffer, JNI_ABORT);
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
    if (!mix || !dst) {
        return;
    }

    // Step1:准备更新的数组
    jbyte *dst_buffer = (*env)->GetDirectBufferAddress(env, dst);
    jlong dst_len = (*env)->GetDirectBufferCapacity(env, dst);

	// 转换成short数组
	int short_dst_len = (int) (dst_len / 2);
	jshortArray *array_short_dst = (*env)->NewShortArray(env, short_dst_len);
	jshort *short_dst = (*env)->GetShortArrayElements(env, array_short_dst, 0);
	// 转换byte到short, 2个byte对应1个short
	for (int i = 0; i < short_dst_len; i++) {
		short low = dst_buffer[i * 2];
		short high = dst_buffer[i * 2 + 1];
		short_dst[i] = (short) ((low & 0xff) | (high << 8));
	}

	// Step2:需要混音的数据
	jbyte *mix_buffer = (*env)->GetByteArrayElements(env, mix, 0);
	jsize len_mix = (*env)->GetArrayLength(env, mix);
	int short_mix_len = len_mix / 2;
	jshortArray *array_short_mix = (*env)->NewShortArray(env, short_mix_len);
	jshort *short_mix = (*env)->GetShortArrayElements(env, array_short_mix, 0);
	// 转换byte到short, 2个byte对应1个short
	for (int i = 0; i < short_mix_len; i++) {
		short low = mix_buffer[i * 2];
		short high = mix_buffer[i * 2 + 1];
		short_mix[i] = (short) ((low & 0xff) | (high << 8));
	}

	// 转换byte到short, 2个byte对应1个short
	for (int i = 0; i < short_dst_len; i++) {
		if (i < short_mix_len) {
			short dst_sample = short_dst[i];
			short mix_sample = short_mix[i];
			short final_sample = TPMixSamples(dst_sample, mix_sample);
			short_dst[i] = final_sample;
		}
	}

	// Step3:更新result
	// 转换short为byte数组
	for (int i = 0; i < short_dst_len; i++) {
		short value = short_dst[i];
		if (value > MAX_AUDIO_SIZE) {
			value = MAX_AUDIO_SIZE;
		} else if (value < MIN_AUDIO_SIZE) {
			value = MAX_AUDIO_SIZE;
		}
		short low = (short) (value & 0xff);
		short high = (short) ((value >> 8) & 0xff);
		dst_buffer[i * 2] = (jbyte) low;
		dst_buffer[i * 2 + 1] = (jbyte) high;
	}
	// 索引重置为0
    jclass cls = (*env)->GetObjectClass(env, dst);
    jmethodID mid = (*env)->GetMethodID(env, cls, "position", "(I)Ljava/nio/Buffer;");
    (*env)->CallObjectMethod(env, dst, mid, 0);

	// 释放dst相关
	// 释放新建的short数组
	(*env)->ReleaseShortArrayElements(env, array_short_dst, short_dst, 0);
	(*env)->DeleteLocalRef(env, array_short_dst);
    (*env)->DeleteLocalRef(env, cls);
	// 释放获取的byte引用
//	(*env)->ReleaseByteArrayElements(env, dst, dst_buffer, JNI_ABORT);

    // 释放mix相关
	// 释放新建的short数组
	(*env)->ReleaseShortArrayElements(env, array_short_mix, short_mix, 0);
	(*env)->DeleteLocalRef(env, array_short_mix);
	// 释放获取的byte引用
	(*env)->ReleaseByteArrayElements(env, mix, mix_buffer, JNI_ABORT);
}

/*
 * Class:     net_sourceforge_resample_Resample
 * Method:    init
 * Signature: (II)V
 */
JNIEXPORT int JNICALL Java_cn_com_lasong_media_Resample_init
		(JNIEnv *env, jclass clz, jint inRate, jint outRate) {
	if (resampler) {
		speex_resampler_destroy(resampler);
		resampler = NULL;
	}
	int err = 0;
	resampler = speex_resampler_init(1, (spx_uint32_t) inRate, (spx_uint32_t) outRate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
	speex_resampler_skip_zeros(resampler);
	return err;
}

/*
 * Class:     net_sourceforge_resample_Resample
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_cn_com_lasong_media_Resample_destroy
		(JNIEnv *env, jclass clz) {
	if (resampler) {
		speex_resampler_destroy(resampler);
		resampler = NULL;
	}
}

JNIEXPORT jint JNICALL Java_cn_com_lasong_media_Resample_resample
        (JNIEnv *env, jclass clz, jbyteArray in, jbyteArray out, jfloat volume) {
    if (!in || !out || !resampler) {
        return -1;
    }
    // in
	jbyte *in_byte = (*env)->GetByteArrayElements(env, in, 0);
    jsize in_len = (*env)->GetArrayLength(env, in);
    int in_short_len = in_len / 2;
    // out
	jbyte *out_byte = (*env)->GetByteArrayElements(env, out, 0);
	jsize out_len = (*env)->GetArrayLength(env, out);
	int out_short_len = out_len / 2;

    // in
	jshortArray *array_short_in = (*env)->NewShortArray(env, in_short_len);
	jshort * short_in = (*env)->GetShortArrayElements(env, array_short_in, 0);
	// 转换byte到short, 2个byte对应1个short
	for (int i = 0; i < in_short_len; i++) {
		short low = in_byte[i * 2];
		short high = in_byte[i * 2 + 1];
        short_in[i] = (short) ((low & 0xff) | (high << 8));
	}

	// out
    jshortArray *array_short_out = (*env)->NewShortArray(env, out_short_len);
    jshort * short_out = (*env)->GetShortArrayElements(env, array_short_out, 0);
    int ret = speex_resampler_process_int(resampler, 0,
                                          short_in/*in*/, (spx_uint32_t *) &in_short_len,
                                          short_out/*out*/, (spx_uint32_t *) &out_short_len);

	// out_short_len会更改为最新的值
	int out_resample_byte_length = out_short_len * 2;
    if (ret == RESAMPLER_ERR_SUCCESS) {
		// 重采样后的数据
		// 转换short为byte数组
		for (int i = 0; i < out_short_len; i++) {
		    short value = (short) (short_out[i] * volume);
		    if (value > MAX_AUDIO_SIZE) {
		    	value = MAX_AUDIO_SIZE;
		    } else if (value < MIN_AUDIO_SIZE) {
		    	value = MAX_AUDIO_SIZE;
		    }
			short low = (short) (value & 0xff);
			short high = (short) ((value >> 8) & 0xff);
			out_byte[i * 2] = (jbyte) low;
			out_byte[i * 2 + 1] = (jbyte) high;
		}
		(*env)->SetByteArrayRegion(env, out, 0, out_resample_byte_length, out_byte);
    }

    // in
    (*env)->ReleaseShortArrayElements(env, array_short_in, short_in, 0);
	(*env)->DeleteLocalRef(env, array_short_in);
    (*env)->ReleaseByteArrayElements(env, in, in_byte, JNI_ABORT);
    // out
    (*env)->ReleaseShortArrayElements(env, array_short_out, short_out, 0);
    (*env)->DeleteLocalRef(env, array_short_out);
    (*env)->ReleaseByteArrayElements(env, out, out_byte, JNI_ABORT);

    return out_resample_byte_length;
}



