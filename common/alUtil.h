#pragma once

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <OpenEXR/ImathVec.h>

#include <ai.h>

inline void concentricSampleDisk(float u1, float u2, float& dx, float& dy)
{
    float r, theta;
    // Map uniform random numbers to $[-1,1]^2$
    float sx = 2 * u1 - 1;
    float sy = 2 * u2 - 1;

    // Map square to $(r,\theta)$

    // Handle degeneracy at the origin
    if (sx == 0.0 && sy == 0.0) {
        dx = 0.0;
        dy = 0.0;
        return;
    }
    if (sx >= -sy) {
        if (sx > sy) {
            // Handle first region of disk
            r = sx;
            if (sy > 0.0) theta = sy/r;
            else          theta = 8.0f + sy/r;
        }
        else {
            // Handle second region of disk
            r = sy;
            theta = 2.0f - sx/r;
        }
    }
    else {
        if (sx <= sy) {
            // Handle third region of disk
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else {
            // Handle fourth region of disk
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }
    theta *= M_PI / 4.f;
    dx = r * cosf(theta);
    dy = r * sinf(theta);
}

inline Imath::V3f cosineSampleHemisphere(float u1, float u2)
{
   Imath::V3f ret;
   concentricSampleDisk(u1, u2, ret.x, ret.z);
   ret.y = sqrtf(std::max(0.0f, 1.0f - ret.x*ret.x - ret.z*ret.z));
   return ret;
}

inline float maxh(const AtRGB& c)
{
   return std::max(std::max(c.r, c.g), c.b);
}

inline float minh(const AtRGB& c)
{
   return std::max(std::max(c.r, c.g ), c.b);
}

inline AtRGB max(const AtRGB& c1, const AtRGB& c2)
{
	AtRGB c;
	c.r = std::max(c1.r, c2.r);
	c.g = std::max(c1.g, c2.g);
	c.b = std::max(c1.b, c2.b);
	return c;
}

inline AtRGB min(const AtRGB& c1, const AtRGB& c2)
{
	AtRGB c;
	c.r = std::min(c1.r, c2.r);
	c.g = std::min(c1.g, c2.g);
	c.b = std::min(c1.b, c2.b);
	return c;
}

inline float clamp(float a, float mn, float mx)
{
	return std::min(std::max(a, mn), mx);
}

inline AtRGB clamp(const AtRGB& a, const AtRGB& mn, const AtRGB& mx)
{
	return min(max(a, mn), mx);
}

inline float lerp(const float a, const float b, const float t)
{
   return (1-t)*a + t*b;
}

inline AtRGB lerp(const AtRGB& a, const AtRGB& b, const float t)
{
   AtRGB r;
   r.r = lerp( a.r, b.r, t );
   r.g = lerp( a.g, b.g, t );
   r.b = lerp( a.b, b.b, t );
   return r;
}

inline float fresnel(float cosi, float etai)
{
   if (cosi >= 1.0f) return 0.0f; // FIXME : return normal reflectance
   float sint = etai * sqrtf(1.0f-cosi*cosi);
   if ( sint >= 1.0f ) return 1.0f;

   float cost = sqrtf(1.0f-sint*sint);
   float pl =     (cosi - (etai * cost))
               /  (cosi + (etai * cost));
   float pp =    ((etai * cosi) - cost)
               / ((etai * cosi) + cost);
   return (pl*pl+pp*pp)*0.5f;
}

inline AtRGB sqrt(AtRGB c)
{
	c.r = sqrtf(c.r);
	c.g = sqrtf(c.g);
	c.b = sqrtf(c.b);
	return c;
}

inline AtRGB exp(AtRGB c)
{
	c.r = expf(c.r);
	c.g = expf(c.g);
	c.b = expf(c.b);
	return c;
}

inline AtRGB pow(AtRGB c, float e)
{
	c.r = powf(c.r, e);
	c.g = powf(c.g, e);
	c.b = powf(c.b, e);
	return c;
}

inline AtRGB rgb(float f)
{
	AtRGB c;
	c.r = c.g = c.b = f;
	return c;
}

inline AtRGB rgb(float r, float g, float b)
{
	AtRGB c;
	c.r = r; c.g = g; c.b = b;
	return c;
}

// For the sake of simplicity we limit eta to 1.3 so cache A here
inline float A(float eta)
{
	float Fdr = -1.440/(eta*eta) + 0.710/eta + 0.668 + 0.0636*eta;
	return (1 + Fdr)/(1 - Fdr);
}

inline AtRGB brdf( const AtRGB& _alpha_prime )
{
	AtRGB sq = sqrt( 3.0f * (AI_RGB_WHITE - _alpha_prime) );
	return _alpha_prime * 0.5f * (AI_RGB_WHITE + exp( -(A(1.3f)*rgb(4.0f/3.0f)*sq ) )) * exp( -sq );
}

void alphaInversion( const AtRGB& scatterColour, float scatterDist, AtRGB& sigma_s_prime_, AtRGB& sigma_a_ );
float alpha1_3( float x );
inline AtRGB alpha1_3(const AtRGB& c)
{
	return rgb(alpha1_3(c.r), alpha1_3(c.g), alpha1_3(c.b));
}

inline std::ostream& operator<<( std::ostream& os, AtVector v )
{
   os << "(" << v.x << "," << v.y << "," << v.z << ")";
   return os;
}

