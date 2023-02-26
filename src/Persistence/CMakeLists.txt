add_library(Recipe include/recipe.hpp src/recipe.cpp)
target_include_directories(Recipe PUBLIC include)

add_executable(RecipeExample examples/main.cpp)
target_link_libraries(RecipeExample PUBLIC Recipe)
target_include_directories(RecipeExample PUBLIC include)
