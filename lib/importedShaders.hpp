#pragma once

namespace importedShaders {
  namespace vert {
    const char pos2d[] = R"glsl(
#version 150 core

in vec2 position;

void main()
{
  gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";
    const char pos2col3[] = R"glsl(
#version 150 core

in vec2 position;
in vec3 color;

out vec3 Color;

void main()
{
  gl_Position = vec4(position, 0., 1.);
  Color = color;
}
)glsl";
  };
  namespace frag {
    const char pass[] = R"glsl(
#version 150 core

in vec3 Color;

out vec4 outColor;

void main()
{
  outColor = vec4(Color, 1.0);
}
)glsl";
    const char white[] = R"glsl(
#version 150 core

out vec4 outColor;

void main()
{
  outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)glsl";
  };
};
