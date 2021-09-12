//
//  DogMode.hpp
//  
//
//  Created by owen ou on 2021/9/9.
//

#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

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
    glm::vec2 player_radius = glm::vec2(8.0f, 4.0f);       //dog size: 2*1 sprite, 16*8 pixel
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
    std::vector<std::vector<int> > floorMap;             //floor map update with player input, 0: rock, -1: green, 1: rock
    glm::vec2 destination = glm::vec2(248.0f, 232.0f);       //location of destination //TODO: may need to tune
    
    //rock positions:
    const int rock_num = 10;                // number of rocks
    std::vector< glm::vec2 > rocks;       // position of obstacles
    
    
    //----- drawing handled by PPU466 -----
    PPU466 ppu;
    
    
};
