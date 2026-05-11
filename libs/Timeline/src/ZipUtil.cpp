// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "ZipUtil.h"

#include <ftk/Core/Format.h>

namespace tl
{
    namespace timeline
    {
        ZipReader::ZipReader(const std::string& fileName) :
            _fileName(fileName)
        {
            if (!mz_zip_reader_init_file(&_zip, fileName.c_str(), 0))
            {
                throw std::runtime_error(
                    ftk::Format("Cannot open zip file: \"{0}\"").arg(fileName));
            }
        }
        
        ZipReader::~ZipReader()
        {
            mz_zip_reader_end(&_zip);
        }

        mz_uint ZipReader::getFileCount() const
        {
            return mz_zip_reader_get_num_files(const_cast<mz_zip_archive*>(&_zip));
        }
        
        std::optional<mz_uint> ZipReader::find(const std::string& name)
        {
            int idx = mz_zip_reader_locate_file(&_zip, name.c_str(), nullptr, 0);
            if (idx < 0) return std::nullopt;
            return static_cast<mz_uint>(idx);
        }

        mz_zip_archive_file_stat ZipReader::stat(mz_uint i)
        {
            mz_zip_archive_file_stat s;
            if (!mz_zip_reader_file_stat(&_zip, i, &s))
            {
                throw std::runtime_error(
                    ftk::Format("Cannot stat zip entry {0}: \"{1}\"").
                        arg(i).
                        arg(_fileName));
            }
            return s;
        }

        std::string ZipReader::readToString(mz_uint i)
        {
            mz_zip_archive_file_stat s;
            if (!mz_zip_reader_file_stat(&_zip, i, &s))
            {
                throw std::runtime_error(
                    ftk::Format("Cannot stat zip entry {0}: \"{1}\"").
                        arg(i).
                        arg(_fileName));
            }
            
            std::string out;
            out.resize(s.m_uncomp_size);
            if (!mz_zip_reader_extract_to_mem(
                &_zip,
                i,
                out.data(),
                out.size(),
                0))
            {
                throw std::runtime_error(
                    ftk::Format("Cannot extract zip entry to memory \"{0}\": \"{1}\"").
                        arg(s.m_filename).
                        arg(_fileName));
            }
            return out;
        }

        std::optional<size_t> ZipReader::getStoredDataOffset(
            mz_uint i,
            const uint8_t* archiveBase,
            size_t archiveSize)
        {
            const auto s = stat(i);
            if (s.m_method != 0)
            {
                return std::nullopt;
            }
            // 30 bytes minimum for the local file header.
            if (s.m_local_header_ofs + 30 > archiveSize)
            {
                return std::nullopt;
            }
            const uint8_t* hdr = archiveBase + s.m_local_header_ofs;
            // Verify the local file header signature 0x04034b50.
            if (hdr[0] != 0x50 || hdr[1] != 0x4b ||
                hdr[2] != 0x03 || hdr[3] != 0x04)
            {
                return std::nullopt;
            }
            const uint16_t nameLen =
                static_cast<uint16_t>(hdr[26]) |
                (static_cast<uint16_t>(hdr[27]) << 8);
            const uint16_t extraLen =
                static_cast<uint16_t>(hdr[28]) |
                (static_cast<uint16_t>(hdr[29]) << 8);
            const size_t dataOffset =
                s.m_local_header_ofs + 30u + nameLen + extraLen;
            if (dataOffset + s.m_uncomp_size > archiveSize)
            {
                return std::nullopt;
            }
            return dataOffset;
        }
    }
}
