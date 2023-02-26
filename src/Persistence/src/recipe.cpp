#include "recipe.hpp"

namespace rcp {

/**
 * Construct a Recipe
 * Recipe name is blank and must be set by the "set_name" method before the recipe can be initialized
*/
Recipe::Recipe():
    _map()
{
    this->_folder = "";
    this->_name = "";
    this->_extension = ".rcp";
    this->_init = false;
}

/**
 * Construct a Recipe
 * 
 * @param name the recipe file name
 * @param folder the directory to store the recipe file
 * @param extension the recipe file extension
*/
Recipe::Recipe(std::string name, std::string folder, std::string extension):
    _map()
{
    this->_name = name;
    this->_folder = folder;
    this->_extension = extension;
    this->_init = false;
}

/**
 * Destruct the recipe
 * Delete dynamically allocated RecipeItem pointers
*/
Recipe::~Recipe() {
    // Iterate all elements in the list
    // delete the RecipeItem pointer for each element
    for (auto p: this->_map) {delete p.second;}
}

/**
 * Initialize the recipe.
 * Check if the file and target directory exists
 * Creates the directory and / or file if they dont.
 * 
 * @return true if the initialization was successfull.
*/
bool Recipe::init() {
    if (this->_name == "") {return false;}

    std::string path_string = this->get_path();
    if (std::filesystem::exists(path_string)) {
        this->_init = true;
    } else {
        try {
            std::filesystem::path path(path_string);
            std::filesystem::create_directories(path.parent_path());
            std::ofstream file(path);
            file.close();
            this->_init = true;
        }
        catch(const std::filesystem::filesystem_error& err) {
            this->_init = false;
        }
    }
    return this->_init;
}

/**
 * Reset the initialized flag.
 * Will block the "load_recipe" and "save_recipe" methods
 * The "init" method must be called again to enable these.
*/
void Recipe::stop() {
    this->_init = false;
}

/**
 * Adds a variable to the recipe.
 * The variable will only be added if the identifier "id" is not yet registered in the recipe.
 * This will NOT modify the recipe file before a call to "save_recipe".
 * 
 * @param id a unique identifier for the variable linked to this recipe
 * @param var a char pointer to the variable
 * @param size the size of the variable in number of bytes
 * 
 * @return true if the variable was added to the recipe
*/
bool Recipe::add_variable(std::string id, char *var, size_t size) {
    if (this->_map.count(id) > 0) {return false;}

    RecipeItem *item = new RecipeItem;
    item->ptr = var;
    item->size = size;

    this->_map.emplace(id, item);
    return true;
}

/**
 * Removes a variable from the recipe.
 * The variable will be removed from the recipe.
 * This will NOT modify the recipe file before a call to "save_recipe".
 * 
 * @param id a unique identifier for the variable linked to this recipe.
 * 
 * @return true if the variable was removed.
*/
bool Recipe::remove_variable(std::string id) {
    if (this->_map.count(id) == 0) {return false;}
    RecipeItem *item = this->_map[id];
    this->_map.erase(id);
    delete item;
    return true;
}

/**
 * Load the recipe file and assign values to corresponding variables.
 * Only accessible if the recipe has been initialized.
 * Application variables not present in the recipe file will not be modified.
 * Variables present in the recipe file, but not present in the application recipe will be skipped.
 * 
 * @return true if the recipe values were written to application variables
*/
bool Recipe::load_recipe() {
    if (!this->_init) {return false;}

    std::ifstream file;
    std::string id;
    char character;
    size_t size;

    file.open(this->get_path());
    while (file.peek(), !file.eof()) {
        size_t num_bytes = 0;

        // Read ID
        id = "";
        file.read((char*)&size, sizeof(size));
        for (size_t i = 0; i < size; i++) {
            file.read(&character, 1);
            id += character;
        }
        num_bytes += size;

        // Read data
        file.read((char*)&size, sizeof(size));
        char *data = new char[size];
        file.read(data, size);
        num_bytes += size;

        // Check for padding
        if (num_bytes % 2 != 0) {
            file.read(&character, 1);
        }

        // Comare recipe variable to variable in memory
        if (this->_map.count(id) == 0) {
            delete[] data;
            continue;
        }

        RecipeItem *item = this->_map[id];

        if (item->size != size) {
            delete[] data;
            continue;
        }

        // Copy recipe data to memory
        for (size_t i = 0; i < size; i++) {
            item->ptr[i] = data[i];
        }

        // Release temporary data array
        delete[] data;
    }   


    file.close();
    return true;
}

/**
 * Saves the application variable values to the recipe file.
 * This will overwrite any previous recipe.
 * If any varables were removed from the application recipe since the previous save, these will no longer be present in the new recipe file.
 * 
 * @return true if the recipe was successfully saved.
*/
bool Recipe::save_recipe() {
    if (!this->_init) {return false;}

    std::ofstream file;
    file.open(this->get_path());
    char padding = 0;
    for (auto var: this->_map) {
        size_t id_size = var.first.length();
        file.write((char*)&id_size, sizeof(id_size));
        file.write((char*)&var.first.front(), id_size);
        file.write((char*)&var.second->size, sizeof(var.second->size));
        file.write(var.second->ptr, var.second->size);
        size_t num_bytes = id_size + var.second->size;
        if (num_bytes % 2 != 0) {file.write(&padding, 1);}
    }
    file.close();
    return true;    
}

/**
 * Check if the recipe is initialized
 * 
 * @return true if the recipe is initialized
*/
bool Recipe::is_init() {
    return this->_init;
}


/**
 * Check the current recipe path
 * 
 * @return the current recipe path
*/
std::string Recipe::get_path() {
    return this->_folder + this->_name + this->_extension;
}

/**
 * Set the recipe file name
 * This will call the "stop" method and the recipe must be reinitialized before any new calls to "load_recipe" or "save_recipe" can be made.
 * 
 * @param name the new file name
*/
void Recipe::set_name(std::string name) {
    this->_name = name;
    this->stop();
}


/**
 * Set the recipe directory
 * This will call the "stop" method and the recipe must be reinitialized before any new calls to "load_recipe" or "save_recipe" can be made.
 * 
 * @param folder the new driectory name
*/
void Recipe::set_folder(std::string folder) {
    this->_folder = folder + ((folder != "" && folder.back() != '/') ? "/":"");
    this->stop();
}


/**
 * Set the recipe extension
 * This will call the "stop" method and the recipe must be reinitialized before any new calls to "load_recipe" or "save_recipe" can be made.
 * 
 * @param extension the new extension
*/
void Recipe::set_extension(std::string extension) {
    this->_extension = ((extension != "" && extension.front() != '.') ? ".":"") + extension;
    this->stop();
}



}

