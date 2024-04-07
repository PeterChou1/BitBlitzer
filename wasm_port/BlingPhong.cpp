#include "stdafx.h"
#include "BlingPhong.h"

void BlinnPhongSIMD::Shade(SIMDPixel& pixel, 
                           std::vector<PointLight>& lights, 
                           Material& texture,
                           Camera& cam)
{
    // setup 
    PointLight light = lights[0];
    SIMDVec3 camPos = cam.Position;
    SIMDVec3 lightPos = light.Position;
    SIMDVec3 lightColor = light.Color;
    SIMDFloat shininess = texture.highlight;
    SIMDVec3 ambientColor = texture.ambient;
    SIMDVec3 diffuseColor = texture.diffuse;
    SIMDVec3 specularColor = texture.specular;
    SIMDVec3 fragPos = pixel.WorldSpacePosition;

    // Phong Shading Model
    SIMDFloat r, g, b;
    texture.SampleSIMD(pixel.TextureCoord, r, g, b);



    SIMDVec3 mappedDiffuseColor = SIMDVec3(r / 255.0, g / 255.0, b / 255.0);
    SIMDVec3 normal = pixel.Normal.Normalize();
    SIMDVec3 lightDir = (lightPos - fragPos).Normalize();
    SIMDVec3 viewDir = (camPos - fragPos).Normalize();
    SIMDFloat diff = normal.Dot(lightDir);

    diff = SIMD::Max(diff, SIMD::ZERO);

    // Blinn Phong Model
    SIMDVec3 halfwayDir = (lightDir + viewDir).Normalize();
    SIMDFloat spec = SIMD::Max(normal.Dot(halfwayDir), SIMD::ZERO);
    spec.pow(shininess);
    SIMDVec3 specular = lightColor * specularColor * spec;
    SIMDVec3 diffuse = lightColor * diff;




    pixel.Color = (specular + ambientColor + diffuse) * mappedDiffuseColor * diffuseColor;
    // Clamp the pixel colors to 0 -> 1
    pixel.Color.X = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, pixel.Color.X);
    pixel.Color.Y = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, pixel.Color.Y);
    pixel.Color.Z = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, pixel.Color.Z);


}
