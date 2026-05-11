// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <miniz.h>

#include <string>

namespace tl
{
    namespace timeline
    {
        // ZIP reader using miniz.
        class ZipReader
        {
        public:
            ZipReader(const std::string& fileName);
            
            ~ZipReader();
            
            ZipReader(ZipReader const&) = delete;
            ZipReader& operator=(ZipReader const&) = delete;
            
            mz_uint getFileCount() const;
            
            std::optional<mz_uint> find(const std::string& name);

            mz_zip_archive_file_stat stat(mz_uint);

            std::string readToString(mz_uint);

            // For a stored (uncompressed) entry, compute the offset
            // within the archive where the entry's raw data begins.
            // Returns nullopt if the entry is compressed (method != 0).
            //
            // The data offset is computed from the local file header:
            //   local_header_offset + 30 + filename_length + extra_length
            // The local header's filename and extra-field lengths can
            // differ from the central directory's lengths, so we have to
            // peek at the local header itself rather than using stat alone.
            std::optional<size_t> getStoredDataOffset(
                mz_uint,
                const uint8_t* archiveBase,
                size_t archiveSize);

        private:
            std::string _fileName;
            mz_zip_archive _zip = {};
        };
    }
}

