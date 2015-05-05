#include "game.h"

static void force_count(double *v_x, double *v_y, int polarity, double r, double x, double y, double mx, double my, double dis){
    double pi = 3.14159265359;
    double beta = 0;
    /*k is fixed for the force*/
    const int k = 500;
    int F = 0;
    F = polarity * r * k;
    /*Four separate cases, depending on the location of the moving charge resides relative to the fixed charge is the one with quadrant.*/
    if(my > y){
        if(mx <= x){
            beta = atan((y - my) / (x - mx));
            beta = pi - beta;
        }else{
            beta = atan((y - my) / (mx - x));
        }
    }
    if (my < y){
        if(mx <= x){
            beta = atan((my - y) / (x - mx));
            beta = pi + beta;
        }else{
            beta = atan((my - y) / (mx - x));
            beta = (-1) * beta;
        }

    }
    /*If the atan is not defined*/
    if (my == y){
        if (mx < x){
            beta = 0;
        }else{
            beta = pi;
        }
    }
    /*increase the size of the x and y velocity*/
    *v_x += F * cos(beta) / (dis * dis);
    *v_y += F * sin(beta) / (dis * dis);
}

double point_distance(Point a, Point b){
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static void cannon_draw(Levels* p, Window window, double a, SDL_Surface *scr){
    double pi = 3.14159265359;
    double y = window.high / 2 + p->up_zone;

    p->x1 = p->left_zone + p->radius * cos(a * pi / 180) - 5 * sin(a * pi / 180);
    p->y1 = y - p->radius * sin(a * pi / 180) - 5 * cos(a * pi / 180);
    p->x2 = p->left_zone + p->radius * cos(a * pi / 180) + 5 * sin(a * pi / 180);
    p->y2 = y - p->radius * sin(a * pi / 180) + 5 * cos(a * pi / 180);
    p->x3 = p->left_zone + (p->radius + p->length) * cos(a * pi / 180) + 5 * sin(a * pi / 180);
    p->y3 = y - (p->radius + p->length) * sin(a * pi / 180) + 5 * cos(a * pi / 180);
    p->x4 = p->left_zone + (p->radius + p->length) * cos(a * pi / 180) - 5 * sin(a * pi / 180);
    p->y4 = y - (p->radius + p->length) * sin(a * pi / 180) - 5 * cos(a * pi / 180);

    /*A kilovő félköre*/
    filledCircleRGBA(scr, p->left_zone, y, p->radius, 255, 0, 0, 150);

    /*A téglalap oldalai*/
    lineRGBA(scr, p->x1, p->y1, p->x2, p->y2, 255, 0, 0, 255);
    lineRGBA(scr, p->x2, p->y2, p->x3, p->y3, 255, 0, 0, 255);
    lineRGBA(scr, p->x3, p->y3, p->x4, p->y4, 255, 0, 0, 255);
    lineRGBA(scr, p->x4, p->y4, p->x1, p->y1, 255, 0, 0, 255);
}

static void goal_draw(double x, double y, double r, SDL_Surface *scr){
    filledCircleRGBA(scr, x, y, r, 255, 255, 0, 255);
}

void level_draw(Levels* level, Window win, double alpha, Point goal, const char* user, particles *used, Point use[], Point f[], int fixpcs, int usesize, Barrier *bar, int barpcs, int shot, int levelnum, SDL_Surface *scr, Barlist *barl, int build_f){
    SDL_Color white = {255, 255, 255};
    SDL_Rect where = { 0, 0, 0, 0 };
    TTF_Font *font;
    SDL_Surface *sub;
    SDL_Surface *backgr;
    SDL_Rect source = { 0, 0, win.width, win.high };
    SDL_Rect goalposition    = { 0, 0, 0, 0 };
    char shotstr[20];
    char levelstr[10];
    char str[5];
    int j;
    particles *p;
    Barlist *b;

    /*print numbers to string*/
    sprintf(shotstr, "Shots: %d", shot);
    sprintf(levelstr, "%d. level", levelnum + 1);

    /*Init font and background*/
    TTF_Init();
    font = TTF_OpenFont("times.ttf", 20);
    if (!font) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    backgr = IMG_Load("back.jpg");
    if (!backgr) {
        fprintf(stderr, "Unable to open the image file!\n");
        exit(2);
    }

    /* Draw the background*/
    SDL_BlitSurface(backgr, &source, scr, &goalposition);

    /*We need only the left zone of the image*/
    source.h = win.high;
    source.w = level->left_zone;

    /*Redraw cannon a goal*/
    goal_draw(goal.x, goal.y, goal.r, scr);

    if(!build_f)
        cannon_draw(level, win, alpha, scr);
    else
        filledCircleRGBA(scr, level->left_zone, win.high/2 + level->up_zone, level->radius + level->length, 255, 0, 0, 255);

    if(!build_f){
        /*Draw fix charges*/
        for(j = 0; j < fixpcs; j++){
            /*If the polarity is negative, then the charge will be blue, if positive, will be red*/
            if(f[j].polarity > 0)
                filledCircleRGBA(scr, f[j].x, f[j].y, f[j].r, 255, 0, 0, 255);
            else
                filledCircleRGBA(scr, f[j].x, f[j].y, f[j].r, 0, 0, 255, 255);
        }
    }

    /*Background to the left zone*/
    SDL_BlitSurface(backgr, &source, scr, &goalposition);

    /*We need only the up zone of the image*/
    source.h = level->up_zone;
    source.w = win.width;

    /*Barriers*/
    for(j = 0; j < barpcs; j++)
		boxRGBA(scr, bar[j].x1, bar[j].y1, bar[j].x2, bar[j].y2, 255, 0, 0, 150);
    if(build_f)
        for(b = barl; b != NULL; b=b->next)
            boxRGBA(scr, b->element.x1, b->element.y1, b->element.x2, b->element.y2, 255, 0, 0, 150);

    for(j = 0; j < usesize; j++){
        sprintf(str, "%.0f", use[j].polarity * use[j].r);
        /*If the polarity is negative, then the charge will be blue, if positive, will be red*/
        if(use[j].polarity > 0)
            filledCircleRGBA(scr, use[j].x, use[j].y, use[j].r, 255, 0, 0, 255);
        else
            filledCircleRGBA(scr, use[j].x, use[j].y, use[j].r, 0, 0, 255, 255);
        stringRGBA(scr, use[j].x - 5 + use[j].polarity, use[j].y + 20, str, 255, 255, 255, 255);
    }

    for(p = used; p != NULL; p=p->next){
        if(p->element.polarity > 0)
            filledCircleRGBA(scr, p->element.x, p->element.y, p->element.r, 255, 0, 0, 255);
        else
            filledCircleRGBA(scr, p->element.x, p->element.y , p->element.r, 0, 0, 255, 255);
    }
    /*Image to the up zone*/
    SDL_BlitSurface(backgr, &source, scr, &goalposition);
    SDL_FreeSurface(backgr);

    if(!build_f)
        lineRGBA(scr, 0, level->up_zone, win.width, level->up_zone, 255, 255, 255, 255);
    else
        lineRGBA(scr, level->left_zone, level->up_zone, win.width, level->up_zone, 255, 255, 255, 255);
    lineRGBA(scr, level->left_zone, 0, level->left_zone, win.high, 255, 255, 255, 255);


    /*Username*/
    sub = TTF_RenderUTF8_Blended(font, user, white);
    where.x = level->left_zone + 10;
    where.y = 10;
    SDL_BlitSurface(sub, NULL, scr, &where);
    SDL_FreeSurface(sub);

    /*Level number*/
    sub = TTF_RenderUTF8_Blended(font, levelstr, white);
    where.x = win.width / 2;
    where.y = 10;
    SDL_BlitSurface(sub, NULL, scr, &where);
    SDL_FreeSurface(sub);

    if(!build_f){
        /*Shot counter*/
        sub = TTF_RenderUTF8_Blended(font, shotstr, white);
        where.x = win.width - 150;
        where.y = 10;
        SDL_BlitSurface(sub, NULL, scr, &where);
        SDL_FreeSurface(sub);
    }

    TTF_CloseFont(font);
    SDL_Flip(scr);
}

static void push(Point *part, Point f){
    /*Don't stand in circuit*/
    if (part->x <= f.x)
        part->x+=0.5;
    else
        part->x-=0.5;
    /*If the moving charge distance is less than 15 pixels and the fixed charge is negative, then the charging fly into the fixed charge*/
    if (f.dist < 15 && f.polarity < 0){
        part->x = f.x;
        part->y = f.y;
    }
}

int shot_an(Point* moving, Point f[], particles *part, int fsize, double *v_x, double *v_y, SDL_Surface *scr){

    particles *p;
    double check;
    int j;

    filledCircleRGBA(scr, moving->x, moving->y, moving->r, 255, 255, 255, 255);

    moving->x += *v_x;
    moving->y -= *v_y;

    for(j = 0; j < fsize; j++){
        force_count(v_x, v_y, f[j].polarity, f[j].r, f[j].x, f[j].y, moving->x, moving->y, f[j].dist);
        check = sqrt((moving->x - f[j].x) * (moving->x - f[j].x) + (moving->y - f[j].y) * (moving->y - f[j].y));
        if (abs(check - f[j].dist) <=0.1){
            push(moving, f[j]);
        }
    }

    for(p = part; p != NULL; p = p->next){
        force_count(v_x, v_y, p->element.polarity, p->element.r, p->element.x, p->element.y, moving->x, moving->y, p->element.dist);
        check = sqrt((moving->x - p->element.x) * (moving->x - p->element.x) + (moving->y - p->element.y) * (moving->y - p->element.y));
        if (abs(check - p->element.dist) <=0.1){
            push(moving, p->element);
        }
    }

    /*The new charge*/
    filledCircleRGBA(scr, moving->x, moving->y, moving->r, 255, 255, 0, 255);
    SDL_Flip(scr);

    return 0;

}
