#include <stm32f031x6.h>
#include "display.h"
#include <stdio.h>

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

int speed = 3;
int hasFinished = 0;
volatile uint32_t milliseconds;

/* ===================== YOUR SPRITES (UNCHANGED) ===================== */
const uint16_t deco1[] =
    {
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        248,
        65415,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        65415,
        65415,
        8068,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        4096,
        4096,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        65535,
        65535,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7940,
        7940,
        7940,
        7940,
        0,
        0,
        0,
};

const uint16_t deco2[] =
    {
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        248,
        65415,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        65415,
        65415,
        8068,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        4096,
        4096,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65415,
        65415,
        65415,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        65535,
        65535,
        65535,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        7936,
        7936,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        40224,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        40224,
        40224,
        7936,
        65535,
        7936,
        0,
        0,
        0,
        0,
        0,
        0,
        65315,
        40224,
        40224,
        7936,
        65535,
        40224,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        65315,
        0,
        65315,
        65315,
        65315,
        65315,
        0,
        0,
};

const uint16_t deco3[] =
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        7936,
        7936,
        4,
        4,
        4,
        4,
        4,
        4,
        7936,
        7936,
        0,
        0,
        65535,
        65535,
        4,
        4,
        4,
        4,
        4,
        4,
        65535,
        65535,
        0,
        0,
        7936,
        7936,
        4,
        4,
        4,
        4,
        4,
        4,
        7936,
        7936,
        0,
        0,
        0,
        0,
        0,
        4,
        4,
        4,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        24327,
        24327,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
};

const uint16_t dg1[] =
    {
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        1994,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        1994,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        1994,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        16142,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        1994,
        1994,
        1994,
        16142,
        16142,
        0,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
        0,
        0,
        0,
        16142,
        16142,
        16142,
        16142,
        16142,
        16142,
        0,
        0,
        0,
};
/* ==================================================================== */

/* --------- Tiny RNG --------- */
static uint32_t rng_state = 0xA5A5A5A5;

static uint32_t xorshift32(void)
{
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

static uint16_t randRange(uint16_t min, uint16_t max)
{
    uint16_t span = (max - min) + 1;
    return (uint16_t)(min + (xorshift32() % span));
}

/* --------- Collision --------- */
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
    uint16_t x2 = x1 + w;
    uint16_t y2 = y1 + h;

    if ((px >= x1) && (px <= x2) && (py >= y1) && (py <= y2))
        return 1;
    return 0;
}

static int playerTouchesTarget(uint16_t tx, uint16_t ty, uint16_t px, uint16_t py)
{
    // player is 12x16
    return isInside(tx, ty, 12, 16, px, py) ||
           isInside(tx, ty, 12, 16, px + 12, py) ||
           isInside(tx, ty, 12, 16, px, py + 16) ||
           isInside(tx, ty, 12, 16, px + 12, py + 16);
}

/* --------- HUD: Score + Level --------- */
static int calcLevel(int score)
{
    int lvl = (score / 10) + 1;
    if (lvl > 3)
        lvl = 3;
    return lvl;
}

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

/* --------- Fake "10% bigger" sprite after score>=10 --------- */
static void erasePlayer(uint16_t px, uint16_t py, int big)
{
    if (big)
        fillRectangle(px - 1, py - 1, 14, 18, 0);
    else
        fillRectangle(px, py, 12, 16, 0);
}

static void drawPlayer(uint16_t px, uint16_t py,
                       const uint16_t *img,
                       int hinv, int vinv,
                       int big)
{
    if (!big)
    {
        putImage(px, py, 12, 16, img, hinv, vinv);
        return;
    }

    // looks bigger/chunkier
    putImage(px - 1, py, 12, 16, img, hinv, vinv);
    putImage(px + 1, py, 12, 16, img, hinv, vinv);
    putImage(px, py - 1, 12, 16, img, hinv, vinv);
    putImage(px, py + 1, 12, 16, img, hinv, vinv);
    putImage(px, py, 12, 16, img, hinv, vinv);
}

/* --------- Coin spawn/respawn --------- */
static void spawnCoinAtNewPos(uint16_t *coinX, uint16_t *coinY, uint16_t px, uint16_t py)
{
    do
    {
        *coinX = randRange(10, 110);
        *coinY = randRange(16, 140);
    } while (playerTouchesTarget(*coinX, *coinY, px, py));
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

    uint16_t coinX = 20, coinY = 80;

    int score = 0;
    int bigSprite = 0;

    // coin visibility state machine
    int coinVisible = 1;
    uint32_t coinStateStart = 0;

    // level 3 challenge
    int inLevel3Challenge = 0;
    uint32_t level3StartMs = 0;
    int level3StartScore = 0;

    initClock();
    initSysTick();
    setupIO();

    rng_state ^= (milliseconds + 0x12345678);

    spawnCoinAtNewPos(&coinX, &coinY, x, y);

    // initial draw
    putImage(coinX, coinY, 12, 16, dg1, 0, 0);
    drawPlayer(x, y, deco2, 0, 0, bigSprite);
    drawHUD(score);

    coinVisible = 1;
    coinStateStart = milliseconds;

    while (1)
    {

        if (hasFinished)
        {
            if ((GPIOA->IDR & (1 << 8)) == 0) // up
            {
                
                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("..", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 90, 100, 110, 0);

                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("....", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 90, 100, 110, 0);

                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("......", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 90, 100, 110, 0);
                
                delay(500);
                if ((GPIOA->IDR & (1 << 8)) == 0)
                    printTextX2("........", 10, 100, RGBToWord(0x00, 0xff, 0x00), 0);
                else
                    fillRectangle(8, 90, 100, 110, 0);

                //delay(2000);
                if ((GPIOA->IDR & (1 << 8)) == 0) // up
                {
                    
                    hasFinished = 0;
                    hinverted = 0;
                    vinverted = 0;
                    toggle = 0;
                    hmoved = 0;
                    vmoved = 0;
                    speed = 3;
                    
                    x = 50, y = 50;
                    oldx = x, oldy = y;
                    
                    coinX = 20, coinY = 80;
                    
                    score = 0;
                    bigSprite = 0;
                    
                    // coin visibility state machine
                    coinVisible = 1;
                    coinStateStart = 0;
                    
                    // level 3 challenge
                    inLevel3Challenge = 0;
                    level3StartMs = 0;
                    level3StartScore = 0;


                    fillRectangle(0, 16, 128, 160, 0);
                    drawHUD(score);
                }
            }
        }
        else
        {

            int level = calcLevel(score);

            // set coin speed by level (disappear faster each level)
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
                inLevel3Challenge = 1;
                level3StartMs = milliseconds;
                level3StartScore = score;

                // show timer immediately
                drawTimerSeconds(30);
            }

            // LEVEL 3: timer + win/lose condition (need +10 coins in 30 seconds)
            if (inLevel3Challenge)
            {
                uint32_t elapsed = milliseconds - level3StartMs;
                int secondsLeft = 30 - (int)(elapsed / 1000);

                // update timer display (cheap + simple)
                drawTimerSeconds(secondsLeft);

                // win early if got 10 coins during level 3
                if ((score - level3StartScore) >= 10)
                {
                    fillRectangle(0, 16, 128, 160, 0);
                    printTextX2("YOU WIN", 20, 60, RGBToWord(0x00, 0xff, 0x00), 0);
                    hasFinished = 1;
                }

                // lose if time runs out
                if (elapsed >= 30000)
                {
                    fillRectangle(0, 16, 128, 160, 0);
                    printTextX2("YOU LOSE", 20, 60, RGBToWord(0xff, 0x00, 0x00), 0);
                    hasFinished = 1;
                }
            }

            // coin appear/disappear state machine (runs even if player doesn't move)
            uint32_t now = milliseconds;
            if (coinVisible)
            {
                if ((now - coinStateStart) >= visibleMs)
                {
                    // hide coin
                    fillRectangle(coinX, coinY, 12, 16, 0);
                    coinVisible = 0;
                    coinStateStart = now;
                }
            }
            else
            {
                if ((now - coinStateStart) >= hiddenMs)
                {
                    // show coin at a new random position
                    spawnCoinAtNewPos(&coinX, &coinY, x, y);
                    putImage(coinX, coinY, 12, 16, dg1, 0, 0);
                    drawHUD(score);
                    coinVisible = 1;
                    coinStateStart = now;
                }
            }

            // read buttons
            hmoved = vmoved = 0;
            hinverted = vinverted = 0;

            if ((GPIOB->IDR & (1 << 4)) == 0) // right
            {
                if (x < 110)
                {
                    x += speed;
                    hmoved = 1;
                    hinverted = 0;
                }
            }

            if ((GPIOB->IDR & (1 << 5)) == 0) // left
            {
                if (x > 10)
                {
                    x -= speed;
                    hmoved = 1;
                    hinverted = 1;
                }
            }

            if ((GPIOA->IDR & (1 << 11)) == 0) // down
            {
                if (y < 140)
                {
                    y += speed;
                    vmoved = 1;
                    vinverted = 0;
                }
            }

            if ((GPIOA->IDR & (1 << 8)) == 0) // up
            {
                if (y > 16)
                {
                    y -= speed;
                    vmoved = 1;
                    vinverted = 1;
                }
            }

            // redraw only on movement
            if (vmoved || hmoved)
            {
                erasePlayer(oldx, oldy, bigSprite);
                oldx = x;
                oldy = y;

                if (hmoved)
                {
                    const uint16_t *frame = toggle ? deco1 : deco2;
                    if (!hasFinished)
                        drawPlayer(x, y, frame, hinverted, 0, bigSprite);
                    toggle ^= 1;
                }
                else
                {
                    if (!hasFinished)
                        drawPlayer(x, y, deco3, 0, vinverted, bigSprite);
                }

                // collect coin only if it's visible
                if (coinVisible && playerTouchesTarget(coinX, coinY, x, y))
                {
                    // collect
                    score++;
                    drawHUD(score);

                    // every multiple of 10 -> speed up
                    if ((score % 10) == 0)
                        speed += 4;

                    // after 10 coins total: big sprite effect
                    if (score >= 10)
                        bigSprite = 1;

                    // immediately hide & respawn cycle
                    fillRectangle(coinX, coinY, 12, 16, 0);
                    coinVisible = 0;
                    coinStateStart = milliseconds;
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