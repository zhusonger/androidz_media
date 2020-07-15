//
// Created by YouXi on 2020/7/14.
//

#include "resample_helper.h"

void clear_context(SwrContextExt *s){


    swr_free(&s->swr_ctx);
    if (s->src_buffers) {
        av_freep(&s->src_buffers[0]);
    }
    av_freep(&s->src_buffers);

    if (s->dst_buffers) {
        av_freep(&s->dst_buffers[0]);
    }
    av_freep(&s->dst_buffers);
    s->src_linesize = 0;
    s->dst_linesize = 0;
    s->src_sample_fmt = AV_SAMPLE_FMT_NONE;
    s->dst_sample_fmt = AV_SAMPLE_FMT_NONE;
    s->src_nb_buffers = 0;
    s->dst_nb_buffers = 0;
    s->src_nb_channels = 0;
    s->dst_nb_channels = 0;
    s->src_nb_samples = 0;
    s->dst_nb_samples = 0;
    s->max_dst_nb_samples = 0;
    s->src_sample_rate = 0;
    s->dst_sample_rate = 0;

    s->in_buffer_index= 0;
    s->in_buffer_count= 0;
    s->resample_in_constraint= 0;
    memset(s->in.ch, 0, sizeof(s->in.ch));
    memset(s->out.ch, 0, sizeof(s->out.ch));
    free_temp(&s->postin);
    free_temp(&s->midbuf);
    free_temp(&s->preout);
    free_temp(&s->in_buffer);
    free_temp(&s->silence);
    free_temp(&s->drop_temp);
    free_temp(&s->dither.noise);
    free_temp(&s->dither.temp);
    swri_audio_convert_free(&s-> in_convert);
    swri_audio_convert_free(&s->out_convert);
    swri_audio_convert_free(&s->full_convert);
    swri_rematrix_free(s);

    s->delayed_samples_fixup = 0;
    s->flushed = 0;
}

void swr_ext_free(SwrContextExt **ss) {
    SwrContextExt *s= *ss;
    if(s){
        clear_context(s);
    }
    av_freep(ss);
}
