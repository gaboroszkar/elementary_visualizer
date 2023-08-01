#include <av_resources.hpp>

namespace elementary_visualizer
{
WrappedAvDictionary::WrappedAvDictionary() : dictionary(nullptr) {}

WrappedAvDictionary::WrappedAvDictionary(const WrappedAvDictionary &other)
    : dictionary(nullptr)
{
    av_dict_copy(&this->dictionary, other.dictionary, 0);
}

WrappedAvDictionary &WrappedAvDictionary::operator=(WrappedAvDictionary &&other)
{
    av_dict_copy(&this->dictionary, other.dictionary, 0);
    return *this;
}

void WrappedAvDictionary::set(const std::string &key, const std::string &value)
{
    av_dict_set(&this->dictionary, key.c_str(), value.c_str(), 0);
}

WrappedAvDictionary::~WrappedAvDictionary()
{
    av_dict_free(&this->dictionary);
}

Expected<std::shared_ptr<WrappedAvPacket>, Error> WrappedAvPacket::create()
{
    AVPacket *packet = av_packet_alloc();
    if (!packet)
        return Unexpected<Error>(Error());

    return std::shared_ptr<WrappedAvPacket>(new WrappedAvPacket(packet));
}

AVPacket *WrappedAvPacket::operator*()
{
    return this->packet;
}

const AVPacket *WrappedAvPacket::operator*() const
{
    return this->packet;
}

WrappedAvPacket::~WrappedAvPacket()
{
    av_packet_free(&this->packet);
}

WrappedAvPacket::WrappedAvPacket(AVPacket *packet) : packet(packet) {}

Expected<std::shared_ptr<WrappedAvFrame>, Error> WrappedAvFrame::create(
    const enum AVPixelFormat pixel_format,
    const unsigned int width,
    const unsigned int height
)
{
    AVFrame *frame = av_frame_alloc();
    if (!frame)
        return Unexpected<Error>(Error());
    frame->format = pixel_format;
    frame->width = width;
    frame->height = height;

    int result;

    // Allocate the buffers for the frame data.
    result = av_frame_get_buffer(frame, 0);
    if (result < 0)
        return Unexpected<Error>(Error());

    std::shared_ptr<WrappedAvFrame> wrapped_frame =
        std::shared_ptr<WrappedAvFrame>(new WrappedAvFrame(frame));

    return wrapped_frame;
}

AVFrame *WrappedAvFrame::operator*()
{
    return this->frame;
}

const AVFrame *WrappedAvFrame::operator*() const
{
    return this->frame;
}

Expected<void, Error>
    WrappedAvFrame::convert_and_copy(const WrappedAvFrame &source)
{
    int result;

    result = av_frame_make_writable(this->frame);
    if (result < 0)
        return Unexpected<Error>(Error());

    if ((source.frame->format == this->frame->format) &&
        (source.frame->width == this->frame->width) &&
        (source.frame->height == this->frame->height))
    {
        result = av_frame_copy(this->frame, source.frame);
        if (result < 0)
            return Unexpected<Error>(Error());
    }
    else
    {
        struct SwsContext *sws_context = sws_getContext(
            source.frame->width,
            source.frame->height,
            static_cast<enum AVPixelFormat>(source.frame->format),
            this->frame->width,
            this->frame->height,
            static_cast<enum AVPixelFormat>(this->frame->format),
            SWS_BICUBIC,
            nullptr,
            nullptr,
            nullptr
        );
        if (!sws_context)
            return Unexpected<Error>(Error());

        result = sws_scale(
            sws_context,
            (const uint8_t *const *)source.frame->data,
            source.frame->linesize,
            0,
            source.frame->height,
            this->frame->data,
            this->frame->linesize
        );
        if (result < 0)
            return Unexpected<Error>(Error());

        sws_freeContext(sws_context);
    }

    return Expected<void, Error>();
}

WrappedAvFrame::~WrappedAvFrame()
{
    av_frame_free(&this->frame);
}

WrappedAvFrame::WrappedAvFrame(AVFrame *frame) : frame(frame) {}

Expected<std::shared_ptr<WrappedAvCodecContext>, Error>
    WrappedAvCodecContext::create(
        const enum AVCodecID codec_id,
        const int64_t bit_rate,
        const unsigned int width,
        const unsigned int height,
        const unsigned int frame_rate,
        const enum AVPixelFormat source_pixel_format,
        const int additional_flags,
        const WrappedAvDictionary &parameters
    )
{
    const AVCodec *codec;
    codec = avcodec_find_encoder(codec_id);
    if (!codec)
        return Unexpected<Error>(Error());

    if (codec->type != AVMEDIA_TYPE_VIDEO)
        return Unexpected<Error>(Error());

    const enum AVPixelFormat pixel_format = avcodec_find_best_pix_fmt_of_list(
        codec->pix_fmts, source_pixel_format, 0, nullptr
    );
    if (pixel_format == AV_PIX_FMT_NONE)
        return Unexpected<Error>(Error());

    AVCodecContext *codec_context = avcodec_alloc_context3(codec);

    codec_context->codec_id = codec_id;

    codec_context->bit_rate = bit_rate;

    // Resolution must be a multiple of two.
    if ((width % 2 != 0) || (height % 2 != 0))
        return Unexpected<Error>(Error());

    codec_context->width = width;
    codec_context->height = height;

    // Timebase: this is the fundamental unit of time (in seconds) in terms
    // of which frame timestamps are represented. For fixed-fps content,
    // timebase should be 1/framerate and timestamp increments should be
    // identical to 1.
    codec_context->time_base = (AVRational){1, static_cast<int>(frame_rate)};

    // Emit one intra frame every twelve frames at most.
    codec_context->gop_size = 12;
    codec_context->pix_fmt = pixel_format;

    if (codec_context->codec_id == AV_CODEC_ID_MPEG2VIDEO)
    {
        // Just for testing, we also add B-frames.
        codec_context->max_b_frames = 2;
    }
    if (codec_context->codec_id == AV_CODEC_ID_MPEG1VIDEO)
    {
        // Needed to avoid using macroblocks in which some coeffs overflow.
        // This does not happen with normal video, it just happens here as
        // the motion of the chroma plane does not match the luma plane.
        codec_context->mb_decision = 2;
    }

    codec_context->flags |= additional_flags;

    WrappedAvDictionary copied_parameters(parameters);
    AVDictionary *p_copied_parameters = copied_parameters.dictionary;
    // Open the codec.
    int result = avcodec_open2(codec_context, codec, &p_copied_parameters);
    // avcodec_open2 frees up potentially the memory address of the parameters,
    // and allocates a new dictionary. So we also make the change in our
    // copied_parameters, so that it does not free up already freed memory.
    copied_parameters.dictionary = p_copied_parameters;
    if (result < 0)
        return Unexpected<Error>(Error());

    return std::shared_ptr<WrappedAvCodecContext>(
        new WrappedAvCodecContext(codec_context)
    );
}

AVCodecContext *WrappedAvCodecContext::operator*()
{
    return this->codec_context;
}

const AVCodecContext *WrappedAvCodecContext::operator*() const
{
    return this->codec_context;
}

WrappedAvCodecContext::~WrappedAvCodecContext()
{
    avcodec_free_context(&this->codec_context);
}

WrappedAvCodecContext::WrappedAvCodecContext(AVCodecContext *codec_context)
    : codec_context(codec_context)
{}

AVStream *WrappedVideoAvStream::operator*()
{
    return this->stream;
}

const AVStream *WrappedVideoAvStream::operator*() const
{
    return this->stream;
}

WrappedVideoAvStream::~WrappedVideoAvStream()
{
    if (!this->is_state_eof)
    {
        this->enter_codec_flush_mode();
        this->receive_packet();
    }
    avcodec_flush_buffers(**(this->format_context->codec_context));
}

WrappedVideoAvStream::WrappedVideoAvStream(
    std::shared_ptr<WrappedOutputVideoAvFormatContext> format_context,
    AVStream *stream,
    std::shared_ptr<WrappedAvFrame> frame,
    std::shared_ptr<WrappedAvPacket> packet
)
    : format_context(format_context),
      stream(stream),
      frame(frame),
      packet(packet),
      is_state_eof(true),
      timestamp(0)
{}

Expected<std::shared_ptr<WrappedOutputVideoAvFormatContext>, Error>
    WrappedOutputVideoAvFormatContext::create(
        const std::string &filename,
        const std::optional<std::string> &format_name,
        const int64_t bit_rate,
        const unsigned int width,
        const unsigned int height,
        const unsigned int frame_rate,
        const enum AVPixelFormat source_pixel_format,
        const WrappedAvDictionary &parameters,
        const std::optional<enum AVCodecID> codec_id,
        const bool intermediate_yuv420p_conversion
    )
{
    AVFormatContext *format_context;
    int result = avformat_alloc_output_context2(
        &format_context,
        nullptr,
        format_name ? format_name->c_str() : nullptr,
        filename.c_str()
    );
    if (!format_context || result < 0)
        return Unexpected<Error>(Error());

    // Some formats want stream headers to be separate.
    int codec_additional_flags = 0;
    if (format_context->oformat->flags & AVFMT_GLOBALHEADER)
        codec_additional_flags = AV_CODEC_FLAG_GLOBAL_HEADER;

    Expected<std::shared_ptr<WrappedAvCodecContext>, Error> codec_context =
        WrappedAvCodecContext::create(
            codec_id ? codec_id.value() : format_context->oformat->video_codec,
            bit_rate,
            width,
            height,
            frame_rate,
            source_pixel_format,
            codec_additional_flags,
            parameters
        );
    if (!codec_context)
        return Unexpected<Error>(Error());

    return std::shared_ptr<WrappedOutputVideoAvFormatContext>(
        new WrappedOutputVideoAvFormatContext(
            format_context,
            parameters,
            codec_context.value(),
            intermediate_yuv420p_conversion
        )
    );
}

bool WrappedOutputVideoAvFormatContext::is_opened() const
{
    return this->opened;
}

bool WrappedOutputVideoAvFormatContext::is_intermediate_yuv420p_conversion(
) const
{
    return this->intermediate_yuv420p_conversion;
}

AVFormatContext *WrappedOutputVideoAvFormatContext::operator*()
{
    return this->format_context;
}

const AVFormatContext *WrappedOutputVideoAvFormatContext::operator*() const
{
    return this->format_context;
}

WrappedOutputVideoAvFormatContext::~WrappedOutputVideoAvFormatContext()
{
    if (this->opened)
        this->close();
    avformat_free_context(this->format_context);
}

Expected<void, Error> WrappedOutputVideoAvFormatContext::open()
{
    if (this->opened)
        return Unexpected<Error>(Error());

    const std::string filename = this->format_context->url;

    // Open the output file, if needed.
    if (!this->no_file())
    {
        int result = avio_open(
            &this->format_context->pb, filename.c_str(), AVIO_FLAG_WRITE
        );
        if (result < 0)
            return Unexpected<Error>(Error());
    }

    // Write the stream header, if any.
    WrappedAvDictionary copied_parameters(this->parameters);
    AVDictionary *p_copied_parameters = copied_parameters.dictionary;
    int result =
        avformat_write_header(this->format_context, &p_copied_parameters);
    // avformat_write_header frees up potentially the memory address of the
    // parameters, and allocates a new dictionary. So we also make the change in
    // our copied_parameters, so that it does not free up already freed memory.
    copied_parameters.dictionary = p_copied_parameters;
    if (result < 0)
    {
        if (!this->no_file())
            avio_closep(&this->format_context->pb);
        return Unexpected<Error>(Error());
    }

    this->opened = true;

    return Expected<void, Error>();
}

void WrappedOutputVideoAvFormatContext::close()
{
    if (this->opened)
    {
        av_write_trailer(this->format_context);

        if (!this->no_file())
            // Close the output file.
            avio_closep(&this->format_context->pb);

        this->opened = false;
    }
}

bool WrappedOutputVideoAvFormatContext::no_file()
{
    return (this->format_context->oformat->flags & AVFMT_NOFILE);
}

WrappedOutputVideoAvFormatContext::WrappedOutputVideoAvFormatContext(
    AVFormatContext *format_context,
    WrappedAvDictionary parameters,
    std::shared_ptr<WrappedAvCodecContext> codec_context,
    const bool intermediate_yuv420p_conversion
)
    : format_context(format_context),
      parameters(parameters),
      codec_context(codec_context),
      opened(false),
      intermediate_yuv420p_conversion(intermediate_yuv420p_conversion)
{}

Expected<std::shared_ptr<WrappedVideoAvStream>, Error>
    WrappedVideoAvStream::create_and_open_format_context(
        std::shared_ptr<WrappedOutputVideoAvFormatContext> format_context
    )
{
    if (format_context->is_opened())
        return Unexpected<Error>(Error());

    AVStream *stream =
        avformat_new_stream(format_context->format_context, nullptr);
    if (!stream)
        return Unexpected<Error>(Error());

    stream->id = format_context->format_context->nb_streams - 1;
    stream->time_base = (**(format_context->codec_context))->time_base;

    AVCodecContext *codec_context = **(format_context->codec_context);

    int result =
        avcodec_parameters_from_context(stream->codecpar, codec_context);
    if (result < 0)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<WrappedAvFrame>, Error> frame =
        WrappedAvFrame::create(
            codec_context->pix_fmt, codec_context->width, codec_context->height
        );
    if (!frame)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<WrappedAvPacket>, Error> packet =
        WrappedAvPacket::create();
    if (!packet)
        return Unexpected<Error>(Error());

    // Print information about the stream to
    // the standard output.
    const std::string filename = (**format_context)->url;
    av_dump_format(**format_context, stream->index, filename.c_str(), 1);

    // We store a reference to the format context,
    // so that it will not be closed while
    // the stream still exists.
    std::shared_ptr<WrappedVideoAvStream> wrapped_stream =
        std::shared_ptr<WrappedVideoAvStream>(new WrappedVideoAvStream(
            format_context, stream, frame.value(), packet.value()
        ));

    Expected<void, Error> open_result = format_context->open();
    if (!open_result)
        return Unexpected<Error>(Error());

    return wrapped_stream;
}

Expected<void, Error>
    WrappedVideoAvStream::write_frame(std::shared_ptr<WrappedAvFrame> frame_in)
{
    if (!frame_in)
        return Unexpected<Error>(Error());

    if (!this->format_context->is_opened())
        return Unexpected<Error>(Error());

    int result;

    result = av_frame_make_writable(**this->frame);
    if (result < 0)
        return Unexpected<Error>(Error());

    const enum AVPixelFormat pixel_format_yuv420p = AV_PIX_FMT_YUV420P;
    if (this->format_context->is_intermediate_yuv420p_conversion() &&
        (**this->frame)->format != pixel_format_yuv420p)
    {
        Expected<std::shared_ptr<WrappedAvFrame>, Error> tmp_frame_yuv420p =
            WrappedAvFrame::create(
                pixel_format_yuv420p, (**frame_in)->width, (**frame_in)->height
            );
        if (!tmp_frame_yuv420p)
            return Unexpected<Error>(Error());
        (tmp_frame_yuv420p.value())->convert_and_copy(*frame_in);
        this->frame->convert_and_copy(*tmp_frame_yuv420p.value());
    }
    else
    {
        this->frame->convert_and_copy(*frame_in);
    }

    (**(this->frame))->pts = this->timestamp;

    AVCodecContext *codec_context = **(this->format_context->codec_context);

    // Send the frame to the encoder.
    result = avcodec_send_frame(codec_context, **(this->frame));
    if (result < 0)
        return Unexpected<Error>(Error());

    Expected<void, Error> receive_packet_result = this->receive_packet();
    if (!receive_packet_result)
        return Unexpected<Error>(Error());

    ++this->timestamp;

    return Expected<void, Error>();
}

Expected<void, Error> WrappedVideoAvStream::enter_codec_flush_mode()
{
    AVCodecContext *codec_context = **(this->format_context->codec_context);

    // Send empty frame to enter into flushing mode for the codec.
    int result = avcodec_send_frame(codec_context, nullptr);
    if (result < 0)
        return Unexpected<Error>(Error());
    return Expected<void, Error>();
}

Expected<void, Error> WrappedVideoAvStream::receive_packet()
{
    AVCodecContext *codec_context = **(this->format_context->codec_context);
    AVPacket *packet = **(this->packet);

    int result = 0;
    while (result >= 0)
    {
        result = avcodec_receive_packet(codec_context, packet);
        if (result == AVERROR_EOF)
        {
            this->is_state_eof = true;
            break;
        }
        else
        {
            this->is_state_eof = false;

            if (result == AVERROR(EAGAIN))
                break;
            else if (result < 0)
                return Unexpected<Error>(Error());
        }

        // Rescale output packet timestamp values from codec to stream timebase.
        av_packet_rescale_ts(
            packet, codec_context->time_base, this->stream->time_base
        );
        packet->stream_index = this->stream->index;

        // Write the compressed frame to the media file.
        result = av_interleaved_write_frame(**(this->format_context), packet);
        // The packet is now blank (av_interleaved_write_frame() takes ownership
        // of its contents and resets packet), so that no unreferencing is
        // necessary. This would be different if one used av_write_frame().
        if (result < 0)
            return Unexpected<Error>(Error());
    }
    return Expected<void, Error>();
}
}
