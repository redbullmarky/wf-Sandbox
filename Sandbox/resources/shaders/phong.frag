#version 330
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColour;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragBitangent;

// textures
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
// uniform sampler2D shadowMap;

// flags
uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasSpecularMap;
uniform bool hasShadowMap;

// material props
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform float normalStrength;
uniform float specularShininess;
uniform float specularIntensity;

// LIGHTS & CAMERA
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec4 lightColour;
uniform float ambientLevel;

out vec4 finalColour;

void main()
{
	// Build TBN matrix for normal mapping
    mat3 TBN = mat3(normalize(fragTangent), normalize(fragBitangent), normalize(fragNormal));

    vec3 normal = normalize(fragNormal);
    if (hasNormalMap) {
        vec3 normalMapSample = texture(normalMap, fragTexCoord).xyz * 2.0 - 1.0;
        normalMapSample.xy *= normalStrength;
        normal = normalize(TBN * normalMapSample);
    }

	// Base diffuse colour
    vec4 baseDiffuse = diffuseColour * fragColour;
    if (hasDiffuseMap) {
        baseDiffuse *= texture(diffuseMap, fragTexCoord);
	}

	// Lighting
    float diff = max(dot(normal, -lightDir), 0.0);
    vec4 diffuse  = diff * baseDiffuse * lightColour;

    // Blinn-Phong specular (no diff>0 gate)
    vec3 viewDir   = normalize(viewPos - fragPosition);
    vec3 halfDir   = normalize(-lightDir + viewDir);
    float specAngle= max(dot(normal, halfDir), 0.0);
    float spec     = pow(specAngle, specularShininess) * specularIntensity;
    vec4 specular  = vec4(specularColour.rgb * spec, specularColour.a * spec);
    if (hasSpecularMap) {
        specular *= texture(specularMap, fragTexCoord);
    }

    // Ambient
    vec4 ambient = baseDiffuse * ambientLevel * lightColour;

    // Done
    finalColour = ambient + diffuse + specular;

	// Optional gamma correction
    // finalColour = pow(finalColour, vec4(1.0 / 2.2));

    // Debug: visualize normals
    // finalColor = vec4(normal * 0.5 + 0.5, 1.0);

	// Visualize depth delta to debug shadow acne
	// float depthDelta = curDepth - texture(shadowMap, sampleCoords).r;
	// float scale = 1000.0;
	// vec3 debugColor = clamp(vec3(depthDelta * scale), 0.0, 1.0);
	// finalColor = vec4(debugColor, 1.0);
}
