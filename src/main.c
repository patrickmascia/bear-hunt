#include <stm32f031x6.h>
#include "display.h"
#include <stdio.h>
#include <sound.h>
#include <musical_notes.h>

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

typedef struct
{
    uint32_t note;
    uint32_t duration;
} SongNote;

// 0 = silence/rest
static const SongNote menuSong[] = {
    {E5, 200}, {0, 40},
    {G5, 200}, {0, 40},
    {A5, 240}, {0, 60},
    {G5, 200}, {0, 40},
    {E5, 240}, {0, 80},
    {D5, 200}, {0, 40},
    {F5, 200}, {0, 40},
    {G5, 240}, {0, 60},
    {F5, 200}, {0, 40},
    {D5, 240}, {0, 80},
    {E5, 200}, {0, 40},
    {G5, 200}, {0, 40},
    {B5, 260}, {0, 60},
    {A5, 200}, {0, 40},
    {G5, 260}, {0, 80},
    {E5, 200}, {0, 40},
    {D5, 200}, {0, 40},
    {E5, 240}, {0, 60},
    {G5, 200}, {0, 40},
    {E5, 300}, {0, 120},
};

static const uint32_t menuSongLength = sizeof(menuSong) / sizeof(menuSong[0]);

int speed = 3;
int hasFinished = 0;
int newGame = 1;
int showStartMenu = 0;
volatile uint32_t milliseconds;

//specifies the current playing note
uint32_t songIndex = 0;
//specifies when the note started playing
uint32_t noteStartTime = 0;
// checks if menu song is playing
int songPlaying = 0;
// used for 30s timer, stores last beep time in order to beep every 1000ms
uint32_t lastBeepTime = 0;

/* ===================== SPRITES (CHANGED) ===================== */
const uint16_t flower[] = {0,0,0,20106,20106,20106,20106,0,0,0,0,0,0,20106,31419,31419,20106,0,0,0,0,0,0,20106,31419,31419,20106,0,0,0,20106,20106,20106,20106,20106,20106,20106,20106,20106,20106,20106,31419,20106,24375,24375,24375,24375,20106,31419,20106,20106,31419,20106,24375,24375,24375,24375,20106,31419,20106,20106,31419,20106,24375,24375,24375,24375,20106,31419,20106,20106,31419,20106,24375,24375,24375,24375,20106,31419,20106,20106,20106,20106,20106,20106,20106,20106,20106,20106,20106,0,0,0,20106,31419,31419,20106,0,0,0,52533,52533,52533,20106,20106,20106,20106,0,0,0,52533,45911,45911,52533,52533,52533,0,0,0,0,52533,52533,45911,45911,52533,52533,0,0,0,0,0,52533,52533,45911,45911,52533,0,0,0,0,0,0,0,52533,52533,52533,0,0,0,0,0,0,0,0,52533,52533,0,0,0,0,0,0,0,0,52533,52533,0,0,0,0,};
const uint16_t bear_front_1[] = {0,0,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,52505,52505,52505,52505,52505,0,52505,52505,52505,21803,21803,52505,52505,52505,52505,52505,52505,21803,21803,52505,52505,52505,52505,52505,21803,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,21803,52505,52505,0,52505,52505,52505,52505,65535,65535,52505,52505,65535,65535,52505,52505,52505,52505,0,0,0,52505,52505,52505,65535,26332,52505,52505,26332,65535,52505,52505,52505,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,52505,55763,52505,52505,21803,21803,52505,52505,55763,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,55763,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,55763,52505,55763,52505,52505,52505,52505,52505,21803,21803,52505,52505,52505,52505,52505,55763,52505,0,52505,52505,0,52505,52505,21803,21803,21803,21803,52505,52505,0,52505,52505,0,0,0,0,0,52505,52505,21803,21803,21803,21803,52505,52505,0,0,0,0,0,0,0,52505,52505,52505,21803,21803,21803,21803,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,21803,21803,21803,21803,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,52505,55763,52505,0,0,52505,55763,52505,0,0,0,0};
const uint16_t bear_front_2[] = {0,0,52505,52505,0,0,0,0,0,0,0,0,52505,52505,0,0,0,52505,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,21803,21803,52505,52505,52505,52505,52505,52505,52505,52505,21803,21803,52505,52505,52505,52505,21803,52505,52505,65535,65535,52505,52505,65535,65535,52505,52505,21803,52505,52505,0,52505,52505,52505,52505,65535,26332,52505,52505,26332,65535,52505,52505,52505,52505,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,52505,55763,52505,52505,21803,21803,52505,52505,55763,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,55763,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,55763,52505,55763,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,55763,52505,0,52505,52505,0,52505,52505,52505,52505,52505,52505,52505,52505,0,52505,52505,0,0,0,0,0,52505,52505,52505,21803,21803,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,21803,21803,21803,21803,52505,52505,0,0,0,0,0,0,0,0,52505,52505,21803,21803,21803,21803,52505,52505,0,0,0,0,0,0,0,52505,52505,52505,21803,21803,21803,21803,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,21803,21803,21803,21803,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,52505,55763,52505,0,0,52505,55763,52505,0,0,0,0};
const uint16_t bear_top_1[] = {0,0,0,0,24576,44313,55763,52505,52505,55763,52505,24576,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,36121,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,36121,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,8192,52505,19217,19217,19217,19217,44313,16384,0,0,0,0,0,0,0,0,0,19217,52505,52505,52505,52505,19217,0,0,0,0,0,0,0,0,0,52505,52505,19217,52505,52505,55763,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,0,0,52505,52505,52505,52505,52505,52505,19217,65535,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19156,52505,52505,52505,52505,19217,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19156,52505,52505,52505,52505,52505,52505,52505,0,52505,52505,52505,52505,52505,52505,19217,65535,52505,52505,52505,52505,52505,52505,0,0,0,0,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,52505,52505,19217,52505,52505,55763,52505,52505,0,0,0,0,0,0,0,0,0,19217,52505,52505,52505,52505,19217,0,0,0,0,0,0,0,0,0,0,52505,19217,19217,19217,19217,52505,0,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,52505,55763,52505,52505,55763,52505,0,0,0,0,0};
const uint16_t bear_top_2[] = {0,0,0,0,24576,0,0,0,52505,52505,55763,52505,52505,0,0,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,55763,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,8192,52505,19217,19217,19217,19217,44313,52505,0,0,0,0,0,0,0,0,0,19217,52505,52505,52505,52505,19217,0,0,0,0,0,0,0,0,0,52505,52505,19217,52505,52505,55763,52505,52505,0,0,0,0,0,0,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,0,0,52505,52505,52505,52505,52505,52505,19217,65535,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19156,52505,52505,52505,52505,19217,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19217,52505,52505,52505,52505,52505,52505,52505,52505,52505,52505,19156,52505,52505,52505,52505,52505,52505,52505,0,52505,52505,52505,52505,52505,52505,19217,65535,52505,52505,52505,52505,52505,52505,0,0,0,0,52505,52505,52505,19217,21803,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,52505,52505,19217,52505,52505,55763,52505,52505,0,0,0,0,0,0,0,0,0,19217,52505,52505,52505,52505,19217,0,0,0,0,0,0,0,0,0,0,52505,19217,19217,19217,19217,52505,0,0,0,0,0,0,0,0,0,52505,44313,19217,19217,19217,19217,52505,8192,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,55763,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,52505,52505,52505,52505,52505,52505,52505,0,0,0,0,0,0,0,0,0,0,52505,52505,55763,52505,52505,0,0,0,24576,0,0,0,0};
/* ==================================================================== */

/* --------- Psuedo Random number generator --------- */
static uint32_t rng_state = 0xA5A5A5A5;

//shifts values around to make it look random
static uint32_t xorshift32(void)
{
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

//choosing where the flower can spawn
static uint16_t randRange(uint16_t min, uint16_t max)
{
    uint16_t span = (max - min) + 1;
    return (uint16_t)(min + (xorshift32() % span));
}

/* --------- Collision (when sprite hits the flower)--------- */
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
    uint16_t x2 = x1 + w;
    uint16_t y2 = y1 + h;

    if ((px >= x1) && (px <= x2) && (py >= y1) && (py <= y2))
        return 1;
    return 0;
}

//checking if any of the corners of the spite is inside the parameters; colliding with the flower
static int playerTouchesTarget(uint16_t tx, uint16_t ty, uint16_t px, uint16_t py)
{
    // player is 12x16
    return isInside(tx, ty, 16, 24, px, py) ||
           isInside(tx, ty, 16, 24, px + 16, py) ||
           isInside(tx, ty, 16, 24, px, py + 24) ||
           isInside(tx, ty, 16, 24, px + 16, py + 24);
}

/* --------- HUD: Score + Level --------- */
static int calcLevel(int score)
{
    int lvl = (score / 10) + 1;
    if (lvl > 3)
        lvl = 3;
    return lvl;
}

//drawing the score and the level
static void drawHUD(int score)
{
    int level = calcLevel(score);

    char bufS[16];
    char bufL[16];

    sprintf(bufS, "S:%d", score);
    sprintf(bufL, "L:%d", level);

    // clear HUD areas
    fillRectangle(0, 0, 70, 16, 0);  // score area
    fillRectangle(92, 0, 36, 16, 0); // level area (assumes ~128px wide)

    printTextX2(bufS, 2, 2, RGBToWord(0xff, 0xff, 0xff), 0);
    printTextX2(bufL, 92, 2, RGBToWord(0xff, 0xff, 0xff), 0);
}

/* --------- Timer display (Level 3) --------- */
static void drawTimerSeconds(int secondsLeft)
{
    char bufT[16];
    if (secondsLeft < 0)
        secondsLeft = 0;
    sprintf(bufT, "T:%d", secondsLeft);

    // bottom-right area (assumes ~128x160 display)
    fillRectangle(82, 144, 44, 16, 0);
    printTextX2(bufT, 82, 144, RGBToWord(0xff, 0xff, 0xff), 0);
}

/* --------- Erase player --------- */
static void erasePlayer(uint16_t px, uint16_t py)
{
    fillRectangle(px, py, 16, 24, 0);
}

static void drawPlayer(uint16_t px, uint16_t py, const uint16_t *img, int hinv, int vinv){
    putImage(px, py, 16, 24, img, hinv, vinv);
}

/* --------- flower spawn/respawn --------- */
static void spawnflowerAtNewPos(uint16_t *flowerX, uint16_t *flowerY, uint16_t px, uint16_t py)
{
    do
    {
        *flowerX = randRange(10, 110);
        *flowerY = randRange(16, 140);
    } while (playerTouchesTarget(*flowerX, *flowerY, px, py));
}
// sound that plays if you pickup a flower
void playFlowerPickupSound(){
    playNote(C4);
    delay(100);
    playNote(F4);
    delay(100);
    playNote(C5);
    delay(100);
    stopNote();
}
// song that plays if you win
void playWinSong(){
    playNote(E5);
    delay(120);
    playNote(G5);
    delay(120);
    playNote(A5);
    delay(150);
    playNote(E6);
    delay(250);
    playNote(A5);
    delay(120);
    playNote(C6);
    delay(300);
    stopNote();
}
// song that plays if u lose
void playLoseSong(){
    playNote(A5);
    delay(150);
    playNote(F5);
    delay(150);
    playNote(D5);
    delay(180);
    playNote(B4);
    delay(220);
    playNote(A4);
    delay(300);
    stopNote();
}
int main()
{
    int hinverted = 0;
    int vinverted = 0;
    int toggle = 0;
    int hmoved = 0;
    int vmoved = 0;

    uint16_t x = 50, y = 50;
    uint16_t oldx = x, oldy = y;

    uint16_t flowerX = 20, flowerY = 80;

    int score = 0;

    // flower visibility state machine
    int flowerVisible = 1;
    uint32_t flowerStateStart = 0;

    // level 3 challenge
    int inLevel3Challenge = 0;
    uint32_t level3StartMs = 0;
    int level3StartScore = 0;

    initClock();
    initSysTick();
    setupIO();  
    initSound();

    rng_state ^= (milliseconds + 0x12345678);

    flowerVisible = 1;
    flowerStateStart = milliseconds;

    
    while (1)
    {
        
        //plays the song menu if it's playing
        if (songPlaying){
            // change note only if elapsed time since last note is greater than it's duration
            if ((milliseconds - noteStartTime) >= menuSong[songIndex].duration)
            {
                songIndex++;

                if (songIndex >= menuSongLength)
                {
                    songIndex = 0; // loop song from start
                }

                noteStartTime = milliseconds;
                //silence if empty note
                if (menuSong[songIndex].note == 0)
                    stopNote();
                else
                    playNote(menuSong[songIndex].note);
            }
        }

        // start menu logic
        if(newGame){
            if(!showStartMenu){
                playNote(C5);
                printTextX2("Bear Hunt", 10, 60, RGBToWord(0x00, 0xff, 0x00), 0);
                printText("press UP to start", 5, 90, RGBToWord(0x00, 0xff, 0x00), 0);
                showStartMenu = 1;

                songPlaying = 1;
                songIndex = 0;
                noteStartTime = milliseconds;

                if (menuSong[0].note == 0)
                    stopNote();
                else
                    playNote(menuSong[0].note);
            }
             if ((GPIOA->IDR & (1 << 8)) == 0){
                fillRectangle(0, 16, 128, 160, 0);
                drawHUD(score);
                stopNote();
                songPlaying = 0;
                newGame = 0;
            }
            continue;
        } 

        if (hasFinished)
        {
            if ((GPIOA->IDR & (1 << 8)) == 0) // up
            {
                
                //checking if your holding the up button or not and resetting when it reaches a certain time
                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("..", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 100, 100, 110, 0);

                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("....", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 100, 100, 110, 0);

                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("......", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 100, 100, 110, 0);
                
                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("........", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 100, 100, 110, 0);

                //delay(2000);
                if ((GPIOA->IDR & (1 << 8)) == 0) // up
                {
                    
                    //resets to start state
                    hasFinished = 0;
                    hinverted = 0;
                    vinverted = 0;
                    toggle = 0;
                    hmoved = 0;
                    vmoved = 0;
                    speed = 3;
                    
                    x = 50, y = 50;
                    oldx = x, oldy = y;
                    
                    flowerX = 20, flowerY = 80;
                    
                    score = 0;
                    
                    // flower visibility state machine
                    flowerVisible = 1;
                    flowerStateStart = 0;
                    
                    // level 3 challenge
                    inLevel3Challenge = 0;
                    level3StartMs = 0;
                    level3StartScore = 0;


                    //resetting the screen and redrawing the HUD
                    fillRectangle(0, 16, 128, 160, 0);
                    drawHUD(score);
                }
            }
        }
        else
        {

            int level = calcLevel(score);

            // set flower speed by level (disappear faster each level)
            // Level 1: slower, Level 2: faster, Level 3: super quick
            uint32_t visibleMs, hiddenMs;
            if (level == 1)
            {
                visibleMs = 2000;
                hiddenMs = 300;
            }
            else if (level == 2)
            {
                visibleMs = 1000;
                hiddenMs = 200;
            }
            else
            {
                visibleMs = 500;
                hiddenMs = 120;
            } // level 3: super quick

            // LEVEL 3: start challenge as soon as you ENTER level 3 (score >= 20)
            if (level == 3 && !inLevel3Challenge)
            {
                // rests 30s beep timer
                lastBeepTime = milliseconds;

                inLevel3Challenge = 1;
                level3StartMs = milliseconds;
                level3StartScore = score;

                // show timer immediately
                drawTimerSeconds(30);
            }

            
            // flower appear/disappear state machine (runs even if player doesn't move)
            uint32_t now = milliseconds;
            if (flowerVisible)
            {
                if ((now - flowerStateStart) >= visibleMs)
                {
                    // hide flower
                    fillRectangle(flowerX, flowerY, 10, 17, 0);
                    flowerVisible = 0;
                    flowerStateStart = now;
                }
            }
            else
            {
                if ((now - flowerStateStart) >= hiddenMs)
                {
                    // show flower at a new random position
                    spawnflowerAtNewPos(&flowerX, &flowerY, x, y);
                    putImage(flowerX, flowerY, 10, 17, flower, 0, 0);
                    drawHUD(score);
                    flowerVisible = 1;
                    flowerStateStart = now;
                }
            }
            
            // read buttons
            hmoved = vmoved = 0;
            hinverted = vinverted = 0;
            
            if ((GPIOB->IDR & (1 << 4)) == 0) // right
            {
                if (x < (112-speed))
                {
                    x += speed;
                    hmoved = 1;
                    hinverted = 0;
                }
            }
            
            if ((GPIOB->IDR & (1 << 5)) == 0) // left
            {
                if (x > (speed))
                {
                    x -= speed;
                    hmoved = 1;
                    hinverted = 1;
                }
            }
            
            if ((GPIOA->IDR & (1 << 11)) == 0) // down
            {
                if (y < (136-speed))
                {
                    y += speed;
                    vmoved = 1;
                    vinverted = 0;
                }
            }
            
            if ((GPIOA->IDR & (1 << 8)) == 0) // up
            {
                if (y > (24))
                {
                    y -= speed;
                    vmoved = 1;
                    vinverted = 1;
                }
            }
            
            // redraw only on movement
            if (vmoved || hmoved)
            {
                erasePlayer(oldx, oldy);
                oldx = x;
                oldy = y;
                
                // horizontal movement
                if (hmoved)
                {
                    // switch between different frames
                    const uint16_t *frame = toggle ? bear_top_1 : bear_top_2;
                    if (!hasFinished)
                    drawPlayer(x, y, frame, hinverted, 0);
                    toggle ^= 1;
                }
                else
                {
                    // switch between different frames
                    const uint16_t *frame = toggle ? bear_front_1 : bear_front_2;
                    
                    if (!hasFinished)
                    drawPlayer(x, y, frame, 0, vinverted);
                    
                    toggle ^= 1;
                }
                
                // collect flower only if it's visible
                if (flowerVisible && playerTouchesTarget(flowerX, flowerY, x, y))
                {
                    playNote(C5);
                    // collect
                    score++;
                    drawHUD(score);
                    
                    // every multiple of 10 -> speed up
                    if ((score % 10) == 0){
                        speed += 4;
                        playFlowerPickupSound();
                    }
                    
                    // immediately hide & respawn cycle
                    fillRectangle(flowerX, flowerY, 10, 17, 0);
                    flowerVisible = 0;
                    flowerStateStart = milliseconds;
                    delay(20);
                    stopNote();
                }
            }
            // LEVEL 3: timer + win/lose condition (need +10 flowers in 30 seconds)
            if (inLevel3Challenge)
            {
                uint32_t elapsed = milliseconds - level3StartMs;
                int secondsLeft = 30 - (int)(elapsed / 1000);

                drawTimerSeconds(secondsLeft);

                // SIMPLE BEEP EVERY 1s
                if (milliseconds - lastBeepTime >= 1000)
                {
                    lastBeepTime = milliseconds;
                    //play higher pitch in last 5s
                    if(secondsLeft>5)
                        playNote(C3);
                    else
                        playNote(C6);
                    delay(30);
                    stopNote();
                }
                // win early if got 10 flowers during level 3
                if ((score - level3StartScore) >= 10)
                {
                    fillRectangle(0, 16, 128, 160, 0);
                    printTextX2("YOU WIN", 20, 60, RGBToWord(0x00, 0xff, 0x00), 0);
                    printText("hold UP to restart", 5, 90, RGBToWord(0x00, 0xff, 0x00), 0);

                    playWinSong();

                    hasFinished = 1;
                }
            
                // lose if time runs out
                if (elapsed >= 30000)
                {
                    fillRectangle(0, 16, 128, 160, 0);
                    printTextX2("YOU LOSE", 20, 60, RGBToWord(0xff, 0x00, 0x00), 0);
                    printText("hold UP to restart", 5, 90, RGBToWord(0xff, 0x00, 0x00), 0);
                    playLoseSong();
                    hasFinished = 1;
                }
            }
            
            delay(50);
        }
    }
}

/* ===================== ORIGINAL SUPPORT CODE ===================== */

void initSysTick(void)
{
    SysTick->LOAD = 48000; // 1ms tick at 48MHz
    SysTick->CTRL = 7;
    SysTick->VAL = 10;
    __asm(" cpsie i ");
}

void SysTick_Handler(void)
{
    milliseconds++;
}

void initClock(void)
{
    RCC->CR &= ~(1u << 24);
    while ((RCC->CR & (1 << 25)))
        ;

    FLASH->ACR |= (1 << 0);
    FLASH->ACR &= ~((1u << 2) | (1u << 1));
    FLASH->ACR |= (1 << 4);

    RCC->CFGR &= ~((1u << 21) | (1u << 20) | (1u << 19) | (1u << 18));
    RCC->CFGR |= ((1 << 21) | (1 << 19));

    RCC->CFGR |= (1 << 14);

    RCC->CR |= (1 << 24);
    RCC->CFGR |= (1 << 1);
}

void delay(volatile uint32_t dly)
{
    uint32_t end_time = dly + milliseconds;
    while (milliseconds != end_time)
        __asm(" wfi ");
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
    Port->PUPDR = Port->PUPDR & ~(3u << BitNumber * 2);
    Port->PUPDR = Port->PUPDR | (1u << BitNumber * 2);
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
    uint32_t mode_value = Port->MODER;
    Mode = Mode << (2 * BitNumber);
    mode_value = mode_value & ~(3u << (BitNumber * 2));
    mode_value = mode_value | Mode;
    Port->MODER = mode_value;
}

void setupIO()
{
    RCC->AHBENR |= (1 << 18) + (1 << 17);
    display_begin();

    pinMode(GPIOB, 4, 0);
    pinMode(GPIOB, 5, 0);
    pinMode(GPIOA, 8, 0);
    pinMode(GPIOA, 11, 0);

    enablePullUp(GPIOB, 4);
    enablePullUp(GPIOB, 5);
    enablePullUp(GPIOA, 11);
    enablePullUp(GPIOA, 8);
}