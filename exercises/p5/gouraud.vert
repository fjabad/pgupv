#version 420

$GLMatrices
$Lights
$Material

in vec4 position;
in vec3 normal;

out vec4 color;

// Suponiendo fuentes puntuales
vec4 iluminacion(vec3 pos, vec3 N, vec3 V) {
  // Componente emisiva del material
  vec4 color = emissive;
  for (int i = 0; i < lights.length(); i++) {
    // Si la fuente est� apagada, no tenerla en cuenta
    if (lights[i].enabled == 0)
      continue;
    // Vector iluminaci�n (desde v�rtice a la fuente)
    vec3 L = normalize(vec3(lights[i].positionEye) - pos);
    // Multiplicador de la componente difusa
    float diffuseMult = max(dot(N, L), 0.0);
    float specularMult = 0.0;
    if (diffuseMult > 0.0) {
      // Multiplicador de la componente especular
      vec3 R = reflect(-L, N);
      specularMult = max(0.0, dot(R, V));
      specularMult = pow(specularMult, shininess);
    }

    color += lights[i].ambient * ambient +
             lights[i].diffuse * diffuse * diffuseMult +
             lights[i].specular * specular * specularMult;
  }

  return color;
}

void main() {
  // Normal en el espacio de la c�mara
  vec3 eN = normalize(normalMatrix * normal);
  // V�rtice en el espacio de la c�mara
  vec3 eposition = vec3(modelviewMatrix * position);
  // Vector vista (desde v�rtice a la c�mara)
  vec3 V = normalize(-eposition.xyz);
  // C�lculo de la iluminaci�n
  color = iluminacion(eposition, eN, V);
  gl_Position = modelviewprojMatrix * position;
}
