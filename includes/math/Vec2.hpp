#ifndef SCOP_VEC2_HPP
# define SCOP_VEC2_HPP

# include <cmath>
# include <math_utils.hpp>

class Vec2
{
    private:
        float x;
        float y;

    public:
        Vec2();
        Vec2(float xValue, float yValue);
        Vec2(const Vec2& other);
        ~Vec2();
    
        float getX() const;
        float getY() const;

        bool isValidVector() const;
};


#endif
