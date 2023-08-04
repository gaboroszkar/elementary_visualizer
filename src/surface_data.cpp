#include <elementary_visualizer/elementary_visualizer.hpp>

namespace elementary_visualizer
{

constexpr int float_per_triangle_vertex = 3 + 4 + 3;

SurfaceData::SurfaceData(
    const std::vector<Vertex> &vertices, const size_t u_size, const Mode mode
)
    : triangle_vertex_data(
          float_per_triangle_vertex * (2 * 3) *
          SurfaceData::number_of_squares(vertices.size(), u_size)
      ),
      u_size(u_size),
      mode(mode)
{
    if (!SurfaceData::is_size_correct(vertices.size(), u_size))
        return;

    const size_t v_size = this->v_size();

    for (size_t v = 0; v != v_size; ++v)
        for (size_t u = 0; u != u_size; ++u)
            this->set_position_explicitly(
                u, v, vertices[v * u_size + u].position
            );

    if (mode == Mode::smooth)
    {
        for (size_t v = 0; v != v_size; ++v)
        {
            for (size_t u = 0; u != u_size; ++u)
            {
                this->set_color_explicitly(
                    u, v, vertices[v * u_size + u].color
                );
                this->update_normal(u, v);
            }
        }
    }
    else if (mode == Mode::flat)
    {
        for (size_t v = 0; v != v_size - 1; ++v)
        {
            for (size_t u = 0; u != u_size - 1; ++u)
            {
                for (const bool is_lower_triangle : {false, true})
                {
                    this->set_color_explicitly(
                        u, v, is_lower_triangle, vertices[v * u_size + u].color
                    );
                    this->update_normal(u, v, is_lower_triangle);
                }
            }
        }
    }
}

const std::vector<float> &SurfaceData::get_data() const
{
    return this->triangle_vertex_data;
}

void SurfaceData::set_position(
    const size_t u, const size_t v, const glm::vec3 &position
)
{
    if (u < this->u_size && v < this->v_size())
    {
        this->set_position_explicitly(u, v, position);
        if (this->mode == Mode::smooth)
        {
            this->update_normal(u, v);
        }
        else if (this->mode == Mode::flat)
        {
            this->update_normal(u, v, false);
            this->update_normal(u, v, true);
        }
    }
}

void SurfaceData::set_color(
    const size_t u, const size_t v, const glm::vec4 &color
)
{
    if (u < this->u_size && v < this->v_size())
    {
        if (this->mode == Mode::smooth)
        {
            this->set_color_explicitly(u, v, color);
        }
        else if (this->mode == Mode::flat)
        {
            this->set_color_explicitly(u, v, false, color);
            this->set_color_explicitly(u, v, true, color);
        }
    }
}

size_t SurfaceData::get_u_size() const
{
    return this->u_size;
}

bool SurfaceData::is_size_correct(const size_t size, const size_t u_size)
{
    return (size > 2) && (u_size > 2) && (size % u_size == 0);
}

size_t SurfaceData::number_of_squares(const size_t size, const size_t u_size)
{
    if (!SurfaceData::is_size_correct(size, u_size))
        return 0;

    const size_t v_size = size / u_size;
    return (u_size - 1) * (v_size - 1);
}

size_t SurfaceData::v_size() const
{
    const size_t number_of_triangle_vertices =
        this->triangle_vertex_data.size() / float_per_triangle_vertex;
    const size_t number_of_squares = number_of_triangle_vertices / (2 * 3);
    return number_of_squares / (this->u_size - 1) + 1;
}

/**
 * @brief Returns the offset for the vertex for the triangle_vertex_data.
 *
 * The u, v parameters tell the u and v coordinates respectively,
 * and for the is_lower_triangle and vertex_offset, see the figure below.
 *
 * If is_lower_triangle is false,
 * the vertex_offset means the following vertices.
 *  (0) - (2)
 *   |    /|
 *   |  /  |
 *   |/    |
 *  (1) ---
 *
 * If is_lower_triangle is true,
 * the vertex_offset means the following vertices.
 *    --- (0)
 *   |    /|
 *   |  /  |
 *   |/    |
 *  (1) - (2)
 */
size_t SurfaceData::offset(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const size_t vertex_offset
) const
{
    const size_t square = v * (this->u_size - 1) + u;
    return float_per_triangle_vertex *
           (3 * (2 * square + (is_lower_triangle ? 1 : 0)) + vertex_offset);
}

void SurfaceData::set_vertex_explicitly(
    const size_t u,
    const size_t v,
    std::function<void(const size_t, const size_t, const bool, const size_t)>
        set_function
)
{
    const size_t v_size = this->v_size();

    if (u > 0 && v > 0)
    {
        set_function(u - 1, v - 1, true, 2);
    }
    if ((u + 1) < u_size && v > 0)
    {
        set_function(u, v - 1, false, 1);
        set_function(u, v - 1, true, 1);
    }
    if ((u + 1) < u_size && (v + 1) < v_size)
    {
        set_function(u, v, false, 0);
    }
    if (u > 0 && (v + 1) < v_size)
    {
        set_function(u - 1, v, false, 2);
        set_function(u - 1, v, true, 0);
    }
}

void SurfaceData::set_vertex_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    std::function<void(const size_t, const size_t, const bool, const size_t)>
        set_function
)
{
    set_function(u, v, is_lower_triangle, 0);
    set_function(u, v, is_lower_triangle, 1);
    set_function(u, v, is_lower_triangle, 2);
}

void SurfaceData::set_position_explicitly(
    const size_t u, const size_t v, const glm::vec3 &position
)
{
    this->set_vertex_explicitly(
        u,
        v,
        [&](const size_t u,
            const size_t v,
            const bool is_lower_triangle,
            const size_t vertex_offset)
        {
            this->set_position_explicitly(
                u, v, is_lower_triangle, vertex_offset, position
            );
        }
    );
}

void SurfaceData::set_position_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const size_t vertex_offset,
    const glm::vec3 &position
)
{
    const size_t offset_position = 0;
    size_t offset =
        this->offset(u, v, is_lower_triangle, vertex_offset) + offset_position;
    this->triangle_vertex_data[offset + 0] = position.x;
    this->triangle_vertex_data[offset + 1] = position.y;
    this->triangle_vertex_data[offset + 2] = position.z;
}

void SurfaceData::set_color_explicitly(
    const size_t u, const size_t v, const glm::vec4 &color
)
{
    this->set_vertex_explicitly(
        u,
        v,
        [&](const size_t u,
            const size_t v,
            const bool is_lower_triangle,
            const size_t vertex_offset) {
            this->set_color_explicitly(
                u, v, is_lower_triangle, vertex_offset, color
            );
        }
    );
}

void SurfaceData::set_color_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const glm::vec4 &color
)
{
    this->set_vertex_explicitly(
        u,
        v,
        is_lower_triangle,
        [&](const size_t u,
            const size_t v,
            const bool is_lower_triangle,
            const size_t vertex_offset) {
            this->set_color_explicitly(
                u, v, is_lower_triangle, vertex_offset, color
            );
        }
    );
}

void SurfaceData::set_color_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const size_t vertex_offset,
    const glm::vec4 &color
)
{
    const size_t offset_color = 3;
    size_t offset =
        this->offset(u, v, is_lower_triangle, vertex_offset) + offset_color;
    this->triangle_vertex_data[offset + 0] = color.r;
    this->triangle_vertex_data[offset + 1] = color.g;
    this->triangle_vertex_data[offset + 2] = color.b;
    this->triangle_vertex_data[offset + 3] = color.a;
}

void SurfaceData::set_normal_explicitly(
    const size_t u, const size_t v, const glm::vec3 &normal
)
{
    this->set_vertex_explicitly(
        u,
        v,
        [&](const size_t u,
            const size_t v,
            const bool is_lower_triangle,
            const size_t vertex_offset) {
            this->set_normal_explicitly(
                u, v, is_lower_triangle, vertex_offset, normal
            );
        }
    );
}

void SurfaceData::set_normal_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const glm::vec3 &normal
)
{
    this->set_vertex_explicitly(
        u,
        v,
        is_lower_triangle,
        [&](const size_t u,
            const size_t v,
            const bool is_lower_triangle,
            const size_t vertex_offset) {
            this->set_normal_explicitly(
                u, v, is_lower_triangle, vertex_offset, normal
            );
        }
    );
}

void SurfaceData::set_normal_explicitly(
    const size_t u,
    const size_t v,
    const bool is_lower_triangle,
    const size_t vertex_offset,
    const glm::vec3 &normal
)
{
    const size_t offset_normal = 3 + 4;
    size_t offset =
        this->offset(u, v, is_lower_triangle, vertex_offset) + offset_normal;
    this->triangle_vertex_data[offset + 0] = normal.x;
    this->triangle_vertex_data[offset + 1] = normal.y;
    this->triangle_vertex_data[offset + 2] = normal.z;
}

glm::vec3 SurfaceData::get_position(const size_t u, const size_t v) const
{
    const size_t v_size = this->v_size();

    const size_t offset_position = 0;
    size_t offset;
    if ((u + 1) == this->u_size && (v + 1) == v_size)
        offset = this->offset(u - 1, v - 1, true, 2);
    else if ((u + 1) == this->u_size && (v + 1) != v_size)
        offset = this->offset(u - 1, v, true, 0);
    else if ((u + 1) != this->u_size && (v + 1) == v_size)
        offset = this->offset(u, v - 1, true, 1);
    else
        offset = this->offset(u, v, false, 0);
    offset += offset_position;

    return glm::vec3(
        this->triangle_vertex_data[offset + 0],
        this->triangle_vertex_data[offset + 1],
        this->triangle_vertex_data[offset + 2]
    );
}

void SurfaceData::update_normal(const size_t u, const size_t v)
{
    const size_t v_size = this->v_size();
    const glm::vec3 p = this->get_position(u, v);

    glm::vec3 normal(0.0f);
    if (u > 0 && v > 0)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u - 1, v), this->get_position(u, v - 1) - p
        ));
    if ((u + 1) < u_size && v > 0)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u, v - 1), this->get_position(u + 1, v) - p
        ));
    if ((u + 1) < u_size && (v + 1) < v_size)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u + 1, v), this->get_position(u, v + 1) - p
        ));
    if (u > 0 && (v + 1) < v_size)
        normal += glm::normalize(glm::cross(
            p - this->get_position(u, v + 1), this->get_position(u - 1, v) - p
        ));
    normal = glm::normalize(normal);

    this->set_normal_explicitly(u, v, normal);
}

void SurfaceData::update_normal(
    const size_t u, const size_t v, const bool is_lower_triangle
)
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
    normal = glm::normalize(normal);

    this->set_normal_explicitly(u, v, is_lower_triangle, normal);
}
}
