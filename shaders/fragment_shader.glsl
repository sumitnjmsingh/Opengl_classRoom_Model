#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NUM_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[NUM_LIGHTS];
uniform int numActiveLights;
uniform float alpha;
uniform bool useWoodTexture;
uniform bool useCeilingTiles;
uniform bool useCeilingTileTexture;
uniform bool useFloorTiles;
uniform bool useWallTexture;
uniform bool useBoardTexture;
uniform bool useBenchTexture;
uniform bool usePodiumTexture;
uniform bool useClockTexture;
uniform sampler2D benchTexture;
uniform sampler2D ceilingTileTexture;
uniform sampler2D podiumTexture;
uniform sampler2D clockTexture;

// Procedural floor tile texture generator - Square grid tiles
float floorTiles(vec2 uv) {
    // Scale for tile size (reduced for bigger tiles)
    vec2 tileUV = uv * 12.0;  // 12x12 tiles for bigger floor tiles
    
    // Get tile grid coordinates
    vec2 tilePos = fract(tileUV);
    
    // Create grout lines (gaps between tiles)
    float groutWidth = 0.04;
    float groutX = step(tilePos.x, groutWidth) + step(1.0 - groutWidth, tilePos.x);
    float groutY = step(tilePos.y, groutWidth) + step(1.0 - groutWidth, tilePos.y);
    float grout = clamp(groutX + groutY, 0.0, 1.0);
    
    // Add slight texture variation to tiles (checkerboard-like pattern)
    float tileVariation = mod(floor(tileUV.x) + floor(tileUV.y), 2.0) * 0.05 + 0.95;
    
    // Mix tile and grout colors
    float pattern = mix(tileVariation, 0.6, grout);
    
    return pattern;
}

// Procedural ceiling tile texture generator - Voronoi-based irregular tiles
float ceilingTiles(vec2 uv) {
    // Scale for overall tile density
    vec2 scaledUV = uv * 8.0;
    
    // Voronoi diagram for irregular polygon shapes
    vec2 cellID = floor(scaledUV);
    vec2 cellUV = fract(scaledUV);
    
    float minDist = 1.0;
    vec2 closestPoint;
    
    // Find closest cell point (creates irregular polygons)
    for(int y = -1; y <= 1; y++) {
        for(int x = -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x), float(y));
            
            // Random point in each cell
            vec2 point = neighbor + vec2(
                sin(dot(cellID + neighbor, vec2(12.9898, 78.233))) * 0.5 + 0.5,
                sin(dot(cellID + neighbor, vec2(93.9898, 67.345))) * 0.5 + 0.5
            );
            
            vec2 diff = point - cellUV;
            float dist = length(diff);
            
            if(dist < minDist) {
                minDist = dist;
                closestPoint = cellID + neighbor;
            }
        }
    }
    
    // Create smooth edges for each tile
    float tileEdge = smoothstep(0.02, 0.05, minDist);
    
    // Add variation to each tile based on its ID
    float tileColor = sin(dot(closestPoint, vec2(12.9898, 78.233)) * 43758.5453) * 0.1 + 0.9;
    
    // Mix tile interior and edges (grout)
    float pattern = mix(0.75, tileColor, tileEdge);
    
    // Add border/wall around ceiling tiles (molding/trim)
    float borderWidth = 0.08;  // Width of border
    float borderEdge = 0.02;   // Smooth edge transition
    
    // Check if we're in the border region
    float distFromEdgeX = min(uv.x, 1.0 - uv.x);
    float distFromEdgeY = min(uv.y, 1.0 - uv.y);
    float distFromEdge = min(distFromEdgeX, distFromEdgeY);
    
    // Create border with smooth transition
    float border = 1.0 - smoothstep(borderWidth - borderEdge, borderWidth, distFromEdge);
    
    // Mix pattern with border (border is lighter/whiter)
    pattern = mix(pattern, 0.95, border);
    
    return pattern;
}

// Procedural wood grain texture generator - Enhanced version
float woodGrain(vec2 uv) {
    // Scale UV for better grain detail
    vec2 scaled_uv = uv * 3.0;
    
    // Create parallel wood grain lines along one axis
    float baseGrain = sin(scaled_uv.y * 25.0) * 0.5 + 0.5;
    
    // Add subtle wavy distortion to simulate natural wood curves (reduced curviness)
    float wave = sin(scaled_uv.x * 8.0 + scaled_uv.y * 2.0) * 0.08;  // Reduced from 0.3 to 0.08
    float wavyGrain = sin((scaled_uv.y + wave) * 25.0) * 0.5 + 0.5;
    
    // Create annual growth rings (circular patterns) - less prominent
    vec2 center = vec2(1.5, 1.5);
    float dist = length(scaled_uv - center);
    float rings = sin(dist * 15.0 + sin(atan(scaled_uv.y - center.y, scaled_uv.x - center.x) * 2.0)) * 0.5 + 0.5;
    
    // Add fine detail grain texture with minimal waviness
    float fineGrain = sin(scaled_uv.y * 80.0 + sin(scaled_uv.x * 40.0) * 0.15) * 0.5 + 0.5;  // Reduced distortion from 0.5 to 0.15
    
    // Create knots and imperfections
    vec2 knotPos1 = vec2(2.0, 1.0);
    vec2 knotPos2 = vec2(1.0, 2.5);
    float knot1 = smoothstep(0.3, 0.0, length(scaled_uv - knotPos1));
    float knot2 = smoothstep(0.25, 0.0, length(scaled_uv - knotPos2));
    float knots = knot1 * 0.4 + knot2 * 0.3;
    
    // Combine all patterns for realistic wood
    float wood = wavyGrain * 0.5 + rings * 0.25 + fineGrain * 0.15 + baseGrain * 0.1;
    
    // Darken knot areas
    wood = mix(wood, wood * 0.3, knots);
    
    // Add subtle noise for natural variation
    float noise = sin(scaled_uv.x * 47.3 + scaled_uv.y * 31.7) * 0.03;
    wood += noise;
    
    return clamp(wood, 0.0, 1.0);
}

// Procedural wall texture generator - Plain smooth vertical lines
float wallTexture(vec2 uv) {
    // Scale UV for vertical line spacing
    float scaled_x = uv.x * 40.0;  // Controls number of vertical lines
    
    // Create smooth vertical lines using sine wave
    float verticalLines = sin(scaled_x) * 0.5 + 0.5;
    
    // Smoothstep to make lines more subtle and smooth
    verticalLines = smoothstep(0.3, 0.7, verticalLines);
    
    // Add very subtle variation for smoothness (minimal noise)
    float subtleNoise = sin(uv.x * 100.0) * 0.02;
    
    // Combine for smooth vertical line pattern
    float pattern = verticalLines * 0.95 + 0.05 + subtleNoise;
    
    return clamp(pattern, 0.0, 1.0);
}

// Procedural green board texture - chalk dust and subtle scratches
float boardTexture(vec2 uv) {
    // Base chalkboard surface with fine grain
    float grain = sin(uv.x * 300.0) * sin(uv.y * 250.0) * 0.03 + 0.97;
    
    // Add horizontal chalk streaks (from erasing)
    float streaks = sin(uv.y * 80.0 + sin(uv.x * 20.0) * 0.5) * 0.05 + 0.95;
    
    // Add subtle vertical lines (from chalk usage)
    float verticalMarks = sin(uv.x * 150.0 + sin(uv.y * 30.0) * 0.3) * 0.02 + 0.98;
    
    // Random dust/chalk residue spots
    float dust = step(0.97, sin(uv.x * 400.0) * sin(uv.y * 380.0)) * 0.1;
    
    // Combine all elements
    float pattern = grain * streaks * verticalMarks + dust;
    
    return clamp(pattern, 0.0, 1.0);
}

void main()
{
    vec3 baseColor = material.diffuse;
    
    // Apply procedural wood texture if enabled
    if (useWoodTexture) {
        float woodPattern = woodGrain(TexCoord * 2.0);
        
        // Wood color variations - medium brown to light brown (brownish tone)
        vec3 darkWood = vec3(0.35, 0.20, 0.10);   // Medium brown
        vec3 lightWood = vec3(0.55, 0.35, 0.20);  // Light brown
        
        // Mix wood colors based on grain pattern
        vec3 woodColor = mix(darkWood, lightWood, woodPattern);
        baseColor = woodColor;
    }
    
    // Apply procedural ceiling tile texture if enabled
    if (useCeilingTiles) {
        float tilePattern = ceilingTiles(TexCoord);
        
        // Ceiling tile colors - white tiles with gray grout
        vec3 tileColor = vec3(0.95, 0.95, 0.95);   // Off-white tile
        vec3 groutColor = vec3(0.75, 0.75, 0.75);  // Light gray grout
        
        baseColor = mix(groutColor, tileColor, tilePattern);
    }
    
    // Apply ceiling tile image texture if enabled
    if (useCeilingTileTexture) {
        vec3 texColor = texture(ceilingTileTexture, TexCoord).rgb;
        // Blend texture with white to make it appear more whitish
        baseColor = mix(texColor, vec3(1.0, 1.0, 1.0), 0.4);  // 40% white blend
    }
    
    // Apply procedural floor tile texture if enabled
    if (useFloorTiles) {
        float tilePattern = floorTiles(TexCoord);
        
        // Floor tile colors - light gray tiles with darker grout
        vec3 tileColor = vec3(0.85, 0.85, 0.85);   // Light gray tile
        vec3 groutColor = vec3(0.55, 0.55, 0.55);  // Darker gray grout
        
        baseColor = mix(groutColor, tileColor, tilePattern);
    }
    
    // Apply procedural wall texture if enabled
    if (useWallTexture) {
        float wallPattern = wallTexture(TexCoord);
        
        // Wall colors - light cream/beige for painted plaster look
        vec3 baseWallColor = vec3(0.92, 0.88, 0.82);   // Light cream/beige
        vec3 darkWallColor = vec3(0.82, 0.78, 0.72);   // Slightly darker shade
        
        baseColor = mix(darkWallColor, baseWallColor, wallPattern);
    }
    
    // Apply procedural green board texture if enabled
    if (useBoardTexture) {
        float boardPattern = boardTexture(TexCoord);
        
        // Green board colors - dark blackish-green with subtle variations
        vec3 darkGreen = vec3(0.03, 0.10, 0.03);   // Very dark green
        vec3 lightGreen = vec3(0.05, 0.18, 0.05);  // Slightly lighter green
        
        baseColor = mix(darkGreen, lightGreen, boardPattern);
    }
    
    // Apply bench image texture if enabled
    if (useBenchTexture) {
        vec3 texColor = texture(benchTexture, TexCoord).rgb;
        baseColor = texColor;
    }
    
    // Apply podium image texture if enabled
    if (usePodiumTexture) {
        vec3 texColor = texture(podiumTexture, TexCoord).rgb;
        baseColor = texColor;
    }
    
    // Apply clock image texture if enabled
    if (useClockTexture) {
        vec3 texColor = texture(clockTexture, TexCoord).rgb;
        baseColor = texColor;
    }
    
    // Calculate lighting from all active lights
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 totalAmbient = vec3(0.0);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);
    
    for(int i = 0; i < numActiveLights && i < NUM_LIGHTS; i++) {
        // ambient
        totalAmbient += lights[i].ambient * baseColor;
        
        // diffuse 
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        totalDiffuse += lights[i].diffuse * (diff * baseColor);
        
        // specular
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        totalSpecular += lights[i].specular * (spec * material.specular);
    }
        
    vec3 result = totalAmbient + totalDiffuse + totalSpecular;
    FragColor = vec4(result, alpha);
}