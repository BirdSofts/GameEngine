// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,27.06.2020</created>
/// <state></state>
// ********************************************************************************

// global declarations (modified externally)
Texture2D shaderTexture; // texture resource
SamplerState samplerType; // the way the texture is rendered

cbuffer LightBuffer // buffer object type (diffuse light properties)
{
  float4 diffuseColour; // diffuse light colour
  float3 lightDirection; // diffuse light direction
  float padding; // to size the structure at multiple of 16
}

struct Pixel // pixel shader input type
{
  float4 position : SV_POSITION; // corrected vertex position
  float2 tex : TEXCOORD0; // texture coordinate
  float3 normal : NORMAL; // the calculated and normalized light
};


float4 main( Pixel input ) : SV_TARGET // processes on pixel
{
  
  // SV_TARGET: the semantic indicates that the return value of texture pixel shader should match the render target format
  
  
  // process:
  //-- sampling from the texture, render the polygon face 
  //-- calculation of the amount of light on polygon face
  
  // output:
  //-- resulted pixel containing location and properties combined from vertex, texture and light
  
  
  float4 textureColour;
  float3 lightDir;
  float lightIntensity;
  float4 colour;

  // using sampler, sample the pixel colour from the texture at this texture coordinate location.
  textureColour = shaderTexture.Sample(samplerType, input.tex);
  
  // light direction inversion
  lightDir = -lightDirection;
  
  // calculation of the amount of light:
  // dot product between the normal vector of polygon and the light direction vector
  // clamp the result within the range of 0 and 1
  lightIntensity = saturate(dot(input.normal, lightDir));
  
  // calculation of the final amount of diffuse colour
  colour = saturate(diffuseColour * lightIntensity);
  
  // combination (multiplication) of textured pixel and the final diffuse colour
  colour = colour * textureColour;
  
  return colour; // final pixel colour
  
}
