#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//for animation
#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors  for displaying colors*/
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

/*this is to get absolute as we could not use bits/ library*/
#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1
// for animation

typedef struct
{
    int x;
    int y;
    double velocity;
} Character;

typedef struct
{
    int x;
    int y;
} Pipe;

void swap(int *first, int *second);
void clearScreen();
void plotPixel(int x, int y, short int lineColor);
void waitForVSync();
void drawLine(int x0, int y0, int x1, int y1, short int color);
void drawVerticalLine(int x, int y0, int y1, short int color);
void drawray(int i1, int j1, short int colour);
void delay();
void drawStartIcon(int xInit, int yInit);
void drawCharacter(Character *character);
void eraseCharacter(Character *character);
void selfpipe();
void drawpipe(int x, int y);
void cleararray();
void write_char(int x, int y, char c);
void draw_box(int x, int y, short int colour);

const int SCREEN_WIDTH = 320;  // X
const int SCREEN_HEIGHT = 240; // Y

volatile int pixelBufferStart = 0xc8000000; // global variable
#define KEY_BASE 0xFF200050
#define CHARACTER_WIDTH 3
#define CHARACTER_HEIGHT 3
#define PIPE_WIDTH 6
#define PIPE_GAP 50
#define MAX_VELOCITY 8


bool gameOver = false;
int score = 0;
int level = 1;
int array[320][240] = {0};

int main()
{

    int i = 0;
    clearScreen();
    while (i < 240)
    {
        drawLine(0, i, 319, 239 - i, 0xFFE0);
        i++;
    }

    delay();
    clearScreen();

    drawStartIcon(SCREEN_WIDTH / 2 - 83 / 2, 200);


    volatile int *kp = (int *)KEY_BASE;
    unsigned int kv;
    kv = *kp;
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
    Pipe pipe = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    int jump = 0;
    selfpipe();
    while (!gameOver)
    {

        drawCharacter(&character);
        delay();
        eraseCharacter(&character);
        if (character.x == 319)
        {
            clearScreen();
            cleararray();
            level++;
            selfpipe();
        }

        // Apply gravity to character's velocity
        character.velocity = character.velocity + 0.2;

        character.x = character.x + 1;
        if (character.x > SCREEN_WIDTH)
            character.x = 0;

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
        kv = 0;
        kv = *kp;
        if (kv == 4)
        {
            jump=20;
            character.velocity = -0.5;
            character.y -= 2;
            kv = 0;
        }
        // while (jump)
        // {
        //     drawCharacter(&character);
        //     delay();
        //     eraseCharacter(&character);
        //     character.y += character.velocity;
        //     jump--;
        //     character.x = character.x + 1;
        //           if(array[character.x][character.y]==1)
        // {
        //     gameOver=true;
        // }
        // }
        if(array[character.x+CHARACTER_WIDTH][character.y+CHARACTER_HEIGHT]==1)
        {
            gameOver=true;
        }
        if(array[character.x+CHARACTER_WIDTH][character.y-CHARACTER_HEIGHT]==1)
        {
            gameOver=true;
        }
    }

    if(gameOver == true){
        clearScreen();

        volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
        // declare other variables(not shown)
        // initialize location and direction of rectangles(not shown)

        /* set front pixel buffer to start of FPGA On-chip memory */
        *(pixel_ctrl_ptr + 1) = 0xc8000000; // first store the address in the back buffer
        /* now, swap the front/back buffers, to set the front buffer location */
        waitForVSync();
        /* initialize a pointer to the pixel buffer, used by drawing functions */
        /* set back pixel buffer to start of SDRAM memory */
        *(pixel_ctrl_ptr + 1) = 0xc0000000;


        int x2[8];
        int y2[8];

        int dx[8];
        int dy[8];

        int prev_x[8];
        int prev_y[8];

        for(int i = 0; i<8 ; i++){
            x2[i] = rand()%320; 
            y2[i] = rand()%240;
            dx[i] = (rand()%2)*2-1; 
            dy[i] = (rand()%2)*2-1;
            prev_x[i] = 0;
            prev_y[i] = 0;
        }


        while (1)
        {
            //clear_screen();
            for(int i = 0; i<8 ; i++){
                /* Erase any boxes and lines that were drawn in the last iteration */
                draw_box(prev_x[i], prev_y[i], 0);
                drawLine(prev_x[i], prev_y[i], prev_x[(i+1)%8], prev_y[(i+1)%8], 0); 
            }

            for(int i = 0; i<8 ; i++){  
                // code for updating the locations of boxes (not shown)
                if(x2[i] <= 0) dx[i] = 1;
                if(x2[i] >= 319) dx[i] = -1;

                if(y2[i] <= 0) dy[i] = 1;
                if(y2[i] >= 239) dy[i] = -1;
                
                prev_x[i] = x2[i];
                prev_y[i] = y2[i];
                
                x2[i] += dx[i];
                y2[i] += dy[i]; 
            }

            // code for drawing the boxes and lines (not shown)
            draw_box(x2[0], y2[0], WHITE);
            drawLine(x2[0], y2[0], x2[1], y2[1], WHITE);
            
            draw_box(x2[1], y2[1], YELLOW);
            drawLine(x2[1], y2[1], x2[2], y2[2], YELLOW);

            draw_box(x2[2], y2[2], RED);
            drawLine(x2[2], y2[2], x2[3], y2[3], RED);

            draw_box(x2[3], y2[3], GREEN);
            drawLine(x2[3], y2[3], x2[4], y2[4], GREEN);

            draw_box(x2[4], y2[4], BLUE);
            drawLine(x2[4], y2[4], x2[5], y2[5], BLUE);

            draw_box(x2[5], y2[5], CYAN);
            drawLine(x2[5], y2[5], x2[6], y2[6], CYAN);

            draw_box(x2[6], y2[6], ORANGE);
            drawLine(x2[6], y2[6], x2[7], y2[7], ORANGE);

            draw_box(x2[7], y2[7], PINK);
            drawLine(x2[7], y2[7], x2[0], y2[0], PINK);
            
            waitForVSync(); // swap front and back buffers on VGA vertical sync
        }
        
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
    plotPixel(i1 + 1, j1, colour);
    plotPixel(i1, j1 + 1, colour);
    plotPixel(i1 - 1, j1, colour);
    plotPixel(i1, j1 - 1, colour);
    plotPixel(i1 + 2, j1, colour);
    plotPixel(i1, j1 + 2, colour);
    plotPixel(i1 - 2, j1, colour);
    plotPixel(i1, j1 - 2, colour);
    plotPixel(i1 + 1, j1 + 1, colour);
    plotPixel(i1 - 1, j1 + 1, colour);
    plotPixel(i1 + 1, j1 - 1, colour);
    plotPixel(i1 - 1, j1 - 1, colour);
}
void drawVerticalLine(int x, int y0, int y1, short int color)
{
    if (y0 > y1)
    {
        swap(&y0, &y1);
    }

    for (int i = y0; i <= y1; ++i)
    {
        plotPixel(x, i, color);
    }
}

void drawLine(int x0, int y0, int x1, int y1, short int color)
{
    bool isSteep = abs(y1 - y0) > abs(x1 - x0);

    if (isSteep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltaX = x1 - x0;
    int deltaY = abs(y1 - y0);
    int error = -(deltaX / 2);
    int y = y0;
    int yStep;

    if (y0 < y1)
    {
        yStep = 1;
    }
    else
    {
        yStep = -1;
    }

    for (int x = x0; x <= x1; ++x)
    {
        if (isSteep)
        {
            plotPixel(y, x, color);
        }
        else
        {
            plotPixel(x, y, color);
        }

        error += deltaY;
        if (error >= 0)
        {
            y += yStep;
            error -= deltaX;
        }
    }
}

void waitForVSync()
{
    volatile int *pixelCtrlPtr = (int *)0xFF203020; // pixel ctrl
    register int status;

    *pixelCtrlPtr = 1;            // start sync process
    status = *(pixelCtrlPtr + 3); // status reg of DMA ctrl

    while ((status & 0x01) != 0)
    {
        status = *(pixelCtrlPtr + 3);
    }
}
void plotPixel(int x, int y, short int lineColor)
{
    *(short int *)(pixelBufferStart + (y << 10) + (x << 1)) = lineColor;
}
void clearScreen()
{
    short int black = 0;
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT; j++)
        {
            plotPixel(i, j, black);
        }
    }
}

void swap(int *first, int *second)
{
    int temp = *first;
    *first = *second;
    *second = temp;
}

const short int tapToPlayIcon[656] = {
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, // 0x0010 (16) pixels
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, // 0x0020 (32) pixels
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0861, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0030 (48) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x0040 (64) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0020, 0x1082, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, // 0x0050 (80) pixels
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, // 0x0060 (96) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0070 (112) pixels
    0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, // 0x0080 (128) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0, // 0x0090 (144) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0x0000, // 0x00A0 (160) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x00B0 (176) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x00C0 (192) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x00D0 (208) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, // 0x00E0 (224) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x10A2, 0xFFE0, 0xFFE0, 0x0000, 0xFFE0, // 0x00F0 (240) pixels
    0xFFE0, 0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x0100 (256) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0110 (272) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0861, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x0120 (288) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, // 0x0130 (304) pixels
    0xFFE0, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x18E3, 0xFFE0, 0xFFE0, // 0x0140 (320) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0150 (336) pixels
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, // 0x0160 (352) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0020, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x0170 (368) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, // 0x0180 (384) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, // 0x0190 (400) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x01A0 (416) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, // 0x01B0 (432) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0841, 0x0000, 0x0000, // 0x01C0 (448) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, // 0x01D0 (464) pixels
    0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x18C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x01E0 (480) pixels
    0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x01F0 (496) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x0200 (512) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0841, // 0x0210 (528) pixels
    0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0220 (544) pixels
    0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x0230 (560) pixels
    0xFFE0, 0xFFE0, 0x0020, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0240 (576) pixels
    0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0xFFE0, // 0x0250 (592) pixels
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, // 0x0260 (608) pixels
    0xFFE0, 0x0861, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0270 (624) pixels
    0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, // 0x0280 (640) pixels
    0x2104, 0x0000, 0xFFE0, 0xFFE0, 0x0020, 0x0000, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, // 0x0290 (656) pixels
};

void drawStartIcon(int xInit, int yInit)
{
    int index = 0;
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 82; ++x)
        {
            plotPixel(xInit + x, yInit + y, tapToPlayIcon[index]);
            index++;
        }
    }
}
void drawCharacter(Character *character)
{
    short int colour = 0xFFE0;
    for (int i = -4; i <= 4; i++)
    {

        for (int j = -4; j <= 4; j++)
        {
            if (i == 0 && j == 0)
            {
                plotPixel(character->x, character->y, colour);
            }
            else if (i == 1 && j == 1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -1 && j == 1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 1 && j == -1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -1 && j == -1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 2 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -2 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == 3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == -3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 2 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -2 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == 3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == -3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
        }
    }
}
void eraseCharacter(Character *character)
{
    short int colour = 0x0000;
    for (int i = -4; i <= 4; i++)
    {

        for (int j = -4; j <= 4; j++)
        {
            if (i == 0 && j == 0)
            {
                plotPixel(character->x, character->y, colour);
            }
            else if (i == 1 && j == 1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -1 && j == 1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 1 && j == -1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -1 && j == -1)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 2 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -2 && j == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == 3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 0 && j == -3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == 2 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (i == -2 && j == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == 2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == -2)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == 3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
            else if (j == 0 && i == -3)
            {
                plotPixel(character->x + i, character->y + j, colour);
            }
        }
    }
}

void selfpipe()
{
    short int colour = 0xFFE0;
    int diff = 319 / (level + 1);

    for (int x = diff / 2; x < 320; x = x + diff)
    {
        int y = rand() % (SCREEN_HEIGHT - PIPE_GAP);
        drawpipe(x, y);
    }
}
void drawpipe(int x, int y)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = x; j < x + 11; j++)
        {
            plotPixel(j, i, 0x07FF);
            array[j][i]=1;
        }
    }
    for (int i = y + PIPE_GAP; i < SCREEN_HEIGHT; i++)
    {
        for (int j = x; j < x + 11; j++)
        {
            plotPixel(j, i, 0x07FF);
            array[j][i]=1;
        }
    }
}
void cleararray()
{
    for(int x=0; x<320; x++)
    {
        for(int y=0; y<240; y++)
        {
            array[x][y]=0;
        }
    }
}
void write_char(int x, int y, char c)
{
    // VGA character buffer
    volatile char *cha = (char *)(0xc9000000 + (y << 7) + x);
    *cha = c;
}


void draw_box(int x, int y, short int colour){
    plotPixel(x,y,colour);
    plotPixel(x+1,y,colour);
    plotPixel(x,y+1,colour);
    plotPixel(x+1,y+1,colour);
}