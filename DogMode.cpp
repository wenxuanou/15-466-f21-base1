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
#include <math.h>


DogMode::DogMode(){
    //---------load asset-------------
    //PPU screen: 256x240, (0,0) at lower left
    //PPU Palette table: 8 palatte; Palatte: 2-bit indexed, RGBA
    
    //PPU Tile table: 256 -- 16*16 tiles; Tile: 2-bit indexed, 8*8
    // For example, to read the color index at pixel (2,7):
    //  bit0_at_2_7 = (tile.bit0[7] >> 2) & 1;
    //  bit1_at_2_7 = (tile.bit1[7] >> 2) & 1;
    //  color_index_at_2_7 = (bit1_at_2_7 << 1) | bit0_at_2_7;
        
    //PPU backgound layer: 64x60 tiles (512 x 480 pixels), lower-left pixel can positioned anywhere
        
    //PPU sprite: positions (x,y) place the bottom-left, sprite index is an index into the tile table
    //only draw 64 sprite a time, other should move outside screen
        
    
    //----------Initialize floor map--------------
    //initialize floor color
    //screen is 256 * 240, floor block is 16*16, so map is 16*15
    for(int i = 0; i < floorMap_radius.x; i++){
        for(int j = 0; j < floorMap_radius.y; j++){
            floorMap[i][j] -= 2 * ((i+j) % 2);      //make evey even block change color
        }
    }
    
    //initialize rock position
    for(int count = 0; count < rock_num; count++){
        float i = floor( (mt() / float(mt.max())) * 240.0f);    //TODO: check if need to shift by 8 pixel
        float j = floor( (mt() / float(mt.max())) * 224.0f);
        rocks.push_back( glm::vec2(i, j) );
    }
    
    //initialize cat position
    for(int count = 0; count < cat_num; count++){
        float i = floor( (mt() / float(mt.max())) * 240.0f + 2.0f);     //little shift towards center
        float j = floor( (mt() / float(mt.max())) * 224.0f + 2.0f);
        cats.push_back( glm::vec2(i,j) );
    }
    
    //finalize floor map
    for(int count = 0; count < rock_num; count++){
        int i = (int)floor(rocks[i].x / 16.0f);      //convert to floormap index
        int j = (int)floor(rocks[i].y / 16.0f);
        floorMap[i][j] = 0;
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
    constexpr float MoveStep = 16.0f;        // 1.0f is 1 pixel //TODO: need to determine step size to fit floor block
    constexpr float AnimationTime = 2.0f;      // TODO: produce moving animation in time interval, may equal to move_cd
    
    //player:
    if (left.pressed) player_at.x -= MoveStep;
    if (right.pressed) player_at.x += MoveStep;
    if (down.pressed) player_at.y -= MoveStep;
    if (up.pressed) player_at.y += MoveStep;
    //reset button press counters:      //TODO: not use
    left.downs = 0;
    right.downs = 0;
    up.downs = 0;
    down.downs = 0;
    
    //cat:
    //cat move only when player moves
    if(left.pressed || right.pressed || down.pressed || up.pressed){
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
    }
    
    //update timing
    move_tick += elapsed;
    
    //reset press state
    left.pressed = false;
    right.pressed = false;
    down.pressed = false;
    up.pressed = false;
    
    
    
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
    
    
    //--------floor color update-----
    if (left.pressed || right.pressed || up.pressed || down.pressed){
        for(int i = 0; i < floorMap.size(); i++){
            for(int j = 0; j < floorMap[0].size(); j++){
                floorMap[i][j] *= -1;   //only swap pink and green floor, rock not change
            }
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
