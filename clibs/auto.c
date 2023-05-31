#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void updateField(int *field, int width, int height, int *upd, char *rulesB, char *rulesS, int rulesD);
void setupField(int *field, int width, int height, int *pos, int posDots);
void printField(int *field, int width, int height);
void swapPtrs(int **field, int **upd);

int neigh[8][3]={
                {-1, -1},
                {-1, 0},
                {-1, +1},
                {0, -1},
                {0, +1},
                {+1, 0},
                {+1, -1},
                {+1, +1}
};

struct updArgs {
    int **argField;
    int **argUpd;
    char *argRulesB;
    char *argRulesS;
    int argWidth;
    int argHeight;
    int x1, y1;
    int x2, y2;
    int core;
};

void setupField(int *field, int width, int height, int *pos, int posDots){

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            field[y*width + x]=0;
        }
    }

    for(int y=0; y<posDots; y+=2){
        if(pos[y]<width && pos[y+1]<=height){
            field[pos[y]+width*pos[y+1]] = 1;
        }
    }

}

void printField(int *field, int width, int height){
    printf("-----------\n");
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            printf(" ");
            if(field[x+y*width]==1){
                printf("#");
                continue;
            }
            printf(" ");
        }
        printf("=");
        printf("\n");
    }

}
void updateField(int *field, int width, int height, int *upd, char *rulesB, char *rulesS, int rulesD){
    int count;
    int currCell;
    int xx;
    int yy;
    char countStr[1];

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            count=0;
            for(int inc=0; inc<8; inc++){
                xx=x+neigh[inc][0];
                yy=y+neigh[inc][1];
                if(xx<0 || xx>=width || yy<0 || yy>=height){continue;}
                if(field[xx+yy*width]==1){
                    count++;
                }
            }
            currCell = field[x+y*width];
            //sprintf(countStr, "%d", count);
            countStr[0]=count + '0';
            if(currCell==0 && strstr(rulesB, countStr)!=NULL){
                upd[x+y*width]=1;
            } else if(currCell==1 && strstr(rulesS, countStr)!=NULL){
                upd[x+y*width]=1;
            } else {
                upd[x+y*width]=(currCell!=0 ? upd[x+y*width]+1 : 0);
                upd[x+y*width]%=rulesD;
            }
        }
    }
}

void swapPtrs(int **field, int **upd){
    int *tmp = *field;
    *field = *upd;
    *upd = tmp;
}
