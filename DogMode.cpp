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
    if (evt.type == SDL_KEYDOWN) {
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
}

void DogMode::update(float elapsed){
    //TODO: implement this
    
    //--------collision check--------
    
    
    
    
    
    
    
    //--------player movement--------
    
    constexpr float PlayerSpeed = 30.0f;
    if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
    if (right.pressed) player_at.x += PlayerSpeed * elapsed;
    if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
    if (up.pressed) player_at.y += PlayerSpeed * elapsed;

    //reset button press counters:
    left.downs = 0;
    right.downs = 0;
    up.downs = 0;
    down.downs = 0;
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
