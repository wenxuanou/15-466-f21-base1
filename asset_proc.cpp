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
#include "read_write_chunk.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

int main(){
    //--------------read path-----------------
    std::vector<std::string> png_paths = {"assets/cat_head.png",
                                            "assets/cat_tail.png",
                                            "assets/dog_head.png",
                                            "assets/dog_tail.png",
                                            "assets/rock_1.png",
                                            "assets/rock_2.png",
                                            "assets/greenfloor_1-1.png",
                                            "assets/greenfloor_1-2.png",
                                            "assets/greenfloor_2-1.png",
                                            "assets/greenfloor_2-2.png",
                                            "assets/pinkfloor_1-1.png",
                                            "assets/pinkfloor_1-2.png",
                                            "assets/pinkfloor_2-1.png",
                                            "assets/pinkfloor_2-2.png"};
    
    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    
    //make absolute path
    for(int i = 0; i < png_paths.size(); i++){
        png_paths[i] = data_path(png_paths[i]);
    }
    
    //--------------write path-----------------
    const std::string tile_table_path = data_path("assets/patterns.tile");
    const std::string palette_table_path = data_path("assets/colors.palette");
    
    std::vector<std::string> tile_paths = {"assets/cat_head.tile",
                                            "assets/cat_tail.tile",
                                            "assets/dog_head.tile",
                                            "assets/dog_tail.tile",
                                            "assets/rock_1.tile",
                                            "assets/rock_2.tile",
                                            "assets/greenfloor_1-1.tile",
                                            "assets/greenfloor_1-2.tile",
                                            "assets/greenfloor_2-1.tile",
                                            "assets/greenfloor_2-2.tile",
                                            "assets/pinkfloor_1-1.tile",
                                            "assets/pinkfloor_1-2.tile",
                                            "assets/pinkfloor_2-1.tile",
                                            "assets/pinkfloor_2-2.tile" };
    std::vector<std::string> palette_paths = {"assets/cat_head.palette",
                                                "assets/cat_tail.palette",
                                                "assets/dog_head.palette",
                                                "assets/dog_tail.palette",
                                                "assets/rock_1.palette",
                                                "assets/rock_2.palette",
                                                "assets/greenfloor_1-1.palette",
                                                "assets/greenfloor_1-2.palette",
                                                "assets/greenfloor_2-1.palette",
                                                "assets/greenfloor_2-2.palette",
                                                "assets/pinkfloor_1-1.palette",
                                                "assets/pinkfloor_1-2.palette",
                                                "assets/pinkfloor_2-1.palette",
                                                "assets/pinkfloor_2-2.palette"};

    //0~1: cat; 2~3: dog; 4~5: rock; 6~9: greenfloor; 10~13: pink floor
    
    //ensure size match
    assert(tile_paths.size() == palette_paths.size() && png_paths.size() == tile_paths.size());
    //make absolute path
    for(int i = 0; i < tile_paths.size(); i++){
        tile_paths[i] = data_path(tile_paths[i]);
        palette_paths[i] = data_path(palette_paths[i]);
    }
    

    
    //--------------load png--------------------
    std::filebuf fb_tile, fb_palette;
    for(int i = 0; i < png_paths.size(); i++){
        glm::uvec2 imgSize;
        std::vector< glm::u8vec4 > imgData;
        load_png(png_paths[i], &imgSize, &imgData, UpperLeftOrigin);    //imgData:
        
        std::cout << png_paths[i] << " size: (" << imgSize.x << ", " << imgSize.y << ")"<< std::endl;
        
        
        //separate tile and palette
        //TODO: figure out how to convert png to tile and palette
        std::vector< uint8_t > tile;                //8 8-bit integer, one hot coded, 8*8 tile mask
        std::vector< glm::u8vec4 > palettel;         //4 u8vec4 vector, RGBA in hex
        
        
        
//        //ensure size correct
//        assert(tile.size() == 8);
//        assert(palette.size() == 4);
        
//        //save to disk
//        fb_tile.open(tile_paths[i], std::ios::binary);      //in binary mode
//        fb_palette.open(palette_paths[i], std::ios::binary);
//        std::ostream os_tile(&fb_tile);
//        std::ostream os_palette(&fb_palette);
//
//        write_chunk< uint8_t >(std::string const &magic, stile, &os_tile);
//        write_chunk< glm::u8vec4 >(std::string const &magic, std::vector< T > const &from, &os_palette);
//
//        fb_tile.close();
//        fb_palette.close();
    }
    
    
    
    
    //TODO: separate pattern and color
    
    //TODO: use loop for something, maybe consider divide tile in game instead of right here
    
    
    
    return 0;
}
