#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    int x;
    int y;
    double velocity;
} Character;


void swap(int* first, int* second);
void clearScreen();
void plotPixel(int x, int y, short int lineColor);
void waitForVSync();
void drawLine(int x0, int y0, int x1, int y1, short int color);
void drawVerticalLine(int x, int y0, int y1, short int color);
void drawray(int i1, int j1, short int colour);
void delay();
void drawStartIcon(int xInit, int yInit);
void drawCharacter(Character* character);
void eraseCharacter(Character* character);
void selfpipe();
int Times  = 3;

const int SCREEN_WIDTH = 320; //X
const int SCREEN_HEIGHT = 240; //Y

volatile int pixelBufferStart=0xc8000000; // global variable
#define KEY_BASE 0xFF200050
#define CHARACTER_WIDTH 3
#define CHARACTER_HEIGHT 3
#define PIPE_WIDTH 6
#define PIPE_GAP 8
#define MAX_VELOCITY 8

bool gameOver = false;
int score = 0;



int main()
{

    int i=0;
    clearScreen();
    while(i<240){
    drawLine(0,i,319,239-i,0xFFE0);
    i++;
    }
    delay();
    clearScreen();



    drawStartIcon(SCREEN_WIDTH / 2 - 83 / 2, 200);

    volatile int * kp  = (int *) KEY_BASE;
    unsigned int kv;
    kv =  * kp  ;
    while (1)
    {
     
    kv = *kp;
    if (kv == 8)
    {
        clearScreen();
        break;
    }
    }
    
    Character character = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1};
  int jump=0;
    while(!gameOver)
{
    
    drawCharacter(&character);
    selfpipe();
    delay();
    eraseCharacter(&character);
    

    // Apply gravity to character's velocity
    character.velocity = character.velocity + 0.2;
    
    character.x = character.x +1;
    if(character.x > SCREEN_WIDTH) character.x = 0;
    
    // Apply velocity to character's position
    character.y += character.velocity;

    // Limit the maximum velocity of the character
    if (character.velocity > MAX_VELOCITY)
    {
        character.velocity = MAX_VELOCITY;
    }
    // Check if character has hit the ground
    if (character.y + CHARACTER_HEIGHT >= 238 || character.y - CHARACTER_HEIGHT <= 8)
    {
        // Stop the character from falling and reset velocity
        character.y = 238 - CHARACTER_HEIGHT;
        character.velocity = 0;
    }

    kv=0;
     kv = *kp;
    if (kv == 4)
    {
        jump=20;
        character.velocity = -0.5;
        character.y -= 3;
        kv = 0;
    }
    // while(jump)
    // {
    //     drawCharacter(&character);
    //     delay();
    //     eraseCharacter(&character);
    //     character.y += character.velocity;
    //     character.velocity = (double)character.velocity/2;
    //     jump--;
    //     character.x = character.x +1;
    // }
}

}


void delay()
{

    volatile int *pixel_ctrl_ptr = (int *)0xff203020;
    int status;
    *pixel_ctrl_ptr = 1;
    status = *(pixel_ctrl_ptr + 3);

    while ((status & 0x01) != 0)
    {

        status = *(pixel_ctrl_ptr + 3);
    }
}
void draw_ray(int i1, int j1, short int colour)
{

        plotPixel(i1, j1, colour);
        plotPixel(i1+1, j1, colour);
        plotPixel(i1, j1+1, colour);
        plotPixel(i1-1 , j1, colour);
        plotPixel(i1, j1-1, colour);
        plotPixel(i1+2, j1, colour);
        plotPixel(i1, j1+2, colour);
        plotPixel(i1-2 , j1, colour);
        plotPixel(i1, j1-2, colour);
        plotPixel(i1+1, j1+1, colour);
        plotPixel(i1-1, j1+1, colour);
        plotPixel(i1+1 , j1-1, colour);
        plotPixel(i1-1, j1-1, colour);
}
void drawVerticalLine(int x, int y0, int y1, short int color) {
    if (y0 > y1) {
        swap(&y0, &y1);
    }

    for (int i = y0; i <= y1; ++i) {
        plotPixel(x, i, color);
    }
}

void drawLine(int x0, int y0, int x1, int y1, short int color) {
    bool isSteep = abs(y1 - y0) > abs(x1 - x0);

    if (isSteep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltaX = x1 - x0;
    int deltaY = abs(y1 - y0);
    int error = -(deltaX / 2);
    int y = y0;
    int yStep;

    if (y0 < y1) { yStep = 1; }
    else { yStep = -1; }

    for (int x = x0; x <= x1; ++x) {
        if (isSteep) { plotPixel(y, x, color); }
        else { plotPixel(x, y, color); }

        error += deltaY;
        if (error >= 0) {
            y += yStep;
            error -= deltaX;
        }
    }
}

void waitForVSync() {
    volatile int* pixelCtrlPtr = (int*)0xFF203020;	// pixel ctrl
    register int status;

    *pixelCtrlPtr = 1;	// start sync process
    status = *(pixelCtrlPtr + 3);	//status reg of DMA ctrl

    while ((status & 0x01) != 0) {
        status = *(pixelCtrlPtr + 3);
    }
}
void plotPixel(int x, int y, short int lineColor) {
    *(short int*)(pixelBufferStart + (y << 10) + (x << 1)) = lineColor;
}
void clearScreen() {
    short int black = 0;
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            plotPixel(i, j, black);
        }
    }
}


void swap(int* first, int* second) {
    int temp = *first;
    *first = *second;
    *second = temp;
}

const short int tapToPlayIcon[656] = {
        0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
        0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0,   // 0x0020 (32) pixels
        0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0861, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0030 (48) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x0040 (64) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0020, 0x1082, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000,   // 0x0050 (80) pixels
        0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,   // 0x0060 (96) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0070 (112) pixels
        0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000,   // 0x0080 (128) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0,   // 0x0090 (144) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0x0000,   // 0x00A0 (160) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x00B0 (176) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x00D0 (208) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000,   // 0x00E0 (224) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x10A2, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0,   // 0x00F0 (240) pixels
        0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x0100 (256) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0110 (272) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0861, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x0120 (288) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0,   // 0x0130 (304) pixels
        0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x18E3, 0xFFE0, 0xFFE0,   // 0x0140 (320) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0150 (336) pixels
        0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000,   // 0x0160 (352) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0020, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x0170 (368) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0,   // 0x0180 (384) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,   // 0x0190 (400) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x01A0 (416) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x01B0 (432) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0841, 0x0000, 0x0000,   // 0x01C0 (448) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0,   // 0x01D0 (464) pixels
        0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x18C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x01E0 (480) pixels
        0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x01F0 (496) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x0200 (512) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0841,   // 0x0210 (528) pixels
        0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0220 (544) pixels
        0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x0230 (560) pixels
        0xFFE0, 0xFFE0, 0x0020, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0240 (576) pixels
        0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0,   // 0x0250 (592) pixels
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0,   // 0x0260 (608) pixels
        0xFFE0, 0x0861, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0270 (624) pixels
        0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,   // 0x0280 (640) pixels
        0x2104, 0x0000, 0xFFE0, 0xFFE0, 0x0020, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000,   // 0x0290 (656) pixels
};

void drawStartIcon(int xInit, int yInit) {
    int index = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 82; ++x) {
            plotPixel(xInit + x, yInit + y, tapToPlayIcon[index]);
            index++;
        }
    }
}
void drawCharacter(Character* character) 
 {
    short int colour=0xFFE0;
    for (int i = -4; i <= 4; i++) {
  
        for (int j = -4; j <= 4; j++) {
            if (i == 0 && j == 0) {
                plotPixel(character->x, character->y, colour);
            } else if (i == 1 && j == 1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -1 && j == 1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 1 && j == -1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -1 && j == -1) {
                plotPixel(character->x + i, character->y + j, colour);
            }else if (i == 2 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -2 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == 3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == -3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 2 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -2 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == -2) {
                plotPixel(character->x + i, character->y + j, colour);      
            } else if (j == 0 && i == 3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == -3) {
                plotPixel(character->x + i, character->y + j, colour);      
            }
    }
    }
}
void eraseCharacter(Character* character)
{
    short int colour=0x0000;
    for (int i = -4; i <= 4; i++) {
  
        for (int j = -4; j <= 4; j++) {
            if (i == 0 && j == 0) {
                plotPixel(character->x, character->y, colour);
            } else if (i == 1 && j == 1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -1 && j == 1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 1 && j == -1) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -1 && j == -1) {
                plotPixel(character->x + i, character->y + j, colour);
            }else if (i == 2 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -2 && j == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == 3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 0 && j == -3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == 2 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (i == -2 && j == -2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == 2) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == -2) {
                plotPixel(character->x + i, character->y + j, colour);      
            } else if (j == 0 && i == 3) {
                plotPixel(character->x + i, character->y + j, colour);
            } else if (j == 0 && i == -3) {
                plotPixel(character->x + i, character->y + j, colour);      
            }
    }
    }
}

void selfpipe(){
    short int colour=0xFFE0;
    // xinit is giving x space , y init is giving y space from top left corner each

    //x and y are giving it height and width only
    int xInit = 10,yInit =  0;
        // printf("Hello, World!");
    // for(int tt = 0;  tt <3; tt++){
        for(int tst = 1; tst <Times ; tst++){
            int h1 = rand()%(100);
            for (int y = 0; y < h1; ++y) {
                for (int x = 0; x < 10; ++x) {
                    plotPixel(xInit + x, y, colour);
                }
            }
            
            int h2 = SCREEN_HEIGHT - h1;

            for (int y = SCREEN_HEIGHT; y > h2; --y) {
                for (int x = 0; x < 10; ++x) {
                    plotPixel(xInit + x,y, colour);
                }
            }
            xInit += tst*20;
            
        }

        // xInit += 40;
        // if(tt%2 == 0) yInit =  SCREEN_HEIGHT;
        // else yInit =  0;
        
    // }
}
