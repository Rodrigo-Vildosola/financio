project(stb_image)

file(GLOB_RECURSE stb_image_SRC
    "src/*.cpp"
)

add_library(stb_image STATIC ${stb_image_SRC})

target_include_directories(stb_image PUBLIC include)
