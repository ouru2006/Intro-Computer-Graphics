import qbs

CppApplication {
    consoleApplication: true
    files: [
        "aabb.h",
        "box.h",
        "bvh_node.h",
        "camera.h",
        "constant_medium.h",
        "diffuse_light.h",
        "flip_normals.h",
        "hitable.h",
        "hitable_list.h",
        "main.cpp",
        "main.h",
        "material.h",
        "moving_sphere.h",
        "perlin.h",
        "pic.jpeg",
        "pic.qrc",
        "picture/earthmap.jpg",
        "ray.h",
        "rect.h",
        "rotate.h",
        "sphere.h",
        "stb_image.h",
        "stb_image_write.h",
        "texture.h",
        "translate.h",
        "vec3.h",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
}
