#ifndef LIBAEC_H
#define LIBAEC_H 1

#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

struct internal_state;

struct aec_stream {
    const unsigned char *next_in;
    size_t avail_in;
    size_t total_in;

    unsigned char *next_out;
    size_t avail_out;
    size_t total_out;

    unsigned int bits_per_sample;
    unsigned int block_size;
    unsigned int rsi;
    unsigned int flags;

    struct internal_state *state;
};

/* Sample data description flags */
#define AEC_DATA_SIGNED 1
#define AEC_DATA_3BYTE 2
#define AEC_DATA_MSB 4
#define AEC_DATA_PREPROCESS 8
#define AEC_RESTRICTED 16
#define AEC_PAD_RSI 32
#define AEC_NOT_ENFORCE 64

/* Return codes */
#define AEC_OK 0
#define AEC_CONF_ERROR (-1)
#define AEC_STREAM_ERROR (-2)
#define AEC_DATA_ERROR (-3)
#define AEC_MEM_ERROR (-4)

/* Flush options */
#define AEC_NO_FLUSH 0
#define AEC_FLUSH 1

/* Streaming encoding and decoding functions */
int aec_encode_init(struct aec_stream *strm);
int aec_encode(struct aec_stream *strm, int flush);
int aec_encode_end(struct aec_stream *strm);

int aec_decode_init(struct aec_stream *strm);
int aec_decode(struct aec_stream *strm, int flush);
int aec_decode_end(struct aec_stream *strm);

/* Utility functions for encoding or decoding a memory buffer */
int aec_buffer_encode(struct aec_stream *strm);
int aec_buffer_decode(struct aec_stream *strm);

#ifdef __cplusplus
}
#endif

#endif /* LIBAEC_H */
