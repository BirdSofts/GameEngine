// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,30.08.2020</created>
/// <state></state>
// ********************************************************************************

SamplerState samplerType; // how the pixels are written on the polygon face

cbuffer LightBuffer // buffer object type (ambient / diffuse light properties)
{
  float4 ambientColour; // ambient light colour
  float4 diffuseColour; // diffuse light colour
  float3 lightDirection; // diffuse light direction
  float padding; // to size the structure at multiple of 4
}

// colour pixel shader input type
struct Pixel
{
  float4 position : SV_POSITION;
  float4 colour : COLOR;
  float3 normal : NORMAL;
};


// colour pixel shader: taking the colour draws each pixel on the polygons that will be rendered to the screen.
float4 main(Pixel input) : SV_TARGET // called by GPU when the output of vertex shader is ready
{
  
  // SV_TARGET: the semantic indicates that the return value of colour pixel shader should match the render target format
  
  float3 lightDir;
  float lightIntensity;
  float4 colour;
  
  // default output colour is set to ambient light value for all pixels
  colour = ambientColour;
  
  // light direction inversion
  lightDir = -lightDirection;
  
  // calculation of the amount of light:
  // dot product between the normal vector of polygon and the light direction vector
  // clamp the result within the range of 0 and 1
  lightIntensity = saturate(dot(input.normal, lightDir));
  
  if (lightIntensity > 0.0f)
  {
    // calculate the final colour (ambient) based on diffuse colour and light intensity
    colour += (diffuseColour * lightIntensity);
  }
  
  colour = colour + input.colour;
  
  // clamp the final light colour
  colour = saturate(colour);
  
  return colour;
  
}
