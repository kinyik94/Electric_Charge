#include "game.h"
#include "level.h"

static int no_there(Point temp, Point goal, Point fix[], int fixpcs, particles *f){
    int i;
    particles *p;

    /*If the user wants to put the charge in a place where there is already a fixed charge or there is the goal, don't put it into the list*/
    if (point_distance(temp, goal) < temp.r + goal.r)
        return 0;

    for(i = 0; i < fixpcs; i++)
        if(point_distance(fix[i], temp) < fix[i].r + temp.r)
            return 0;
    for(p = f; p != NULL; p = p->next)
        if(point_distance(p->element, temp) < p->element.r + temp.r)
            return 0;

    return 1;

}

particles* new_used_element(particles* part, Point which){

    particles *rec, *p;
    rec = (particles*) malloc(sizeof(particles));
    rec->element = which;
    rec->next = NULL;
    if (part == NULL)
        return rec;
    for(p = part; p->next != NULL; p=p->next);
    p->next = rec;
    return part;
}

void part_free(particles *part){
    particles *temp, *p;
    p = part;
    while(p != NULL){
        temp = p->next;
        free(p);
        p = temp;
    }
}

particles* del(particles *part, particles *which){
    particles *ptr;

    if(part == which){
        ptr = which->next;
        free(which);
        return ptr;
    }

    for(ptr = part; (ptr->next != which) && (ptr != NULL); ptr = ptr -> next);
    if(ptr != NULL){
        ptr->next = which->next;
        free(which);
    }
    return part;
}

int* levelman(char* user, SDL_Surface *screen, Window window, int levelpcs){

    FILE *level;

    double pi = 3.14159265359;
    SDL_Event setting;

    int deleted;
    int quit;
    int i,j;
    int collision = 0;
    int* shots;
    int fixpcs = 0;
    int barpcs = 0;
    Barrier *bar;
    Point goal;
    Point *fix;

    double alpha = 0;
    int usepcs = 4;

    double v = 10;
    double vx, vy;

    Point use[4];
    Point moving;
    Levels lev;

    particles *p, *used;

    Uint8 *keystates = SDL_GetKeyState(NULL);

    /*Moving charge's radius*/
    moving.r = 3;

    use[0].x = 20;
    use[0].y = 100;
    use[0].r = 5;
    use[0].polarity = -1;
    use[1].x = 20;
    use[1].y = 200;
    use[1].r = 5;
    use[1].polarity = 1;
    use[2].x = 20;
    use[2].y = 300;
    use[2].r = 10;
    use[2].polarity = -1;
    use[3].x = 20;
    use[3].y = 400;
    use[3].r = 10;
    use[3].polarity = 1;

    /*The level's datas*/
    lev.radius = 30;
    lev.length = 20;
    lev.left_zone = 50;
    lev.up_zone = 50;

    /*Temporary point variable*/
    Point temp;

    level = fopen("level.txt", "rt");
    fscanf(level, "%d\n", &levelpcs);

    shots = (int*) malloc(levelpcs * sizeof(int));

    for (i = 0; i < levelpcs; i++)
        shots[i] = 0;

    for(j = 0; j < levelpcs; j++){
        quit = 0;

        fscanf(level, "%d ", &barpcs);
        bar = (Barrier*) malloc(barpcs * sizeof(Barrier));
        for(i = 0; i < barpcs; i++){
            fscanf(level, "%lf %lf %lf %lf ", &bar[i].x1, &bar[i].x2, &bar[i].y1, &bar[i].y2);
        }
        fscanf(level, "\n");

        fscanf(level, "%d ", &fixpcs);
        fix = (Point*) malloc(fixpcs * sizeof(Point));
        for(i = 0; i < fixpcs; i++){
            fscanf(level, "%lf %lf %lf %lf ", &fix[i].x, &fix[i].y, &fix[i].r, &fix[i].polarity);
        }
        fscanf(level, "\n");

        fscanf(level, "%lf %lf %lf\n", &goal.x, &goal.y, &goal.r);

        p = used = NULL;

        for(i = 0; i < usepcs; i++)
            use[i].click = 0;
        temp.click = 0;

        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);

        while(!quit){
            /*Wait for event*/
            SDL_PollEvent(&setting);

            switch(setting.type){

                case SDL_QUIT:
                    quit = 1;
                    shots[j] = 0;
                    j = levelpcs;
                break;

                case SDL_KEYDOWN:
                    if (setting.key.keysym.sym == SDLK_ESCAPE){
                        quit = 1;
                        shots[j] = 0;
                        j = levelpcs;
                    }
                break;

                /* Mouse click */
                case SDL_MOUSEBUTTONDOWN:

                        if (setting.button.button == SDL_BUTTON_LEFT && !temp.click){
                            i = 0;
                            while((i < usepcs) && !temp.click){
                                if((abs(setting.button.x - use[i].x) < use[i].r) && (abs(setting.button.y - use[i].y) <use[i].r)){
                                    temp.x = use[i].x;
                                    temp.y = use[i].y;
                                    temp.r = use[i].r;
                                    temp.polarity = use[i].polarity;
                                    temp.click = 1;
                                }
                                else
                                    i++;
                            }
                            p = used;
                            while(p != NULL && !temp.click){
                                if((abs(setting.button.x - p->element.x) < p->element.r) && (abs(setting.button.y - p->element.y) < p->element.r)){
                                    temp.x = p->element.x;
                                    temp.y = p->element.y;
                                    temp.r = p->element.r;
                                    temp.polarity = p->element.polarity;
                                    used = del(used, p);
                                    temp.click = 1;
                                }
                                else
                                    p = p->next;
                            }
                        }
                        if (setting.button.button == SDL_BUTTON_RIGHT){
                            deleted = 0;
                            for(p = used; !deleted && p != NULL; p = p->next)
                                if((abs(setting.button.x - p->element.x) < p->element.r) && (abs(setting.button.y - p->element.y) < p->element.r)){
                                    used = del(used, p);
                                    deleted = 1;
                                    level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
                                }
                        }
                break;
                case SDL_MOUSEBUTTONUP:
                    if (setting.button.button == SDL_BUTTON_LEFT && temp.click){
                        temp.click = 0;
                        if ((temp.x > lev.left_zone + temp.r) && no_there(temp, goal, fix, fixpcs, used)){
                            used = new_used_element(used, temp);
                        }
                        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
                    }
                break;
                case SDL_MOUSEMOTION:
                    if ((temp.click)
                    && (setting.motion.x + temp.r < window.width) && (setting.motion.x - temp.r > 0)
                    && (setting.motion.y + temp.r < window.high) && (setting.motion.y - temp.r > lev.up_zone)){
                        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
                        temp.x = setting.motion.x;
                        temp.y = setting.motion.y;
                        if(temp.polarity == -1)
                            filledCircleRGBA(screen, temp.x, temp.y, temp.r, 0, 0, 255, 255);
                        else
                            filledCircleRGBA(screen, temp.x, temp.y, temp.r, 255, 0, 0, 255);
                        SDL_Flip(screen);
                    }
                break;
            }

                /*If the up arrow is pressed, move the cannon up with 15ms delay*/
                if(keystates[SDLK_UP]){
                    if ((alpha + 1) <= 90){
                        SDL_Delay(15);
                        alpha += 0.5;
                        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
                    }
                }

                /*If the up arrow is pressed, move the cannon up with 15ms delay*/
                if (keystates[SDLK_DOWN]){
                    if ((alpha - 1) >= -90){
                        SDL_Delay(15);
                        alpha -= 0.5;
                        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
                    }
                }

                /*If space, the cannon shots the charge */
                if (keystates[SDLK_SPACE]){
                        shots[j]++;
                        collision = 0;
                        level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);

                        /*The first charge coordinates*/
                        moving.x = (lev.x3 + lev.x4)/2;
                        moving.y = (lev.y3 + lev.y4)/2;

                        vx = v * cos(alpha * pi / 180);
                        vy = v * sin(alpha * pi / 180);

                        while ((!quit) && ((moving.x <= window.width) && (moving.y <= window.high) && (moving.y - 2 * moving.r>= lev.up_zone) && (moving.x + 2 * moving.r>= lev.left_zone)) && (!collision)){

                            goal.dist = point_distance(goal, moving);
                            for (i = 0; i < fixpcs; i++)
                                fix[i].dist = point_distance(moving, fix[i]);

                            /*If faild, try again*/
                            for(p = used; p != NULL; p = p->next){
                                p->element.dist = point_distance(moving, p->element);
                                if (p->element.dist <= p->element.r + 5)
                                        collision = 1;
                            }

                            for (i = 0; i < fixpcs; i++)
                                if (fix[i].dist <= fix[i].r+5)
                                        collision = 1;

                            /*If successful*/
                            if (goal.dist <= goal.r+1)
                               quit = 1;

                            /*crashed into a barrier, failed shot*/
                            for(i = 0; i < barpcs; i++){
                                if((moving.x >= bar[i].x1 - moving.r) && (moving.x <= bar[i].x2 + moving.r) && (moving.y <= bar[i].y2 + moving.r) && (moving.y >= bar[i].y1 - moving.r)){
                                   collision = 1;
                                }
                            }
                            if((!collision) && (!quit)){
                                SDL_Delay(5);
                                shot_an(&moving, fix, used, fixpcs, &vx, &vy, screen);
                            }
                        }
                }
        }
        if (j == levelpcs - 1)
            level_draw(&lev, window, alpha, goal, user, used, use, fix, fixpcs, usepcs, bar, barpcs, shots[j], j, screen, NULL, 0);
        part_free(used);
        free(bar);
        free(fix);
    }
    return shots;
}


