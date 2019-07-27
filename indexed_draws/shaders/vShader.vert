#version 330

in vec3 pos;
out vec4 vertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    vertColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}
