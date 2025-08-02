#version 330
in vec3 fragPosition;
in vec4 fragColour;

uniform vec4 diffuseColour;

out vec4 finalColour;

void main()
{
    if (diffuseColour != vec4(0.0)) {
        finalColour = diffuseColour;
    } else if (fragColour != vec4(0.0)) {
        finalColour = fragColour;
    } else {
        finalColour = vec4(1.0);
    }
}
