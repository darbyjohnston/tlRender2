// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Timeline.h>

#include <ftk/Core/Format.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>

#include <filesystem>

#include <miniz.h>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        namespace
        {
            Time timeFromOTIO(const OTIO_NS::RationalTime& rt, double projectRate)
            {
                return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
            }

            Duration durationFromOTIO(const OTIO_NS::RationalTime& rt, double projectRate)
            {
                return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
            }

            OTIO_NS::RationalTime timeToOTIO(Time t, double projectRate)
            {
                return OTIO_NS::RationalTime(
                    static_cast<double>(t.frames),
                    projectRate);
            }

            OTIO_NS::RationalTime durationToOTIO(Duration d, double projectRate)
            {
                return OTIO_NS::RationalTime(
                    static_cast<double>(d.frames),
                    projectRate);
            }

            MediaTime mediaTimeFromOTIO(const OTIO_NS::RationalTime& rt)
            {
                // OTIO stores rate as a single double; recovering an exact
                // integer num/den isn't always possible. We special-case the
                // common NTSC-family rates that lose precision in double form
                // and otherwise fall back to (round(rate), 1), which is exact
                // for integer-valued rates (24, 25, 30, 48000, 44100, etc.)
                // but lossy for unusual rationals not in the table below.
                const double rate = rt.rate();
                MediaRate r{ static_cast<int>(std::nearbyint(rate)), 1 };
                if      (std::abs(rate - 24000.0 / 1001.0) < 1e-6) r = mediaRate23_976();
                else if (std::abs(rate - 30000.0 / 1001.0) < 1e-6) r = mediaRate29_97();
                else if (std::abs(rate - 60000.0 / 1001.0) < 1e-6) r = mediaRate59_94();
                return MediaTime{ static_cast<int64_t>(rt.value()), r };
            }

            MediaDuration mediaDurationFromOTIO(const OTIO_NS::RationalTime& rt)
            {
                const auto t = mediaTimeFromOTIO(rt);
                return MediaDuration{ t.frames, t.rate };
            }

            OTIO_NS::RationalTime mediaTimeToOTIO(const MediaTime& t)
            {
                return OTIO_NS::RationalTime(
                    static_cast<double>(t.frames),
                    t.rate.toDouble());
            }

            OTIO_NS::RationalTime mediaDurationToOTIO(const MediaDuration& d)
            {
                return OTIO_NS::RationalTime(
                    static_cast<double>(d.frames),
                    d.rate.toDouble());
            }

            // Resolve an ExternalReference's target_url relative to the
            // timeline file's directory. Handles absolute paths, relative
            // paths, and the file:// URL scheme. URL-encoded characters are
            // not currently decoded; OTIO files in the wild rarely use them
            // for local references.
            std::optional<ftk::Path> resolveExternalReference(
                const std::string& targetUrl,
                const std::filesystem::path& timelineDir)
            {
                if (targetUrl.empty())
                {
                    return std::nullopt;
                }

                std::string s = targetUrl;
                constexpr const char* fileScheme = "file://";
                if (s.compare(0, 7, fileScheme) == 0)
                {
                    s = s.substr(7);
                }

                std::filesystem::path p(s);
                if (p.is_relative() && !timelineDir.empty())
                {
                    p = timelineDir / p;
                }
                std::error_code ec;
                auto resolved = std::filesystem::weakly_canonical(p, ec);
                if (ec)
                {
                    resolved = p;
                }
                return ftk::Path(resolved.string());
            }

            // ZIP reader using miniz.
            class ZipReader
            {
            public:
                ZipReader(std::string const& fileName) :
                    _fileName(fileName)
                {
                    if (!mz_zip_reader_init_file(&_zip, fileName.c_str(), 0))
                    {
                        throw std::runtime_error(
                            ftk::Format("Cannot open zip file: \"{0}\"").arg(fileName));
                    }
                }
                
                ~ZipReader()
                {
                    mz_zip_reader_end(&_zip);
                }
                
                ZipReader(ZipReader const&) = delete;
                ZipReader& operator=(ZipReader const&) = delete;
                
                mz_uint getFileCount() const
                {
                    return mz_zip_reader_get_num_files(const_cast<mz_zip_archive*>(&_zip));
                }
                
                std::optional<mz_uint> find(std::string const& name)
                {
                    int idx = mz_zip_reader_locate_file(&_zip, name.c_str(), nullptr, 0);
                    if (idx < 0) return std::nullopt;
                    return static_cast<mz_uint>(idx);
                }

                mz_zip_archive_file_stat stat(mz_uint i)
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

                std::string readToString(mz_uint i)
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

            private:
                std::string _fileName;
                mz_zip_archive _zip = {};
            };
        }

        struct Timeline::Private
        {
            std::shared_ptr<ftk::Context> context;
            ftk::Path path;
            std::shared_ptr<ftk::FileIO> fileIO;
            std::vector<Media> media;
            std::shared_ptr<ftk::Observable<Time> > startTime;
            std::shared_ptr<ftk::Observable<Duration> > duration;
        };

        void Timeline::_init(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            FTK_P();
            p.context = context;
            p.path = path;
            p.startTime = ftk::Observable<Time>::create();
            p.duration = ftk::Observable<Duration>::create();

            OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> otioTimeline;
            OTIO_NS::ErrorStatus errorStatus;
            if (".otioz" == ftk::toLower(path.getExt()))
            {
                ZipReader zip(path.get());
                auto id = zip.find("content.otio");
                if (!id)
                {
                    throw std::runtime_error(ftk::Format("Cannot find \"content.otio\": {0}").
                        arg(path.get()));
                }
                const std::string json = zip.readToString(*id);
                otioTimeline = dynamic_cast<OTIO_NS::Timeline*>(
                    OTIO_NS::Timeline::from_json_string(json, &errorStatus));
                if (!otioTimeline || OTIO_NS::is_error(errorStatus))
                {
                    throw std::runtime_error(ftk::Format("Cannot read timeline \"{0}\": {1}").
                        arg(path.get()).
                        arg(errorStatus.full_description));
                }

                p.fileIO = ftk::FileIO::create(
                    path.get(),
                    ftk::FileMode::Read,
                    ftk::FileRead::MMap);
            }
            else if (!path.get().empty())
            {
                otioTimeline = dynamic_cast<OTIO_NS::Timeline*>(
                    OTIO_NS::Timeline::from_json_file(path.get(), &errorStatus));
                if (!otioTimeline || OTIO_NS::is_error(errorStatus))
                {
                    throw std::runtime_error(ftk::Format("Cannot read timeline \"{0}\": {1}").
                        arg(path.get()).
                        arg(errorStatus.full_description));
                }
            }

            // Collect external media references.
            if (otioTimeline)
            {
                const auto timelineDir = std::filesystem::path(path.get()).parent_path();
                std::set<std::string> seen;
                for (const auto& otioClip : otioTimeline->find_clips())
                {
                    if (auto otioExternalRef =
                        dynamic_cast<OTIO_NS::ExternalReference*>(otioClip->media_reference()))
                    {
                        if (auto resolved = resolveExternalReference(
                            otioExternalRef->target_url(),
                            timelineDir))
                        {
                            const std::string key = resolved->get();
                            if (seen.insert(key).second)
                            {
                                p.media.push_back({ *resolved });
                            }
                        }
                    }
                }
            }
        }

        Timeline::Timeline() :
            _p(new Private)
        {}

        Timeline::~Timeline()
        {}

        std::shared_ptr<Timeline> Timeline::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(context, ftk::Path());
            return out;
        }

        std::shared_ptr<Timeline> Timeline::create(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(context, path);
            return out;
        }

        const ftk::Path& Timeline::getPath() const
        {
            return _p->path;
        }

        const std::vector<Media>& Timeline::getMedia() const
        {
            return _p->media;
        }

        const Time& Timeline::getStartTime() const
        {
            return _p->startTime->get();
        }

        std::shared_ptr<ftk::IObservable<Time> > Timeline::observeStartTime() const
        {
            return _p->startTime;
        }

        const Duration& Timeline::getDuration() const
        {
            return _p->duration->get();
        }

        std::shared_ptr<ftk::IObservable<Duration> > Timeline::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IVideoNode> Timeline::getVideo(const Time&)
        {
            return nullptr;
        }

        std::future<std::shared_ptr<Audio> > Timeline::getAudio(int64_t seconds)
        {
            return std::future<std::shared_ptr<Audio> >();
        }
    }
}
