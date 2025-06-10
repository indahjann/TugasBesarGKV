#include "scenery.h"
#include "render.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ctime> 

std::vector<SceneryObject> sceneryObjects;
std::vector<Building> buildings;
GLuint buildingTexture;
GLuint rockTexture;
GLuint flowerTexture;

void initScenery() {
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Clear any existing scenery
    sceneryObjects.clear();
    buildings.clear();
    
    // Load textures
    buildingTexture = loadBMP("texture/quartz-block.bmp");
    rockTexture = loadBMP("texture/rock.bmp");
    flowerTexture = loadBMP("texture/flowers.bmp");
    
    // Define the main building area to avoid placing trees there
    float buildingMinX = -15.0f;
    float buildingMaxX = 15.0f;
    float buildingMinZ = 0.0f;
    float buildingMaxZ = 35.0f;
    
    // Define the fence boundary (slightly inside actual fence for aesthetics)
    float fenceMinX = -95.0f;
    float fenceMaxX = 95.0f;
    float fenceMinZ = -95.0f;
    float fenceMaxZ = 95.0f;
    
    // Distribute trees evenly throughout the entire fenced area
    // More trees for denser forest feel
    int numRegularTrees = 60;
    int numPineTrees = 60;
    
    // Add regular trees throughout the map
    for (int i = 0; i < numRegularTrees; i++) {
        SceneryObject tree;
        
        // Keep trying positions until we find one not in the building area
        bool validPosition = false;
        float x, z;
        
        int attempts = 0;
        const int maxAttempts = 10;
        
        while (!validPosition && attempts < maxAttempts) {
            x = fenceMinX + static_cast<float>(rand()) / RAND_MAX * (fenceMaxX - fenceMinX);
            z = fenceMinZ + static_cast<float>(rand()) / RAND_MAX * (fenceMaxZ - fenceMinZ);
            
            // Check if this position is far enough from the building
            if (x < buildingMinX || x > buildingMaxX || z < buildingMinZ || z > buildingMaxZ) {
                // Also ensure trees aren't too close to each other
                bool tooClose = false;
                for (const auto& obj : sceneryObjects) {
                    if (obj.type == SCENERY_TREE || obj.type == SCENERY_PINE_TREE) {
                        float dx = obj.position.x - x;
                        float dz = obj.position.z - z;
                        float distSquared = dx*dx + dz*dz;
                        
                        // Minimum distance between trees (squared)
                        if (distSquared < 36.0f) {  // 6.0 units minimum distance
                            tooClose = true;
                            break;
                        }
                    }
                }
                
                if (!tooClose) {
                    validPosition = true;
                }
            }
            
            attempts++;
        }
        
        if (validPosition) {
            tree.position = {x, 0.0f, z};
            tree.rotationY = static_cast<float>(rand() % 360);
            tree.scale = 0.8f + static_cast<float>(rand()) / RAND_MAX * 0.8f;  // Scale between 0.8 and 1.6
            tree.type = SCENERY_TREE;
            tree.castShadow = true;
            sceneryObjects.push_back(tree);
        }
    }
    
    // Add pine trees throughout the map for variety
    for (int i = 0; i < numPineTrees; i++) {
        SceneryObject pineTree;
        
        bool validPosition = false;
        float x, z;
        
        int attempts = 0;
        const int maxAttempts = 10;
        
        while (!validPosition && attempts < maxAttempts) {
            x = fenceMinX + static_cast<float>(rand()) / RAND_MAX * (fenceMaxX - fenceMinX);
            z = fenceMinZ + static_cast<float>(rand()) / RAND_MAX * (fenceMaxZ - fenceMinZ);
            
            // Check if this position is far enough from the building
            if (x < buildingMinX || x > buildingMaxX || z < buildingMinZ || z > buildingMaxZ) {
                // Also ensure trees aren't too close to each other
                bool tooClose = false;
                for (const auto& obj : sceneryObjects) {
                    if (obj.type == SCENERY_TREE || obj.type == SCENERY_PINE_TREE) {
                        float dx = obj.position.x - x;
                        float dz = obj.position.z - z;
                        float distSquared = dx*dx + dz*dz;
                        
                        // Minimum distance between trees (squared)
                        if (distSquared < 36.0f) {
                            tooClose = true;
                            break;
                        }
                    }
                }
                
                if (!tooClose) {
                    validPosition = true;
                }
            }
            
            attempts++;
        }
        
        if (validPosition) {
            pineTree.position = {x, 0.0f, z};
            pineTree.rotationY = static_cast<float>(rand() % 360);
            pineTree.scale = 0.7f + static_cast<float>(rand()) / RAND_MAX * 0.7f;  // Scale between 0.7 and 1.4
            pineTree.type = SCENERY_PINE_TREE;
            pineTree.castShadow = true;
            sceneryObjects.push_back(pineTree);
        }
    }
    
    // Add some bushes scattered around
    for (int i = 0; i < 80; i++) {
        SceneryObject bush;
        bush.position = {
            fenceMinX + static_cast<float>(rand()) / RAND_MAX * (fenceMaxX - fenceMinX),
            0.0f,
            fenceMinZ + static_cast<float>(rand()) / RAND_MAX * (fenceMaxZ - fenceMinZ)
        };
        
        // Check if this position is far enough from the building
        if (bush.position.x >= buildingMinX && bush.position.x <= buildingMaxX && 
            bush.position.z >= buildingMinZ && bush.position.z <= buildingMaxZ) {
            continue;  // Skip this bush if it's in the building area
        }
        
        bush.rotationY = static_cast<float>(rand() % 360);
        bush.scale = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
        bush.type = SCENERY_BUSH;
        bush.castShadow = false;
        sceneryObjects.push_back(bush);
    }
    
    // Add some rocks for additional variety
    for (int i = 0; i < 50; i++) {
        SceneryObject rock;
        rock.position = {
            fenceMinX + static_cast<float>(rand()) / RAND_MAX * (fenceMaxX - fenceMinX),
            0.0f,
            fenceMinZ + static_cast<float>(rand()) / RAND_MAX * (fenceMaxZ - fenceMinZ)
        };
        
        // Check if this position is far enough from the building
        if (rock.position.x >= buildingMinX && rock.position.x <= buildingMaxX && 
            rock.position.z >= buildingMinZ && rock.position.z <= buildingMaxZ) {
            continue;  // Skip this rock if it's in the building area
        }
        
        rock.rotationY = static_cast<float>(rand() % 360);
        rock.scale = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
        rock.type = SCENERY_ROCK;
        rock.castShadow = true;
        sceneryObjects.push_back(rock);
    }
    
    // Keep your existing lamp posts, benches, fountain, and other specific scenery items
    // Lamp posts along the path to the building
    for (int i = 0; i < 8; i++) {
        SceneryObject lamp;
        lamp.position = {(i % 2 == 0 ? -3.0f : 3.0f), 0.0f, -5.0f + i * 4.0f};
        lamp.rotationY = 0.0f;
        lamp.scale = 1.0f;
        lamp.type = SCENERY_LAMPPOST;
        lamp.castShadow = true;
        sceneryObjects.push_back(lamp);
    }
    
    // Benches near the building
    SceneryObject bench1;
    bench1.position = {-5.0f, 0.0f, 12.0f};
    bench1.rotationY = 90.0f;
    bench1.scale = 1.0f;
    bench1.type = SCENERY_BENCH;
    bench1.castShadow = true;
    sceneryObjects.push_back(bench1);
    
    SceneryObject bench2;
    bench2.position = {5.0f, 0.0f, 14.0f};
    bench2.rotationY = -90.0f;
    bench2.scale = 1.0f;
    bench2.type = SCENERY_BENCH;
    bench2.castShadow = true;
    sceneryObjects.push_back(bench2);
    
    // Fountain in front of building
    SceneryObject fountain;
    fountain.position = {0.0f, 0.0f, -5.0f};
    fountain.rotationY = 0.0f;
    fountain.scale = 2.0f;
    fountain.type = SCENERY_FOUNTAIN;
    fountain.castShadow = true;
    sceneryObjects.push_back(fountain);
    
    // Flower beds
    for (int i = 0; i < 2; i++) {
        SceneryObject flowerBed;
        flowerBed.position = {-8.0f + i * 16.0f, 0.0f, 6.0f};
        flowerBed.rotationY = 0.0f;
        flowerBed.scale = 1.0f;
        flowerBed.type = SCENERY_FLOWER_BED;
        flowerBed.castShadow = false;
        sceneryObjects.push_back(flowerBed);
    }
    
    // Distant buildings - keep these as they are
    for (int i = 0; i < 3; i++) {
        Building building;
        building.position = {-60.0f + i * 60.0f, 0.0f, -80.0f};
        building.width = 20.0f + (rand() % 10);
        building.length = 15.0f + (rand() % 8);
        building.height = 20.0f + (rand() % 15);
        building.rotationY = 0.0f;
        building.numWindows = 4;
        building.numFloors = 4;
        building.baseColor = {0.7f, 0.7f, 0.7f};
        building.roofColor = {0.4f, 0.4f, 0.4f};
        building.texture = buildingTexture;
        buildings.push_back(building);
    }
    
    // Add a few more buildings for additional variety
    for (int i = 0; i < 3; i++) {
        Building building;
        building.position = {-70.0f + i * 70.0f, 0.0f, 80.0f};
        building.width = 25.0f + (rand() % 10);
        building.length = 18.0f + (rand() % 8);
        building.height = 18.0f + (rand() % 15);
        building.rotationY = 180.0f;
        building.numWindows = 5;
        building.numFloors = 3;
        building.baseColor = {0.75f, 0.75f, 0.8f};
        building.roofColor = {0.45f, 0.45f, 0.5f};
        building.texture = buildingTexture;
        buildings.push_back(building);
    }
}

// Draw all scenery
void drawScenery() {
    // Draw all scenery objects based on their types
    for (const auto& obj : sceneryObjects) {
        glPushMatrix();
        glTranslatef(obj.position.x, obj.position.y, obj.position.z);
        glRotatef(obj.rotationY, 0.0f, 1.0f, 0.0f);
        glScalef(obj.scale, obj.scale, obj.scale);
        
        switch(obj.type) {
            case SCENERY_TREE:
                drawTree(0, 0, 0, 1.0f);
                break;
            case SCENERY_PINE_TREE:
                drawPineTree(0, 0, 0, 1.0f);
                break;
            case SCENERY_BUSH:
                drawBush(0, 0, 0, 1.0f);
                break;
            case SCENERY_ROCK:
                drawRock(0, 0, 0, 1.0f);
                break;
            case SCENERY_BENCH:
                drawBench(0, 0, 0, 0, 1.0f);
                break;
            case SCENERY_LAMPPOST:
                drawLampPost(0, 0, 0, 1.0f);
                break;
            case SCENERY_FOUNTAIN:
                drawFountain(0, 0, 0, 1.0f);
                break;
            case SCENERY_FLOWER_BED:
                drawFlowerBed(0, 0, 0, 2.0f, 1.0f, 0);
                break;
        }
        
        glPopMatrix();
    }
    
    // Draw all buildings
    for (const auto& building : buildings) {
        drawDistantBuilding(building);
    }
}

void drawTree(float x, float y, float z, float scale) {
    // Draw tree shadow first
    //drawTreeShadow(x, z, scale);
    

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    
    // Trunk
    glColor3f(0.5f, 0.35f, 0.05f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *trunk = gluNewQuadric();
    gluQuadricTexture(trunk, GL_TRUE);
    gluCylinder(trunk, 0.2, 0.15, 2.5, 12, 3);
    gluDeleteQuadric(trunk);
    glPopMatrix();
    
    // Leaves - multiple layers for fuller appearance
    glColor3f(0.1f, 0.6f, 0.1f);
    
    // Bottom layer - widest
    glPushMatrix();
    glTranslatef(0, 1.2f, 0);
    glutSolidSphere(0.9f, 16, 16);
    glPopMatrix();
    
    // Middle layer
    glPushMatrix();
    glTranslatef(0, 1.8f, 0);
    glutSolidSphere(0.7f, 16, 16);
    glPopMatrix();
    
    // Top layer
    glPushMatrix();
    glTranslatef(0, 2.3f, 0);
    glutSolidSphere(0.5f, 16, 16);
    glPopMatrix();
    
    glPopMatrix();
}

// Pine tree - different style for variety
void drawPineTree(float x, float y, float z, float scale) {
    // Draw tree shadow first
    //drawPineTreeShadow(x, z, scale);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    
    // Trunk
    glColor3f(0.45f, 0.30f, 0.05f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *trunk = gluNewQuadric();
    gluQuadricTexture(trunk, GL_TRUE);
    gluCylinder(trunk, 0.15, 0.1, 2.8, 10, 2);
    gluDeleteQuadric(trunk);
    glPopMatrix();
    
    // Pine needles - multiple cones stacked
    glColor3f(0.0f, 0.5f, 0.1f);
    
    // Bottom layer - widest
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.9f, 1.0f, 12, 4);
    glPopMatrix();
    
    // Middle layer
    glPushMatrix();
    glTranslatef(0, 1.3f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.7f, 0.9f, 12, 4);
    glPopMatrix();
    
    // Top layer
    glPushMatrix();
    glTranslatef(0, 1.8f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.5f, 0.8f, 12, 4);
    glPopMatrix();
    
    // Top layer - smallest
    glPushMatrix();
    glTranslatef(0, 2.3f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.3f, 0.6f, 12, 4);
    glPopMatrix();
    
    glPopMatrix();
}

// Bush - rounded shape with texture
void drawBush(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale * 0.7f, scale);
    
    // Main bush shape
    glColor3f(0.0f, 0.5f, 0.0f);
    
    // Multiple spheres for more detailed shape
    glutSolidSphere(0.6f, 12, 12);
    
    glPushMatrix();
    glTranslatef(0.3f, 0.1f, 0.3f);
    glutSolidSphere(0.4f, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.3f, 0.0f, 0.2f);
    glutSolidSphere(0.5f, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.2f, 0.0f, -0.3f);
    glutSolidSphere(0.4f, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.2f, 0.1f, -0.2f);
    glutSolidSphere(0.5f, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
}

// Textured rock
void drawRock(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale * 0.6f, scale);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glColor3f(0.7f, 0.7f, 0.7f);
    
    // Create irregular rock shape with multiple spheres
    glutSolidSphere(0.7f, 10, 10);
    
    glPushMatrix();
    glTranslatef(0.4f, 0.0f, 0.2f);
    glutSolidSphere(0.5f, 8, 8);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.3f, 0.1f, 0.3f);
    glutSolidSphere(0.4f, 8, 8);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Bench with wooden slats
void drawBench(float x, float y, float z, float rotationY, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotationY, 0, 1, 0);
    glScalef(scale, scale, scale);
    
    // Wood texture for bench
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    
    // Bench legs (metal)
    glColor3f(0.3f, 0.3f, 0.3f);
    
    // Left front leg
    glPushMatrix();
    glTranslatef(-0.8f, 0.25f, 0.4f);
    glScalef(0.1f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Right front leg
    glPushMatrix();
    glTranslatef(0.8f, 0.25f, 0.4f);
    glScalef(0.1f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Left back leg
    glPushMatrix();
    glTranslatef(-0.8f, 0.25f, -0.4f);
    glScalef(0.1f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Right back leg
    glPushMatrix();
    glTranslatef(0.8f, 0.25f, -0.4f);
    glScalef(0.1f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Bench seat (wooden slats)
    glColor3f(0.6f, 0.4f, 0.2f);
    
    // Draw 5 wooden slats for the seat
    for (int i = 0; i < 5; i++) {
        float slat_pos = -0.8f + i * 0.4f;
        
        glPushMatrix();
        glTranslatef(slat_pos, 0.5f, 0.0f);
        glScalef(0.2f, 0.05f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    // Bench backrest (wooden slats)
    for (int i = 0; i < 3; i++) {
        float slat_height = 0.7f + i * 0.3f;
        
        glPushMatrix();
        glTranslatef(0.0f, slat_height, -0.45f);
        glScalef(1.8f, 0.06f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Lamp post with light source
void drawLampPost(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    
    // Post (metal gray)
    glColor3f(0.4f, 0.4f, 0.4f);
    
    // Main pole
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *pole = gluNewQuadric();
    gluCylinder(pole, 0.1, 0.08, 3.0, 12, 2);
    gluDeleteQuadric(pole);
    glPopMatrix();
    
    // Lamp housing
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    
    // Lamp base
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *base = gluNewQuadric();
    gluDisk(base, 0.0, 0.2, 12, 1);
    gluDeleteQuadric(base);
    glPopMatrix();
    
    // Lamp globe
    glColor4f(0.9f, 0.9f, 0.6f, 0.8f);
    glutSolidSphere(0.25f, 12, 12);
    
    // Light emission effect with smaller brighter sphere
    glColor4f(1.0f, 1.0f, 0.7f, 1.0f);
    glutSolidSphere(0.15f, 12, 12);
    
    glPopMatrix();
    
    glPopMatrix();
}

// Distant building with windows
void drawDistantBuilding(const Building& building) {
    glPushMatrix();
    glTranslatef(building.position.x, building.position.y, building.position.z);
    glRotatef(building.rotationY, 0.0f, 1.0f, 0.0f);
    
    // Enable texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, building.texture);
    
    // Main building body
    glColor3f(building.baseColor.r, building.baseColor.g, building.baseColor.b);
    
    // Draw the building box
    float width = building.width;
    float length = building.length;
    float height = building.height;
    
    // Front face with windows
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width/2, 0.0f, length/2);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(width/2, 0.0f, length/2);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(width/2, height, length/2);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-width/2, height, length/2);
    glEnd();
    
    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width/2, 0.0f, -length/2);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(width/2, 0.0f, -length/2);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(width/2, height, -length/2);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-width/2, height, -length/2);
    glEnd();
    
    // Left face
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width/2, 0.0f, -length/2);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(-width/2, 0.0f, length/2);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(-width/2, height, length/2);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-width/2, height, -length/2);
    glEnd();
    
    // Right face
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width/2, 0.0f, -length/2);
    glTexCoord2f(3.0f, 0.0f); glVertex3f(width/2, 0.0f, length/2);
    glTexCoord2f(3.0f, 3.0f); glVertex3f(width/2, height, length/2);
    glTexCoord2f(0.0f, 3.0f); glVertex3f(width/2, height, -length/2);
    glEnd();
    
    // Roof with different color
    glDisable(GL_TEXTURE_2D);
    glColor3f(building.roofColor.r, building.roofColor.g, building.roofColor.b);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-width/2, height, -length/2);
    glVertex3f(-width/2, height, length/2);
    glVertex3f(width/2, height, length/2);
    glVertex3f(width/2, height, -length/2);
    glEnd();
    
    // Draw windows
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.9f, 0.9f, 0.6f);
    
    int numWindows = building.numWindows;
    int numFloors = building.numFloors;
    float windowWidth = 0.5f;
    float windowHeight = 0.8f;
    
    // Only draw detailed windows if building is close enough
    float distX = building.position.x - player.position.x;
    float distZ = building.position.z - player.position.z;
    float distance = sqrt(distX*distX + distZ*distZ);
    
    if (distance < 150.0f) {
        for (int floor = 0; floor < numFloors; floor++) {
            for (int win = 0; win < numWindows; win++) {
                // Front face windows
                float winX = -width/2 + width/(numWindows+1) * (win+1);
                float winY = height/(numFloors+1) * (floor+1);
                
                glBegin(GL_QUADS);
                glVertex3f(winX - windowWidth/2, winY - windowHeight/2, length/2 + 0.01f);
                glVertex3f(winX + windowWidth/2, winY - windowHeight/2, length/2 + 0.01f);
                glVertex3f(winX + windowWidth/2, winY + windowHeight/2, length/2 + 0.01f);
                glVertex3f(winX - windowWidth/2, winY + windowHeight/2, length/2 + 0.01f);
                glEnd();
            }
        }
    }
    
    glPopMatrix();
}

// Fountain with water animation
void drawFountain(float x, float y, float z, float scale) {
    static float waterAnimation = 0.0f;
    waterAnimation += 0.01f;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    
    // Base
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glScalef(3.0f, 0.2f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Outer ring
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *ring = gluNewQuadric();
    gluCylinder(ring, 1.4, 1.5, 0.2, 24, 1);
    gluDeleteQuadric(ring);
    glPopMatrix();
    
    // Inner pool
    glColor3f(0.2f, 0.4f, 0.8f); // Water blue
    glPushMatrix();
    glTranslatef(0.0f, 0.37f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *pool = gluNewQuadric();
    gluDisk(pool, 0.0, 1.5, 24, 1);
    gluDeleteQuadric(pool);
    glPopMatrix();
    
    // Central pillar
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *pillar = gluNewQuadric();
    gluCylinder(pillar, 0.2, 0.2, 0.5, 12, 2);
    gluDeleteQuadric(pillar);
    glPopMatrix();
    
    // Top basin
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadricObj *basin = gluNewQuadric();
    gluDisk(basin, 0.0, 0.4, 16, 1);
    gluDeleteQuadric(basin);
    glPopMatrix();
    
    // Water particles/spray
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f, 0.8f, 1.0f, 0.6f);
    
    // Generate water droplets in a circular pattern
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f;
        float rad = angle * M_PI / 180.0f;
        float height = 0.2f + 0.1f * sin(waterAnimation + i);
        
        glPushMatrix();
        glTranslatef(0.3f * sin(rad), 0.5f + height, 0.3f * cos(rad));
        glutSolidSphere(0.05f, 8, 8);
        glPopMatrix();
    }
    
    // Central water jet
    glPushMatrix();
    glTranslatef(0.0f, 0.8f + 0.2f * sin(waterAnimation * 2), 0.0f);
    glutSolidSphere(0.08f, 10, 10);
    glPopMatrix();
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

// Flower bed with various colored flowers
void drawFlowerBed(float x, float y, float z, float width, float length, float rotationY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotationY, 0, 1, 0);
    
    // Soil/dirt area
    glColor3f(0.3f, 0.2f, 0.1f);
    glPushMatrix();
    glScalef(width, 0.1f, length);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Border stones
    glColor3f(0.6f, 0.6f, 0.6f);
    
    // Four sides of border stones
    float halfWidth = width / 2;
    float halfLength = length / 2;
    
    for (int i = 0; i < 8; i++) {
        // Front edge
        glPushMatrix();
        glTranslatef(-halfWidth + i * width/7, 0.05f, halfLength);
        glScalef(0.15f, 0.12f, 0.15f);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Back edge
        glPushMatrix();
        glTranslatef(-halfWidth + i * width/7, 0.05f, -halfLength);
        glScalef(0.15f, 0.12f, 0.15f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    for (int i = 0; i < 4; i++) {
        // Left edge
        glPushMatrix();
        glTranslatef(-halfWidth, 0.05f, -halfLength + i * length/3);
        glScalef(0.15f, 0.12f, 0.15f);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Right edge
        glPushMatrix();
        glTranslatef(halfWidth, 0.05f, -halfLength + i * length/3);
        glScalef(0.15f, 0.12f, 0.15f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    // Enable texture for flowers
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    
    // Random flowers
    srand(x + z); // Use position as seed for consistent random pattern
    
    for (int i = 0; i < 20; i++) {
        float posX = ((float)rand() / RAND_MAX) * width - width/2;
        float posZ = ((float)rand() / RAND_MAX) * length - length/2;
        float height = 0.3f + ((float)rand() / RAND_MAX) * 0.15f;
        
        // Randomize flower colors
        float r = 0.1f + ((float)rand() / RAND_MAX) * 0.9f;
        float g = 0.1f + ((float)rand() / RAND_MAX) * 0.2f;
        float b = 0.1f + ((float)rand() / RAND_MAX) * 0.2;
        
        // Stem
        glColor3f(0.1f, 0.5f, 0.1f);
        glPushMatrix();
        glTranslatef(posX, 0.1f, posZ);
        glRotatef(-90, 1, 0, 0);
        GLUquadricObj *stem = gluNewQuadric();
        gluCylinder(stem, 0.03, 0.01, height, 6, 1);
        gluDeleteQuadric(stem);
        glPopMatrix();
        
        // Flower
        glColor3f(r, g, b);
        glPushMatrix();
        glTranslatef(posX, 0.1f + height, posZ);
        glutSolidSphere(0.18f, 8, 8);
        glPopMatrix();
    }
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}