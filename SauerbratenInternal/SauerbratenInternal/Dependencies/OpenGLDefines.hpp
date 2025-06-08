#pragma once

typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLuint;
typedef unsigned int GLbitfield;

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_SRC_ALPHA                      0x0302
#define GL_AMBIENT_AND_DIFFUSE            0x1602
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_TEXTURE_2D                     0x0DE1
#define GL_BLEND                          0x0BE2
#define GL_ZERO                           0x0
#define GL_COLOR_ARRAY                    0x8076
#define GL_LIGHTING                       0x0B50
#define GL_DEPTH_TEST                     0x0B71
#define GL_FRONT_AND_BACK                 0x0408

typedef struct SDL_Window SDL_Window;

// esse quat n era pra ta aqui
struct simple_quat
{
    float x, y, z, w;
};

struct simple_dualquat
{
    simple_quat real;
    simple_quat dual;

  
    void gettranslation(float& outx, float& outy, float& outz) const
    {
        float cx = real.y * dual.z - real.z * dual.y;
        float cy = real.z * dual.x - real.x * dual.z;
        float cz = real.x * dual.y - real.y * dual.x;

        outx = 2.0f * cx + dual.w * real.x - real.w * dual.x;
        outy = 2.0f * cy + dual.w * real.y - real.w * dual.y;
        outz = 2.0f * cz + dual.w * real.z - real.w * dual.z;
    }
};





