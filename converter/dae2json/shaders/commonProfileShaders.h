#ifndef __COLLADA2JSON_SHADERS__
#define __COLLADA2JSON_SHADERS__

#define SHADER_STR(Src) #Src
#define SHADER(Src) SHADER_STR(Src)

//lambert0 -> just a diffuse color
const char* lambert0Vs = SHADER(
                                precision highp float;\n
                                attribute vec3 vert;\n
                                attribute vec3 normal;\n
                                varying vec3 v_normal;\n
                                uniform mat4 u_mvMatrix;\n
                                uniform mat3 u_normalMatrix;\n
                                uniform mat4 u_projMatrix;\n
                                \n
                                void main(void) {\n
                                    v_normal = normalize(u_normalMatrix * normal);\n
                                    gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                });

const char* lambert0Fs = SHADER(
                                precision highp float;\n
                                uniform vec3 u_diffuseColor;\n
                                varying vec3 v_normal;\n
                                \n
                                void main(void) { \n
                                    vec3 normal = normalize(v_normal);\n
                                    float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                    gl_FragColor = vec4(u_diffuseColor.xyz *lambert, 1.); \n
                                });

const char* lambert1Vs = SHADER(
                                precision highp float;\n
                                attribute vec3 vert;\n
                                attribute vec3 normal;\n
                                attribute vec2 texcoord;\n
                                varying vec3 v_normal;\n
                                varying vec2 v_texcoord;\n
                                uniform mat4 u_mvMatrix;\n
                                uniform mat3 u_normalMatrix;\n
                                uniform mat4 u_projMatrix;\n
                                \n
                                void main(void) {\n
                                    v_normal = normalize(u_normalMatrix * normal);\n
                                    v_texcoord = vec2(texcoord.x, 1. - texcoord.y);\n
                                    gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                });

const char* lambert1Fs = SHADER(
                                precision highp float;\n
                                varying vec3 v_normal;\n
                                varying vec2 v_texcoord;\n
                                uniform sampler2D u_diffuseTexture;\n
                                \n
                                void main(void) { \n
                                    vec3 normal = normalize(v_normal);\n
                                    float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                    vec4 color = texture2D(u_diffuseTexture, v_texcoord);\n
                                    gl_FragColor = vec4(color.rgb * lambert, 1.); \n
                                });


#endif