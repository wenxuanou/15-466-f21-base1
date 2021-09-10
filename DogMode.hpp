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


    //player state:
    glm::vec2 player_at = glm::vec2(0.0f);
    std::string floor_color = "pink";       // floor color under player, pink or green
    bool hitCat = false;                    // lose if hit cat
    bool reach_end = false;                 // whether reach destination
    
    //cat state:
    int cat_num = 5;
    std::vector< glm::vec2 > cat_at;
    
    //floor state:
    float colorChange_cd = 2.0f;            // color change cool down
    float colorChange_tick = 0.0f;          // time elapse since last color change

    
    
    
    //----- drawing handled by PPU466 -----
    PPU466 ppu;
    
    
};
