#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
	mat4 modelView = ubo.view * ubo.model;
	float xScale = length(modelView[0].xyz);
	float yScale = length(modelView[1].xyz);

	modelView[0][0] = xScale;
	modelView[0][1] = 0.0;
	modelView[0][2] = 0.0;

	modelView[1][0] = 0.0;
	modelView[1][1] = yScale;
	modelView[1][2] = 0.0;

	modelView[2][0] = 0.0;
	modelView[2][1] = 0.0;
	modelView[2][2] = 1.0;
	gl_Position = ubo.proj * modelView * vec4(inPosition, 1);

    fragColor = inColor;
	// TODO: Should I flip y here or on the host?
	fragTexCoord = vec2(inTexCoord.x, inTexCoord.y * -1);
}
