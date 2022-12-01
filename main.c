#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define DIM 1024
#define NB_SPHERE 100
#define INF 1e9

/*
permet de faire du ray tracing sur un ensemble de sphères
*/

typedef struct Sphere {
    int x, y, z;
    int r;
    float pr, pg, pb;
} Sphere;

typedef struct {
    u_int8_t r, g, b;
} pixel;
pixel images[DIM * DIM];

void init_seed() { srand(time(NULL)); }

int rand_int(int a, int b) { return rand() % (b - a) + a; }

float rand_float(float a, float b) {
    return (float)rand() / (float)(RAND_MAX / (b - a)) + a;
}

void init_sphere(Sphere *s) {
    s->x = rand_int(0, DIM);
    s->y = rand_int(0, DIM);
    s->z = rand_int(0, DIM);
    s->r = rand_int(40, 100);
    s->pr = rand_float(0, 100);
    s->pg = rand_float(0, 100);
    s->pb = rand_float(0, 100);
}

int test_pos_valide(Sphere *mesSpheres, int pos) {
    int j, a, b, c;
    for (j = 0; j < pos; j++) {
        a = mesSpheres[j].x - mesSpheres[pos].x;
        b = mesSpheres[j].y - mesSpheres[pos].y;
        c = mesSpheres[j].z - mesSpheres[pos].z;
        if (sqrt(a * a + b * b + c * c) < mesSpheres[j].r + mesSpheres[pos].r) {
            return 0;
        }
    }
    return 1;
}

void generatRandomSpheres(Sphere *mesSpheres) {
    int i = 0;
    init_sphere(&mesSpheres[i++]);
    while (i < NB_SPHERE) {
        init_sphere(&mesSpheres[i]);
        if (test_pos_valide(mesSpheres, i)) i++;
    }
}

float hit(Sphere *mesSpheres, int ox, int oy, float *n) {
    float dx = ox - mesSpheres->x;
    float dy = oy - mesSpheres->y;
    if (dx * dx + dy * dy < mesSpheres->r * mesSpheres->r) {
        float dz = sqrtf(mesSpheres->r * mesSpheres->r - dx * dx - dy * dy);
        *n = dz / sqrtf(mesSpheres->r * mesSpheres->r);
        return dz + mesSpheres->z;
    }
    return -INF;
}

void render(Sphere *mesSpheres) {
    int x, y, i;
    float n;
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            float z = -INF;
            for (i = 0; i < NB_SPHERE; i++) {
                float dz = hit(&mesSpheres[i], x, y, &n);
                if (dz > z) {
                    z = dz;
                    images[y * DIM + x].r =
                        fmaxf(0, fminf(255, (n * 2 * mesSpheres[i].pr)));
                    images[y * DIM + x].g =
                        fmaxf(0, fminf(255, (n * 2 * mesSpheres[i].pg)));
                    images[y * DIM + x].b =
                        fmaxf(0, fminf(255, (n * 2 * mesSpheres[i].pb)));
                }
            }
        }
    }
}

void image_to_ppm(pixel *image, char *filename) {
    FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n %d %d\n %d\n", DIM, DIM, 255);
    for (int i = 0; i < DIM * DIM; i++)
        fprintf(f, "%d %d %d ", image[i].r, image[i].g, image[i].b);
    fclose(f);
}

int main() {
    init_seed();
    Sphere *mesSpheres = malloc(NB_SPHERE * sizeof(Sphere));
    generatRandomSpheres(mesSpheres);
    render(mesSpheres);
    image_to_ppm(images, "image.ppm");
    free(mesSpheres);
    return EXIT_SUCCESS;
}
