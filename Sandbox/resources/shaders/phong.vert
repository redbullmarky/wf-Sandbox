#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexColour;
in vec2 vertexTexCoord;
in vec4 vertexTangent;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColour;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBitangent;

void main()
{
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));

    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    fragNormal = normalize(normalMatrix * vertexNormal);
    fragTangent = normalize(normalMatrix * vertexTangent.xyz);

    // Calculate bitangent using handedness from tangent.w
    fragBitangent = normalize(cross(fragNormal, fragTangent) * vertexTangent.w);

    fragColour = vertexColour;
    fragTexCoord = vertexTexCoord;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
