#include "menu.h"

static int input_text(char *name, SDL_Surface *screen, Window win, int namelen) {
    SDL_Color black = {0, 0, 0}, white = {255, 255, 255};
    SDL_Rect source = { 0, 0, win.width, win.high}, goalposition = { 0, 0, 0, 0 };
    int x = 420, y = 250, w = 300, h = 40;
    SDL_Rect goal = {0, 0, w, h};
    SDL_Surface *sub, *backgr;
    TTF_Font *font;
    SDL_Event event;
    int i = 0, quit = 0;

    backgr = IMG_Load("back.jpg");
    if (!backgr) {
        fprintf(stderr, "Unable to open the image file!\n");
        exit(2);
    }

    TTF_Init();
    font = TTF_OpenFont("times.ttf", 32);
    if (!font) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }
    SDL_EnableKeyRepeat(500, 30);

    name[i] = 0x0000;
    SDL_EnableUNICODE(1);

    SDL_BlitSurface(backgr, &source, screen, &goalposition);
    SDL_FreeSurface(backgr);

    sub = TTF_RenderUTF8_Blended(font, "Enter your Username!", white);
    goal.x = 400;
    goal.y = 50;
    SDL_BlitSurface(sub, NULL, screen, &goal);
    SDL_FreeSurface(sub);

    sub = TTF_RenderUTF8_Blended(font, "It must contain at least one and not more than 10 characters ", white);
    goal.x = 180;
    goal.y = 100;
    SDL_BlitSurface(sub, NULL, screen, &goal);
    SDL_FreeSurface(sub);

    sub = TTF_RenderUTF8_Blended(font, "Username: ", white);
    goal.x = 270;
    goal.y = y+2;
    SDL_BlitSurface(sub, NULL, screen, &goal);
    SDL_FreeSurface(sub);

    SDL_Flip(screen);
    goal.x = x;
    while (!quit) {
        /* draw the text */
        boxRGBA(screen, x, y, w + x, h + y, 255, 255, 255, 255);

        sub = TTF_RenderText_Blended(font, name, black);
        SDL_BlitSurface(sub, &source, screen, &goal);
        SDL_FreeSurface(sub);
        SDL_Flip(screen);


        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.unicode) {
                    case '\r':
                    case '\n':
                        quit = 1;
                    break;
                    case '\b':
                        /* backspace: delete */
                        if (i>0)
                            name[--i] = 0x0000;
                    break;
                    case SDLK_ESCAPE:
                        return 1;
                    break;
                    default:
                        if((((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z'))
                            || ((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z'))
                            || ((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9'))) && i < namelen){
                            name[i++] = event.key.keysym.unicode;
                            name[i] = 0x0000;
                        }
                    break;
                }
                break;
            case SDL_QUIT:
                return 1;
            break;
        }
    }
    TTF_CloseFont(font);
    return 0;
}

int main(int argc, char *argv[]){

    Window window;
    SDL_Surface *screen;
    char name[11];
    int namelen = 10;
    int again = 1;
    int quit;
    int bad = 1;

    /*Size of the SDL window*/
    window.high = 540;
    window.width = 1080;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_WM_SetCaption("particles", "particles");
    screen=SDL_SetVideoMode(window.width, window.high, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Unable to open the window!\n");
        exit(1);
    }

    while(bad){
        bad = 0;
        printf("Username: ");

        quit = input_text(name, screen, window, namelen);
        if(quit)
            return 0;
        if(name[0] == 0)
            bad = 1;
    }

    printf("\n");

    while (again)
        again = menu(name, screen, window);

    SDL_Quit();
    return 0;
}
