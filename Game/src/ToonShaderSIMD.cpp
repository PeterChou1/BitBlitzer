#include "stdafx.h"
#include "ToonShaderSIMD.h"

void ToonShaderSIMD::Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& camera)
{

    SIMDVec3 lightDirection = lights[0].Position;
    SIMDFloat intensity = pixel.Normal.Dot(lightDirection);
    SIMDFloat highestIntensity = intensity > 0.95f;
    SIMDFloat mediumIntensity = intensity > 0.5f & intensity <= 0.95f;
    SIMDFloat lowIntensity = intensity > 0.25f & intensity <= 0.5f;
    SIMDFloat noIntensity = intensity <= 0.25f;

    SIMDVec3 highColor = SIMDVec3(1.0f, 0.8f, 0.2f);
    SIMDVec3 medColor = SIMDVec3(0.6f, 0.3f, 0.3f);
    SIMDVec3 lowColor = SIMDVec3(0.5f, 0.3f, 0.1f);
    SIMDVec3 noColor = SIMDVec3(0.2f, 0.2f, 0.3f);

    SIMDFloat highR = SIMD::Select(highestIntensity, SIMD::ZERO, highColor.X);
    SIMDFloat highG = SIMD::Select(highestIntensity, SIMD::ZERO, highColor.Y);
    SIMDFloat highB = SIMD::Select(highestIntensity, SIMD::ZERO, highColor.Z);

    SIMDFloat medR = SIMD::Select(mediumIntensity, SIMD::ZERO, medColor.X);
    SIMDFloat medG = SIMD::Select(mediumIntensity, SIMD::ZERO, medColor.Y);
    SIMDFloat medB = SIMD::Select(mediumIntensity, SIMD::ZERO, medColor.Z);

    SIMDFloat lowR = SIMD::Select(lowIntensity, SIMD::ZERO, lowColor.X);
    SIMDFloat lowG = SIMD::Select(lowIntensity, SIMD::ZERO, lowColor.Y);
    SIMDFloat lowB = SIMD::Select(lowIntensity, SIMD::ZERO, lowColor.Z);


    SIMDFloat noR = SIMD::Select(noIntensity, SIMD::ZERO, noColor.X);
    SIMDFloat noG = SIMD::Select(noIntensity, SIMD::ZERO, noColor.Y);
    SIMDFloat noB = SIMD::Select(noIntensity, SIMD::ZERO, noColor.Z);

    pixel.Color.X = highR + medR + lowR + noR;
    pixel.Color.Y = highG + medG + lowG + noG;
    pixel.Color.Z = highB + medB + lowB + noB;

}
