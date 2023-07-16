#include <elementary_visualizer/elementary_visualizer.hpp>

namespace elementary_visualizer
{
Scene::Scene(Scene &&) {}

Scene &Scene::operator=(Scene &&)
{
    return *this;
}

Scene::~Scene() {}
}
