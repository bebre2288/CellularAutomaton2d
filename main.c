/*MIT License

Copyright (c) 2023 Yourchenko Alexandr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Program uses the following files from other authors;
see their respective license headers for more details:
flag.h: Copyright 2021 Alexey Kutepov, MIT License

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "clibs/auto.c"
#define FLAG_IMPLEMENTATION
#include "clibs/flag.h"
#include <raylib.h>
void drawLine(int *field, int width, int height, int x1, int y1, int x2, int y2);
void drawTriangle(int *field, int width, int height, int x1, int y1, int x2, int y2, int x3, int y3);

int main(int argc, char **argv){
    // parse args using "clibs/flag.h"
    char **pRules=flag_str("rules", "b1/s2345678/2", "rules in view b*/s*/*");/**/
    int *pwidth=flag_int("w", 0, "field width");
    int *pheight=flag_int("h", 0, "field height");
    int *pscale=flag_int("scale", 4, "scale of");
    int *psquare=flag_int("square", 256, "squared size instead of width and height");
    int *pdelay=flag_int("fps", 10, "set max FPS");
    int *predVal=flag_int("red", 255/2, "red color value in colored pixels");
    int *pgreenVal=flag_int("green", 255/2, "green color value in colored pixels");
    int *pblueVal=flag_int("blue", 255/2, "blue color value in colored pixels");
    if(argc==1){flag_print_options(); return 1;}
    flag_parse(argc, argv);
    int width=*pwidth;
    int height=*pheight;
    int square=*psquare;
    int scale=*pscale;
    int delay=*pdelay;
    int redVal=*predVal;
    int greenVal=*pgreenVal;
    int blueVal=*pblueVal;
    char rulesB[10];
    char rulesS[10];
    int rulesD;
    int ind=1;
    int arrindex=0;

    // parse born rule
    while((*pRules)[ind]!='/'){
        rulesB[arrindex]=(*pRules)[ind];
        arrindex++;
        ind++;
    }
    arrindex=0;
    // ind+=2 to "jump" over '/s' symbols in rule string
    ind+=2;
    // parse survive rule
    while((*pRules)[ind]!='/'){
        rulesS[arrindex]=(*pRules)[ind];
        arrindex++;
        ind++;
    }
    // get "elderyness" of cell
    rulesD=(*pRules)[++ind]-'1';
    if(width==0 || height==0){
        width=square;
        height=square;
    }

    // setup raylib window
    InitWindow(width*scale, height*scale, "Cellular Automaton");
    SetTargetFPS(delay);
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    // prepair data holders
    int *field=malloc(width*height*sizeof(int));
    int *upd=malloc(width*height*sizeof(int));

    int pos[]={width/2, height/2};
    //#include "clibs/startpos.c"

    setupField(field, width, height, pos, sizeof(pos)/sizeof(int));

    while (!WindowShouldClose()){
        // checking zoom for camera
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        const float zoomIncrement = 0.125f;
        if (IsKeyDown(KEY_W)){
            camera.zoom += zoomIncrement;
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }
        if (IsKeyDown(KEY_S)){
            camera.zoom -= zoomIncrement;
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        };
        BeginDrawing();
            BeginMode2D(camera);
                // iterating over fields
                for(int y=0; y<height; y++){
                    for(int x=0; x<width; x++){
                        // cell must be alive
                        if(field[width*y+x]!=0){
                            // visualise cell "age"
                            int st=field[width*y+x]+1;
                            Color color = {255/st, 255/st, 255/st, 255};
                            // handle canvas scaling of field
                            for(int yinc=0; yinc<scale; yinc++){
                                for(int xinc=0; xinc<scale; xinc++){
                                    DrawPixel(x*scale+xinc, y*scale+yinc, color);
                                }
                            }
                        }
                    }
                }
            EndMode2D();
            DrawFPS(0, 0);
        EndDrawing();
        updateField(field, width, height, upd, rulesB, rulesS, rulesD);
        swapPtrs(&field, &upd); // changes pointers of ready to use field and old iteration
        ClearBackground(WHITE);
    }
    CloseWindow();
    return 0;
}
