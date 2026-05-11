#version 330 core

uniform sampler2D tex;
uniform int displayMode;
uniform bool useTexture;
uniform vec4 color;
uniform vec3 viewPos;

struct Light {
  vec3 position;
  vec3 color;
  float intensity;
};

uniform Light lights[32];
uniform int numLights = 0;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

const int MODE_WIREFRAME = 0;
const int MODE_TEXTURED  = 1;
const int MODE_SHADED    = 2;

const float AMBIENT_STRENGTH  = 0.2;
const float SPECULAR_STRENGTH = 0.5;
const float SHININESS         = 32.0;

vec3 calcLighting(vec3 baseRGB) {
  vec3 result = vec3(0.0);
  vec3 norm    = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  for (int i = 0; i < numLights; i++) {
    vec3 ambient  = AMBIENT_STRENGTH * lights[i].color;
    vec3 lightDir = normalize(lights[i].position - FragPos);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * lights[i].color;
    vec3 halfDir  = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(norm, halfDir), 0.0), SHININESS);
    vec3 specular = SPECULAR_STRENGTH * spec * lights[i].color;
    result += (ambient + diffuse + specular) * lights[i].intensity;
  }

  return result * baseRGB;
}

void main() {
  if (displayMode == MODE_WIREFRAME) {
    FragColor = color;
    return;
  }

  vec4 baseColor = useTexture ? texture(tex, TexCoord) : color;

  if (displayMode == MODE_TEXTURED) {
    FragColor = baseColor;
    return;
  }

  FragColor = vec4(calcLighting(baseColor.rgb), baseColor.a);
}
