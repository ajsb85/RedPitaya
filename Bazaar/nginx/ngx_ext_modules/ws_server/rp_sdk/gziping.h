#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <zlib.h>

/*
 * Originally implemented via Crypto++'s Gzip filter (cryptopp/gzip.h),
 * which is itself a thin wrapper around zlib deflate with a gzip header.
 * Crypto++ isn't available for this cross toolchain/sysroot, so this
 * produces the same standard gzip-compressed output directly with zlib
 * (the library Crypto++'s Gzip filter wraps internally).
 */

typedef unsigned char byte;

static inline void zlib_gzip(const unsigned char* in_data, size_t in_size, std::vector<unsigned char>& out) {
    out.clear();

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    /* windowBits = 15 | 16 selects gzip encapsulation (RFC 1952) */
    if (deflateInit2(&zs, 1, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return;
    }

    zs.next_in = const_cast<Bytef*>(in_data);
    zs.avail_in = (uInt)in_size;

    unsigned char buffer[4096];
    int ret;
    do {
        zs.next_out = buffer;
        zs.avail_out = sizeof(buffer);
        ret = deflate(&zs, Z_FINISH);
        out.insert(out.end(), buffer, buffer + (sizeof(buffer) - zs.avail_out));
    } while (ret == Z_OK);

    deflateEnd(&zs);
}

inline void Gziping(const std::string& in, std::vector<unsigned char>& out) {
    zlib_gzip(reinterpret_cast<const unsigned char*>(in.data()), in.size(), out);
}

inline void GzipingBin(const byte* in_data, size_t in_size, std::vector<uint8_t>& out) {
    zlib_gzip(in_data, in_size, out);
}
