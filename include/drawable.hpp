#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "utils.hpp"
#include <iostream>
#include <csignal>

#ifndef DRAWABLE_H
#define DRAWABLE_H

namespace Drawable{
struct Color{
    int r,g,b;

    Color(int r, int g, int b): r(r), g(g), b(b){}
    Color(): r(255), g(255), b(255){}

    void set_color_from_color(Color color){
        r = color.r;
        g = color.g;
        b = color.b;
    }

    static int valid_color_value(int value){
        int local_value = value;
        
        if(value > 255) local_value = 255;
        if(value < 0) local_value = 0;

        return local_value;
    }

    static Color* get_random_color(){
        int r = get_rand_int(0, 255);
        int g = get_rand_int(0, 255);
        int b = get_rand_int(0, 255);

        return new Color(r,g,b);
    }
};

#define COLOR_BLACK Color(0, 0, 0)
#define COLOR_GRAY Color(127, 127, 127)
#define COLOR_WHITE Color(255, 255, 255)
#define COLOR_RED Color(255, 0, 0)
#define COLOR_BLUE Color(0, 0, 255)
#define COLOR_GREEN Color(0, 255, 0)

class Drawable2D{
public:
    Color **color_array;
    Color default_color;
    sf::Font *font;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RenderTexture rendertexture;
    
    int width, height;

    Drawable2D(int width, int height, sf::Font *font = NULL, Color *default_color = NULL): width(width), height(height), font(font){
        // Allocate our color array
        color_array = (Color**) calloc(width * height, sizeof(Color));
        // Fill the color array with default color
        for(int color_index = 0; color_index < width * height; color_index++){
            if(default_color){
                color_array[color_index] = new Color(default_color->r, default_color->g, default_color->b);
            }else{
                color_array[color_index] = new Color();
            }
            
        }
        // Store our default color
        if(default_color){
            this->default_color.r = default_color->r;
            this->default_color.g = default_color->g;
            this->default_color.b = default_color->b;
        }  
        
        texture.create(width, height);

        rendertexture.create(width, height);

        sprite.setTexture(texture);

    } 

    void fill(Color color){
        for(int color_index = 0; color_index < width * height; color_index++){
            color_array[color_index]->set_color_from_color(color);
        }
    }

    bool valid_pixel(int x, int y){
        if(y > (height - 1) || y < 0) return false;
        if(x > (width - 1) || x < 0) return false;
        return true;
    }

    int get_array_index(int x, int y){
        return (y * width) + x;
    }

    void draw_edge(GridDirection direction, Color color){
        int delta_x = 0, delta_y = 0, start_x = 0, start_y = 0, current_x, current_y;

        if(direction == North) {
            delta_x = 1;
        }
        if(direction == South){
            delta_x = 1;
            start_y = height - 1;
        } 
        if(direction == East){
            delta_y = 1;
            start_x = width - 1;
        }
        if(direction == West){
            delta_y = 1;
        }

        current_x = start_x;
        current_y = start_y;

        while (valid_pixel(current_x, current_y)){
            color_array[get_array_index(current_x, current_y)]->set_color_from_color(color);

            current_x += delta_x;
            current_y += delta_y;
        }
    }

    sf::Image to_sfml_image(){
        int channel_count = 3;
        sf::Image image;
        image.create(width, height);

        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                image.setPixel(x, y, sf::Color(color_array[(y * width) + x]->r, color_array[(y * width) + x]->g, color_array[(y * width) + x]->b));
            }
        }

        return image;
    }

    void apply_letter_to_drawable(char letter){
        // Get our texture
        texture.update(to_sfml_image());

        // Create our text
        sf::Text text(letter, *font);
        text.setCharacterSize(height);
        text.setColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(width / 4, -height / 4));

        // Put it in a render texture
        rendertexture.draw(sprite);
        rendertexture.draw(text);
        rendertexture.display();


        // Get the final image 
        sf::Image final_image = rendertexture.getTexture().copyToImage();

        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                sf::Color color = final_image.getPixel(x, y);

                color_array[(y * width) + x]->r = color.r;
                color_array[(y * width) + x]->g = color.g;
                color_array[(y * width) + x]->b = color.b;
            }
        }
        
    }

    void save_array_as_png(std::string filename){
        sf::Image image = to_sfml_image();

        image.saveToFile(filename);
    }

    void draw_portion(int start_x, int start_y, int input_width, int input_height, Color **input_array){
        int curr_x, curr_y;

        for(int y = 0; y < input_height; y++){
            for(int x = 0; x < input_width; x++){
                int array_index;

                curr_x = start_x + x;
                curr_y = start_y + y;
                array_index = get_array_index(curr_x, curr_y);

                if(!valid_pixel(curr_x, curr_y)) continue;
                color_array[array_index]->r = input_array[(y * input_width) + x]->r;
                color_array[array_index]->g = input_array[(y * input_width) + x]->g;
                color_array[array_index]->b = input_array[(y * input_width) + x]->b;
            }
        }

    }

    virtual Color** draw(){}
};



}
#endif