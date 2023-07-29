#include <elementary_visualizer/elementary_visualizer.hpp>
#include <memory>
#include <optional>
#include <string>
#include <tl/expected.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace elementary_visualizer
{
class WrappedAvDictionary
{
public:

    WrappedAvDictionary();

    WrappedAvDictionary(const WrappedAvDictionary &other);
    WrappedAvDictionary &operator=(WrappedAvDictionary &&other);

    void set(const std::string &key, const std::string &value);

    ~WrappedAvDictionary();

    AVDictionary *dictionary;
};

class WrappedAvPacket
{
public:

    static Expected<std::shared_ptr<WrappedAvPacket>, Error> create();

    AVPacket *operator*();
    const AVPacket *operator*() const;

    ~WrappedAvPacket();

private:

    WrappedAvPacket(AVPacket *packet);

    AVPacket *packet;
};

class WrappedAvFrame
{
public:

    static Expected<std::shared_ptr<WrappedAvFrame>, Error> create(
        const enum AVPixelFormat pixel_format,
        const unsigned int width,
        const unsigned int height
    );

    AVFrame *operator*();
    const AVFrame *operator*() const;

    Expected<void, Error> convert_and_copy(const WrappedAvFrame &source);

    ~WrappedAvFrame();

private:

    WrappedAvFrame(AVFrame *frame);

    AVFrame *frame;
};

class WrappedAvCodecContext
{
public:

    static Expected<std::shared_ptr<WrappedAvCodecContext>, Error> create(
        const enum AVCodecID codec_id,
        const int64_t bit_rate,
        const unsigned int width,
        const unsigned int height,
        const unsigned int frame_rate,
        const enum AVPixelFormat source_pixel_format,
        const int additional_flags,
        const WrappedAvDictionary &parameters
    );

    AVCodecContext *operator*();
    const AVCodecContext *operator*() const;

    ~WrappedAvCodecContext();

private:

    WrappedAvCodecContext(AVCodecContext *codec_context);

    AVCodecContext *codec_context;
};

class WrappedOutputVideoAvFormatContext;

class WrappedVideoAvStream
{
public:

    static Expected<std::shared_ptr<WrappedVideoAvStream>, Error>
        create_and_open_format_context(
            std::shared_ptr<WrappedOutputVideoAvFormatContext> format_context
        );

    AVStream *operator*();
    const AVStream *operator*() const;

    Expected<void, Error> write_frame(std::shared_ptr<WrappedAvFrame> frame);

    ~WrappedVideoAvStream();

private:

    WrappedVideoAvStream(
        std::shared_ptr<WrappedOutputVideoAvFormatContext> format_context,
        AVStream *stream,
        std::shared_ptr<WrappedAvFrame> frame,
        std::shared_ptr<WrappedAvPacket> packet
    );

    std::shared_ptr<WrappedOutputVideoAvFormatContext> format_context;
    AVStream *stream;
    std::shared_ptr<WrappedAvFrame> frame;
    std::shared_ptr<WrappedAvPacket> packet;

    // Presentation timestamp in time_base units.
    int64_t timestamp;
};

class WrappedOutputVideoAvFormatContext
{
public:

    static Expected<std::shared_ptr<WrappedOutputVideoAvFormatContext>, Error>
        create(
            const std::string &filename,
            const std::optional<std::string> &format_name,
            const int64_t bit_rate,
            const unsigned int width,
            const unsigned int height,
            const unsigned int frame_rate,
            const enum AVPixelFormat source_pixel_format,
            const WrappedAvDictionary &parameters,
            const std::optional<enum AVCodecID> codec_id = std::nullopt,
            const bool intermediate_yuv420p_conversion = true
        );

    bool is_opened() const;
    bool is_intermediate_yuv420p_conversion() const;

    AVFormatContext *operator*();
    const AVFormatContext *operator*() const;

    ~WrappedOutputVideoAvFormatContext();

private:

    Expected<void, Error> open();
    void close();

    bool no_file();

    WrappedOutputVideoAvFormatContext(
        AVFormatContext *format_context,
        WrappedAvDictionary parameters,
        std::shared_ptr<WrappedAvCodecContext> codec_context,
        const bool intermediate_yuv420p_conversion
    );

    AVFormatContext *format_context;
    const WrappedAvDictionary parameters;
    std::shared_ptr<WrappedAvCodecContext> codec_context;
    bool opened;
    const bool intermediate_yuv420p_conversion;

    friend class WrappedVideoAvStream;
};
}
