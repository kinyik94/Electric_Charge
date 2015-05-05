#include "menu.h"
#include "level.h"
#include "build.h"

static void list_records(int last, SDL_Surface *screen, SDL_Surface *backgr, TTF_Font *record, SDL_Rect where, SDL_Rect source, SDL_Rect goalposition, SDL_Color white, file *f, int first){

    int i;
    char str[10];
    TTF_Font *inst;
    SDL_Surface *sub;

    TTF_Init();
    inst = TTF_OpenFont("times.ttf", 20);
    if (!inst) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    SDL_BlitSurface(backgr, &source, screen, &goalposition);

    /*Letterhead*/
    sub = TTF_RenderUTF8_Blended(record, "Level                     Shots                        Username", white);
        where.x = 200;
        where.y = 10;
        SDL_BlitSurface(sub, NULL, screen, &where);
        SDL_FreeSurface(sub);

    /*Instructions*/
    sub = TTF_RenderUTF8_Blended(inst, "Press enter or click on the red 'x' to escape", white);
        where.x = 700;
        where.y = 500;
        SDL_BlitSurface(sub, NULL, screen, &where);
        SDL_FreeSurface(sub);
    sub = TTF_RenderUTF8_Blended(inst, "Use left-right arrows to see more records", white);
        where.x = 50;
        where.y = 500;
        SDL_BlitSurface(sub, NULL, screen, &where);
        SDL_FreeSurface(sub);

    for(i = first; i < last; i++){

        /*Level*/
        sprintf(str, "%d.level", i+1);
        sub = TTF_RenderUTF8_Blended(record, str, white);
            where.x = 200;
            where.y = 70 + (i%10) * 40;
            SDL_BlitSurface(sub, NULL, screen, &where);
            SDL_FreeSurface(sub);

        sprintf(str, "%d", f[i].shot);
        sub = TTF_RenderUTF8_Blended(record, str, white);
            where.x = 435;
            where.y = 70 + (i%10) * 40;
            SDL_BlitSurface(sub, NULL, screen, &where);
            SDL_FreeSurface(sub);

        /*Username*/
        sub = TTF_RenderUTF8_Blended(record,f[i].us, white);
            where.x = 695;
            where.y = 70 + (i%10) * 40;
            SDL_BlitSurface(sub, NULL, screen, &where);
            SDL_FreeSurface(sub);
    }
    SDL_Flip(screen);
}

void menudraw(SDL_Surface *scr, Menupoints m[], int mpcs, Window win, int first, int delet){

    SDL_Surface *sub, *backgr;
    TTF_Font *letter, *inst;
    SDL_Rect where = { 0, 0, 0, 0};
    SDL_Color white = {255, 255, 255};
    SDL_Rect source = { 0, 0, win.width, win.high };
    SDL_Rect goalposition    = { 0, 0, 0, 0 };
    int i;

    /*Init the ont and the background*/
    TTF_Init();
    letter = TTF_OpenFont("times.ttf", 50);
    if (!letter) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    TTF_Init();
    inst = TTF_OpenFont("times.ttf", 20);
    if (!inst) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    backgr = IMG_Load("back.jpg");
    if (!backgr) {
        fprintf(stderr, "Unable to open the image file!\n");
        exit(2);
    }

    /*Drow the background*/
    SDL_BlitSurface(backgr, &source, scr, &goalposition);
    SDL_FreeSurface(backgr);

    /*Instruction*/
    sub = TTF_RenderUTF8_Blended(inst, "Select the suitable element with the arrows and press enter", white);
    where.x = 280;
    where.y = 500;
    SDL_BlitSurface(sub, NULL, scr, &where);
    SDL_FreeSurface(sub);
    if (delet){
        sub = TTF_RenderUTF8_Blended(inst, "You can't delete the first 3 level", white);
        where.x = 400;
        where.y = 470;
        SDL_BlitSurface(sub, NULL, scr, &where);
        SDL_FreeSurface(sub);
    }

    /*Draw the rectangles*/
    for(i = first; i < mpcs; i++){
        rectangleRGBA(scr, m[i].x1, m[i].y1, m[i].x2, m[i].y2, 255, 255, 255, 255);
        sub = TTF_RenderUTF8_Blended(letter, m[i].menuname, white);
        where.x = m[i].namex;
        where.y = m[i].y1 + 5;
        SDL_BlitSurface(sub, NULL, scr, &where);
        SDL_FreeSurface(sub);
    }

    /*The first rectangle is red*/
    rectangleRGBA(scr, m[0].x1, m[0].y1, m[0].x2, m[0].y2, 255, 0, 0, 255);
    SDL_Flip(scr);

    /*Close the fonts*/
    TTF_CloseFont(letter);
    TTF_CloseFont(inst);
}

void file_in(FILE *fp, int levnum, file *f){
    int i;

    for(i = 0; (fscanf(fp, "%d %s", &f[i].shot, f[i].us) != EOF) && i<=levnum; i++){
    }
}

static void file_out(char *user, int shots[], int levnum){
    FILE *fp;

    file *f;
    int i;

    f = (file*) malloc(levnum * sizeof(file));
    for(i = 0; i < levnum; i++){
        f[i].shot = 0;
        strcpy(f[i].us, "-");
    }

    fp = fopen("Records.txt", "rt");
    file_in(fp, levnum, f);
    fclose(fp);

    for(i = 0; i < levnum; i++)
        if(f[i].shot == 0)
            f[i].shot = 999;

    for(i = 0; i < levnum; i++)
        if(shots[i] < f[i].shot){
            f[i].shot = shots[i];
            strcpy(f[i].us, user);
        }

    /*Write to file*/
    fp = fopen("Records.txt", "wt");
    for(i = 0; i < levnum; i++)
        fprintf(fp, "%d\t%s\n", f[i].shot, f[i].us);

    fclose(fp);
    free(f);
}

static void Record_out(SDL_Surface *screen, Window window, int levnum){
    FILE *fp;
    file *f;
    int i;
    int q = 0;
    SDL_Surface *sub, *backgr;
    TTF_Font *record;
    SDL_Rect source = { 0, 0, window.width, window.high };
    SDL_Rect goalposition = { 0, 0, 0, 0 };
    SDL_Rect where = { 0, 0, 0, 0};
    SDL_Color white = {255, 255, 255};
    SDL_Event enter;


    /*Init the font and the background*/
    backgr = IMG_Load("back.jpg");
    if (!backgr) {
        fprintf(stderr, "Unable to load the image file!\n");
        exit(2);
    }

    TTF_Init();
    record = TTF_OpenFont("times.ttf", 30);
    if (!record) {
        fprintf(stderr, "Unable to load the font! %s\n", TTF_GetError());
        exit(2);
    }

    fp = fopen("Records.txt", "rt");
    if (fp == NULL){
        /*If unable to open*/
        SDL_BlitSurface(backgr, &source, screen, &goalposition);
        SDL_FreeSurface(backgr);
        sub = TTF_RenderUTF8_Blended(record, "There are no records", white);
            where.x = 420;
            where.y = 240;
            SDL_BlitSurface(sub, NULL, screen, &where);
            SDL_FreeSurface(sub);
            SDL_Flip(screen);
        while(SDL_WaitEvent(&enter) && (enter.type != SDL_QUIT && (enter.type != SDL_KEYDOWN && (enter.key.keysym.sym != '\r' || enter.key.keysym.sym != SDLK_ESCAPE))));
    }else{
        f = (file*) malloc(levnum * sizeof(file));
        for(i = 0; i < levnum; i++){
            f[i].shot = 0;
            strcpy(f[i].us, "-");
        }

        /*Put the datas in a file*/
        file_in(fp, levnum, f);

        i = 0;

        if(levnum <= 10)
            list_records(levnum, screen, backgr, record, where, source, goalposition, white, f, 0);
        else
            list_records(10, screen, backgr, record, where, source, goalposition, white, f, 0);

        /*Wait for enter*/
        while (!q){
            SDL_WaitEvent(&enter);

            switch(enter.type){
                case SDL_QUIT:
                    q = 1;
                break;
                case SDL_KEYDOWN:
                    if(enter.key.keysym.sym == '\r')
                        q = 1;
                    if(enter.key.keysym.sym == SDLK_ESCAPE)
                        q = 1;
                    if((enter.key.keysym.sym == SDLK_RIGHT) && ((i + 1) * 10 < levnum)){
                        i++;
                        if(levnum <= (i+1)*10)
                            list_records(levnum, screen, backgr, record, where, source, goalposition, white, f, i*10);
                        else
                            list_records((i+1)*10, screen, backgr, record, where, source, goalposition, white, f, i*10);
                    }
                    if((enter.key.keysym.sym == SDLK_LEFT) && i){
                        i--;
                        list_records((i+1)*10, screen, backgr, record, where, source, goalposition, white, f, i*10);
                    }
                break;
            }
        }
        free(f);
        SDL_FreeSurface(backgr);
    }
    /*Close the file and free the list*/
    TTF_CloseFont(record);
    fclose(fp);
}

static void levelmenu(SDL_Surface *scr, char* us, Window win, int levnum){
    int i;
    const int levmenupcs = 3;
    Menupoints levmenu[3];
    SDL_Event m;
    int quit = 0;

    for(i = 0; i < levmenupcs; i++){
        levmenu[i].x1 = 400;
        levmenu[i].x2 = 680;
        levmenu[i].y1 = 160 + i * 100;
        levmenu[i].y2 = levmenu[i].y1 + 60;
    }
    strcpy(levmenu[0].menuname, "Build Level");
    strcpy(levmenu[1].menuname, "Delete level");
    strcpy(levmenu[2].menuname, "Back");
    levmenu[0].namex = levmenu[0].x1 + 25;
    levmenu[1].namex = levmenu[1].x1 + 20;
    levmenu[2].namex = levmenu[2].x1 + 90;

    menudraw(scr, levmenu, levmenupcs, win, 0, 0);

    i = 0;
    while (!quit){
        SDL_WaitEvent(&m);

        switch (m.type){
            case SDL_KEYDOWN:
                if (m.key.keysym.sym == SDLK_ESCAPE){
                    quit = 1;
                }
                if (m.key.keysym.sym == SDLK_UP){
                    rectangleRGBA(scr, levmenu[i].x1, levmenu[i].y1, levmenu[i].x2, levmenu[i].y2, 255, 255, 255, 255);
                    if(i == 0)
                        i = levmenupcs-1;
                    else
                        i--;
                    rectangleRGBA(scr, levmenu[i].x1, levmenu[i].y1, levmenu[i].x2, levmenu[i].y2, 255, 0, 0, 255);
                    SDL_Flip(scr);
                }
                if (m.key.keysym.sym == SDLK_DOWN){
                    rectangleRGBA(scr, levmenu[i].x1, levmenu[i].y1, levmenu[i].x2, levmenu[i].y2, 255, 255, 255, 255);
                    if(i == levmenupcs-1)
                        i = 0;
                    else
                        i++;
                    rectangleRGBA(scr, levmenu[i].x1, levmenu[i].y1, levmenu[i].x2, levmenu[i].y2, 255, 0, 0, 255);
                    SDL_Flip(scr);
                }
                if (m.key.keysym.sym == '\r'){
                    quit = 1;
                    /*Press enter to select the menupoint*/
                    switch(i){
                        case 0:
                            levelbuild(scr, win, us, levnum);
                        break;
                        case 1:
                            leveldel(scr, win, levnum);
                        break;
                    }
                }
            break;
            case SDL_QUIT:
                quit = 1;
            break;
        }
    }
}

int menu(char *user, SDL_Surface *screen, Window window){
    FILE *fp;

    SDL_Surface *sub;
    SDL_Event menupoint;
    TTF_Font *letter;
    SDL_Rect where = { 0, 0, 0, 0};
    SDL_Color white = {255, 255, 255};

    int which = 0;
    int quit = 0;
    int i;
    int levelnum;
    int again = 1;
    const int menupcs = 4;
    int* shots;
    Menupoints *menus;

    TTF_Init();
    letter = TTF_OpenFont("times.ttf", 50);
    if (!letter) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    menus = (Menupoints*) malloc(menupcs * sizeof(Menupoints));

    fp = fopen("level.txt", "rt");
    if(fp == NULL)
        levelnum = 0;
    else
        fscanf(fp, "%d", &levelnum);
    fclose(fp);

    /*Rectangles of the menus*/
    for(i = 0; i < menupcs; i++){
        menus[i].x1 = 400;
        menus[i].x2 = 680;
        menus[i].y1 = 120 + i * 100;
        menus[i].y2 = menus[i].y1 + 60;
    }
    /*The name of the menus*/
    strcpy(menus[0].menuname, "Game");
    strcpy(menus[1].menuname, "Records");
    strcpy(menus[2].menuname, "Levels");
    strcpy(menus[3].menuname, "Quit");
    menus[0].namex = menus[0].x1 + 85;
    menus[1].namex = menus[1].x1 + 60;
    menus[2].namex = menus[2].x1 + 70;
    menus[3].namex = menus[3].x1 + 95;

    menudraw(screen, menus, menupcs, window, 0, 0);

    /*Username out*/
    sub = TTF_RenderUTF8_Blended(letter, user, white);
    where.x = 437 + 90 - 9 * strlen(user);
    where.y = 20;
    SDL_BlitSurface(sub, NULL, screen, &where);
    SDL_FreeSurface(sub);
    SDL_Flip(screen);

    while (!quit){
        SDL_WaitEvent(&menupoint);

        switch (menupoint.type){
            case SDL_KEYDOWN:
                if (menupoint.key.keysym.sym == SDLK_ESCAPE){
                    quit = 1;
                    again = 0;
                }
                if (menupoint.key.keysym.sym == SDLK_UP){
                    rectangleRGBA(screen, menus[which].x1, menus[which].y1, menus[which].x2, menus[which].y2, 255, 255, 255, 255);
                    if(which == 0)
                        which = menupcs-1;
                    else
                        which--;
                    rectangleRGBA(screen, menus[which].x1, menus[which].y1, menus[which].x2, menus[which].y2, 255, 0, 0, 255);
                    SDL_Flip(screen);
                }
                if (menupoint.key.keysym.sym == SDLK_DOWN){
                    rectangleRGBA(screen, menus[which].x1, menus[which].y1, menus[which].x2, menus[which].y2, 255, 255, 255, 255);
                    if(which == menupcs-1)
                        which = 0;
                    else
                        which++;
                    rectangleRGBA(screen, menus[which].x1, menus[which].y1, menus[which].x2, menus[which].y2, 255, 0, 0, 255);
                    SDL_Flip(screen);
                }
                if (menupoint.key.keysym.sym == '\r'){

                    quit = 1;
                    /*Press enter to select the menupoint*/
                    switch(which){
                        case 0:
                            /*If the first, start the game*/
                            shots = levelman(user, screen, window, levelnum);

                            for(i = 0; i < levelnum; i++)
                                if(shots[i] >= 1000 || shots[i] == 0)
                                    shots[i] = 999;

                            file_out(user, shots, levelnum);/*Write the record into file*/
                            free(shots);

                        break;
                        case 1:
                            /*If the second, list the records*/
                            Record_out(screen, window, levelnum);
                        break;
                        case 2:
                            /*If the third open the level menu*/
                            levelmenu(screen, user, window, levelnum);
                        break;
                        case 3:
                            /*Quit*/
                            again = 0;
                        break;
                    }
                }
            break;
            case SDL_QUIT:
                quit = 1;
                again = 0;
            break;
        }
    }
    TTF_CloseFont(letter);
    free(menus);

    return again;
}
