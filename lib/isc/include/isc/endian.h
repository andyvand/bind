/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * See the COPYRIGHT file distributed with this work for additional
 * information regarding copyright ownership.
 */

#pragma once

#if defined(__linux__) || defined(__CYGWIN__)

#include <endian.h>

#elif defined __APPLE__

#include <libkern/OSByteOrder.h>

# define htobe16(x) OSSwapHostToBigInt16(x)
# define htole16(x) OSSwapHostToLittleInt16(x)
# define be16toh(x) OSSwapBigToHostInt16(x)
# define le16toh(x) OSSwapLittleToHostInt16(x)

# define htobe32(x) OSSwapHostToBigInt32(x)
# define htole32(x) OSSwapHostToLittleInt32(x)
# define be32toh(x) OSSwapBigToHostInt32(x)
# define le32toh(x) OSSwapLittleToHostInt32(x)

# define htobe64(x) OSSwapHostToBigInt64(x)
# define htole64(x) OSSwapHostToLittleInt64(x)
# define be64toh(x) OSSwapBigToHostInt64(x)
# define le64toh(x) OSSwapLittleToHostInt64(x)

# define __BYTE_ORDER    BYTE_ORDER
# define __BIG_ENDIAN    BIG_ENDIAN
# define __LITTLE_ENDIAN LITTLE_ENDIAN
# define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

# include <sys/endian.h>

# define be16toh(x) betoh16(x)
# define le16toh(x) letoh16(x)

# define be32toh(x) betoh32(x)
# define le32toh(x) letoh32(x)

# define be64toh(x) betoh64(x)
# define le64toh(x) letoh64(x)

#elif defined(_WIN32)
/* Windows is always little endian */

#include <stdlib.h>

# define htobe16(x) _byteswap_ushort(x)
# define htole16(x) (x)
# define be16toh(x) _byteswap_ushort(x)
# define le16toh(x) (x)

# define htobe32(x) _byteswap_ulong(x)
# define htole32(x) (x)
# define be32toh(x) _byteswap_ulong(x)
# define le32toh(x) (x)

# define htobe64(x) _byteswap_uint64(x)
# define htole64(x) (x)
# define be64toh(x) _byteswap_uint64(x)
# define le64toh(x) (x)

# define __BYTE_ORDER    BYTE_ORDER
# define __BIG_ENDIAN    BIG_ENDIAN
# define __LITTLE_ENDIAN LITTLE_ENDIAN
# define __PDP_ENDIAN    PDP_ENDIAN

#else

#include <inttypes.h>

#ifndef bswap16
#define bswap16(x) ((uint16_t)((((uint16_t) (x) & 0xff00) >> 8) |	\
			       (((uint16_t) (x) & 0x00ff) << 8)))
#endif /* bswap16 */

#ifndef bswap32
#define bswap32(x) ((uint32_t)((((uint32_t) (x) & 0xff000000) >> 24) |	\
			       (((uint32_t) (x) & 0x00ff0000) >> 8)  |	\
			       (((uint32_t) (x) & 0x0000ff00) << 8)  |	\
			       (((uint32_t) (x) & 0x000000ff) << 24)))
#endif /* bswap32 */

#ifndef bswap64
#define bswap64(x) \
	((uint64_t)((((uint64_t) (x) & 0xff00000000000000ull) >> 56) |	\
		    (((uint64_t) (x) & 0x00ff000000000000ull) >> 40) |	\
		    (((uint64_t) (x) & 0x0000ff0000000000ull) >> 24) |	\
		    (((uint64_t) (x) & 0x000000ff00000000ull) >> 8)  |	\
		    (((uint64_t) (x) & 0x00000000ff000000ull) << 8)  |	\
		    (((uint64_t) (x) & 0x0000000000ff0000ull) << 24) |	\
		    (((uint64_t) (x) & 0x000000000000ff00ull) << 40) |	\
		    (((uint64_t) (x) & 0x00000000000000ffull) << 56)))
#endif /* bswap64 */

#if WORDS_BIGENDIAN

#define htobe16(x) (x)
#define htole16(x) bswap16(x)
#define be16toh(x) (x)
#define le16toh(x) bswap16(x)

#define htobe32(x) (x)
#define htole32(x) bswap32(x)
#define be32toh(x) (x)
#define le32toh(x) bswap32(x)

#define htobe64(x) (x)
#define htole64(x) bswap64(x)
#define be64toh(x) (x)
#define le64toh(x) bswap64(x)

#else /* WORDS_BIGENDIAN */

#include <arpa/inet.h>

#define htobe16(x) htons(x)
#define htole16(x) (x)
#define be16toh(x) ntohs(x)
#define le16toh(x) (x)

#define htobe32(x) htonl(x)
#define htole32(x) (x)
#define be32toh(x) ntohl(x)
#define le32toh(x) (x)

#define htobe64(x) bswap64(x)
#define htole64(x) (x)
#define be64toh(x) bswap64(x)
#define le64toh(x) (x)

#endif /* WORDS_BIGENDIAN */

#endif
