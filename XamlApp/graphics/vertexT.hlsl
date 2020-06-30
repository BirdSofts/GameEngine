// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,27.06.2020</created>
/// <state></state>
//xx <changed>}Y{,28.06.2020</changed>
// ********************************************************************************

// global declarations
// buffer object type, containing three matrices, updated on each execution
cbuffer MatrixBuffer
{
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};


// type declarations
// texture vertex shader input type
struct Vertex
{
  float4 position : POSITION; // vertex shaders
  float2 tex : TEXCOORD0;
};

// texture pixel shader input type
struct Pixel
{
  float4 position : SV_POSITION; // pixel shaders
  float2 tex : TEXCOORD0;
};


// texture vertex shader: calculate the vertex location by matrices and prepare the output for texture pixel shader
Pixel main( Vertex input ) // called by GPU when processing data from vertex buffer
{
  
  // input: vertex position and texture coordinate defined by six floats
  // indicated by the POSITION and TEXCOORD0 semantic

  // TEXCOORD0 semantic: texture coordinate: float U (width) and float V (height) (texture dimension, each from 0.0f to 1.0f)
  // note that since multiple texture coordinates are allowed,
  // the possibility is there to change the zero to any number, indicating the set of coordinates.
  
  // process:
  // vertex position calculation: (against world, view, and projection matrices)
  
  // output:
  //-- passing the texture coordinate of each position to pixel shader
  
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

  return output;
  
};
