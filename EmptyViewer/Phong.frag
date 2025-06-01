// Phong.frag
#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
uniform float ambientIntensity;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // ���� ���� ���
    vec3 ambient = ambientIntensity * ka;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    if (diff == 0.0) spec = 0.0; // ���� ������ ����ŧ�� ����
    vec3 specular = ks * spec;

    vec3 result = ambient + diffuse + specular;

    // ���� ���� - ����� �ʹ� ���� �ʵ��� ����
    result = clamp(result, 0.0, 1.0);
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
