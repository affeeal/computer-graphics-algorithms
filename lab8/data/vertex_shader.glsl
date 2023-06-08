#version 330 core

layout (location = 0) in vec3 attribute_position;
layout (location = 1) in vec2 attribute_texture;

out vec2 vertex_texture;

void main() {
   gl_Position = vec4(attribute_position, 1.0);
   vertex_texture = attribute_texture;
}

