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
    
    bool loaded = load_asset();
    assert(loaded);
    
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

bool DogMode::load_asset(){
    //load tiles
    assert(tile_paths.size() <= ppu.tile_table.size());                        //ensure enough room to load tiles
    assert(palette_paths.size() <= ppu.palette_table.size());                  //ensure enough room to read palette
    
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    for(int count = 0; count < tile_paths.size(); count++){
        std::filebuf fb_tile;
        std::string magic;
        std::vector< uint8_t > tile;
        
        fb_tile.open(tile_paths[count], std::ios::in | std::ios::binary);      //TODO: check if need to change to binary mode
        std::istream is_tile(&fb_tile);
        
        read_chunk(is_tile, magic, &tile);                                     //tile: 16 8-bit number, include bit 0 and bit 1
        
        for(uint8_t i = 0; i < 8; i++){
            ppu.tile_table[count].bit0[i] = tile[i];
            ppu.tile_table[count].bit1[i] = tile[i + 8];
        }
        
        //check size
        assert(ppu.tile_table[count].bit0.size() == ppu.tile_table[count].bit1.size() && ppu.tile_table[0].bit0.size() == 8);
        
        fb_tile.close();
    }
    
    //load palette
    //0: cat; 1: dog; 2: rock; 3: green; 4: pink
    for(int count = 0; count < palette_paths.size(); count++){
        std::filebuf fb_palette;
        std::string magic;
        std::vector< glm::u8vec4 > palette;
        
        fb_palette.open(palette_paths[count], std::ios::in | std::ios::binary);
        std::istream is_palette(&fb_palette);
        
        read_chunk(is_palette, magic, &palette);                              //palette: 4 
        
        // tile bit is selecting color from the 4 stored in palette table element
        ppu.palette_table[count][1] = palette[0];           //when only bit 0 is 1: 01
        ppu.palette_table[count][2] = palette[1];           //when only bit 1 is 1: 10
        
        fb_palette.close();
    }
    
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
    
    //tile:
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    //palette:
    //0: cat; 1: dog; 2: rock; 3: green; 4: pink
    
    
    //background
    ppu.background_color = glm::u8vec4(0xff, 0xff, 0xff, 0xff);     //all white background
    
    
    //player sprite
    //head
    ppu.sprites[0].x = int32_t(player_at.x);
    ppu.sprites[0].y = int32_t(player_at.y);
    ppu.sprites[0].index = 2;                               //tile table index
    ppu.sprites[0].attributes = 1;                          //palette table index
    //tail
    ppu.sprites[1].x = int32_t(player_at.x + 8.0f);         //tail right shift 8 pixel. tail at the right
    ppu.sprites[1].y = int32_t(player_at.y);
    ppu.sprites[1].index = 3;
    ppu.sprites[1].attributes = 1;
    
    //cat sprite, cat_num: 5
    for(int i = 0; i < cat_num; i++){
        //head
        ppu.sprites[i + 2].x = int32_t(cats[i].x + 8.0f);   //head right shift 8 pixel, head at the right
        ppu.sprites[i + 2].y = int32_t(cats[i].y);
        ppu.sprites[i + 2].index = 0;                      //tile table index
        ppu.sprites[i + 2].attributes = 0;                 //palette table index
        //tail
        ppu.sprites[i*2 + 2].x = int32_t(cats[i].x);
        ppu.sprites[i*2 + 2].y = int32_t(cats[i].y);
        ppu.sprites[i*2 + 2].index = 1;                      //tile table index
        ppu.sprites[i*2+ 2].attributes = 0;                 //palette table index
    }
    //64 - 5*2 - 2 = 52 sprites left, used 12 sprites
    
    
    //rock, rock_num: 3
    for(int i = 0; i < rock_num; i++){
        //11
        ppu.sprites[i + 12].x = int32_t(rocks[i].x);   //head right shift 8 pixel, head at the right
        ppu.sprites[i + 12].y = int32_t(rocks[i].y);
        ppu.sprites[i + 12].index = 4;                      //tile table index
        ppu.sprites[i + 12].attributes = 2;                 //palette table index
        //12
        ppu.sprites[i*2 + 12].x = int32_t(rocks[i].x + 8.0f);   //head right shift 8 pixel, head at the right
        ppu.sprites[i*2 + 12].y = int32_t(rocks[i].y);
        ppu.sprites[i*2 + 12].index = 5;                      //tile table index
        ppu.sprites[i*2 + 12].attributes = 2;                 //palette table index
        //21
        ppu.sprites[i*3 + 12].x = int32_t(rocks[i].x);   //head right shift 8 pixel, head at the right
        ppu.sprites[i*3 + 12].y = int32_t(rocks[i].y + 8.0f);
        ppu.sprites[i*3 + 12].index = 4;                      //tile table index
        ppu.sprites[i*3 + 12].attributes = 2;                 //palette table index
        //22
        ppu.sprites[i*4 + 12].x = int32_t(rocks[i].x + 8.0f);   //head right shift 8 pixel, head at the right
        ppu.sprites[i*4 + 12].y = int32_t(rocks[i].y + 8.0f);
        ppu.sprites[i*4 + 12].index = 5;                      //tile table index
        ppu.sprites[i*4 + 12].attributes = 2;                 //palette table index
    }
    //52 - 3*4 = 40 sprites left, used 24 sprites
    
    
    //floor, not enough sprites
    int count = 0;
    while(count < 10){
        float x = (mt() / float(mt.max())) * 240.0f;
        float y = (mt() / float(mt.max())) * 224.0f;
        
        int i = floor(x / 16);
        int j = floor(y / 16);
        
        
        if(floorMap[i][j] == -1){
            //green
            
            //11
            ppu.sprites[count + 24].x = int32_t(x);   //head right shift 8 pixel, head at the right
            ppu.sprites[count + 24].y = int32_t(y);
            ppu.sprites[count + 24].index = 6;                      //tile table index
            ppu.sprites[count + 24].attributes = 3;                 //palette table index
            //12
            ppu.sprites[count*2 + 24].x = int32_t(x + 8.0f);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*2 + 24].y = int32_t(y);
            ppu.sprites[count*2 + 24].index = 7;                      //tile table index
            ppu.sprites[count*2 + 24].attributes = 3;                 //palette table index
            //21
            ppu.sprites[count*3 + 24].x = int32_t(x);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*3 + 24].y = int32_t(y + 8.0f);
            ppu.sprites[count*3 + 24].index = 8;                      //tile table index
            ppu.sprites[count*3 + 24].attributes = 3;                 //palette table index
            //22
            ppu.sprites[count*4 + 24].x = int32_t(x + 8.0f);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*4 + 24].y = int32_t(y + 8.0f);
            ppu.sprites[count*4 + 24].index = 9;                      //tile table index
            ppu.sprites[count*4 + 24].attributes = 3;                 //palette table index
            
        }else if(floorMap[i][j] == 1){
            //pink
            
            //11
            ppu.sprites[count + 24].x = int32_t(x);   //head right shift 8 pixel, head at the right
            ppu.sprites[count + 24].y = int32_t(y);
            ppu.sprites[count + 24].index = 10;                      //tile table index
            ppu.sprites[count + 24].attributes = 4;                 //palette table index
            //12
            ppu.sprites[count*2 + 24].x = int32_t(x + 8.0f);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*2 + 24].y = int32_t(y);
            ppu.sprites[count*2 + 24].index = 11;                      //tile table index
            ppu.sprites[count*2 + 24].attributes = 4;                 //palette table index
            //21
            ppu.sprites[count*3 + 24].x = int32_t(x);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*3 + 24].y = int32_t(y + 8.0f);
            ppu.sprites[count*3 + 24].index = 12;                      //tile table index
            ppu.sprites[count*3 + 24].attributes = 4;                 //palette table index
            //22
            ppu.sprites[count*4 + 24].x = int32_t(x + 8.0f);   //head right shift 8 pixel, head at the right
            ppu.sprites[count*4 + 24].y = int32_t(y + 8.0f);
            ppu.sprites[count*4 + 24].index = 13;                      //tile table index
            ppu.sprites[count*4 + 24].attributes = 4;                 //palette table index
            
        }
        
        count++;
    }
    

    
    //--- actually draw ---
    ppu.draw(drawable_size);
    
}
