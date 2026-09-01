#version 450
layout(location = 0) in vec3 vsg_Vertex;
layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelView;
} pc;
void main() {
    gl_Position = pc.projection * pc.modelView * vec4(vsg_Vertex, 1.0);
}
