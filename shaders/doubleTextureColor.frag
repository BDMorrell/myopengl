#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texA;
uniform sampler2D texB;

void main()
{
  vec4 a = texture(texA, Texcoord);
  vec4 b = texture(texB, Texcoord);
  outColor = mix(a, b, 0.5) * vec4(Color, 1.0);
}
