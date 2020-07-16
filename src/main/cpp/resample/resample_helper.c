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
}

void swr_ext_free(SwrContextExt **ss) {
    SwrContextExt *s= *ss;
    if(s){
        clear_context(s);
    }
    av_freep(ss);
}
