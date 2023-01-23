#include <cmath>
#include <vector>
#include <memory>
#include <random>

#ifndef UTILS_H
#define UTILS_H

enum GridDirection{
    North = 0,
    South = 1,
    West = 2,
    East = 3,
    None = 4
};

template <typename T>
void remove_item_from_vector(std::vector<T*> &list, T *item, bool use_delete = true){
    typename std::vector<T*>::iterator curr_element = list.begin();

    while(curr_element != list.end()){
        if(*curr_element == item){
            if(*curr_element && use_delete)
                delete *curr_element;
            curr_element = list.erase(curr_element);
        }else{
            curr_element++;
        }
    } 
}

template <typename T>
void copy_double_pointer_array(T ***array_from, T ***array_to, int count){
    for(int i = 0; i < count; i++){
        (*array_to)[i] = (*array_from)[i];
    }
}

GridDirection get_opposite_direction(GridDirection direction){
    if(direction == North) return South;
    if(direction == South) return North;
    if(direction == East) return West;
    if(direction == West) return East;
}

/**
 * @brief Used for randomness
 */
std::default_random_engine generator;

/**
 * @brief Used to encapsulate a position in the world
 */
struct Position
{
    float x;
    float y;
    Position(float x, float y) : x(x), y(y) {}
};

/**
 * @brief Get the distance between two positions
 *
 * @param pos1 Position 1
 * @param pos2 Position 2
 * @return float The distance between the two positions
 */
float get_distance(Position pos1, Position pos2)
{
    return sqrt(pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2));
}

/**
 * @brief Get a random uniform float
 *
 * @param from From float
 * @param to To float
 * @return float Random float between from and to float
 */
float get_rand_uniform_float(float from, float to)
{
    std::uniform_real_distribution<float> distribution(from, to);
    return distribution(generator);
}

/**
 * @brief Get the random normal float
 *
 * @param mean Mean of the random float distribution
 * @param std_dev Standard deviation for the float distribution
 * @return float Random float given the inputs
 */
float get_rand_normal_float(float mean, float std_dev)
{
    std::normal_distribution<float> distribution(mean, std_dev);
    return distribution(generator);
}

/**
 * @brief Get a random integer
 *
 * @param from From Integer
 * @param to To Integer
 * @return int Random Integer between from and to
 */
int get_rand_int(int from, int to)
{
    std::uniform_real_distribution<float> distribution(from, to);
    return (int)distribution(generator);
}

/**
 * @brief Get a random boolean
 *
 * @param chance The float chance for true (must be between 0-1)
 * @return true
 * @return false
 */
bool get_rand_bool(float chance = 0.5)
{
    return get_rand_uniform_float(0.0, 1.0) < chance;
}


#endif