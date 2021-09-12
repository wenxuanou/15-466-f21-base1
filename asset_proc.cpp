//
//  asset_proc.cpp
//  
//
//  Created by owen ou on 2021/9/11.
//

// Converting png asset to tiles and palette

#include <glm/glm.hpp>

#include "data_path.hpp"
#include "load_save_png.hpp"

#include <string>
#include <vector>

int main(){
    {//--------------read path-----------------
        const std::string asset_folder = "assets/";
        //cat
        const std::string cat_head_path = data_path(asset_folder + "cat_head.png");
        const std::string cat_tail_path = data_path(asset_folder + "cat_tail.png");
        //dog
        const std::string dog_head_path = data_path(asset_folder + "dog_head.png");
        const std::string dog_tail_path = data_path(asset_folder + "dog_tail.png");
        //rock
        const std::string rock_1_path = data_path(asset_folder + "rock_1.png");
        const std::string rock_2_path = data_path(asset_folder + "rock_2.png");
        //green floor
        const std::string greenfloor_11_path = data_path(asset_folder + "greenfloor_1-1.png");
        const std::string greenfloor_12_path = data_path(asset_folder + "greenfloor_1-2.png");
        const std::string greenfloor_21_path = data_path(asset_folder + "greenfloor_2-1.png");
        const std::string greenfloor_22_path = data_path(asset_folder + "greenfloor_2-2.png");
        //pink floor
        const std::string pinkfloor_11_path = data_path(asset_folder + "pinkfloor_1-1.png");
        const std::string pinkfloor_12_path = data_path(asset_folder + "pinkfloor_1-2.png");
        const std::string pinkfloor_21_path = data_path(asset_folder + "pinkfloor_2-1.png");
        const std::string pinkfloor_22_path = data_path(asset_folder + "pinkfloor_2-2.png");
        
        //--------------write path-----------------
        const std::string tile_table_path = data_path(asset_folder + "patterns.tile");
        const std::string palette_table_path = data_path(asset_folder + "colors.palette");
    }
    
    //--------------load png--------------------
    glm::uvec2 imgSize;
    std::vector< glm::u8vec4 > imgData;
    load_png(cat_head_path, &imgSize, &imgData, UpperLeftOrigin);   //match origin location in PPU buffer
    
    //TODO: separate pattern and color
    
    //TODO: use loop for something, maybe consider divide tile in game instead of right here
    
    
    
    return 0;
}
