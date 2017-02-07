#include "main.h"
using namespace std;


int main()
{   
    int nx=400;
    int ny=400;
    int ns = 100;
    rgba8* pixels = new rgba8[nx * ny];
   // hitable *world =final();
    hitable *world =my();
    vec3 lookfrom(378,278,-700);
    vec3 lookat(278,278,0);


 //vec3 lookfrom(13,2,3);
 //vec3 lookat(0,0,0);
 float dist_to_focus = 10.0;
 float aperture=0.0;
 float vfov = 40.0;

    //vec3 lookfrom(600,150,-800);
    //vec3 lookat(278,278,0);
    //float dist_to_focus = 10.0;
    //float aperture = 0.0;
    //float vfov = 40.0;
    camera cam(lookfrom, lookat, vec3(0,1,0),vfov, float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);
    for (int j=ny -1; j>=0; j--)
        for(int i=0;i<nx;i++){
            vec3 col(0,0,0);
            for(int s=0; s<ns; s++){
                float u = float(i+drand48())/float(nx);
                float v = float(j+drand48())/float(ny);
                ray r=cam.get_ray(u, v);
                //vec3 p=r.point_at_parameter(2.0);
                col+=color( r, world,0);
            }
            col /= float(ns);
            col = vec3( pow(col[0],0.3),pow(col[1],0.3),pow(col[2],0.3));
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            //std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
            pixels[(ny-1-j) * nx + i] =rgba8(ir, ig, ib, 255);
        }
    stbi_write_png("raytrace.png", nx, ny, 4, pixels, nx * 4);
    system("open raytrace.png");
    delete[] pixels;
    return 0;
}


