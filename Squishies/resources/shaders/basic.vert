#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec4 vertexColour;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragPosition;
out vec4 fragColour;

void main()
{
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragColour = vertexColour;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
