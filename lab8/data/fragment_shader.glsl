#version 330 core

in vec2 vertex_texture;

out vec4 fragment_color;

uniform sampler2D texture_object;

void main() {
   fragment_color = texture(texture_object, vertex_texture);
}

