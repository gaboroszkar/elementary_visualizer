#ifndef ELEMENTARY_VISUALIZER_VIDEO_HPP
#define ELEMENTARY_VISUALIZER_VIDEO_HPP

#include <av_resources.hpp>
#include <elementary_visualizer/elementary_visualizer.hpp>
#include <memory>

namespace elementary_visualizer
{
class Video::Impl
{
public:

    Impl(
        const glm::ivec2 size,
        std::shared_ptr<WrappedAvFrame> frame,
        std::shared_ptr<WrappedVideoAvStream> stream
    );

    Impl(Impl &&other);
    Impl &operator=(Impl &&other);

    void render(
        std::shared_ptr<const GlTexture> rendered_scene,
        const RenderMode render_mode
    );

    ~Impl();

    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

private:

    glm::ivec2 size;
    std::shared_ptr<WrappedAvFrame> frame;
    std::shared_ptr<WrappedVideoAvStream> stream;
};
}

#endif
