#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;        
uniform sampler2D u_paletteTexture; 
uniform sampler2D u_normalMap;      

const vec3 LIGHT_DIR = normalize(vec3(0.5, 0.5, 1.0));
const float AMBIENT = 0.5; 

void main()
{

    vec4 dataColor = texture2D(u_texture, v_texCoord);
    if (dataColor.a < 0.1) discard;

    vec2 lookupUV = vec2(dataColor.r, dataColor.g);
    vec3 paletteColor = texture2D(u_paletteTexture, lookupUV).rgb;

    vec3 normalColor = texture2D(u_normalMap, v_texCoord).rgb;
    
    vec3 normal = normalize(normalColor * 2.0 - 1.0);
    
    float diffuse = max(dot(normal, LIGHT_DIR), 0.0);
    
    vec3 litColor = paletteColor * (AMBIENT + diffuse);

    if (dataColor.b > 0.1) {
        litColor += vec3(0.2, 0.1, 0.4) * dataColor.b * 2.0;
    }

    gl_FragColor = v_fragmentColor * vec4(litColor, dataColor.a);
}