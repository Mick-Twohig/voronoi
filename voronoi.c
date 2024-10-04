#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <raymath.h>

#define WIDTH 1000
#define HEIGHT 1000
#define NUM_POINTS 25

Color boundaryColor = (Color) {0, 0, 0, 255};
Color interiorColor = (Color) {255, 255, 255, 255};
int metric = 0; // distance metric

typedef struct {
    float x;
    float y;
    int radius;
    Color color;
    Color pointColor;
} Point;

Point *createRandomPoint(void) {
    Point *p = malloc(sizeof(Point));
    p->x = (float) (rand() % 1000);
    p->y = (float) (rand() % 1000);

    p->color = (Color) {
            (rand() % 255),
            (rand() % 255),
            (rand() % 255),
            255
    };
    p->pointColor = BLUE;
    p->radius = 5;
    return p;
}

typedef struct Node {
    Point *point;
    struct Node *next;
} Node;

Color *calculate(Node *nodeList) {
    Color *pixels = malloc(sizeof(Color) * WIDTH * HEIGHT);
    if (!pixels) {
        fprintf(stderr, "Could not allocate pixel data");
        return NULL;
    }

    for (size_t x = 0; x < WIDTH; ++x) {
        for (size_t y = 0; y < HEIGHT; ++y) {
            //determine the nearest point
            float minDistance = MAXFLOAT;
            Point *closestPoint = NULL;
            Node *iterator = nodeList;
            while (true) {
                if (iterator->point) {
                    float px = iterator->point->x;
                    float py = iterator->point->y;
                    float dx = fabsf(px - x);
                    float dy = fabsf(py - y);
                    float distance;
                    switch (metric) {
                        case 0:
                            distance = fmaxf(dx, dy); // Max
                            break;
                        case 1:
                            distance = dx + dy; // Manhattan distance
                            break;
                        case 2:
                            distance = sqrtf(dx * dx + dy * dy); // Euclidean distance
                    }

                    if (distance < minDistance) {
                        minDistance = distance;
                        closestPoint = iterator->point;
                    }
                    iterator = iterator->next;

                }
                if (!iterator) break;
            };

            *(pixels + y * WIDTH + x) = closestPoint->color;
        }
    }
    return pixels;
}

bool colorsAreEqual(Color c1, Color c2) {
    return c1.r == c2.r &&
           c1.g == c2.g &&
           c1.b == c2.b &&
           c1.a == c2.a;

}

Color *calculateBoundary(Color *pixels) {

    Color *boundary = malloc(sizeof(Color) * WIDTH * HEIGHT);
    if (!boundary) {
        fprintf(stderr, "Could not allocate boundary data");
        return NULL;
    }

    for (int x = 1; x < WIDTH - 1; ++x) {
        for (int y = 1; y < HEIGHT - 1; ++y) {
            // look at the pixels surrounding (x,y); if any of them
            // is a different color than the pixel at (x,y), then we
            // must be on a boundary

            Color c1 = *(pixels + y * WIDTH + x);
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    Color c2 = *(pixels + (y + dy) * WIDTH + (x + dx));
                    if (dx == dy && dy == 0) break;
                    if (colorsAreEqual(c1, c2)) {
                        *(pixels + y * WIDTH + x) = c2;
                    } else {
                        *(pixels + y * WIDTH + x) = boundaryColor;
                        break;
                    }
                }
            }
        }
    }
    return boundary;
}
Point *selectedPoint;

int main(void) {

    srand(time(NULL));

    SetTargetFPS(30);

    Node *nodeList = NULL;
    Node *insertionPoint;

    size_t cap = NUM_POINTS;
    for (size_t i = 0; i < cap; ++i) {

        Node *newNode = malloc(sizeof(Node));
        if (!newNode) {
            fprintf(stderr, "Cannot allocate memory for Node");
            return 1;
        }
        newNode->point = createRandomPoint();
        if (!nodeList) {
            nodeList = newNode;
            insertionPoint = newNode;
        } else {
            insertionPoint->next = newNode;
            insertionPoint = newNode;
        }
    }

    InitWindow(WIDTH, HEIGHT, "Voronoi");

    Color *pixels = calculate(nodeList);
    if (!pixels) {
        fprintf(stderr, "No pixel data\n");
        return 1;
    }

    Color *boundary = calculateBoundary(pixels);
    if (!boundary) {
        fprintf(stderr, "No boundary data\n");
        return 1;
    }


    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_Q)) break;
        if (IsKeyPressed(KEY_M)) {
            metric += 1;
            metric %= 3;
            pixels = calculate(nodeList);
            boundary = calculateBoundary(pixels);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            if (selectedPoint) {
                selectedPoint->pointColor = BLUE;
                selectedPoint = NULL;
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            //determine what point is clicked
            Vector2 mousePos = GetMousePosition();

            selectedPoint = NULL;
            Node *iterator = nodeList;
            while (true) {
                if (iterator->point) {
                    Point *p = iterator->point;
                    if (Vector2Distance(mousePos, (Vector2){p->x, p->y}) <= p->radius){
                        p->pointColor = RED;
                        selectedPoint = p;

                    } else {
                        p->pointColor = BLUE;
                    }
                    iterator = iterator->next;
                }
                if (!iterator) break;
            };
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)){
            if (selectedPoint){
                Vector2 mouseDelta = GetMouseDelta();
                selectedPoint->x += mouseDelta.x;
                selectedPoint->y += mouseDelta.y;
                pixels = calculate(nodeList);
                boundary = calculateBoundary(pixels);
            }
        }

        BeginDrawing();
        ClearBackground((Color) {0x18, 0x18, 0x18, 0xFF});


        // render pixels for the voronoi map
        for (size_t x = 0; x < WIDTH; ++x) {
            for (size_t y = 0; y < HEIGHT; ++y) {
                Color c = *(pixels + y * WIDTH + x);
                DrawPixel(x, y, c);
            }
        }

        // render boundary
        for (int x = 0; x < WIDTH; ++x) {
            for (int y = 0; y < HEIGHT; ++y) {
                Color c = *(boundary + y * WIDTH + x);
                if (colorsAreEqual(c, boundaryColor)) {
                    DrawPixel(x, y, boundaryColor);
                    //DrawCircle(x, y, 5, boundaryColor);
                }
            }
        }

        Node *iterator = nodeList;
        while (true) {
            if (iterator->point) {
//                DrawCircle(iterator->point->x, iterator->point->y, 10, iterator->point->color);
                DrawCircle(iterator->point->x, iterator->point->y, iterator->point->radius, iterator->point->pointColor);
                iterator = iterator->next;
            }
            if (!iterator) break;
        };

        switch (metric) {
            case 0:
                DrawText("Max Metric", 10, HEIGHT - 30, 20, WHITE);
                break;
            case 1:
                DrawText("Manhattan Metric", 10, HEIGHT - 30, 20, WHITE);
                break;
            case 2:
                DrawText("Euclidean Metric", 10, HEIGHT - 30, 20, WHITE);
                break;

        }

        EndDrawing();

    }

    CloseWindow();

    // Cleanup
    Node *iterator = nodeList;
    while (true) {
        if (iterator->point) {
            free(iterator->point);
            iterator = iterator->next;
        }
        if (!iterator) break;
    };
    free(nodeList);

    return 0;
}
