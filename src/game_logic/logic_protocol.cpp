#include "game/logic_types.h"
#include "game/libg_api.h"

#include <cstdio>
#include <cstring>
#include <vector>

// ============================================================================
// Protocol message handling
// Replaces the original message codec and serialization.
// ============================================================================

// Byte stream helpers for message serialization
namespace {

struct ByteStream {
    std::vector<uint8_t> data;
    size_t pos = 0;

    void write_byte(uint8_t b) { data.push_back(b); }
    void write_int32(int32_t v) {
        write_byte((v >> 24) & 0xFF);
        write_byte((v >> 16) & 0xFF);
        write_byte((v >> 8) & 0xFF);
        write_byte(v & 0xFF);
    }
    void write_int64(int64_t v) {
        write_int32((int32_t)(v >> 32));
        write_int32((int32_t)(v & 0xFFFFFFFF));
    }
    void write_string(const char* s) {
        int32_t len = s ? strlen(s) : 0;
        write_int32(len);
        for (int32_t i = 0; i < len; i++) write_byte(s[i]);
    }

    uint8_t read_byte() {
        return pos < data.size() ? data[pos++] : 0;
    }
    int32_t read_int32() {
        int32_t v = ((int32_t)read_byte() << 24)
                  | ((int32_t)read_byte() << 16)
                  | ((int32_t)read_byte() << 8)
                  | (int32_t)read_byte();
        return v;
    }
    int64_t read_int64() {
        return ((int64_t)read_int32() << 32) | (int64_t)(uint32_t)read_int32();
    }
};

} // anonymous namespace

// ============================================================================
// LoginCodec (message type 10100 / 10101)
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN10LoginCodecC1Ev(void* self) {
    memset(self, 0, 8);
}

extern "C" LIBG_EXPORT
void _ZN10LoginCodecD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
int32_t _ZN10LoginCodec14decodeMessageIdEPKhi(void* self, const uint8_t* data, int32_t len) {
    // Decode the message type from the beginning of the byte stream
    if (!data || len < 4) return -1;
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

extern "C" LIBG_EXPORT
int32_t _ZN10LoginCodec14encodeMessageIdEiPh(void* self, int32_t msg_id, uint8_t* out) {
    if (!out) return 0;
    out[0] = (msg_id >> 24) & 0xFF;
    out[1] = (msg_id >> 16) & 0xFF;
    out[2] = (msg_id >> 8) & 0xFF;
    out[3] = msg_id & 0xFF;
    return 4;
}

// ============================================================================
// Message serialization
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN7MessageC1Ev(void* self) {
    memset(self, 0, 16);
}

extern "C" LIBG_EXPORT
void _ZN7MessageD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
int32_t _ZN7Message8getSizeEv(void* self) {
    // Return serialized message size
    return 0;
}

// ============================================================================
// Command serialization
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN13CommandStreamC1Ev(void* self) {
    memset(self, 0, 32);
}

extern "C" LIBG_EXPORT
void _ZN13CommandStreamD1Ev(void* self) {
}

extern "C" LIBG_EXPORT
void _ZN13CommandStream15addCommandToEndEPN5Logic7CommandE(void* self, void* cmd) {
    // TODO: Serialize and queue command
}

extern "C" LIBG_EXPORT
void _ZN13CommandStream11clearStreamEv(void* self) {
    // TODO: Clear command queue
}

// ============================================================================
// Factory
// ============================================================================

extern "C" LIBG_EXPORT
void _ZN7FactoryC1Ev(void* self) {
    memset(self, 0, 8);
}

extern "C" LIBG_EXPORT
void* _ZN7Factory17createMessageFromEPKhi(void* self, const uint8_t* data, int32_t len) {
    // TODO: Create message object from serialized data
    return nullptr;
}
