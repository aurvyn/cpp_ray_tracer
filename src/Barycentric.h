#ifndef __BARYCENTRIC
#define __BARYCENTRIC

class Barycentric
{
public:
    Barycentric(Vector3 const & a, Vector3 const & b, Vector3 const & c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    Vector3 getFromP(Vector3 p)
    {
        const float areaABC = getArea(a, b, c);
        const float areaPBC = getArea(p, b, c);
        const float areaAPC = getArea(a, p, c);
        const float areaABP = getArea(a, b, p);

        float aConstant = areaPBC / areaABC;
        float bConstant = areaAPC / areaABC;
        float cConstant = areaABP / areaABC;

        return {aConstant, bConstant, cConstant};
    }

private:
    Vector3 a;
    Vector3 b;
    Vector3 c;

    float denominator;

    static float getArea(Vector3 a, Vector3 b, Vector3 c)
    {
        // Vector3 ab = b - a;
        // Vector3 ac = c - a;
        // return abs(ab.cross(ac))/2;
        return ((a - b).cross(c-a)).length() / 2;
    }

};

#endif