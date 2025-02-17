#ifndef __ANTHONYMATH
#define __ANTHONYMATH

namespace GLSLTypes
{

    bool marchLoop(Ray& currentRay, const Scene& scene, Hitpoint& hit){
        for (int i = 0; i < MAX_RAYMARCH_STEPS; i++)
        {
            float safeStepSize = scene.getRootPrimitive()->getSignedDistance(currentRay.getOrigin(), hit);
            if (safeStepSize < MIN_RAYMARCH_STEP_SIZE)
            {
                return true;
            }
            currentRay = Ray(currentRay.getDirection(), currentRay.pointAtParameter(safeStepSize));;
        }
		
		return false;
    }

    typedef struct vec2
    {
        float x;
        float y;

        vec2(float x, float y) : x(x), y(y) {}

        vec2 operator+(vec2 b) { return vec2(x + b.x, y + b.y); }

        vec2 operator*(float b) { return vec2(x * b, y * b); }

        vec2 operator/(float b) { return vec2(x / b, y / b); }
    } vec2;

    typedef struct vec3
    {
        float x;
        float y;
        float z;

        vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        float length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        vec3 operator-(vec3 b) { return vec3(x - b.x, y - b.y, z - b.z); }
    } vec3;

    typedef struct vec4
    {
        float x;
        float y;
        float z;
        float w;

        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        vec4 operator+(vec4 b) { return vec4(x + b.x, y + b.y, z + b.z, w + b.w); }

        vec4 operator*(float b) { return vec4(x * b, y * b, z * b, w * b); }
    } vec4;

    typedef struct mat4x4
    {
        float m[4][4];

        mat4x4(float x11, float x12, float x13, float x14,
               float x21, float x22, float x23, float x24,
               float x31, float x32, float x33, float x34,
               float x41, float x42, float x43, float x44)
        {
            float tmp[4][4] = {
                {x11, x12, x13, x14},
                {x21, x22, x23, x24},
                {x31, x32, x33, x34},
                {x41, x42, x43, x44}};
            memcpy(m, tmp, sizeof(m));
        }

        mat4x4()
        {
            memset(m, 0, sizeof(m));
        }

        mat4x4 operator*(mat4x4 B)
        {
            auto b = B.m;
            mat4x4 C;
            auto c = C.m;

            for (int h = 0; h < 4; ++h)
            {
                for (int w = 0; w < 4; ++w)
                {

                    float acc = 0;
                    for (int i = 0; i < 4; ++i)
                        acc += m[h][i] * b[i][w];
                    c[h][w] = acc;
                }
            }
            return C;
        }

        vec4 operator*(vec4 v)
        {
            return vec4(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
        }

    } mat4x4;

    mat4x4 identity_4x4()
    {
        return mat4x4(1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1);
    }
}

#endif