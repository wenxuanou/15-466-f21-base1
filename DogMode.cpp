//
//  DogMode.cpp
//  
//
//  Created by owen ou on 2021/9/9.
//

#include "DogMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>


DogMode::DogMode(){
    //TODO: implement this
    {
    //---------load asset-------------
    
    }
    
}

DogMode::~DogMode(){
    
}

bool DogMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size){
    //key events
    //TODO: change strategy, limit player move one block a time
    if (evt.type == SDL_KEYDOWN && move_tick > move_cd) {
        if (evt.key.keysym.sym == SDLK_LEFT) {
            left.downs += 1;
            left.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_RIGHT) {
            right.downs += 1;
            right.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_UP) {
            up.downs += 1;
            up.pressed = true;
            return true;
        } else if (evt.key.keysym.sym == SDLK_DOWN) {
            down.downs += 1;
            down.pressed = true;
            return true;
        }
        move_tick = 0;  //reset move cd
        
    } else if (evt.type == SDL_KEYUP) {
        if (evt.key.keysym.sym == SDLK_LEFT) {
            left.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_RIGHT) {
            right.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_UP) {
            up.pressed = false;
            return true;
        } else if (evt.key.keysym.sym == SDLK_DOWN) {
            down.pressed = false;
            return true;
        }
    }
    
    //TODO: handle game over event here
    if(hitCat){
        //TODO: print something
        continue;
    }
    
    if(reach_end){
        //TODO: print something
        continue;
    }
}

void DogMode::update(float elapsed){
    static std::mt19937 mt;     //mersenne twister pseudo-random number generator
    
    //--------game character movement--------
    //TODO: restrict only move constant step a time, with cool down of floor color change
    constexpr float MoveStep = 16.0f;        // 1.0f is 1 pixel //TODO: need to determine step size to fit floor block
    constexpr float AnimationTime = 2.0f;      // TODO: produce moving animation in time interval, may equal to move_cd
    
    //player:
    if (left.pressed) player_at.x -= MoveStep;
    if (right.pressed) player_at.x += MoveStep;
    if (down.pressed) player_at.y -= MoveStep;
    if (up.pressed) player_at.y += MoveStep;
    //reset button press counters:
    left.downs = 0;
    right.downs = 0;
    up.downs = 0;
    down.downs = 0;
    //TODO: check whether is step movement
    //update timing
    move_tick += elapsed;
    
    
    //cat:
    //TODO: cat move with player action
    float choice;
    for(int i = 0; cat_num; i++){
        choice = (mt() / float(mt.max())) * 4.0f;   //random choose up, down, left, right
        if(choice <= 1.0f){
            cats[i].y += MoveStep;  //up
        }else if(choice <= 2.0f){
            cats[i].y -= MoveStep;  //down
        }else if(choice <= 3.0f){
            cats[i].x += MoveStep;  //right
        }else{
            cats[i].x -= MoveStep;  //left
        }
    }
    
    
    
    //--------collision check--------
    
    //player vs cats:
    for(int i = 0; i < cat_num; i++){
        //compute area of overlap:
        glm::vec2 min = glm::max(cats[i] - cat_radius, player_at - player_radius);
        glm::vec2 max = glm::min(cats[i] + cat_radius, player_at + player_radius);
        
        if (min.x > max.x || min.y > max.y){
            //if no overlap, no collision:
            continue;
        }else{
            // if collide, trigger game over
            hitCat = true;
        }
    }
    
    //player vs rocks:
    for(int i = 0; i < rock_num; i++){
        //compute area of overlap:
        glm::vec2 min = glm::max(rocks[i] - rocks_radius, player_at - player_radius);
        glm::vec2 max = glm::min(rocks[i] + rocks_radius, player_at + player_radius);
        
        //if no overlap, no collision:
        if (min.x > max.x || min.y > max.y) continue;
        
        if (max.x - min.x > max.y - min.y) {
            //wider overlap in x => restrict movement in y direction:
            if (player_at.y > rocks[i].y) {
                //player at top
                player_at.y = rocks[i].y + rocks_radius.y + player_radius.y;
            } else {
                //player at bottom
                player_at.y = rocks[i].y - rocks_radius.y - player_radius.y;
            }
            
        }else{
            //wider overlap in y => restrict movement in x direction:
            if (player_at.x > rocks[i].x) {
                //player at right
                player_at.x = rocks[i].x + rocks_radius.x + player_radius.x;
            } else {
                //player at left
                player_at.x = rocks[i].x - rocks_radius.x - player_radius.x;
            }
        }
    }
    
    //cat vs rock:
    for(int i = 0; i < rock_num; i++){
        for(int j = 0; j < cat_num; j++){
            //compute area of overlap:
            glm::vec2 min = glm::max(rocks[i] - rocks_radius, cats[j] - cat_radius);
            glm::vec2 max = glm::min(rocks[i] + rocks_radius, cats[j] + cat_radius);
            
            //if no overlap, no collision:
            if (min.x > max.x || min.y > max.y) continue;
            
            if (max.x - min.x > max.y - min.y) {
                //wider overlap in x => restrict movement in y direction:
                if (cats[j].y > rocks[i].y) {
                    //player at top
                    cats[j].y = rocks[i].y + rocks_radius.y + cat_radius.y;
                } else {
                    //player at bottom
                    cats[j].y = rocks[i].y - rocks_radius.y - cat_radius.y;
                }
                
            }else{
                //wider overlap in y => restrict movement in x direction:
                if (cats[j].x > rocks[i].x) {
                    //player at right
                    cats[j].x = rocks[i].x + rocks_radius.x + cat_radius.x;
                } else {
                    //player at left
                    cats[j].x = rocks[i].x - rocks_radius.x - cat_radius.x;
                }
            }
        }
    }
    
    
    //player vs scene boundary:
    if(player_at.y > scene_radius.y - player_radius.y){
        player_at.y = scene_radius.y - player_radius.y;
    }
    if(player_at.y < -scene_radius.y + player_radius.y){
        player_at.y = -scene_radius.y + player_radius.y;
    }
    if(player_at.x > scene_radius.x - player_radius.x){
        player_at.x = scene_radius.x - player_radius.x;
    }
    if(player_at.x < -scene_radius.x + player_radius.x){
        player_at.x = -scene_radius.x + player_radius.x;
    }
    
    //cat vs scene boundary:
    for(int i = 0; i < cat_num; i++){
        if(cats[i].y > scene_radius.y - cat_radius.y){
            cats[i].y = scene_radius.y - cat_radius.y;
        }
        if(cats[i].y < -scene_radius.y + cat_radius.y){
            cats[i].y = -scene_radius.y + cat_radius.y;
        }
        if(cats[i].x > scene_radius.x - cat_radius.x){
            cats[i].x = scene_radius.x - cat_radius.x;
        }
        if(cats[i].x < -scene_radius.x + cat_radius.x){
            cats[i].x = -scene_radius.x + cat_radius.x;
        }
    }
}

void DogMode::draw(glm::uvec2 const &drawable_size){
    //TODO: implement this
    
    //--- set ppu state based on game state ---
    
    //recompute tilemap
    
    
    //player sprite
    
    
    //cat sprite
    
    
    //floor
    
    
    //rock
    
    
    
    //--- actually draw ---
    ppu.draw(drawable_size);
    
}
