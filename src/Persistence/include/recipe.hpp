#ifndef RCP_RECIPE_HPP
#define RCP_RECIPE_HPP

#include <unordered_map>
#include <filesystem>
#include <fstream>

namespace rcp {

/**
 * A container for application variables.
 * Used by the Recipe class
*/
struct RecipeItem {
    char *ptr;
    size_t size;
};


/**
 * The Recipe class represents a set of variables linked to a file for persistent storage.
 * 
 * --------------------------------------------
 * Usage description:
 * --------------------------------------------
 * Provide a file name for the recipe as parameter in the constructor or assign one by calling "set_name".
 * Optional: provide a destination folder for the recipe file (default: none)
 * Optional: provide a custom extension for the recipe file (default: rcp)
 * 
 * Add variables through the "add_variable" method.
 * Provide a unique identifier for the variable, a char pointer to the variable and the variable size in number of bytes.
 * 
 * Call the "init" method to enable the recipe.
 * This creates the file at the specified folder if it does not exists.
 * 
 * After the "init" method has been called the "load_recipe" and "save_recipe" methods become available.
 * "load_recipe" reads the recipe file and transfers values from the recipe file to the application variables.
 * "save_recipe" overwrites the recipe file and transfers values from the application variables to the recipe file.
 * 
 * --------------------------------------------
 * Notes:
 * --------------------------------------------
 * Be careful with loading a recipe if a variable has changed its type.
 * Adding and removing variables from the recipe is not a problem, but a changed type will cause undefined behaviour.
 * If the size of a variable changes, for instance changing an int to a long type, the value will not be assigned to the variable when loading a recipe.
 * If the size of a variable is the same but the datastructure is different, for instance a struct where the fields have changed their order or changing from uint32_t to float, the struct will be updated with values from the recipe. 
*/
class Recipe {
    public:
        Recipe();
        Recipe(std::string name, std::string file="", std::string extension=".rcp");
        ~Recipe();

        bool init();
        void stop();
        bool add_variable(std::string, char*, size_t);
        bool remove_variable(std::string);
        bool load_recipe();
        bool save_recipe();

        bool is_init();
        std::string get_path();
        void set_name(std::string);
        void set_folder(std::string);
        void set_extension(std::string);
    protected:
    private:
        std::string _folder;
        std::string _extension;
        std::string _name;
        std::unordered_map<std::string, RecipeItem*> _map;
        bool _init;


};
}

#endif
