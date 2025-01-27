//
//  DogMode.hpp
//  
//
//  Created by owen ou on 2021/9/9.
//

#include "PPU466.hpp"
#include "Mode.hpp"
#include "read_write_chunk.hpp"
#include "data_path.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <fstream>
#include <iostream>

struct DogMode : Mode {
    DogMode();
    virtual ~DogMode();
    
    //functions called by main loop:
    virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
    virtual void update(float elapsed) override;
    virtual void draw(glm::uvec2 const &drawable_size) override;
    
    //----- game state -----

    //input tracking:
    struct Button {
        uint8_t downs = 0;
        uint8_t pressed = 0;
    } left, right, down, up;
    
    //PPU screen is 256x240 with (0,0) at lower left
    glm::vec2 scene_radius = glm::vec2(256.0f, 240.0f);        //shape of scene //TODO: need to figure out exact size in pixel
    glm::vec2 player_radius = glm::vec2(0.0f, 0.0f);       //dog size: 2*1 sprite, 16*8 pixel
    glm::vec2 cat_radius = glm::vec2(8.0f, 4.0f);          //cat size: 2*1 sprite, 16*8 pixel
    glm::vec2 rock_radius = glm::vec2(8.0f, 8.0f);         //rock size: 2*2 sprite, 16*16 pixel
    glm::vec2 floor_radius = glm::vec2(8.0f, 8.0f);        //floor size: 2*2 sprite, 16*16 pixel
    

    //player state:
    glm::vec2 player_at = glm::vec2(2.0f, 2.0f);    //location of dog head // TODO: set player initial position
    //std::string floor_color = "pink";               //floor color under player, pink or green //TODO: not used
    bool hitCat = false;                            //lose if hit cat
    bool reach_end = false;                         //whether reach destination
    
    float move_cd = 2.0f;            //move cool down
    float move_tick = 0.0f;          //time elapse since last move
    
    //cat state:
    const int cat_num = 5;                  //number of cats
    std::vector< glm::vec2 > cats;          //cat positions
    
    //floor state:
    const glm::vec2 floorMap_radius = glm::vec2(16.0f, 15.0f);  //16*15 floor blocks to fill up screen
    int floorMap[16][15];            //floor map update with player input, 0: rock, -1: green, 1: pink
    glm::vec2 destination = glm::vec2(248.0f, 232.0f);       //location of destination //TODO: may need to tune
    
    //rock positions:
    const int rock_num = 3;                //number of rocks
    std::vector< glm::vec2 > rocks;         //position of obstacles
    
    
    //----- drawing handled by PPU466 -----
    PPU466 ppu;
    
    
    //----- asset loading ---------
    //TODO: implement this part
    bool load_asset();          //false if fail to load
    
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    const std::vector<std::string> tile_paths = {"../asset_pip/assets/cat_head.tile",
                                                "../asset_pip/assets/cat_tail.tile",
                                                "../asset_pip/assets/dog_head.tile",
                                                "../asset_pip/assets/dog_tail.tile",
                                                "../asset_pip/assets/rock_1.tile",
                                                "../asset_pip/assets/rock_2.tile",
                                                "../asset_pip/assets/greenfloor_1-1.tile",
                                                "../asset_pip/assets/greenfloor_1-2.tile",
                                                "../asset_pip/assets/greenfloor_2-1.tile",
                                                "../asset_pip/assets/greenfloor_2-2.tile",
                                                "../asset_pip/assets/pinkfloor_1-1.tile",
                                                "../asset_pip/assets/pinkfloor_1-2.tile",
                                                "../asset_pip/assets/pinkfloor_2-1.tile",
                                                "../asset_pip/assets/pinkfloor_2-2.tile"
        
    };
    
    //0: cat; 1: dog; 2: rock; 3: green; 4: pink
    const std::vector<std::string> palette_paths = {"../asset_pip/assets/cat_head.palette",
                                                    "../asset_pip/assets/dog_head.palette",
                                                    "../asset_pip/assets/rock_1.palette",
                                                    "../asset_pip/assets/greenfloor_1-1.palette",
                                                    "../asset_pip/assets/pinkfloor_1-1.palette"
        
    };   //not read repeated palette
};
