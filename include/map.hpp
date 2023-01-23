#include <iostream>
#include <vector>
#include "utils.hpp"
#include "drawable.hpp"

#ifndef MAP_H
#define MAP_H

using namespace Drawable;

struct ObservableList{
private:
    std::vector<GridDirection> direction_list;
public:
    bool has_changed;
    ObservableList(std::vector<GridDirection> list_in): direction_list(list_in), has_changed(false){}
    ObservableList(): has_changed(false){}

    void remove(GridDirection direction){
        // Just return if we have no elements
        if(direction_list.size() < 1) return;
        // Get our start iterator
        std::vector<GridDirection>::iterator curr_element = direction_list.begin();
        // Remove that direction from the list if it exists
        while(curr_element != direction_list.end()){
            if(*curr_element == direction){
                curr_element = direction_list.erase(curr_element);
                has_changed = true;
            }else{
                curr_element++;
            }
            
        }

    }

    void add(GridDirection direction){
        // Check if this direction already exists in our vector
        for(GridDirection curr_direction : direction_list){
            if(curr_direction == direction){
                return;
            }
        }
        // Don't add a None direction
        if(direction == None) return;
        // If we made it this far then we know it isn't already
        // in the list.
        direction_list.emplace_back(direction);
        has_changed = true;
    }

    bool direction_in_list(GridDirection direction){
        for(GridDirection d : direction_list){
            if(d == direction) return true;
        }
        return false;
    }

    void clear_list(){
        direction_list.clear();
        has_changed = true;
    }

    int get_size(){
        return direction_list.size();
    }
};

struct Block: public Drawable2D{
private:
    GridDirection entry_direction;
    ObservableList exit_directions;
    bool has_changed;
    char letter;
public:
    Color wall_color, background_color;
    bool explored;
    Block(int width, int height): Drawable2D(width, height), wall_color(COLOR_BLACK), background_color(COLOR_WHITE), has_changed(true), explored(false), entry_direction(None), letter(0){}

    void set_letter(char new_letter){
        if(letter == new_letter) return;

        letter = new_letter;
        has_changed = true;
    }

    GridDirection get_entry_direction(){
        return entry_direction;
    }

    void set_entry_direction(GridDirection direction){
        if(entry_direction == direction) return;

        entry_direction = direction;
        has_changed = true;
    }

    bool is_entry_direction(GridDirection direction){
        return entry_direction == direction;
    }

    void remove_exit_direction(GridDirection direction){
        exit_directions.remove(direction);
    }

    void set_exit_directions(std::vector<GridDirection> &directions){
        exit_directions.clear_list();
        for(GridDirection direction: directions){
            exit_directions.add(direction);
        }
    }

    void add_exit_direction(GridDirection direction){
        exit_directions.add(direction);
    }

    bool is_exit_direction(GridDirection direction){
        return exit_directions.direction_in_list(direction);
    }

    int exit_count(){
        return exit_directions.get_size();
    }

    bool is_mulit_exit(){
        return exit_directions.get_size() > 1;
    }

    void clear_exits(){
        exit_directions.clear_list();
    }

    Color** draw(){
        if(!explored){
            fill(COLOR_BLACK);
        }else{
            fill(background_color);
        }

        for(int direction = 0; direction < None; direction++){
            if(is_exit_direction(GridDirection(direction)) || is_entry_direction(GridDirection(direction))) continue;
            draw_edge(GridDirection(direction), wall_color);
        }

        if(letter != 0){
            // std::cout << "Applying letter!" << std::endl;
            apply_letter_to_drawable(letter);
        }

        return color_array;
    }

    void print_debug_info(){
        std::cout << "Block Info: Entry Direction = " << entry_direction;
        bool exit_printed = false;
        if(exit_directions.get_size() > 0){
            std::cout << " Exit Directions = ";
        }
        for(int direction = 0; direction < None; direction++){
            if(exit_directions.direction_in_list(GridDirection(direction))){
                if(exit_printed) std::cout << ", ";
                std::cout << GridDirection(direction);
                exit_printed = true;
            }
                
        } 
        std::cout << std::endl;
    }
};

struct Map: public Drawable2D{
    int grid_width, grid_height, block_width, block_height;
    Block **block_grid;
    Map(int grid_width, int grid_height, int block_width = 10, int block_height = 10): Drawable2D(grid_width * block_width, grid_height * block_height), grid_width(grid_width), grid_height(grid_height), block_width(block_width), block_height(block_height){
        block_grid = (Block**) calloc(grid_width * grid_height, sizeof(Block*));
        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                block_grid[(y * grid_width) + x] = new Block(block_width, block_height);
            }
        }
    }
    ~Map(){
        for(int block_index = 0; block_index < grid_width * grid_height; block_index++){
            delete block_grid[block_index];
        }

        free(block_grid);
    }

    std::pair<int, Block*> get_lowest_block(){
        for(int y = grid_height - 1; y > -1; y--){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];

                if(curr_block->explored){
                    return std::pair<int, Block*>(y, curr_block);
                }
            }
        }

        return std::pair<int, Block*>(-1, NULL);
    }

    std::vector<Block*> get_all_explored_blocks(){
        std::vector<Block*> ret_blocks;

        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];
                if(curr_block->explored) ret_blocks.push_back(curr_block);
            }
        }

        return ret_blocks;
    }

    Block* get_start_block(){
        int random_index = get_rand_int(0, grid_width - 1);
        return block_grid[random_index];
    }

    void clean_all_blocks(){
        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];
                clean_block_relationships(curr_block);
            }
        }
    }

    void clean_block_relationships(Block* block){
        GridDirection entry = block->get_entry_direction();

        if(entry != None)
            block->remove_exit_direction(entry);

        for(std::pair<GridDirection, Block*> dir_block: get_blocks_in_all_directions(block, false)){
            if(!dir_block.second) continue;

            GridDirection check_relative_direction = get_opposite_direction(dir_block.first);

            if(dir_block.first != entry && !(block->is_exit_direction(dir_block.first))){
                // Make sure the other block isn't exiting or entering from our direction
                if(dir_block.second->is_entry_direction(check_relative_direction)){
                    dir_block.second->set_entry_direction(None);
                }

                dir_block.second->remove_exit_direction(check_relative_direction);
            }else if(block->is_exit_direction(dir_block.first)){
                // Ensure that exit is actually an entry into the neighbor block
                if(!(dir_block.second->is_entry_direction(check_relative_direction))){
                    block->remove_exit_direction(dir_block.first);
                }
            }
        }
    }

    std::vector<Block*> get_all_junctions(){
        std::vector<Block*> ret_blocks;

        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];

                if(curr_block->exit_count() < 2) continue;

                ret_blocks.push_back(curr_block);
            }
        }

        return ret_blocks;
    }

    std::pair<int, int> get_block_x_y_tuple(Block *block){
        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];

                if(curr_block == block) return std::pair<int, int>(x, y);
            }
        }

        return std::pair<int, int>(-1, -1);
    }

    Block* get_block_in_direction(Block* block, GridDirection direction, bool ignore_explored = true){
        std::pair<int, int> curr_block_coor = get_block_x_y_tuple(block);

        if(curr_block_coor.first == -1 || curr_block_coor.second == -1 || direction == None){
            return NULL;
        }

        int check_x = curr_block_coor.first, check_y = curr_block_coor.second;

        if(direction == North) check_y -= 1;
        if(direction == South) check_y += 1;
        if(direction == West) check_x -= 1;
        if(direction == East) check_x += 1;

        if(check_y >= grid_height || check_y < 0) return NULL;
        if(check_x >= grid_width || check_x < 0) return NULL;

        if(ignore_explored && block_grid[(grid_width * check_y) + check_x]->explored) return NULL;

        return block_grid[(grid_width * check_y) + check_x];

    }

    std::vector<std::pair<GridDirection, Block*>> get_blocks_in_all_directions(Block *block, bool ignore_explored = true){
        std::vector<std::pair<GridDirection, Block*>> ret_pair;

        for(int direction = 0; direction < None; direction++){
            Block* curr_block = get_block_in_direction(block, GridDirection(direction), ignore_explored);
            if(curr_block) ret_pair.push_back(std::pair<GridDirection, Block*>(GridDirection(direction), curr_block));
        }

        return ret_pair;
    }

    Color** draw(){
        for(int y = 0; y < grid_height; y++){
            for(int x = 0; x < grid_width; x++){
                Block *curr_block = block_grid[(y * grid_width) + x];
                clean_block_relationships(curr_block);
                draw_portion(x * block_width, y * block_height, block_width, block_height, curr_block->draw());
            }
        }
        return color_array;
    }



};

struct Path{
    Map *map;
    Block **path;
    int curr_path_len, max_path_len;
    bool complete;

    Path(Map *map, Block *start_block): map(map), curr_path_len(1), max_path_len(1), complete(false){
        path = (Block**) calloc(1, sizeof(Block*));
        path[0] = start_block;
    }

    Path(Path* copy_path): map(copy_path->map), curr_path_len(copy_path->curr_path_len), max_path_len(copy_path->max_path_len), complete(copy_path->complete){
        path = (Block**) calloc(copy_path->max_path_len, sizeof(Block*));
        copy_double_pointer_array(&(copy_path->path), &path, curr_path_len);
    }

    ~Path(){
        free(path);
    }

    void expand_max_path_length(int amount = 5){
        Block **temp = (Block**) calloc(max_path_len + amount, sizeof(Block*));

        copy_double_pointer_array(&path, &temp, curr_path_len);
        max_path_len += amount;

        free(path);
        path = temp;
    }

    bool check_path_needs_to_expand(bool expand = true){
        if((curr_path_len + 1) >= (max_path_len - 1)){
            if(expand) expand_max_path_length();
            return true;
        }

        return false;
    }
    void clean_path_walls(){
        for(int block_index = 0; block_index < curr_path_len; block_index++){
            map->clean_block_relationships(path[block_index]);
        }
    }

    std::vector<Block*> set_random_exits(Block *block, float chance = 0.6){
        std::vector<Block*> ret;

        for(std::pair<GridDirection, Block*> dir_block : map->get_blocks_in_all_directions(block)){
            if(dir_block.second && get_rand_bool(chance)){
                block->add_exit_direction(dir_block.first);
                dir_block.second->set_entry_direction(get_opposite_direction(dir_block.first));
                ret.push_back(dir_block.second);
            }
        }

        return ret;
    }

    void add_block(Block *new_block){
        check_path_needs_to_expand();
        path[curr_path_len] = new_block;
        curr_path_len++;
    }

    std::vector<Block*> step_path(){
        std::vector<Block*> ret;
        std::vector<Block*> exit_block_list;     

        if(complete) return ret;

        exit_block_list.push_back(path[curr_path_len - 1]);

        while(exit_block_list.size() > 0){
            Block* current_block = exit_block_list.at(get_rand_int(0, exit_block_list.size() - 1));
            remove_item_from_vector(exit_block_list, current_block, false);
            remove_item_from_vector(ret, current_block, false);

            current_block->explored = true;

            if(exit_block_list.size() > 0)
                ret.insert(ret.end(), exit_block_list.begin(), exit_block_list.end());

            exit_block_list = set_random_exits(current_block);

            check_path_needs_to_expand();
            path[curr_path_len] = current_block;
            curr_path_len++;
        }
        complete = true;
        return ret;
    }

    bool block_in_path(Block *block){
        for(int block_index = 0; block_index < curr_path_len; block_index++){
            if(path[block_index] == block) return true;
        }
        return false;
    }

};

struct Maze{
    Map *map;
    Path *solution_path;
    Block *map_start, *map_end;

    Maze(int grid_width, int grid_height, int block_width, int block_height){
        map = new Map(grid_width, grid_height, block_width, block_height);

        generate_maze();
        solve_maze();
    }

    ~Maze(){
        delete map;
    }

    void generate_paths(Block *start_block){
        std::vector<Block*> possible_path_starts;
        std::vector<Block*> new_exits;
        possible_path_starts.push_back(start_block);

        while(possible_path_starts.size() > 0){
            Block *current_start = possible_path_starts.at(get_rand_int(0, possible_path_starts.size() - 1));
            remove_item_from_vector(possible_path_starts, current_start, false);

            if(current_start->explored) continue;

            Path *path = new Path(map, current_start);

            while(!path->complete){
                new_exits = path->step_path();
                if(new_exits.size() > 0)
                    possible_path_starts.insert(possible_path_starts.end(), new_exits.begin(), new_exits.end());
            }

            delete path;

        }
    }

    void generate_maze(){
        Block* new_start;
        map_start = map->get_start_block();
        map_end = NULL;
        map_start->set_entry_direction(North);

        new_start = map_start;

        while(!map_end){
            generate_paths(new_start);

            std::pair<int, Block*> lowest_location = map->get_lowest_block();
            lowest_location.second->add_exit_direction(South);

            if(lowest_location.first == map->grid_height - 1){
                map_end = lowest_location.second;
            }else{
                new_start = map->get_block_in_direction(lowest_location.second, South);
                new_start->set_entry_direction(North);
            }
        }
        
        map->draw();
    }

    void solve_maze(){
        std::vector<Path*> exploring_paths;
        std::vector<Path*> temp_exploring_paths;
        Block *start = map_start;
        solution_path = NULL;
        
        exploring_paths.push_back(new Path(map, start));

        while(!solution_path && exploring_paths.size() > 0){
            temp_exploring_paths = exploring_paths;
            for(Path *path : temp_exploring_paths){
                Block *curr_block = path->path[path->curr_path_len - 1];

                if(curr_block->exit_count() == 0 && curr_block != map_end){
                    remove_item_from_vector(exploring_paths, path); 
                    continue;
                }else if(curr_block == map_end){
                    solution_path = path;
                    for(Path* possible_path : exploring_paths){
                        if(possible_path != solution_path){
                            delete possible_path;
                        }
                    }
                    break;
                }

                if(curr_block->is_mulit_exit()){
                    for(int direction = 0; direction < None; direction++){
                        if(curr_block->is_exit_direction(GridDirection(direction))){
                            Path *new_path = new Path(path);
                            Block *next_block = map->get_block_in_direction(curr_block, GridDirection(direction), false);
                            new_path->add_block(next_block);
                            exploring_paths.push_back(new_path);
                        }
                    }
                    remove_item_from_vector(exploring_paths, path);
                }else{
                    for(int direction = 0; direction < None; direction++){
                        if(curr_block->is_exit_direction(GridDirection(direction))){
                            Block *next_block = map->get_block_in_direction(curr_block, GridDirection(direction), false);
                            path->add_block(next_block);
                        }
                    }
                }
            }
        }
        
        map->clean_all_blocks();
    }

    void save_to_png(std::string filename){
        map->save_array_as_png(filename);
    }
};

struct WordMaze: public Maze{
    std::string word;
    WordMaze(std::string word, int grid_width = 20, int grid_height = 20, int block_width = 20, int block_height = 20): Maze(grid_width, grid_height, block_width, block_height), word(word){
        sf::Font font;

        if(!font.loadFromFile("../res/font.ttf")){
            std::cout << "Couldn't load font!" << std::endl;
            std::raise(SIGTERM);
        }

        for(int block_index = 0; block_index < grid_height * grid_width; block_index++){
            map->block_grid[block_index]->font = &font;
        }
        apply_word();
        map->draw();
    }

    void close_and_unexplore_connected_blocks(Block *block){
        std::vector<Block*> blocks_to_clear;
        Block *exit_block = map->get_block_in_direction(block, block->get_entry_direction(), false);

        exit_block->remove_exit_direction(get_opposite_direction(block->get_entry_direction()));
        blocks_to_clear.push_back(block);

        while(blocks_to_clear.size() > 0){
            Block *curr_block = blocks_to_clear.at(0);
            blocks_to_clear.erase(blocks_to_clear.begin());

            if(!curr_block) {
                std::cout << "Continuing!" << std::endl;
                continue;
            }

            curr_block->set_entry_direction(None);
            curr_block->explored = false;

            for(int direction = 0; direction < None; direction++){
                if(curr_block->is_exit_direction(GridDirection(direction))){
                    blocks_to_clear.push_back(map->get_block_in_direction(curr_block, GridDirection(direction), false));
                }
            }

            curr_block->clear_exits();
        }
    }

    std::vector<char> get_invalid_letters(std::string input_string){
        std::vector<char> ret;
        for(char curr_char = 'a'; curr_char <= 'z'; curr_char++){
            bool char_in_str = false;
            for(char word_char: input_string){
                if(word_char == curr_char){
                    char_in_str = true;
                    break;
                }
            }
            if(!char_in_str){
                ret.push_back(curr_char);
            }
        }
        return ret;
    }

    void place_letter_in_exit_blocks(Block *block_with_exits, int word_index = -1){
        std::vector<char> invalid_letters = get_invalid_letters(word);

        for(int direction = 0; direction < None; direction++){
            if(!(block_with_exits->is_exit_direction(GridDirection(direction)))) continue;

            Block *letter_block = map->get_block_in_direction(block_with_exits, GridDirection(direction), false);

            if(!letter_block){
                block_with_exits->remove_exit_direction(GridDirection(direction));
                continue;
            }

            if(solution_path->block_in_path(letter_block) && word_index != -1){
                letter_block->set_letter(word[word_index]);
            }else{
                if(word_index != -1){
                    letter_block->set_letter(word[(word_index + 1) % word.length()]);
                }else{
                    int invalid_letter_choice = get_rand_int(0, invalid_letters.size() - 1);
                    letter_block->set_letter(invalid_letters.at(invalid_letter_choice));
                }
            }
        }
    }

    Block** select_solution_path_junctions(std::vector<Block*> junctions){
        Block **ret = (Block**) calloc(word.length(), sizeof(Block*));
        std::vector<int> indexes_chosen;

        while(indexes_chosen.size() < word.length()){
            int random_index = get_rand_int(0, junctions.size() - 1);
            bool index_already_selected = false;

            for(int chosen_index = 0; chosen_index < indexes_chosen.size(); chosen_index++){
                if(indexes_chosen.at(chosen_index) == random_index) {
                    index_already_selected = true;
                    break;
                }
            }

            if(index_already_selected) continue;

            ret[indexes_chosen.size()] = junctions.at(random_index);
            indexes_chosen.push_back(random_index);
        }

        return ret;
    }

    void close_all_solution_path_junctions(std::vector<Block*> &junctions, Block **excluded_junctions, int excluded_junctions_length){
        for(Block *block: junctions){
            bool valid_junction = true;

            for(int exc_junc_index = 0; exc_junc_index < excluded_junctions_length; exc_junc_index++){
                if(excluded_junctions[exc_junc_index] == block){
                    valid_junction = false;
                    break;
                }
            }

            if(!valid_junction) continue;

            for(int direction = 0; direction < None; direction++){
                if(!block->is_exit_direction(GridDirection(direction))) continue;
                Block *next_block = map->get_block_in_direction(block, GridDirection(direction), false);

                if(!next_block) continue;

                if(!solution_path->block_in_path(next_block)){
                    close_and_unexplore_connected_blocks(next_block);
                }
            }
        }
    }

    std::vector<Block*> get_all_explored_path_end_blocks(){
        std::vector<Block*> all_explored = map->get_all_explored_blocks();
        std::vector<Block*> ret;

        for(Block *curr_block: all_explored){
            if(curr_block->exit_count() > 0) continue;
            ret.push_back(curr_block);
        }

        return ret;
    }

    void fill_out_unexplored_areas(){
        std::vector<std::pair<GridDirection,Block*>> unexplored_blocks_around;
        std::vector<Block*> all_explored_ends = get_all_explored_path_end_blocks();
        int new_end_count = all_explored_ends.size();
        int past_end_count = -1;

        while(past_end_count != new_end_count){
            past_end_count = new_end_count;

            for(Block *block: all_explored_ends){
                unexplored_blocks_around = map->get_blocks_in_all_directions(block);

                if(unexplored_blocks_around.size() > 0){
                    std::pair<GridDirection, Block*> dir_to = unexplored_blocks_around.at(get_rand_int(0, unexplored_blocks_around.size() - 1));
                    if(!block->is_exit_direction(dir_to.first)){
                        block->add_exit_direction(dir_to.first);
                        dir_to.second->set_entry_direction(get_opposite_direction(dir_to.first));
                        generate_paths(dir_to.second);
                    }
                }
            }

            all_explored_ends = get_all_explored_path_end_blocks();
            new_end_count = all_explored_ends.size();
        }

        map->clean_all_blocks();
    }

    void apply_letters_to_junctions(){
        for(Block *block: map->get_all_junctions()){
            place_letter_in_exit_blocks(block);
        }

        int exit_num = 0;
        for(int block_index = 0; block_index < solution_path->curr_path_len; block_index++){
            Block *curr_block = solution_path->path[block_index];
            if(curr_block->exit_count() > 1){
                place_letter_in_exit_blocks(curr_block, exit_num);
                exit_num++;
            }
        }
    }

    std::vector<Block*> get_solution_path_junctions(){
        std::vector<Block*> junctions = map->get_all_junctions();
        std::vector<Block*> ret;

        for(Block *start_block: junctions){
            if(solution_path->block_in_path(start_block)){
                ret.push_back(start_block);
            }
        }

        return ret;
    }

    void apply_word(){
        int exit_count = word.length();
        std::vector<Block*> solution_junctions = get_solution_path_junctions();

        if(solution_junctions.size() < exit_count){
            std::cout << "Not enough exits points to write word!" << std::endl;
            return;
        }
        map->save_array_as_png("PreWordApply.png");
        Block **selected_junctions = select_solution_path_junctions(solution_junctions);
        close_all_solution_path_junctions(solution_junctions, selected_junctions, word.length());
        fill_out_unexplored_areas();
        apply_letters_to_junctions();
    }
};

#endif