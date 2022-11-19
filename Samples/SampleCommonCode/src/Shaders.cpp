// GLSL shaders
#define STRINGIFY(A) #A

// particle vertex shader
const char *particleVS = STRINGIFY(
uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform float densityThreshold = 500.0;
//uniform float densityThreshold = 0.00001;
uniform float pointShrink = 0.2;
//uniform float pointShrink = 0.002;
void main()
{
    // scale down point size based on density
    float density = gl_MultiTexCoord1.x;
    float scaledPointRadius = pointRadius * (pointShrink + smoothstep(densityThreshold, densityThreshold*2.0, density)*(1.0-pointShrink));
    //float scaledPointRadius = pointRadius;

    // calculate window-space point size
    vec4 eyeSpacePos = gl_ModelViewMatrix * gl_Vertex;
    float dist = length(eyeSpacePos.xyz);
    gl_PointSize = scaledPointRadius * (pointScale / dist);

    gl_TexCoord[0] = gl_MultiTexCoord0;                         // sprite texcoord
    gl_TexCoord[1] = vec4(eyeSpacePos.xyz, scaledPointRadius);  // eye space pos
    gl_TexCoord[2] = vec4(gl_MultiTexCoord2.xyz, density);      // velocity and density
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    if (density < densityThreshold) gl_Position.w = -1.0;	// cull particles with small density

    gl_FrontColor = gl_Color;	
}
);

// fog particle vertex shader
const char *fogParticleVS = STRINGIFY(
uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform float densityThreshold = 5.0;
//uniform float densityThreshold = 0.00001;
uniform float pointShrink = 0.1;
//uniform float pointShrink = 0.002;
void main()
{
    // scale down point size based on density
    float density = gl_MultiTexCoord1.x;
    float scaledPointRadius = pointRadius * (pointShrink + smoothstep(densityThreshold, densityThreshold * 2, density)*(1.0-pointShrink));
    //float scaledPointRadius = pointRadius;

    // calculate window-space point size
    vec4 eyeSpacePos = gl_ModelViewMatrix * gl_Vertex;
    float dist = length(eyeSpacePos.xyz);
    gl_PointSize = scaledPointRadius * (pointScale / dist);

    gl_TexCoord[0] = gl_MultiTexCoord0;                         // sprite texcoord
    gl_TexCoord[1] = vec4(eyeSpacePos.xyz, scaledPointRadius);  // eye space pos
    gl_TexCoord[2] = vec4(gl_MultiTexCoord2.xyz, density);      // velocity and density
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   // if (density < densityThreshold) gl_Position.w = -1.0;	// cull particles with small density
	
    //gl_FrontColor = gl_Color;

	//gl_FrontColor.x = 0.0;
}
);

// render particle as constant shaded disc
const char *particleDebugPS = STRINGIFY(
uniform float pointRadius;
void main()
{
    // calculate eye-space sphere normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle

//    gl_FragColor = gl_Color;
    gl_FragColor = gl_TexCoord[2].w * 0.001;  // show density
//    gl_FragColor = gl_TexCoord[2];  // show vel
}
);

// render particle as lit sphere
const char *particleSpherePS = STRINGIFY(
uniform float pointRadius;
uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
void main()
{
    // calculate eye-space sphere normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-r2);

    // calculate depth
    vec4 eyeSpacePos = vec4(gl_TexCoord[1].xyz + N*pointRadius, 1.0);   // position of this pixel on sphere in eye space
    vec4 clipSpacePos = gl_ProjectionMatrix * eyeSpacePos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;

    float diffuse = max(0.0, dot(N, lightDir));

    gl_FragColor = diffuse*gl_Color;
}
);

// visualize density
const char *particleDensityPS = STRINGIFY(
uniform float pointRadius;
uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform float densityScale;
uniform float densityOffset;
void main()
{
    // calculate eye-space sphere normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-r2);

    // calculate depth
    vec4 eyeSpacePos = vec4(gl_TexCoord[1].xyz + N*pointRadius, 1.0);   // position of this pixel on sphere in eye space
    vec4 clipSpacePos = gl_ProjectionMatrix * eyeSpacePos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;

    float diffuse = max(0.0, dot(N, lightDir));

    // calculate color based on density
    float x = 1.0 - saturate((gl_TexCoord[2].w  - densityOffset) * densityScale);
    vec3 color = lerp(gl_Color.xyz, vec3(1.0, 1.0, 1.0), x);
	 //vec3 color = lerp(gl_Color.xyz, vec3(1.0, 1.0, 1.0), 1.0);

    gl_FragColor = vec4(diffuse*color, 1.0);
}
);


// visualize density
const char *particleFogPS = STRINGIFY(
uniform float pointRadius;
uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform float densityScale;
uniform float densityOffset;
void main()
{
    // calculate eye-space sphere normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-r2);

    // calculate depth
    vec4 eyeSpacePos = vec4(gl_TexCoord[1].xyz + N*pointRadius, 1.0);   // position of this pixel on sphere in eye space
    vec4 clipSpacePos = gl_ProjectionMatrix * eyeSpacePos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;

    float diffuse = max(0.0, dot(N, lightDir));

    // calculate color based on density
    //float x = 1.0 - saturate((gl_TexCoord[2].w  - densityOffset) * densityScale);
	float x = saturate((gl_TexCoord[2].w  - densityOffset) * densityScale);
	vec3 color = lerp( gl_Color.xyz,vec3(1.0f, 1.0f, 1.0f), 1.0);
   // gl_FragColor = vec4(diffuse*color, x);
	 gl_FragColor = vec4(color, x);
	
}
);

// renders eye-space depth
const char *particleSurfacePS = STRINGIFY(
uniform float pointRadius;
void main()
{
    // calculate eye-space normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-r2);

    // calculate depth
    vec4 eyeSpacePos = vec4(gl_TexCoord[1].xyz + N*gl_TexCoord[1].w, 1.0); // position of this pixel on sphere in eye space
    vec4 clipSpacePos = gl_ProjectionMatrix * eyeSpacePos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;

    gl_FragColor = eyeSpacePos.z; // output eye-space depth
//    gl_FragColor = -eyeSpacePos.z/10.0;
}
);

// render particle thickness
const char *particleThicknessPS = STRINGIFY(
uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform float pointRadius;
uniform float faceScale = 1.0;
void main()
{
    // calculate eye-space normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-r2) * faceScale;

    // calculate depth
    vec4 eyeSpacePos = vec4(gl_TexCoord[1].xyz + N*gl_TexCoord[1].w, 1.0); // position of this pixel on sphere in eye space
    vec4 clipSpacePos = gl_ProjectionMatrix * eyeSpacePos;
    gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;

    float alpha = exp(-r2*2.0);

//    gl_FragColor = eyeSpacePos.z;     // output distance
    gl_FragColor = N.z*pointRadius*2.0*alpha; // output thickness
}
);

// motion blur shaders
const char *mblurVS = STRINGIFY(
uniform float timestep = 0.02;
void main()
{
    vec3 pos = gl_Vertex.xyz;
    vec3 vel = gl_MultiTexCoord2.xyz;
    vec3 pos2 = (pos - vel*timestep);   // previous position

    gl_Position = gl_ModelViewMatrix * vec4(pos, 1.0);     // eye space
    gl_TexCoord[0] = gl_ModelViewMatrix * vec4(pos2, 1.0);
    gl_TexCoord[1].x = gl_MultiTexCoord1.x;

    gl_FrontColor = gl_Color;
}
);

const char *mblurGS = STRINGIFY(
#version 120\n
#extension GL_EXT_geometry_shader4 : enable\n
uniform float pointRadius;  // point size in world space
uniform float densityThreshold = 500.0;
uniform float pointShrink = 0.25;
void main()
{
    gl_FrontColor = gl_FrontColorIn[0];
    float density = gl_TexCoordIn[0][1].x;

    // scale down point size based on density
//    float scale = pointShrink + smoothstep(0.0, densityThreshold, density)*(1.0-pointShrink);
    float pointSize = pointRadius;

    // eye space
    vec3 pos = gl_PositionIn[0].xyz;
    vec3 pos2 = gl_TexCoordIn[0][0].xyz;
    vec3 motion = pos - pos2;
    vec3 dir = normalize(motion);
    float len = length(motion);

    vec3 x = dir * pointSize;
    vec3 view = normalize(-pos);
    vec3 y = normalize(cross(dir, view)) * pointSize;
    float facing = dot(view, dir);

    // check for very small motion to avoid jitter
    float threshold = 0.01;
//    if (len < threshold) {
    if ((len < threshold) || (facing > 0.95) || (facing < -0.95)) {
        pos2 = pos;
        x = vec3(pointSize, 0.0, 0.0);
        y = vec3(0.0, -pointSize, 0.0);
    }
    
    if (density < densityThreshold) {

        gl_TexCoord[0] = vec4(0, 0, 0, 0);
        gl_TexCoord[2] = gl_PositionIn[0];
        gl_Position = gl_ProjectionMatrix * vec4(pos + x + y, 1);
        EmitVertex();

        gl_TexCoord[0] = vec4(0, 1, 0, 0);
        gl_Position = gl_ProjectionMatrix * vec4(pos + x - y, 1);
        EmitVertex();

        gl_TexCoord[0] = vec4(1, 0, 0, 0);
        gl_Position = gl_ProjectionMatrix * vec4(pos2 - x + y, 1);
        EmitVertex();

        gl_TexCoord[0] = vec4(1, 1, 0, 0);
        gl_Position = gl_ProjectionMatrix * vec4(pos2 - x - y, 1);
        EmitVertex();
    }
}
);

const char *particleSprayPS = STRINGIFY(
void main()
{
    // calculate eye-space normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle

    float falloff = exp(-mag*4.0);

    gl_FragColor = gl_Color*falloff;
}
);


// screen-space shaders
const char *passThruVS = STRINGIFY(
void main()
{
    gl_Position = gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
);

// blur depth map
// separable version
const char *depthBlurPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
sampler2DRect colorTex;
uniform vec2 scale = 1.0;
const float r = 10.0;
uniform float blurScale = 2.0 / r;
uniform float blurDepthFalloff = 2.0;
uniform float depthThreshold = 0.5;
void main()
{
    float center = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (center < -9999.0) {
        // skip background pixels
        discard;
        return;
    }

    float sum = 0;
    float wsum = 0;
    for(float x=-r; x<=r; x+=1.0) {
        float sample = texture2DRect(colorTex, gl_TexCoord[0].xy + x*scale).x;

        // bilateral filter
        // spatial domain
        float r = x * blurScale;
        float w = exp(-r*r);
        //float w = 1.0;

        // range domain (based on depth difference)
        float r2 = (sample - center) * blurDepthFalloff;
        //float g = 1.0;
        float g = exp(-r2*r2);
        //float g = abs(sample - center) < depthThreshold;

        sum += sample * w * g;
        wsum += w * g;
    }

    if (wsum > 0.0) {
        sum /= wsum;
    }
    gl_FragColor.x = sum;
}
);

// 2D non-separable version
const char *depthBlur2DPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
sampler2DRect colorTex;
uniform vec2 scale = 1.0;
const float r = 10.0;
uniform float blurScale = 2.0 / r;
uniform float blurDepthFalloff = 1.0;
uniform float depthThreshold = 0.1;
void main()
{
    float center = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (center < -9999.0) {
        discard;
        return;
    }

    float sum = 0;
    float wsum = 0;
    for(float y=-r; y<=r; y+=1.0) {
        for(float x=-r; x<=r; x+=1.0) {
            float sample = texture2DRect(colorTex, gl_TexCoord[0].xy + vec2(x, y)).x;

            // bilateral filter
            // spatial domain
			//float r = length(vec2(x, y)) * blurScale;
   //         float w = exp(-r*r);
            float rsq = (x*x + y*y) * blurScale * blurScale;
            float w = exp(-rsq);
            //float w = 1.0;

            // range domain (based on depth difference)
            float r2 = (sample - center) * blurDepthFalloff;
            //float g = 1.0;
            float g = exp(-r2*r2);
//            float g = abs(sample - center) < depthThreshold;

            sum += sample * w * g;
            wsum += w * g;
        }
    }

    if (wsum > 0.0) {
        sum /= wsum;
    }
    gl_FragColor.x = sum;
}
);

// symmetrical filter (see "Screen Space Meshes" paper)
const char *depthBlurSymPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
sampler2DRect colorTex;
uniform vec2 scale = 1.0;
const float r = 10.0;
uniform float blurScale = 2.0;
uniform float blurDepthFalloff = 2.0;
uniform float depthThreshold = 0.25;
void main()
{
    float center = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (center == 0.0) {
        discard; return;
    }

    float sum = center;
    float wsum = 1.0;

    for(float x=1.0; x<=r; x+=1.0) {
        float sample = texture2DRect(colorTex, gl_TexCoord[0].xy + x*scale).x;
        float sample2 = texture2DRect(colorTex, gl_TexCoord[0].xy - x*scale).x;

        bool valid = abs(sample - center) < depthThreshold;
        bool valid2 = abs(sample2 - center) < depthThreshold;

        if (valid && valid2) {
            float r = (x / r) * blurScale;
            float w = exp(-r*r);

            sum += sample * w;
            wsum += w;

            sum += sample2 * w;
            wsum += w;
        }
    }

    if (wsum > 0.0) {
        sum /= wsum;
    }
    gl_FragColor.x = sum;
}
);

// display final shaded surface
const char *displaySurfacePS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect colorTex;
uniform sampler2DRect depthTex;
uniform sampler2DRect thicknessTex;
uniform sampler2DRect sceneTex;
uniform samplerCube cubemapTex;

uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform vec2 invViewport = vec2(1.0 / 800, 1.0 / 600);
uniform float2 invFocalLen;
uniform float depthThreshold = 0.1;
uniform float shininess = 40.0;
uniform vec4 fluidColor = vec4(0.5, 0.7, 1.0, 0.0);

uniform vec4 colorFalloff = vec4(2.0, 1.0, 0.5, 1.0);
uniform vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0); 
//uniform vec4 specularColor = vec4(0.25, 0.25, 0.25, 1.0); 

uniform float falloffScale = 0.3;
//uniform float falloffScale = 0.1;
uniform vec3 thicknessRefraction = vec3(2.0, 2.3, 2.6);
uniform float subSample = 1;

uniform float fresnelBias = 0.1;
uniform float fresnelScale = 0.4;
uniform float fresnelPower = 2.0;   // 5.0 is physically correct

// convert [0,1] uv coords and eye-space Z to eye-space position
vec3 uvToEye(vec2 uv, float eyeZ)
{
    uv = uv * vec2(-2.0, -2.0) - vec2(-1.0, -1.0);
    return vec3(uv * invFocalLen * eyeZ, eyeZ);
}

vec3 getEyePos(sampler2DRect tex, vec2 texCoord)
{
    float eyeZ = texture2DRect(tex, texCoord).x;
    return uvToEye(texCoord*invViewport, eyeZ);
}

void main()
{
    float c = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (c < -9999.0) {
        discard;
        return;
    }

    // calculate normal
    // taking silohette edges into account
    vec2 uv = gl_TexCoord[0].xy * invViewport;
    vec3 eyePos = uvToEye(uv, c);

    vec3 ddx = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(1, 0)) - eyePos;
    if (abs(ddx.z) > depthThreshold) {
        ddx = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(-1, 0));
    }

    vec3 ddy = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, 1)) - eyePos;
    if (abs(ddy.z) > depthThreshold) { 
        ddy = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, -1));
    }

    vec3 n = cross(ddx, ddy);
    n = normalize(n);

    // lighting
//    float diffuse = max(0.0, dot(n, lightDir));
    float diffuse = dot(n, lightDir)*0.5+0.5; // wrap lighting

    vec3 v = normalize(-eyePos);
    vec3 h = normalize(lightDir + v);
    float specular = pow(max(0.0, dot(n, h)), shininess);

    float fresnel = fresnelBias + fresnelScale*pow(1.0 - max(0.0, dot(n, v)), fresnelPower);

    // cubemap
    vec3 r = reflect(-v, n);
    r = r * gl_NormalMatrix;
    vec4 reflectColor = textureCube(cubemapTex, r);

    // color attenuation based on thickness
    float thickness = texture2DRect(thicknessTex, gl_TexCoord[0].xy * subSample).x;
    vec4 attenuatedColor = fluidColor * exp(-thickness*falloffScale*colorFalloff);
//    vec4 attenuatedColor = exp(-thickness*falloffScale*colorFalloff);

    // refraction
    float refraction = thickness*thicknessRefraction.x;
    vec4 sceneCol = texture2DRect(sceneTex, (gl_TexCoord[0].xy * subSample) + (n.xy * refraction));
/*
    // dispersive refraction
    vec3 refraction = thickness*thicknessRefraction;
    vec4 sceneCol;
    sceneCol.r = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.x)).r;
    sceneCol.g = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.y)).g;
    sceneCol.b = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.z)).b;
    sceneCol.a = 1.0;
*/

    vec4 finalCol = vec4(attenuatedColor.xyz + reflectColor.xyz*fresnel + specularColor.xyz*specular, 1.0);
    float alpha = saturate(attenuatedColor.w);

//    gl_FragColor = -c.z;
//    gl_FragColor = vec4(n*0.5+0.5, 1.0);
//	  gl_FragColor = vec4(diffuse.xxx, 1.0);
//	  gl_FragColor = vec4(specular.xxx, 1.0);
//	  gl_FragColor = vec4(fresnel.xxx, 1.0);
//    gl_FragColor = diffuse*fluidColor + specular;
//    gl_FragColor = vec4(eyePos*0.5+0.5, 1.0);
//    gl_FragColor = thickness;
//    gl_FragColor = vec4(attenuatedColor.xyz, 1.0);
//    gl_FragColor = attenuatedColor.w;
//    gl_FragColor = alpha;

    gl_FragColor = lerp(finalCol, sceneCol, alpha);
//    gl_FragColor = sceneCol*attenuatedColor + reflectColor*fresnel + specularColor*specular;

//    gl_FragDepth = texture2DRect(depthTex, gl_TexCoord[0].xy).x;
    gl_FragDepth = texture2DRect(depthTex, gl_TexCoord[0].xy * subSample).x;
}
);

// new version (handles transparency differently)
const char *displaySurfaceNewPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect colorTex;
uniform sampler2DRect depthTex;
uniform sampler2DRect thicknessTex;
uniform sampler2DRect sceneTex;
uniform samplerCube cubemapTex;

uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform vec2 invViewport = vec2(1.0 / 800, 1.0 / 600);
uniform float2 invFocalLen;
uniform float depthThreshold = 0.1;
uniform float shininess = 40.0;
uniform vec4 fluidColor = vec4(0.5, 0.7, 1.0, 0.0);

uniform vec4 colorFalloff = vec4(2.0, 1.0, 0.5, 1.0);
uniform vec4 specularColor = vec4(0.5, 0.5, 0.5, 1.0); 

uniform float falloffScale = 0.01;
uniform vec3 thicknessRefraction = vec3(2.0, 2.1, 2.2);
uniform float subSample = 1;

uniform float fresnelBias = 0.1;
uniform float fresnelScale = 0.4;
uniform float fresnelPower = 2.0;   // 5.0 is physically correct

// convert [0,1] uv coords and eye-space Z to eye-space position
vec3 uvToEye(vec2 uv, float eyeZ)
{
    uv = uv * vec2(-2.0, -2.0) - vec2(-1.0, -1.0);
    return vec3(uv * invFocalLen * eyeZ, eyeZ);
}

vec3 getEyePos(sampler2DRect tex, vec2 texCoord)
{
    float eyeZ = texture2DRect(tex, texCoord).x;
    return uvToEye(texCoord*invViewport, eyeZ);
}

void main()
{
    float c = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (c < -9999.0) {
        discard;
        return;
    }

    // calculate normal
    // taking silohette edges into account
    vec2 uv = gl_TexCoord[0].xy * invViewport;
    vec3 eyePos = uvToEye(uv, c);

    vec3 ddx = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(1, 0)) - eyePos;
    if (abs(ddx.z) > depthThreshold) {
        ddx = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(-1, 0));
    }

    vec3 ddy = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, 1)) - eyePos;
    if (abs(ddy.z) > depthThreshold) { 
        ddy = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, -1));
    }

    vec3 n = cross(ddx, ddy);
    n = normalize(n);

    // lighting
//    float diffuse = max(0.0, dot(n, lightDir));
    float diffuse = dot(n, lightDir)*0.5+0.5; // wrap lighting

    vec3 v = normalize(-eyePos);
    vec3 h = normalize(lightDir + v);
    float specular = pow(max(0.0, dot(n, h)), shininess);

    float fresnel = fresnelBias + fresnelScale*pow(1.0 - max(0.0, dot(n, v)), fresnelPower);

    // cubemap
    vec3 r = reflect(-v, n);
    r = r * gl_NormalMatrix;
    vec4 reflectColor = textureCube(cubemapTex, r);

    // color attenuation based on thickness
    float thickness = texture2DRect(thicknessTex, gl_TexCoord[0].xy * subSample).x;
    vec4 attenuatedColor = exp(-thickness*falloffScale*colorFalloff);

    // refraction
//    float refraction = thickness*thicknessRefraction.x;
//    vec4 sceneCol = texture2DRect(sceneTex, (gl_TexCoord[0].xy * subSample) + (n.xy * refraction));

    // dispersive refraction
    vec3 refraction = thickness*thicknessRefraction;
    vec4 sceneCol;
    sceneCol.r = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.x)).r;
    sceneCol.g = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.y)).g;
    sceneCol.b = texture2DRect(sceneTex, gl_TexCoord[0].xy + (n.xy * refraction.z)).b;
    sceneCol.a = 1.0;
    
    gl_FragColor = sceneCol*attenuatedColor + reflectColor*fresnel + specularColor*specular;

    gl_FragDepth = texture2DRect(depthTex, gl_TexCoord[0].xy * subSample).x;
}
);

// non-transparent version for oil etc.
const char *displaySurfaceSolidPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect colorTex;
uniform sampler2DRect depthTex;
uniform samplerCube cubemapTex;

uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform vec2 invViewport = vec2(1.0 / 800, 1.0 / 600);
uniform float2 invFocalLen;
uniform float depthThreshold = 0.1;
uniform float shininess = 40.0;
uniform vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
uniform vec4 specularColor = vec4(0.75, 0.75, 0.75, 1.0); 

uniform float subSample = 1;

uniform float fresnelBias = 0.1;
uniform float fresnelScale = 0.4;
uniform float fresnelPower = 2.0;   // 5.0 is physically correct

// convert [0,1] uv coords and eye-space Z to eye-space position
vec3 uvToEye(vec2 uv, float eyeZ)
{
    uv = uv * vec2(-2.0, -2.0) - vec2(-1.0, -1.0);
    return vec3(uv * invFocalLen * eyeZ, eyeZ);
}

vec3 getEyePos(sampler2DRect tex, vec2 texCoord)
{
    float eyeZ = texture2DRect(tex, texCoord).x;
    return uvToEye(texCoord*invViewport, eyeZ);
}

void main()
{
    float c = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (c < -9999.0) {
        discard;
        return;
    }

    // calculate normal
    // taking silohette edges into account
    vec2 uv = gl_TexCoord[0].xy * invViewport;
    vec3 eyePos = uvToEye(uv, c);

    vec3 ddx = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(1, 0)) - eyePos;
    if (abs(ddx.z) > depthThreshold) {
        ddx = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(-1, 0));
    }

    vec3 ddy = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, 1)) - eyePos;
    if (abs(ddy.z) > depthThreshold) { 
        ddy = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, -1));
    }

    vec3 n = cross(ddx, ddy);
    n = normalize(n);

    // lighting
    float diffuse = max(0.0, dot(n, lightDir));
//    float diffuse = dot(n, lightDir)*0.5+0.5; // wrap lighting

    vec3 v = normalize(-eyePos);
    vec3 h = normalize(lightDir + v);
    float specular = pow(max(0.0, dot(n, h)), shininess);

    float fresnel = fresnelBias + fresnelScale*pow(1.0 - max(0.0, dot(n, v)), fresnelPower);

    // cubemap
    vec3 r = reflect(-v, n);
    r = r * gl_NormalMatrix;
    vec4 reflectColor = textureCube(cubemapTex, r);
    
    gl_FragColor = diffuseColor*diffuse + reflectColor*fresnel + specularColor*specular;

    gl_FragDepth = texture2DRect(depthTex, gl_TexCoord[0].xy * subSample).x;
}
);

// chrome surface using cubemap
const char *displaySurfaceChromePS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect colorTex;
uniform sampler2DRect depthTex;
uniform samplerCube cubemapTex;

uniform vec3 lightDir = vec3(0.577, 0.577, 0.577);
uniform vec2 invViewport = vec2(1.0 / 800, 1.0 / 600);
uniform float2 invFocalLen;
uniform float depthThreshold = 0.1;
uniform float shininess = 40.0;
uniform vec4 fluidColor = vec4(0.5, 0.7, 1.0, 0.0);
uniform float subSample = 1;

// convert [0,1] uv coords and eye-space Z to eye-space position
vec3 uvToEye(vec2 uv, float eyeZ)
{
    uv = uv * vec2(-2.0, -2.0) - vec2(-1.0, -1.0);
    return vec3(uv * invFocalLen * eyeZ, eyeZ);
}

vec3 getEyePos(sampler2DRect tex, vec2 texCoord)
{
    float eyeZ = texture2DRect(tex, texCoord).x;
    return uvToEye(texCoord*invViewport, eyeZ);
}

void main()
{
    float c = texture2DRect(colorTex, gl_TexCoord[0].xy).x;
    if (c < -9999.0) {
        discard;
        return;
    }

    // calculate normal
    // taking silohette edges into account
    vec2 uv = gl_TexCoord[0].xy * invViewport;
    vec3 eyePos = uvToEye(uv, c);

    vec3 ddx = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(1, 0)) - eyePos;
    vec3 ddx2 = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(-1, 0));
//    if (abs(ddx.z) > abs(ddx2.z)) {
    if (abs(ddx.z) > depthThreshold) { 
        ddx = ddx2;
    }

    vec3 ddy = getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, 1)) - eyePos;
    vec3 ddy2 = eyePos - getEyePos(colorTex, gl_TexCoord[0].xy + vec2(0, -1));
//    if (abs(ddy2.z) < abs(ddy.z)) {
    if (abs(ddy.z) > depthThreshold) { 
        ddy = ddy2;
    }

    vec3 n = cross(ddx, ddy);
    n = normalize(n);

    // lighting
    float diffuse = max(0.0, dot(n, lightDir));
//    float diffuse = dot(n, lightDir)*0.5+0.5; // wrap lighting

//    vec3 v = vec3(0, 0, 1);
    vec3 v = normalize(-eyePos);
    vec3 h = normalize(lightDir + v);
    float specular = pow(max(0.0, dot(n, h)), shininess);

//    float fresnel = pow(1.0 - max(0.0, dot(n, v)), 5.0);
    float fresnel = 0.2 + 0.8*pow(1.0 - max(0.0, dot(n, v)), 2.0);
	//float fresnel = 1.0 - max(0.0, dot(n, v));

    // cubemap
    vec3 r = reflect(-v, n);
//    r = (mat3) gl_ModelViewMatrixInverse * r;
    r = r * gl_NormalMatrix;
    vec4 reflectColor = textureCube(cubemapTex, r);

//    gl_FragColor = reflectColor * fresnel;
    gl_FragColor = reflectColor * (0.5 + 0.5*diffuse);
    gl_FragDepth = texture2DRect(depthTex, gl_TexCoord[0].xy * subSample).x;
}
);


const char *textureRectPS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect tex;

void main()
{
    gl_FragColor = texture2DRect(tex, gl_TexCoord[0].xy);
}
);

// dilate depth image by taking maximum value of neighbourhood
const char *dilatePS = STRINGIFY(
#extension GL_ARB_texture_rectangle : enable\n
uniform sampler2DRect tex;

void main()
{
    float c = texture2DRect(tex, gl_TexCoord[0].xy).x;
    if (c < -9999.0) {
        c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, 0)).x);
        c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, 1)).x);
        c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(0, 1)).x);

        //c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(-1, 0)).x);
        //c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, 0)).x);
        //c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(0, -1)).x);
        //c = max(c, texture2DRect(tex, gl_TexCoord[0].xy + vec2(0, 1)).x);
    }

    gl_FragColor = c;
}
);
