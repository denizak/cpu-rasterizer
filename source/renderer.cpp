#include <rasterizer/renderer.hpp>

#include <algorithm>
#include <cmath>

namespace rasterizer
{
  void clear(image_view const & color_buffer, vector4f const & color)
  {
    auto ptr = color_buffer.pixels;
    auto size = color_buffer.width * color_buffer.height;
    std::fill_n(ptr, size, to_color4ub(color));
  }

  void draw(image_view const & color_buffer, draw_command const & command)
  {
    for (std::uint32_t vertex_index = 0;
         vertex_index + 2 < command.mesh.vertex_count;
         vertex_index += 3)
    {
      auto v0 = as_point(command.mesh.positions[vertex_index + 0]);
      auto v1 = as_point(command.mesh.positions[vertex_index + 1]);
      auto v2 = as_point(command.mesh.positions[vertex_index + 2]);

      std::int32_t xmin = std::min({std::floor(v0.x), std::floor(v1.x), std::floor(v2.x)});
      std::int32_t xmax = std::max({std::floor(v0.x), std::floor(v1.x), std::floor(v2.x)});
      std::int32_t ymin = std::min({std::floor(v0.y), std::floor(v1.y), std::floor(v2.y)});
      std::int32_t ymax = std::max({std::floor(v0.y), std::floor(v1.y), std::floor(v2.y)});

      xmin = std::max<std::int32_t>(0, xmin);
      xmax = std::min<std::int32_t>(color_buffer.width - 1, xmax);
      ymin = std::max<std::int32_t>(0, ymin);
      ymax = std::min<std::int32_t>(color_buffer.height - 1, ymax);

      for (std::int32_t y = ymin; y < ymax; ++y)
      {
        for (std::int32_t x = xmin; x < xmax; ++x)
        {
          vector4f p{x+ 0.5f, y + 0.5f, 0.f, 0.f};

          float det01p = det2D(v1 - v0, p - v0);
          float det12p = det2D(v2 - v1, p - v1);
          float det20p = det2D(v0 - v2, p - v2);

          if (det01p >= 0.f && det12p >= 0.f && det20p >= 0.f)
          {
            color_buffer.at(x, y) = to_color4ub(command.mesh.color);
          }
        }
      }
    }
  }
}
