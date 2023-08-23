#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define CHARACTER_WIDTH 3
#define CHARACTER_HEIGHT 3
#define PIPE_WIDTH 6
#define PIPE_GAP 8

typedef struct {
    int x;
    int y;
    int velocity;
} Character;

typedef struct {
    int x;
    int y;
} Pipe;

bool gameOver = false;
int score = 0;

void initializeGame() {
    srand(time(NULL));
}

void drawCharacter(Character* character) {
    for (int i = 0; i < CHARACTER_HEIGHT; i++) {
        move(character->y + i, character->x);
        addstr("o o");
    }
}

void drawPipe(Pipe* pipe) {
    for (int i = 0; i < pipe->y; i++) {
        move(i, pipe->x);
        addstr("=======");
    }
    for (int i = pipe->y + PIPE_GAP; i < SCREEN_HEIGHT; i++) {
        move(i, pipe->x);
        addstr("=======");
    }
}

void generatePipes(Pipe* pipes, int numPipes) {
    int x = SCREEN_WIDTH + PIPE_WIDTH;
    for (int i = 0; i < numPipes; i++) {
        pipes[i].x = x;
        pipes[i].y = rand() % (SCREEN_HEIGHT - PIPE_GAP);
        x += PIPE_WIDTH * 2;
    }
}

bool checkCollision(Character* character, Pipe* pipe) {
    if (character->x + CHARACTER_WIDTH > pipe->x && character->x < pipe->x + PIPE_WIDTH) {
        if (character->y < pipe->y || character->y + CHARACTER_HEIGHT > pipe->y + PIPE_GAP) {
            return true;
        }
    }
    return false;
}

int main() {
    Character character = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, -1};
    Pipe pipes[3];
    initializeGame();
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    while (!gameOver) {
        clear();
        drawCharacter(&character);
        for (int i = 0; i < 3; i++) {
            drawPipe(&pipes[i]);
            pipes[i].x--;
            if (pipes[i].x < -PIPE_WIDTH) {
                pipes[i].x = SCREEN_WIDTH + PIPE_WIDTH;
                pipes[i].y = rand() % (SCREEN_HEIGHT - PIPE_GAP);
            }
            if (checkCollision(&character, &pipes[i])) {
                gameOver = true;
            }
            if (pipes[i].x == character.x) {
                score++;
            }
        }
        mvprintw(0, 0, "Score: %d", score);
        character.velocity++;
        character.y += character.velocity;
        if (character.y < 0 || character.y + CHARACTER_HEIGHT > SCREEN_HEIGHT) {
            gameOver = true;
        }
        refresh();
        usleep(16666);
    }
    endwin();
    printf("Game over. Final score: %d\n", score);
    return 0;
}
