#include <iostream>
#include "recipe.hpp"

struct TestStruct {
    bool bool1;
    bool bool2;
    int int1;
    int int2;
    int int3;
    long long1;
    long long2;
    float float1;
    float float2;
};

int main(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        std::cout << i << ": " << argv[i] << std::endl;
    }

    std::filesystem::path path(argv[0]);
    int i = 0;
    long l = 0;

    TestStruct test{};
    
    rcp::Recipe test_recipe("test_recipe");
    test_recipe.set_folder(path.parent_path().string() + "/example_output/recipes");
    test_recipe.set_extension("rcp");

    test_recipe.add_variable("integer", (char*)&i, sizeof(i));
    test_recipe.add_variable("long thing", (char*)&l, sizeof(l));
    test_recipe.add_variable("test_struct", (char*)&test, sizeof(test));
    test_recipe.init();

    bool success = test_recipe.load_recipe();
    std::cout << "Load \"" << test_recipe.get_path() << "\": " << success << std::endl;
    std::cout << "integer: " << i << std::endl;
    std::cout << "long thing: " << l << std::endl;
    
    i = 69;
    l = 6969;
    test.bool1 = false;
    test.bool2 = true;
    test.int1 = 11;
    test.int2 = 12;
    test.int3 = 13;
    test.long1 = 21;
    test.long2 = 22;
    test.float1 = 3.1;
    test.float2 = 3.2;
    
    success = test_recipe.save_recipe();
    std::cout << "Save \"" << test_recipe.get_path() << "\": " << success << std::endl;
    std::cout << "integer: " << i << std::endl;
    std::cout << "long thing: " << l << std::endl;
    
    return 0;
}