#include "clibs/auto.c"
//#include "clibs/startpos.c"
#define FLAG_IMPLEMENTATION
#include "clibs/flag.h"
#include <raylib.h>
void drawLine(int *field, int width, int height, int x1, int y1, int x2, int y2);
void drawTriangle(int *field, int width, int height, int x1, int y1, int x2, int y2, int x3, int y3);

int main(int argc, char **argv){

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

    while((*pRules)[ind]!='/'){
        rulesB[arrindex]=(*pRules)[ind];
        arrindex++;
        ind++;
    }
    arrindex=0;
    ind+=2;
    while((*pRules)[ind]!='/'){
        rulesS[arrindex]=(*pRules)[ind];
        arrindex++;
        ind++;
    }
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

    int *field=malloc(width*height*sizeof(int));
    int *upd=malloc(width*height*sizeof(int));
    //int pos[]={};
    int pos[]={width/2, height/2};
    /*int pos[]={
        1,1, 2,1,
        1,2, 2,2,
                    3,3, 4,3,
                    3,4, 4,4
    };
    int pos[]={ 10,20,
				10,20-1,
			    10,20-2,
			    10-1,20,
			    10-2,20};
    */

    setupField(field, width, height, pos, sizeof(pos)/sizeof(int));
    printf("started\n");

    int i=0;
    while (!WindowShouldClose()){
        // Zoom
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
        i++;
        BeginDrawing();
            BeginMode2D(camera);
                for(int y=0; y<height; y++){
                    for(int x=0; x<width; x++){
                        if(field[width*y+x]!=0){
                            unsigned char dec=1;
                            for(int i=field[width*y + x]; i!=1; i--){
                                dec+=(255/(rulesD/2));
                            }
                            int st=field[width*y+x]+1;
                            Color color = {255/st, 255/st, 255/st, 255};
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
        swapPtrs(&field, &upd);
        ClearBackground(WHITE);
    }
    CloseWindow();
    return 0;
}
