#include <elementary_visualizer/elementary_visualizer.hpp>
#include <entity.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

namespace elementary_visualizer
{
Error::operator std::string() const
{
    return "";
}
}
