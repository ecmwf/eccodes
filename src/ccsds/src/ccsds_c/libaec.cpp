#include "libaec.h"
#include "encoder/EncoderSeq.h"
#include "decoder/DecoderSeq.h"
#include "Span.h"
#include "BitBuffer.h"

#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

struct internal_state {
    enum Mode { ENCODE, DECODE } mode;
    // Buffered input/output for streaming
    std::vector<unsigned char> in_buf;
    std::vector<unsigned char> out_buf;
    bool finished;
};

static int validate_params(const struct aec_stream *strm) {
    if (!strm)
        return AEC_STREAM_ERROR;
    if (strm->bits_per_sample < 1 || strm->bits_per_sample > 32)
        return AEC_CONF_ERROR;
    if (strm->block_size == 0)
        return AEC_CONF_ERROR;
    if (strm->rsi == 0)
        return AEC_CONF_ERROR;

    // Validate block_size is one of the allowed values
    unsigned int bs = strm->block_size;
    if (bs != 8 && bs != 16 && bs != 32 && bs != 64) {
        if (!(strm->flags & AEC_NOT_ENFORCE))
            return AEC_CONF_ERROR;
    }

    return AEC_OK;
}

static size_t bytes_per_sample(unsigned int bps) {
    if (bps <= 8)  return 1;
    if (bps <= 16) return 2;
    // AEC_DATA_3BYTE would use 3, but we handle it as 4 for simplicity
    return 4;
}

static CodeOptionSet get_code_option_set(unsigned int flags) {
    return (flags & AEC_RESTRICTED) ? CodeOptionSet::Restricted : CodeOptionSet::Basic;
}

// Pack input bytes into a sample vector, respecting MSB/LSB flag
template <typename T>
static void unpack_samples(const unsigned char *src, size_t n_samples,
                           unsigned int flags, std::vector<T> &out) {
    out.resize(n_samples);
    size_t bps_bytes = sizeof(T);

    if (flags & AEC_DATA_MSB) {
        for (size_t i = 0; i < n_samples; ++i) {
            T val = 0;
            for (size_t b = 0; b < bps_bytes; ++b) {
                val = (val << 8) | src[i * bps_bytes + b];
            }
            out[i] = val;
        }
    } else {
        for (size_t i = 0; i < n_samples; ++i) {
            T val = 0;
            for (size_t b = 0; b < bps_bytes; ++b) {
                val |= static_cast<T>(src[i * bps_bytes + b]) << (b * 8);
            }
            out[i] = val;
        }
    }
}

// Pack sample vector into output bytes, respecting MSB/LSB flag
template <typename T>
static void pack_samples(const std::vector<T> &samples, unsigned int flags,
                         unsigned char *dst) {
    size_t bps_bytes = sizeof(T);

    if (flags & AEC_DATA_MSB) {
        for (size_t i = 0; i < samples.size(); ++i) {
            T val = samples[i];
            for (size_t b = 0; b < bps_bytes; ++b) {
                dst[i * bps_bytes + (bps_bytes - 1 - b)] = static_cast<unsigned char>(val & 0xFF);
                val >>= 8;
            }
        }
    } else {
        for (size_t i = 0; i < samples.size(); ++i) {
            T val = samples[i];
            for (size_t b = 0; b < bps_bytes; ++b) {
                dst[i * bps_bytes + b] = static_cast<unsigned char>(val & 0xFF);
                val >>= 8;
            }
        }
    }
}

// Encode a buffer of samples using the ccsds encoder
template <typename T>
static int do_encode(struct aec_stream *strm) {
    size_t sample_bytes = sizeof(T);
    size_t n_samples = strm->avail_in / sample_bytes;
    if (n_samples == 0) return AEC_OK;

    std::vector<T> samples;
    unpack_samples<T>(strm->next_in, n_samples, strm->flags, samples);

    CodeOptionSet cos = get_code_option_set(strm->flags);

    // Allocate output buffer large enough
    size_t max_out = n_samples * sample_bytes * 2 + 1024;
    std::vector<uint8_t> encoded(max_out, 0);

    // Add safety padding for BitBuffer read-ahead (8 bytes)
    encoded.resize(max_out + 8, 0);

    encoder::EncoderSeq<T, T> enc(
        static_cast<uint8_t>(strm->bits_per_sample),
        static_cast<uint8_t>(strm->block_size),
        static_cast<uint16_t>(strm->rsi),
        cos
    );

    size_t enc_bytes = enc.encode(
        Span<T>(samples.data(), samples.size()),
        Span<uint8_t>(encoded.data(), max_out)
    );

    size_t to_copy = std::min(enc_bytes, strm->avail_out);
    std::memcpy(strm->next_out, encoded.data(), to_copy);

    strm->next_in += n_samples * sample_bytes;
    strm->avail_in -= n_samples * sample_bytes;
    strm->total_in += n_samples * sample_bytes;

    strm->next_out += to_copy;
    strm->avail_out -= to_copy;
    strm->total_out += to_copy;

    return AEC_OK;
}

// Decode a buffer of compressed data
template <typename T>
static int do_decode(struct aec_stream *strm) {
    size_t sample_bytes = sizeof(T);
    size_t n_samples = strm->avail_out / sample_bytes;
    if (n_samples == 0) return AEC_OK;

    // We need to pass the compressed input to the decoder.
    // Add safety padding for BitBuffer read-ahead (8 bytes)
    size_t padded_size = strm->avail_in + 8;
    std::vector<uint8_t> input(padded_size, 0);
    std::memcpy(input.data(), strm->next_in, strm->avail_in);

    CodeOptionSet cos = get_code_option_set(strm->flags);

    std::vector<T> decoded(n_samples, 0);
    DecoderSeq<T, T> dec(
        static_cast<uint8_t>(strm->bits_per_sample),
        static_cast<uint8_t>(strm->block_size),
        static_cast<uint16_t>(strm->rsi),
        cos
    );

    dec.decode(
        Span<uint8_t>(input.data(), padded_size),
        Span<T>(decoded.data(), decoded.size())
    );

    // Pack decoded samples back to output
    pack_samples<T>(decoded, strm->flags, strm->next_out);

    size_t out_bytes = n_samples * sample_bytes;
    strm->next_out += out_bytes;
    strm->avail_out -= out_bytes;
    strm->total_out += out_bytes;

    strm->next_in += strm->avail_in;
    strm->total_in += strm->avail_in;
    strm->avail_in = 0;

    return AEC_OK;
}

extern "C" {

int aec_encode_init(struct aec_stream *strm) {
    int rc = validate_params(strm);
    if (rc != AEC_OK) return rc;

    auto *state = new (std::nothrow) internal_state;
    if (!state) return AEC_MEM_ERROR;
    state->mode = internal_state::ENCODE;
    state->finished = false;
    strm->state = state;
    strm->total_in = 0;
    strm->total_out = 0;
    return AEC_OK;
}

int aec_encode(struct aec_stream *strm, int flush) {
    if (!strm || !strm->state) return AEC_STREAM_ERROR;
    if (strm->state->mode != internal_state::ENCODE) return AEC_STREAM_ERROR;

    if (flush == AEC_FLUSH) {
        // Accumulate all input first if streaming
        strm->state->in_buf.insert(
            strm->state->in_buf.end(),
            strm->next_in,
            strm->next_in + strm->avail_in
        );
        strm->total_in += strm->avail_in;
        strm->next_in += strm->avail_in;
        strm->avail_in = 0;

        // Now encode the full accumulated buffer
        size_t sample_bytes = bytes_per_sample(strm->bits_per_sample);
        size_t n_samples = strm->state->in_buf.size() / sample_bytes;

        // Temporarily point strm at the accumulated buffer
        const unsigned char *orig_in = strm->next_in;
        size_t orig_avail_in = strm->avail_in;
        size_t orig_total_in = strm->total_in;

        strm->next_in = strm->state->in_buf.data();
        strm->avail_in = n_samples * sample_bytes;
        strm->total_in = 0;

        int rc;
        if (strm->bits_per_sample <= 8)
            rc = do_encode<uint8_t>(strm);
        else if (strm->bits_per_sample <= 16)
            rc = do_encode<uint16_t>(strm);
        else
            rc = do_encode<uint32_t>(strm);

        // Restore total_in to accumulated total
        strm->total_in = orig_total_in;
        strm->state->in_buf.clear();
        strm->state->finished = true;
        return rc;
    }

    // AEC_NO_FLUSH: buffer the input for later
    strm->state->in_buf.insert(
        strm->state->in_buf.end(),
        strm->next_in,
        strm->next_in + strm->avail_in
    );
    strm->total_in += strm->avail_in;
    strm->next_in += strm->avail_in;
    strm->avail_in = 0;

    return AEC_OK;
}

int aec_encode_end(struct aec_stream *strm) {
    if (!strm || !strm->state) return AEC_STREAM_ERROR;
    delete strm->state;
    strm->state = nullptr;
    return AEC_OK;
}

int aec_decode_init(struct aec_stream *strm) {
    int rc = validate_params(strm);
    if (rc != AEC_OK) return rc;

    auto *state = new (std::nothrow) internal_state;
    if (!state) return AEC_MEM_ERROR;
    state->mode = internal_state::DECODE;
    state->finished = false;
    strm->state = state;
    strm->total_in = 0;
    strm->total_out = 0;
    return AEC_OK;
}

int aec_decode(struct aec_stream *strm, int flush) {
    if (!strm || !strm->state) return AEC_STREAM_ERROR;
    if (strm->state->mode != internal_state::DECODE) return AEC_STREAM_ERROR;

    if (flush == AEC_FLUSH) {
        strm->state->in_buf.insert(
            strm->state->in_buf.end(),
            strm->next_in,
            strm->next_in + strm->avail_in
        );
        strm->total_in += strm->avail_in;
        strm->next_in += strm->avail_in;
        strm->avail_in = 0;

        const unsigned char *orig_in = strm->next_in;
        size_t orig_avail_in = strm->avail_in;
        size_t orig_total_in = strm->total_in;

        strm->next_in = strm->state->in_buf.data();
        strm->avail_in = strm->state->in_buf.size();
        strm->total_in = 0;

        int rc;
        if (strm->bits_per_sample <= 8)
            rc = do_decode<uint8_t>(strm);
        else if (strm->bits_per_sample <= 16)
            rc = do_decode<uint16_t>(strm);
        else
            rc = do_decode<uint32_t>(strm);

        strm->total_in = orig_total_in;
        strm->state->in_buf.clear();
        strm->state->finished = true;
        return rc;
    }

    // AEC_NO_FLUSH: buffer the input
    strm->state->in_buf.insert(
        strm->state->in_buf.end(),
        strm->next_in,
        strm->next_in + strm->avail_in
    );
    strm->total_in += strm->avail_in;
    strm->next_in += strm->avail_in;
    strm->avail_in = 0;

    return AEC_OK;
}

int aec_decode_end(struct aec_stream *strm) {
    if (!strm || !strm->state) return AEC_STREAM_ERROR;
    delete strm->state;
    strm->state = nullptr;
    return AEC_OK;
}

int aec_buffer_encode(struct aec_stream *strm) {
    int rc = aec_encode_init(strm);
    if (rc != AEC_OK) return rc;
    rc = aec_encode(strm, AEC_FLUSH);
    aec_encode_end(strm);
    return rc;
}

int aec_buffer_decode(struct aec_stream *strm) {
    int rc = aec_decode_init(strm);
    if (rc != AEC_OK) return rc;
    rc = aec_decode(strm, AEC_FLUSH);
    aec_decode_end(strm);
    return rc;
}

} // extern "C"
