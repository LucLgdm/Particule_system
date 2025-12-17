#version 330 core

in vec2 TexCoord;
in vec3 Color;
flat in float HasTex;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D uTexture;
uniform vec3 lightPos;
uniform bool uLightOn;
uniform float lightIntensity;

out vec4 FragColor;

void main()
{
	vec4 baseColor;
	
	if (HasTex > 0.5)
		baseColor = texture(uTexture, TexCoord);
	else
		baseColor = vec4(Color, 1.0);
	
	if (uLightOn) {
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(Normal, lightDir), 0.0);
		baseColor.rgb *= diff * lightIntensity;
	}

	FragColor = baseColor;
}
