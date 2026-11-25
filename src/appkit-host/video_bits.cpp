// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The bits of video streams.

#include "video_private.h"

namespace cutefish::reef::video {

Rbsp unescape(const uint8_t* data, size_t size)
{
    Rbsp rbsp;
    rbsp.bytes.reserve(size);
    int zeros = 0;
    for (size_t i = 0; i < size; i++) {
        const uint8_t byte = data[i];
        if (zeros >= 2 && byte == 3) {
            rbsp.removed.push_back(i);
            zeros = 0;
            continue;
        }
        rbsp.bytes.push_back(byte);
        zeros = byte == 0 ? zeros + 1 : 0;
    }
    return rbsp;
}

size_t Rbsp::removedBefore(size_t position) const
{
    size_t count = 0;
    while (count < removed.size() && removed[count] - count < position)
        count++;
    return count;
}

BitReader::BitReader(const uint8_t* data, size_t size)
    : m_data(data)
    , m_size(size)
{
}

BitReader::BitReader(const Rbsp& rbsp)
    : m_data(rbsp.bytes.data())
    , m_size(rbsp.bytes.size())
{
}

uint32_t BitReader::bits(int count)
{
    uint32_t value = 0;
    for (int i = 0; i < count; i++) {
        uint32_t next = 0;
        if (m_position < m_size * 8)
            next = m_data[m_position / 8] >> (7 - m_position % 8) & 1;
        else
            m_failed = true;
        m_position++;
        value = value << 1 | next;
    }
    return value;
}

bool BitReader::bit()
{
    return bits(1);
}

uint32_t BitReader::ue()
{
    int zeros = 0;
    while (!bit()) {
        if (++zeros == 32) {
            m_failed = true;
            return 0;
        }
    }
    if (zeros == 0)
        return 0;
    return uint32_t((uint64_t(1) << zeros) - 1 + bits(zeros));
}

int32_t BitReader::se()
{
    const uint32_t value = ue();
    return value & 1 ? int32_t((value + 1) / 2) : -int32_t(value / 2);
}

void BitReader::skip(size_t count)
{
    m_position += count;
    if (m_position > m_size * 8)
        m_failed = true;
}

bool BitReader::more() const
{
    size_t last = m_size;
    while (last > 0 && m_data[last - 1] == 0)
        last--;
    if (last == 0)
        return false;
    const uint8_t byte = m_data[last - 1];
    int zeros = 0;
    while (!(byte >> zeros & 1))
        zeros++;
    return m_position < (last - 1) * 8 + (7 - zeros);
}

} // namespace cutefish::reef::video
