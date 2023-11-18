/**
* Author:  Jack Ma
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Entity.hpp"
#include <iostream>
using namespace std;

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
   for (int i = 0; i < objectCount; i++)
   {
       Entity *object = &objects[i];
       if (CheckCollision(object))
       {
           float ydist = fabs(position.y - object->position.y);
           float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
           if (velocity.y > 0) {
               position.y -= penetrationY;
               velocity.y = 0;
               collidedTop = true;
               object->collidedBottom = true;
           }
           else if (velocity.y < 0) {
               position.y += penetrationY;
               velocity.y = 0;
               collidedBottom = true;
               object->collidedTop = true;
} }
} }

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
   for (int i = 0; i < objectCount; i++)
   {
       Entity *object = &objects[i];
       if (CheckCollision(object))
       {
           float xdist = fabs(position.x - object->position.x);
           float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
           if (velocity.x > 0) {
               position.x -= penetrationX;
               velocity.x = 0;
               collidedRight = true;
               object->collidedLeft = true;
           }
           else if (velocity.x < 0) {
               position.x += penetrationX;
               velocity.x = 0;
               collidedLeft = true;
               object->collidedRight = true;
} }
} }

void Entity::CheckEnemyCollided(Entity *enemies, int enemyCount) {
    for (int i = 0; i < enemyCount; ++i) {
        if (CheckCollision(&enemies[i])) {
            if (velocity.y < 0 && enemies[i].position.y <= position.y) {
                enemies[i].isActive = false;
            }
            else {
                isActive = false;
            }
        }
    }
}

void Entity::CheckPit(Entity *platforms, int platformCount) {
    int leftLocation = -1;
    int rightLocation = -1;
    glm::vec3 sensorRight = glm::vec3(position.x + 0.6f, position.y - 0.6f, 0);
    glm::vec3 sensorLeft = glm::vec3(position.x - 0.6f, position.y - 0.6f, 0);
    for (int i = 0; i < platformCount; ++i) {
        float leftDistX = fabs(sensorLeft.x - platforms[i].position.x) - ((width + platforms[i].width) / 2.0f);
        float rightDistX = fabs(sensorRight.x - platforms[i].position.x) - ((width + platforms[i].width) / 2.0f);
        float leftDistY = fabs(sensorLeft.y - platforms[i].position.y) - ((height + platforms[i].height) / 2.0f);
        float rightDistY = fabs(sensorRight.y - platforms[i].position.y) - ((height + platforms[i].height) / 2.0f);
        
        if (leftDistX < 0 && leftDistY < 0) leftLocation = i;
        if (rightDistX < 0 && rightDistY < 0) rightLocation = i;
    }
    if (leftLocation == -1) pitLeft = true;
    if (rightLocation == -1) pitRight = true;
}

void Entity::AI(Entity *player, Entity *platforms, int platformCount) {
    switch(aiType) {
        case WALKER:
            AIWalker(platforms, platformCount);
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case JUMPER:
            AIJumper();
            break;
    }
}

void Entity::AIJumper() {
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
}

void Entity::AIWalker(Entity *platforms, int platformCount) {
    if (movement == glm::vec3(0)) {
        movement = glm::vec3(-1, 0, 0);
    }
    CheckPit(platforms, platformCount);
    if (pitLeft) {
        movement = glm::vec3(1, 0, 0);
        pitLeft = false;
    }
    if (pitRight) {
        movement = glm::vec3(-1, 0, 0);
        pitRight = false;
    }
}

void Entity::AIWaitAndGo(Entity *player) {
    switch(aiState) {
        case IDLE:
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            }
            else {
                movement = glm::vec3(1, 0, 0);
            }
            break;
            
        case JUMPING:
            break;
            
        case ATTACKING:
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *platforms, int platformCount)
{
    if (isActive == false) {
        return;
    }
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    pitLeft = false;
    pitRight = false;
    
    if (entityType == ENEMY) {
        AI(player, platforms, platformCount);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            if (animTime >= 0.25f) {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames) {
                    animIndex = 0;
                }
            }
        } 
        else {
            animIndex = 0;
        }
    }
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isActive == false) {
        return;
    }
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
