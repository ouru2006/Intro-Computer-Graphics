#ifndef MAIN_H
#define MAIN_H
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include "hitable_list.h"
#include "float.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "moving_sphere.h"
#include "rect.h"
#include "flip_normals.h"
#include "box.h"
#include "translate.h"
#include "rotate.h"
#include "constant_medium.h"
//#include "bvh_node.h"

vec3 color(const ray& r, hitable *world, int depth){
    hit_record rec;
    if(world->hit(r, 0.001, MAXFLOAT, rec)){
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u,rec.v,rec.p);
        if(depth<50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return emitted + attenuation*color(scattered,world,depth+1);
        }else{
            return emitted;
        }
    }else{
        return vec3(0,0,0);
    }
}
/*
vec3 color(const ray& r, hitable *world, int depth){
    hit_record rec;
    if(world->hit(r, 0.001, MAXFLOAT, rec)){
        ray scattered;
        vec3 attenuation;
        if(depth<50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation*color(scattered,world,depth+1);
        }else{
            return vec3(0,0,0);
        }
    }else{

        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y()+1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0);
    }
}
*/
struct rgba8 {
    unsigned char r, g, b, a;
    rgba8() { }
    rgba8(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa)
    : r(rr),g(gg),b(bb),a(aa) { }
};

hitable *two_perlin_spheres(){
    texture *pertext = new noise_texture(5);
    hitable **list = new hitable*[2];
    list[0]= new sphere(vec3(0,-1000,0),1000,new lambertian(pertext));
    list[1]= new sphere(vec3(0,2,0),2,new lambertian(pertext));
    return new hitable_list(list,2);
}
hitable *earth(){
    int nx,ny,nn;
    unsigned char *tex_data = stbi_load("./picture/earthmap.jpg",&nx,&ny,&nn,0);
    material *mat = new lambertian(new image_texture(tex_data,nx,ny));
    //texture *pertext = new noise_texture(5);
    hitable **list = new hitable*[1];
    list[0]= new sphere(vec3(0,0,0),2, mat);
    return new hitable_list(list,1);
}

hitable *two_spheres(){
    texture *checker=new checker_texture(new constant_texture(vec3(0.2,0.3,0.1)),new constant_texture(vec3(0.9,0.9,0.9)));
    int n=50;
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0,-10,0),10, new lambertian(checker));
    list[1] = new sphere(vec3(0,10,0),10, new lambertian(checker));
    return new hitable_list(list,2);

}

hitable *random_scence(){
    int n=200;
    hitable **list = new hitable*[n+1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3,0.1)),new constant_texture(vec3(0.9,0.9,0.9)));
    list[0] = new sphere(vec3(0,-1000,0),1000, new lambertian(checker));
    int i=1;
    for(int a = -10; a<10; a++){
        for(int b = -10; b< 10; b++){
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            if((center-vec3(4,0.2,0)).length()>0.9){
                if(choose_mat <0.8 ){
                    list[i++] = new moving_sphere(center,center+vec3(0,0.5*drand48(),0),0.0,1.0,0.2, new lambertian(new constant_texture(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48()))));
                }else if(choose_mat<0.95){
                    list[i++] = new sphere(center,0.2, new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())),0.5*drand48()));
                }else {
                    list[i++] =new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(new constant_texture(vec3(0.4,0.2,0.1))));
    list[i++] = new sphere(vec3(4,1,0), 1.0, new metal(vec3(0.7, 0.6, 0.5),0.0));

    return new hitable_list(list,i);
}

hitable *simple_light(){
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[4];
    list[0] = new sphere(vec3(0,-1000,0),1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0,2,0),2,new lambertian(pertext));
    list[2] = new sphere(vec3(0,7,0),2,new diffuse_light(new constant_texture(vec3(4,4,4))));
    list[3] = new xy_rect(3,5,1,3,-2,new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hitable_list(list, 4);
}

hitable *cornell_box(){
    hitable **list = new hitable*[8];
    int i=0;
    material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73,0.73,0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12,0.45,0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(7,7,7)) );
    list[i++] = new flip_normals(new yz_rect(0,555,0,555,555,green));
    //list[i++] = new yz_rect(0, 555, 0, 555, 555,green);
    list[i++] = new yz_rect(0, 555, 0, 555, 0,red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554,light);
    list[i++] = new flip_normals(new xz_rect(0,555,0,555,555,white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0,white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555,white));
    //list[i++] = new yz_rect(0, 555, 0, 555, 0,new diffuse_light(new constant_texture(vec3(0.65,0.05,0.05))));
    //list[i++] = new box(vec3(130,0,65), vec3(295,165,230),white);
    //list[i++] = new box(vec3(265,0,295), vec3(430,330,460),white);
    list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,165,165),white),-18),vec3(130,0,65));
    list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,330,165),white),15),vec3(265,0,295));
    return new hitable_list(list,i);
}
hitable *cornell_smoke(){
    hitable **list = new hitable*[8];
    int i=0;
    material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73,0.73,0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12,0.45,0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(1,1,1)) );
    list[i++] = new flip_normals(new yz_rect(0,555,0,555,555,green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0,red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554,light);
    list[i++] = new flip_normals(new xz_rect(0,555,0,555,555,white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0,white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555,white));

    hitable *b1 = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,165,165),white),-18),vec3(130,0,65));
    hitable *b2 = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,330,165),white),15),vec3(265,0,295));
    list[i++] = new constant_medium(b1,0.01,new constant_texture(vec3(1.0,1.0,1.0)));
    list[i++] = new constant_medium(b2,0.01,new constant_texture(vec3(0.0,0.0,0.0)));
    return new hitable_list(list,i);
}

hitable *final(){
   // int nb = 20;
    hitable **list =new hitable*[30];
    hitable **boxlist2 = new hitable*[10000];
    material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
    material *green = new lambertian(new constant_texture(vec3(0.12,0.45,0.15)));
    material *white = new lambertian (new constant_texture(vec3(0.73,0.73,0.73)));
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83,0.53)));

    int l=0;

    list[l++] = new flip_normals(new yz_rect(0,555,0,555,555,green));
   // list[l++] = new bvh_node(boxlist, b, 0,1);
    material *light = new diffuse_light(new constant_texture(vec3(1,1,1)) );
    list[l++] = new flip_normals(new xy_rect(0,555,0,555,555,green));
    //material *light = new diffuse_light(new constant_texture(vec3(7,7,7)));
    list[l++] = new xz_rect(123,423,147,412,554,light);
    list[l++] = new xz_rect(0,555,0,555,0,ground);
    vec3 center(400,400,200);
    list[l++] = new yz_rect(0, 555, 0, 555, 0,red);
    list[l++] = new moving_sphere(center, center+vec3(30,0,0), 0, 1, 50,new lambertian(new constant_texture(vec3(0.7,0.3,0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(60,150,145), 50, new metal(vec3(0.8,0.8,0.9),10.0));
    hitable *boundary = new sphere(vec3(360,150,145), 70, new dielectric(1.5));
    list[l++] = boundary;
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2,0.4,0.9)));
    boundary = new sphere(vec3(0, 0 , 0), 5000, new dielectric(1.5));
    list[l++] = new constant_medium(boundary,0.0001, new constant_texture(vec3(1.0,1.0,1.0)));
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("./pic.jpeg",&nx,&ny,&nn,0);
    material *emat = new lambertian(new image_texture(tex_data,nx,ny));
    list[l++] = new sphere(vec3(400,200,400), 100,emat);
    texture *pertext = new noise_texture(0.01);
    list[l++] = new sphere(vec3(220, 280, 300),80, new lambertian (pertext));
    int ns = 1000;
    /*
    for(int j=0;j<ns;j++){
        boxlist2[j] = new sphere(vec3(165*drand48(), 168*drand48(), 165*drand48()),10,white);
    }
    */
    //list[l++] = new translate(new rotate_y(new bvh_node(boxlist2,ns, 0.0,1.0),15),vec3(-100,270,395));
    return new hitable_list(list,l);
}
hitable *my(){
   // int nb = 20;
    hitable **list =new hitable*[30];
    //hitable **boxlist2 = new hitable*[10000];
    material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
    material *green = new lambertian(new constant_texture(vec3(0.12,0.15,0.45)));
    material *white = new lambertian (new constant_texture(vec3(0.73,0.73,0.73)));
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83,0.53)));

    int l=0;

    list[l++] = new flip_normals(new yz_rect(0,555,0,555,555,green));
    material *light = new diffuse_light(new constant_texture(vec3(1,1,1)) );

    list[l++] = new flip_normals(new xz_rect(0,555,0,555,555,white));
    list[l++] = new xz_rect(0, 555, 0, 555, 0,white);
    list[l++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555,white));

    list[l++] = new xz_rect(123,423,147,412,554,light);
    list[l++] = new xz_rect(0,555,0,555,0,ground);
    vec3 center(400,400,200);
    list[l++] = new yz_rect(0, 555, 0, 555, 0,red);
    list[l++] = new sphere(vec3(320, 150, 95), 70, new dielectric(1.5));
    list[l++] = new sphere(vec3(110,150,345), 70, new metal(vec3(0.7, 0.6, 0.5),0.0));
    hitable *boundary=new sphere(vec3(420,350,445), 70, new dielectric(1.5));
    list[l++] = boundary;
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2,0.4,0.9)));
    boundary = new sphere(vec3(0, 0 , 0), 5000, new dielectric(1.5));
    list[l++] = new constant_medium(boundary,0.0001, new constant_texture(vec3(1.0,1.0,1.0)));

    return new hitable_list(list,l);
}
#endif // MAIN_H
