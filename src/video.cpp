#include <gl_resources.hpp>
#include <video.hpp>

namespace elementary_visualizer
{
Video::Impl::Impl(
    const glm::ivec2 size,
    std::shared_ptr<WrappedAvFrame> frame,
    std::shared_ptr<WrappedVideoAvStream> stream
)
    : size(size), frame(frame), stream(stream)
{}

Video::Impl::Impl(Video::Impl &&other)
    : size(std::move(other.size)),
      frame(std::move(other.frame)),
      stream(std::move(other.stream))
{}
Video::Impl &Video::Impl::operator=(Video::Impl &&other)
{
    this->size = other.size;
    this->frame = std::move(other.frame);
    this->stream = std::move(other.stream);
    return *this;
}

void Video::Impl::render(
    std::shared_ptr<const GlTexture> rendered_scene, const RenderMode
)
{
    if (!rendered_scene)
        return;

    // TODO: implement `RenderMode` correctly instead of
    // not rendering when there is a size mismatch.
    if (this->size != rendered_scene->get_size())
        return;

    std::vector<float> rendered_scene_data(4 * this->size.x * this->size.y);
    rendered_scene->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &rendered_scene_data[0]);

    AVFrame *av_frame = **(this->frame);
    const int linesize = av_frame->linesize[0];
    uint8_t *data = av_frame->data[0];

    for (int y = 0; y < this->size.y; ++y)
    {
        for (int x = 0; x < this->size.x; ++x)
        {
            uint8_t r =
                int(255 * rendered_scene_data[4 * (this->size.x * y + x) + 0]);
            uint8_t g =
                int(255 * rendered_scene_data[4 * (this->size.x * y + x) + 1]);
            uint8_t b =
                int(255 * rendered_scene_data[4 * (this->size.x * y + x) + 2]);
            const int y_mirrored = (this->size.y - 1) - y;
            data[(y_mirrored * linesize + 3 * x) + 0] = r;
            data[(y_mirrored * linesize + 3 * x) + 1] = g;
            data[(y_mirrored * linesize + 3 * x) + 2] = b;
        }
    }

    this->stream->write_frame(this->frame);
}

Video::Impl::~Impl(){};

Expected<Video, Error> Video::create(
    const std::string &filename,
    const glm::ivec2 &size,
    const int frame_rate,
    const int64_t bit_rate
)
{
    const enum AVPixelFormat source_pixel_format = AV_PIX_FMT_RGB24;

    Expected<std::shared_ptr<WrappedOutputVideoAvFormatContext>, Error>
        format_context = WrappedOutputVideoAvFormatContext::create(
            filename,
            std::nullopt,
            bit_rate,
            size.x,
            size.y,
            frame_rate,
            source_pixel_format,
            WrappedAvDictionary()
        );
    if (!format_context)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<WrappedVideoAvStream>, Error> stream =
        WrappedVideoAvStream::create_and_open_format_context(
            format_context.value()
        );
    if (!stream)
        return Unexpected<Error>(Error());

    Expected<std::shared_ptr<WrappedAvFrame>, Error> frame =
        WrappedAvFrame::create(source_pixel_format, size.x, size.y);
    if (!frame)
        return Unexpected<Error>(Error());

    return Video(std::make_unique<Impl>(size, frame.value(), stream.value()));
}

Video::Video(Video &&other) : impl(std::move(other.impl)) {}

Video &Video::operator=(Video &&other)
{
    this->impl = std::move(other.impl);
    return *this;
}

void Video::render(
    std::shared_ptr<const GlTexture> rendered_scene,
    const RenderMode render_mode
)
{
    this->impl->render(rendered_scene, render_mode);
}

Video::~Video() {}

Video::Video(std::unique_ptr<Video::Impl> &&impl) : impl(std::move(impl)) {}
}
