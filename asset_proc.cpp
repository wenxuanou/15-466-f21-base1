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
    
    
    //---------------outer loop-------------------
    for(int i = 0; i < png_paths.size(); i++){
        
        
        //--------------load png--------------------
        std::filebuf fb_tile, fb_palette;
        
        glm::uvec2 imgSize;
        std::vector< glm::u8vec4 > imgData;
        load_png(png_paths[i], &imgSize, &imgData, UpperLeftOrigin);    //imgData: 64*1 vector
        
        std::string magic;
        
        //construct palette
        std::vector< glm::u8vec4 > palette(4, glm::u8vec4(0x00, 0x00, 0x00, 0x00)); //4 u8vec4 vector, RGBA in hex
        //color picking
        switch(i){
            case 0:     //cat head
                palette[0] = imgData[1];    //black
                palette[1] = imgData[26];   //red
                magic = "cath";
            case 1:     //cat tail
                palette[0] = imgData[0];    //black
                palette[1] = imgData[31];   //red
                magic = "catt";
            case 2:     //dog head
                palette[0] = imgData[4];    //yellow
                palette[1] = imgData[32];   //black
                magic = "dogh";
            case 3:     //dog tail
                palette[0] = imgData[24];   //red
                palette[1] = imgData[25];   //yellow
                magic = "dogt";
            case 4:     //rock 1
                palette[0] = imgData[12];   //black
                palette[1] = imgData[20];   //grey
//                palette[2] = imgData[29];   //lighter grey        //TODO: third color
                magic = "roc1";
            case 5:     //rock 2
                palette[0] = imgData[40];   //black
                palette[1] = imgData[41];   //grey
//                palette[2] = imgData[44];   //lighter grey
                magic = "roc2";
            case 6:     //greenfloor 1-1
                palette[0] = imgData[9];    //green
                palette[1] = imgData[13];   //red
                magic = "gf11";
            case 7:     //greenfloor 1-2
                palette[0] = imgData[8];    //green
                palette[1] = imgData[40];   //red
                magic = "gf12";
            case 8:     //greenfloor 2-1
                palette[0] = imgData[1];    //green
                palette[1] = imgData[2];    //red
                magic = "gf21";
            case 9:     //greenfloor 2-2
                palette[0] = imgData[0];    //green
                palette[1] = imgData[4];    //red
                magic = "gf22";
            case 10:    //pinkfloor 1-1
                palette[0] = imgData[9];    //pink
                palette[1] = imgData[15];   //blue
                magic = "pf11";
            case 11:    //pinkfloor 1-2
                palette[0] = imgData[9];    //pink
                palette[1] = imgData[8];    //blue
                magic = "pf12";
            case 12:    //pinkfloor 2-1
                palette[0] = imgData[1];    //pink
                palette[1] = imgData[2];    //blue
                magic = "pf21";
            case 13:    //pinkfloor 2-2
                palette[0] = imgData[0];    //pink
                palette[1] = imgData[5];    //blue
                magic = "pf22";
        }
        
        //construct tile
        //16 8-bit integer, one hot coded, 8*8 tile mask
        //0~8 bit zeros mask, 0~16 bit one mask
        std::vector< uint8_t > tile(16, 0b00000000);
        for(uint32_t i = 0; i < 8; i++){                    //i is row, j is col
            for(uint32_t j = 0; j < 8; j++){
                if(imgData[i * 8 + j].w != 0){
                    //if not transparent
                    if(imgData[i * 8 + j] == palette[0]){
                        tile[i] |= (1 << j);
                    }else{
                        tile[i + 8] |= (1 << j);
                    }
                }
            }
        }
            
        
        
        //ensure size correct
        assert(tile.size() == 16);
        assert(palette.size() == 4);
        
        
        
        //------------save to disk------------------
        fb_tile.open(tile_paths[i], std::ios::out | std::ios::binary);      //in binary mode
        std::ostream os_tile(&fb_tile);
        write_chunk< uint8_t >(magic, tile, &os_tile);
        fb_tile.close();
        
        fb_palette.open(palette_paths[i], std::ios::out | std::ios::binary);
        std::ostream os_palette(&fb_palette);
        write_chunk< glm::u8vec4 >(magic, palette, &os_palette);
        fb_palette.close();
    }
    
    
    
    
    //TODO: separate pattern and color
    
    //TODO: use loop for something, maybe consider divide tile in game instead of right here
    
    
    
    return 0;
}
