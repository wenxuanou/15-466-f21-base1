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
    
    static std::mt19937 mt;     //mersenne twister pseudo-random number generator
    
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
    
    std::cout << "about to load" << std::endl;
    
    bool loaded = load_asset();
    assert(loaded);
    
    //----------Initialize floor map--------------
    //initialize floor color
    //screen is 256 * 240, floor block is 16*16, so map is 16*15
    for(int i = 0; i < floorMap_radius.x; i++){
        for(int j = 0; j < floorMap_radius.y; j++){
            floorMap[i][j] = 1 - 2 * ((i+j) % 2);      //make evey even/odd block change color
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
        int i = (int)floor(rocks[count].x / 16.0f);      //convert to floormap index
        int j = (int)floor(rocks[count].y / 16.0f);
        floorMap[i][j] = 0;
    }
}

DogMode::~DogMode(){
    
}

bool DogMode::load_asset(){
    //return false if failed to load
    
    //load tiles
    assert(tile_paths.size() <= ppu.tile_table.size());                        //ensure enough room to load tiles
    assert(palette_paths.size() <= ppu.palette_table.size());                  //ensure enough room to read palette
    
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    for(int count = 0; count < tile_paths.size(); count++){
        std::filebuf fb_tile;
        std::string magic;
        std::vector< uint8_t > tile;
        
//        std::cout << "tile path: " << data_path(tile_paths[count]) << std::endl;    //TODO: for debug
        
        //select magic number
        switch(count){
            case 0: magic = "cath"; break;
            case 1: magic = "catt"; break;
            case 2: magic = "dogh"; break;
            case 3: magic = "dogt"; break;
            case 4: magic = "roc1"; break;
            case 5: magic = "roc2"; break;
            case 6: magic = "gf11"; break;
            case 7: magic = "gf12"; break;
            case 8: magic = "gf21"; break;
            case 9: magic = "gf22"; break;
            case 10: magic = "pf11"; break;
            case 11: magic = "pf12"; break;
            case 12: magic = "pf21"; break;
            case 13: magic = "pf22"; break;
        }
        
        
        fb_tile.open(data_path(tile_paths[count]), std::ios::in | std::ios::binary);      //TODO: check if need to change to binary mode
        std::istream is_tile(&fb_tile);
        
        read_chunk(is_tile, magic, &tile);                                     //tile: 16 8-bit number, include bit 0 and bit 1
        
        std::cout << "tile: " << std::endl;
        for(uint8_t i = 0; i < 8; i++){
            ppu.tile_table[count].bit0[i] = tile[i];
            ppu.tile_table[count].bit1[i] = tile[i + 8];
            std::cout << std::bitset<8>(tile[i + 8]) << " "<< std::bitset<8>(tile[i]) << std::endl;
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
        
//        std::cout << "pallete path: " << data_path(palette_paths[count]) << std::endl;    //TODO: for debug

        //select magic number
        switch(count){
            case 0: magic = "cath"; break;
            case 1: magic = "dogh"; break;
            case 2: magic = "roc1"; break;
            case 3: magic = "gf11"; break;
            case 4: magic = "pf11"; break;
        }
        
        fb_palette.open(data_path(palette_paths[count]), std::ios::in | std::ios::binary);
        std::istream is_palette(&fb_palette);
        
        read_chunk(is_palette, magic, &palette);                              //palette: 4 
        
        // tile bit is selecting color from the 4 stored in palette table element
//        ppu.palette_table[count] = {
//            glm::u8vec4(0x00, 0x00, 0x00, 0x00),
//            palette[0],
//            palette[1],
//            glm::u8vec4(0x00, 0x00, 0x00, 0x00)
//        };
        
        ppu.palette_table[count][0] = glm::u8vec4(0x00, 0x00, 0x00, 0x00);
        ppu.palette_table[count][1] = palette[0];               //when only bit 0 is 1: 01
        ppu.palette_table[count][2] = palette[1];               //when only bit 1 is 1: 10
        ppu.palette_table[count][3] = glm::u8vec4(0x00, 0x00, 0x00, 0x00);
        
        fb_palette.close();
    }
    
    return true;    //TODO: add a load fail detection
}


bool DogMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size){
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
    
//    //TODO: handle game over event here
//    if(hitCat){
//        //TODO: print something
//        return false;
//    }
//
//    if(reach_end){
//        //TODO: print something
//        return false;
//    }
    
    return false;
}

void DogMode::update(float elapsed){
    static std::mt19937 mt;     //mersenne twister pseudo-random number generator
    
    //--------game character movement--------
    constexpr float MoveStep = 8.0f;        // 1.0f is 1 pixel //TODO: need to determine step size to fit floor block
//    constexpr float AnimationTime = 2.0f;      // TODO: produce moving animation in time interval, may equal to move_cd
    
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
        glm::vec2 min = glm::max(rocks[i] - rock_radius, player_at - player_radius);
        glm::vec2 max = glm::min(rocks[i] + rock_radius, player_at + player_radius);
        
        //if no overlap, no collision:
        if (min.x > max.x || min.y > max.y) continue;
        
        if (max.x - min.x > max.y - min.y) {
            //wider overlap in x => restrict movement in y direction:
            if (player_at.y > rocks[i].y) {
                //player at top
                player_at.y = rocks[i].y + rock_radius.y + player_radius.y;
            } else {
                //player at bottom
                player_at.y = rocks[i].y - rock_radius.y - player_radius.y;
            }
            
        }else{
            //wider overlap in y => restrict movement in x direction:
            if (player_at.x > rocks[i].x) {
                //player at right
                player_at.x = rocks[i].x + rock_radius.x + player_radius.x;
            } else {
                //player at left
                player_at.x = rocks[i].x - rock_radius.x - player_radius.x;
            }
        }
    }
    
    //cat vs rock:
    for(int i = 0; i < rock_num; i++){
        for(int j = 0; j < cat_num; j++){
            //compute area of overlap:
            glm::vec2 min = glm::max(rocks[i] - rock_radius, cats[j] - cat_radius);
            glm::vec2 max = glm::min(rocks[i] + rock_radius, cats[j] + cat_radius);
            
            //if no overlap, no collision:
            if (min.x > max.x || min.y > max.y) continue;
            
            if (max.x - min.x > max.y - min.y) {
                //wider overlap in x => restrict movement in y direction:
                if (cats[j].y > rocks[i].y) {
                    //player at top
                    cats[j].y = rocks[i].y + rock_radius.y + cat_radius.y;
                } else {
                    //player at bottom
                    cats[j].y = rocks[i].y - rock_radius.y - cat_radius.y;
                }
                
            }else{
                //wider overlap in y => restrict movement in x direction:
                if (cats[j].x > rocks[i].x) {
                    //player at right
                    cats[j].x = rocks[i].x + rock_radius.x + cat_radius.x;
                } else {
                    //player at left
                    cats[j].x = rocks[i].x - rock_radius.x - cat_radius.x;
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
        for(int i = 0; i < floorMap_radius.x; i++){
            for(int j = 0; j < floorMap_radius.y; j++){
                floorMap[i][j] *= -1;   //only swap pink and green floor, rock not change
            }
        }
    }
    
}

void DogMode::draw(glm::uvec2 const &drawable_size){
    
    static std::mt19937 mt;     //mersenne twister pseudo-random number generator
    
    //--- set ppu state based on game state ---
    
    //tile:
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    //palette:
    //0: cat; 1: dog; 2: rock; 3: green; 4: pink
    
    
    //background
    ppu.background_color = glm::u8vec4(int32_t(255), int32_t(255), int32_t(255), 0xff);     //all white background
    
    for (uint32_t y = 0; y < PPU466::BackgroundHeight / 4; ++y) {
        for (uint32_t x = 0; x < PPU466::BackgroundWidth / 4; ++x) {
            //64x60 tiles,
            
            if(floorMap[x][y] == -1){
                //green
                //11
                ppu.background[x*2 + PPU466::BackgroundWidth*(y*2+1)] = 0x0306; //tile: 6; palette: 3
                //12
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*(y*2+1)] = 0x0307; //tile: 7; palette: 3
                //21
                ppu.background[x*2 + PPU466::BackgroundWidth*y*2] = 0x0308; //tile: 8; palette: 3
                //22
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*y*2] = 0x0309; //tile: 9; palette: 3
                
            }else if(floorMap[x][y] == 1){
                //pink
                //11
                ppu.background[x*2 + PPU466::BackgroundWidth*(y*2+1)] = 0x040a; //tile: 10; palette: 4
                //12
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*(y*2+1)] = 0x040b; //tile: 11; palette: 4
                //21
                ppu.background[x*2 + PPU466::BackgroundWidth*y*2] = 0x040c; //tile: 12; palette: 4
                //22
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*y*2] = 0x040d; //tile: 13; palette: 4
            }else if(floorMap[x][y] == 0){
                //rock
                //11
                ppu.background[x*2 + PPU466::BackgroundWidth*(y*2+1)] = 0x0204; //tile: 4; palette: 2
                //12
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*(y*2+1)] = 0x0205; //tile: 5; palette: 2
                //21
                ppu.background[x*2 + PPU466::BackgroundWidth*y*2] = 0x0204; //tile: 4; palette: 2
                //22
                ppu.background[(x*2+1) + PPU466::BackgroundWidth*y*2] = 0x0205; //tile: 5; palette: 2
            }
            
        }
    }
    
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
    ppu.sprites[1].attributes = 1;                          //palette index
    
    
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
    
    for (uint32_t i = 13; i < 63; ++i) {

        ppu.sprites[i].x = 0;
        ppu.sprites[i].y = 250;
        ppu.sprites[i].index = 14;
        ppu.sprites[i].attributes = 8;                      //palette index

    }
    
    //--- actually draw ---
    ppu.draw(drawable_size);
    
}
