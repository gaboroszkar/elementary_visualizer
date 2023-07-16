#ifndef ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP
#define ELEMENTARY_VISUALIZER_ELEMENTARY_VISUALIZER_HPP

namespace elementary_visualizer
{
class Scene
{
public:

    Scene(Scene &&other);
    Scene &operator=(Scene &&other);

    ~Scene();

    Scene(const Scene &other) = delete;
    Scene &operator=(const Scene &other) = delete;

private:

    Scene();
};
}

#endif
