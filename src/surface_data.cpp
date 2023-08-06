#include <elementary_visualizer/elementary_visualizer.hpp>

namespace elementary_visualizer
{

constexpr size_t position_stride = 3;
constexpr size_t color_normal_stride = 4 + 3;

SurfaceData::SurfaceData(
    const std::vector<Vertex> &vertices,
    const size_t u_size,
    const SurfaceMode mode
)
    : position_data(0), color_normal_data(0), u_size(u_size), mode(mode)
{
    if (this->u_size < 2 || (vertices.size() % this->u_size) != 0)
        return;

    position_data.resize(position_stride * vertices.size());
    for (size_t i = 0; i != vertices.size(); ++i)
    {
        this->position_data[position_stride * i + 0] = vertices[i].position.x;
        this->position_data[position_stride * i + 1] = vertices[i].position.y;
        this->position_data[position_stride * i + 2] = vertices[i].position.z;
    }

    const size_t v_size = this->v_size();

    if (this->mode == SurfaceMode::smooth)
    {
        this->color_normal_data.resize(
            color_normal_stride * this->u_size * v_size
        );
        for (size_t v = 0; v != v_size; ++v)
        {
            for (size_t u = 0; u != this->u_size; ++u)
            {
                const size_t i = v * this->u_size + u;
                const size_t j = color_normal_stride * i;
                glm::vec3 normal = this->calculate_normal_smooth(u, v);

                this->color_normal_data[j + 0] = vertices[i].color.r;
                this->color_normal_data[j + 1] = vertices[i].color.g;
                this->color_normal_data[j + 2] = vertices[i].color.b;
                this->color_normal_data[j + 3] = vertices[i].color.a;

                this->color_normal_data[j + 4] = normal.x;
                this->color_normal_data[j + 5] = normal.y;
                this->color_normal_data[j + 6] = normal.z;
            }
        }
    }
    else if (this->mode == SurfaceMode::flat)
    {
        this->color_normal_data.resize(
            color_normal_stride * 2 * (this->u_size - 1) * (v_size - 1)
        );
        for (size_t v = 0; v != v_size - 1; ++v)
        {
            for (size_t u = 0; u != this->u_size - 1; ++u)
            {
                const size_t i = v * this->u_size + u;

                for (bool is_lower_triangle : {false, true})
                {
                    const size_t j = color_normal_stride *
                                     (2 * (v * (this->u_size - 1) + u) +
                                      (is_lower_triangle ? 1 : 0));
                    glm::vec3 normal =
                        this->calculate_normal_flat(u, v, is_lower_triangle);

                    this->color_normal_data[j + 0] = vertices[i].color.r;
                    this->color_normal_data[j + 1] = vertices[i].color.g;
                    this->color_normal_data[j + 2] = vertices[i].color.b;
                    this->color_normal_data[j + 3] = vertices[i].color.a;

                    this->color_normal_data[j + 4] = normal.x;
                    this->color_normal_data[j + 5] = normal.y;
                    this->color_normal_data[j + 6] = normal.z;
                }
            }
        }
    }
}

size_t SurfaceData::get_u_size() const
{
    return this->u_size;
}

const std::vector<float> &SurfaceData::get_position_data() const
{
    return this->position_data;
}

const std::vector<float> &SurfaceData::get_color_normal_data() const
{
    return this->color_normal_data;
}

std::vector<GLuint> SurfaceData::get_index_data() const
{
    if (this->u_size < 2)
        return std::vector<GLuint>();

    const size_t v_size = this->v_size();

    std::vector<GLuint> index_data(
        2 * 2 * 3 * (v_size - 1) * (this->u_size - 1)
    );

    if (this->mode == SurfaceMode::smooth)
    {
        for (size_t v = 0; v != v_size - 1; ++v)
        {
            for (size_t u = 0; u != this->u_size - 1; ++u)
            {
                const size_t i = v * this->u_size + u;
                const size_t j = 2 * 2 * 3 * (v * (this->u_size - 1) + u);

                index_data[j + 0] = i;
                index_data[j + 1] = i;
                index_data[j + 2] = i + this->u_size;
                index_data[j + 3] = i + this->u_size;
                index_data[j + 4] = i + 1;
                index_data[j + 5] = i + 1;

                index_data[j + 6] = i + 1;
                index_data[j + 7] = i + 1;
                index_data[j + 8] = i + this->u_size;
                index_data[j + 9] = i + this->u_size;
                index_data[j + 10] = i + this->u_size + 1;
                index_data[j + 11] = i + this->u_size + 1;
            }
        }
    }
    else if (this->mode == SurfaceMode::flat)
    {
        for (size_t v = 0; v != v_size - 1; ++v)
        {
            for (size_t u = 0; u != this->u_size - 1; ++u)
            {
                const size_t i = v * this->u_size + u;
                const size_t j = 2 * 2 * 3 * (v * (this->u_size - 1) + u);
                const size_t k = 2 * (v * (this->u_size - 1) + u);

                index_data[j + 0] = i;
                index_data[j + 1] = k;
                index_data[j + 2] = i + this->u_size;
                index_data[j + 3] = k;
                index_data[j + 4] = i + 1;
                index_data[j + 5] = k;

                index_data[j + 6] = i + 1;
                index_data[j + 7] = k + 1;
                index_data[j + 8] = i + this->u_size;
                index_data[j + 9] = k + 1;
                index_data[j + 10] = i + this->u_size + 1;
                index_data[j + 11] = k + 1;
            }
        }
    }

    return index_data;
}

size_t SurfaceData::v_size() const
{
    if (this->u_size == 0)
        return 0;
    return this->position_data.size() / position_stride / this->u_size;
}

glm::vec3 SurfaceData::get_position(const size_t u, const size_t v) const
{
    return glm::vec3(
        this->position_data[position_stride * (v * this->u_size + u) + 0],
        this->position_data[position_stride * (v * this->u_size + u) + 1],
        this->position_data[position_stride * (v * this->u_size + u) + 2]
    );
}

glm::vec3
    SurfaceData::calculate_normal_smooth(const size_t u, const size_t v) const
{
    const size_t v_size = this->v_size();
    const glm::vec3 p = this->get_position(u, v);

    glm::vec3 normal(0.0f);
    if (u > 0 && v > 0)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u - 1, v), this->get_position(u, v - 1) - p
        ));
    if ((u + 1) < this->u_size && v > 0)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u, v - 1), this->get_position(u + 1, v) - p
        ));
    if ((u + 1) < this->u_size && (v + 1) < v_size)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u + 1, v), this->get_position(u, v + 1) - p
        ));
    if (u > 0 && (v + 1) < v_size)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u, v + 1), this->get_position(u - 1, v) - p
        ));

    return glm::normalize(normal);
}

glm::vec3 SurfaceData::calculate_normal_flat(
    const size_t u, const size_t v, const bool is_lower_triangle
) const
{
    glm::vec3 normal(0.0f);
    if (is_lower_triangle)
    {
        const glm::vec3 p = get_position(u + 1, v + 1);
        normal =
            glm::cross(p - get_position(u, v + 1), get_position(u + 1, v) - p);
    }
    else
    {
        const glm::vec3 p = get_position(u, v);
        normal =
            glm::cross(p - get_position(u + 1, v), get_position(u, v + 1) - p);
    }
    return glm::normalize(normal);
}
}
