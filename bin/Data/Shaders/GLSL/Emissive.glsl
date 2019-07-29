#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

#define KERNEL_SIZE 9

varying vec2 vTexCoord;
varying vec2 vScreenPos;

#ifdef COMPILEPS
uniform vec2 cblurInvSize;
const float cBlurRadius = 2;
#endif

#ifdef COMBINE
uniform vec2 cEmissiveGlowMix;
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetQuadTexCoord(gl_Position);
    vScreenPos = GetScreenPosPreDiv(gl_Position);
}

void PS()
{
#if defined(DEFERRED)
    gl_FragColor = cMatDiffColor;
#elif defined(EMISSIVE)
    gl_FragColor = vec4(cMatEmissiveColor, 1.0);
#elif defined(BLURH) || defined(BLURV)
#   if defined(BLURH)
    vec2 off = vec2(cblurInvSize.x*cBlurRadius, 0.0);
#   else
    vec2 off = vec2(0.0, cblurInvSize.y*cBlurRadius);
#   endif
#   if KERNEL_SIZE == 5
    vec3 bloom = texture2D(sDiffMap, vTexCoord + off*-2).rgb * 0.06136
               + texture2D(sDiffMap, vTexCoord + off*-1).rgb * 0.24477
               + texture2D(sDiffMap, vTexCoord + off* 0).rgb * 0.38774
               + texture2D(sDiffMap, vTexCoord + off* 1).rgb * 0.24477
               + texture2D(sDiffMap, vTexCoord + off* 2).rgb * 0.06136;
    gl_FragColor = vec4(bloom, 1.0);
#   elif KERNEL_SIZE == 7
    vec3 bloom = texture2D(sDiffMap, vTexCoord + off*-3).rgb * 0.00598
               + texture2D(sDiffMap, vTexCoord + off*-2).rgb * 0.06063
               + texture2D(sDiffMap, vTexCoord + off*-1).rgb * 0.24184
               + texture2D(sDiffMap, vTexCoord + off* 0).rgb * 0.38310
               + texture2D(sDiffMap, vTexCoord + off* 1).rgb * 0.24184
               + texture2D(sDiffMap, vTexCoord + off* 2).rgb * 0.06063
               + texture2D(sDiffMap, vTexCoord + off* 3).rgb * 0.00598;
    gl_FragColor = vec4(bloom, 1.0);
#   elif KERNEL_SIZE == 9
    vec3 bloom = texture2D(sDiffMap, vTexCoord + off*-4).rgb * 0.081812
               + texture2D(sDiffMap, vTexCoord + off*-3).rgb * 0.101701
               + texture2D(sDiffMap, vTexCoord + off*-2).rgb * 0.118804
               + texture2D(sDiffMap, vTexCoord + off*-1).rgb * 0.130417
               + texture2D(sDiffMap, vTexCoord + off* 0).rgb * 0.134535
               + texture2D(sDiffMap, vTexCoord + off* 1).rgb * 0.130417
               + texture2D(sDiffMap, vTexCoord + off* 2).rgb * 0.118804
               + texture2D(sDiffMap, vTexCoord + off* 3).rgb * 0.101701
               + texture2D(sDiffMap, vTexCoord + off* 4).rgb * 0.081812;
    gl_FragColor = vec4(bloom, 1.0);
#   endif
#elif defined(COMBINE)
    vec3 original = texture2D(sDiffMap, vScreenPos).rgb * cEmissiveGlowMix.x;
    vec3 bloom = texture2D(sNormalMap, vTexCoord).rgb  * cEmissiveGlowMix.y;
    // Prevent oversaturation
    //original *= max(vec3(1.0) - bloom, vec3(0.0));
    gl_FragColor = vec4(original + bloom, 1.0);
#endif
}

