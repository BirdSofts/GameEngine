// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,27.06.2020</created>
/// <state></state>
// ********************************************************************************

// global declarations (modified externally)
cbuffer MatrixBuffer // buffer object type (three matrices)
{
  matrix worldMatrix; // vertex position correction
  matrix viewMatrix;
  matrix projectionMatrix;
};


// type declarations
struct Vertex // vertex shader input type
{
  float4 position : POSITION; // vertex shaders
  float2 tex : TEXCOORD0; // texture coordinates
  float3 normal : NORMAL; // normal light direction
};

struct Pixel // pixel shader input type
{
  float4 position : SV_POSITION; // pixel shaders
  float2 tex : TEXCOORD0; // sampled pixel from texture
  float3 normal : NORMAL; // calculated amount of light
};


Pixel main( Vertex input ) // processes on vertex
{
  
  // TEXCOORD0 semantic: texture coordinate: float U (width) and float V (height) (texture dimension, each from 0.0f to 1.0f)
  // note that since multiple texture coordinates are allowed,
  // the possibility is there to change the zero to any number, indicating the set of coordinates.
  
  // NORMAL semantic: light direction
  
  
  // input: vertex position, texture coordinate and normal light direction defined by 9 floats,
  // indicated by the POSITION, TEXCOORD0 NORMAL semantic
  
  // process:
  //-- vertex position calculation: (against world, view, and projection matrices)
  //-- light calculation in the world space
  
  // output:
  //-- corrected vertex position
  //-- passing the texture coordinate to pixel shader
  //-- passing the calculated and normalized light
  
  
  Pixel output; // output vertex structure
  
  // change the position vector to 4 units (proper matrix calculation)
  input.position.w = 1.0f;
  // manipulation of input vertex through world, view and projection matrices,
  // resulting to the correct vertex location for 3D rendering, and then onto the 2D screen
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);

  // additionally store input texture coordinate (for pixel shader)
  output.tex = input.tex;
  
  // normal vector calculation against world matrix
  output.normal = mul(input.normal,(float3x3)worldMatrix);
  
  output.normal = normalize(output.normal); // normalization
  
  return output;
  
};
