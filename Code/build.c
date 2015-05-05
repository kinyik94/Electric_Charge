#include "build.h"
#include "level.h"

static int good_place(Barlist *bar, particles *part, Point goal, Barrier btemp, Point ptemp, int x, int y, int r){
    Barlist *b;
    particles *p;
}

static levelfile* levelfile_in(int out, int levnum){
    FILE *fp;
    levelfile *lev;
    int i, j;

    fp = fopen("level.txt", "rt");
    fscanf(fp, "%d\n", &i);
    if(out)
        lev = (levelfile*) malloc((levnum + 1) * sizeof(levelfile));
    else
        lev = (levelfile*) malloc((levnum) * sizeof(levelfile));
    for (i = 0; i < levnum; i++){
        fscanf(fp, "%d ", &lev[i].barpcs);
        lev[i].bar = (Barrier*) malloc(lev[i].barpcs * sizeof(Barrier));
        for(j = 0; j < lev[i].barpcs; j++)
            fscanf(fp, "%lf %lf %lf %lf ", &lev[i].bar[j].x1, &lev[i].bar[j].x2, &lev[i].bar[j].y1, &lev[i].bar[j].y2);
        fscanf(fp, "\n");
        fscanf(fp, "%d ", &lev[i].partpcs);
        lev[i].part = (Point*) malloc(lev[i].partpcs * sizeof(Point));
        for(j = 0; j < lev[i].partpcs; j++)
            fscanf(fp, "%lf %lf %lf %lf ", &lev[i].part[j].x, &lev[i].part[j].y, &lev[i].part[j].r, &lev[i].part[j].polarity);
        fscanf(fp, "\n");
        fscanf(fp, "%lf %lf %lf\n", &lev[i].goal.x, &lev[i].goal.y, &lev[i].goal.r);
    }
    fclose(fp);
    return lev;
}

static Barlist* new_bar(Barlist* blist, Barrier bar){

    Barlist *rec, *p;
    rec = (Barlist*) malloc(sizeof(Barlist));
    rec->element = bar;
    rec->next = NULL;
    if (blist == NULL)
        return rec;
    for(p = blist; p->next != NULL; p=p->next);
    p->next = rec;
    return blist;
}

static void bar_free(Barlist* blist){
    Barlist *temp, *p;
    p = blist;
    while(p != NULL){
        temp = p->next;
        free(p);
        p = temp;
    }
}

static Barlist* bar_del(Barlist* blist, Barlist *bar){
    Barlist *b;

    if(blist == bar){
        b = bar->next;
        free(bar);
        return b;
    }

    for(b = blist; (b->next != bar) && (b != NULL); b = b -> next);
    if(b != NULL){
        b->next = bar->next;
        free(bar);
    }
    return blist;
}

static void free_level(levelfile *lev, int num){
    int i;
    for(i = 0; i < num; i++){
        free(lev[i].bar);
        free(lev[i].part);
    }
    free(lev);
}

static void to_file(Barlist *blist, particles *plist, Point goal, int levnum){
    FILE *fp;
    levelfile *lev;
    int i, j;
    particles *p;
    Barlist *b;

    lev = levelfile_in(1, levnum);

    lev[levnum].goal = goal;
    lev[levnum].partpcs = lev[levnum].barpcs = 0;
    for(p = plist; p != NULL; p = p->next, lev[levnum].partpcs++);
    for(b = blist; b != NULL; b = b->next, lev[levnum].barpcs++);
    lev[levnum].bar = (Barrier*) malloc(lev[levnum].barpcs * sizeof(Barrier));
    lev[levnum].part = (Point*) malloc(lev[levnum].partpcs * sizeof(Point));
    for(i = 0 , b = blist; i < lev[levnum].barpcs; i++, b = b->next)
        lev[levnum].bar[i] = b->element;
    for(i = 0 , p = plist; i < lev[levnum].partpcs; i++, p = p->next)
        lev[levnum].part[i] = p->element;

    fp = fopen("level.txt", "wt");
    fprintf(fp, "%d\n", levnum + 1);
    for(i = 0; i <= levnum; i++){
        if(i)
            fprintf(fp, "\n");
        fprintf(fp, "%d", lev[i].barpcs);
        for(j = 0; j < lev[i].barpcs; j++)
            fprintf(fp, " %f %f %f %f", lev[i].bar[j].x1, lev[i].bar[j].x2, lev[i].bar[j].y1, lev[i].bar[j].y2);
        fprintf(fp, "\n%d", lev[i].partpcs);
        for(j = 0; j < lev[i].partpcs; j++)
            fprintf(fp, " %f %f %f %f", lev[i].part[j].x, lev[i].part[j].y, lev[i].part[j].r, lev[i].part[j].polarity);
        fprintf(fp, "\n%f %f %f", lev[i].goal.x, lev[i].goal.y, lev[i].goal.r);
    }
    fclose(fp);
    free_level(lev, levnum+1);
}

static int save_level(Window win, particles *used, Barlist *barl, Point goal, SDL_Surface *scr, int levnum){

    SDL_Event event;
    SDL_Surface *sub;
    TTF_Font *let;
    SDL_Rect where = { 0, 0, 0, 0};
    SDL_Color white = {255, 255, 255};

    int i;

    Menupoints answer[2];
    const int anspcs = 2;

    TTF_Init();
    let = TTF_OpenFont("times.ttf", 50);
    if (!let) {
        fprintf(stderr, "Unable to open the font! %s\n", TTF_GetError());
        exit(2);
    }

    for(i = 0; i < anspcs; i++){
        answer[i].x1 = 200+ i * 400;
        answer[i].x2 = answer[i].x1 + 280;
        answer[i].y1 = 300;
        answer[i].y2 = 370;
    }

    strcpy(answer[0].menuname, "Yes");
    strcpy(answer[1].menuname, "No");
    answer[0].namex = answer[0].x1 + 100;
    answer[1].namex = answer[1].x1 + 110;

    menudraw(scr, answer, anspcs, win, 0, 0);

    sub = TTF_RenderUTF8_Blended(let, "Do you want to save it?", white);
    where.x = 300;
    where.y = 150;
    SDL_BlitSurface(sub, NULL, scr, &where);
    SDL_FreeSurface(sub);
    SDL_Flip(scr);

    i = 0;
    while (1){
        SDL_WaitEvent(&event);

        switch (event.type){
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_LEFT){
                    rectangleRGBA(scr, answer[i].x1, answer[i].y1, answer[i].x2, answer[i].y2, 255, 255, 255, 255);
                    if(i == 0)
                        i = anspcs-1;
                    else
                        i--;
                    rectangleRGBA(scr, answer[i].x1, answer[i].y1, answer[i].x2, answer[i].y2, 255, 0, 0, 255);
                    SDL_Flip(scr);
                }
                if (event.key.keysym.sym == SDLK_RIGHT){
                    rectangleRGBA(scr, answer[i].x1, answer[i].y1, answer[i].x2, answer[i].y2, 255, 255, 255, 255);
                    if(i == anspcs-1)
                        i = 0;
                    else
                        i++;
                    rectangleRGBA(scr, answer[i].x1, answer[i].y1, answer[i].x2, answer[i].y2, 255, 0, 0, 255);
                    SDL_Flip(scr);
                }
                if (event.key.keysym.sym == '\r'){
                    switch(i){
                        case 0:
                            to_file(barl, used, goal, levnum);
                            return 1;
                        break;
                        case 1:
                            return 1;
                        break;
                    }
                }
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    return 0;
            break;
            case SDL_QUIT:
                return 0;
            break;
        }
    }
    TTF_CloseFont(let);
}

void leveldel(SDL_Surface *screen, Window win, int levelnum){
    FILE *fp;
    SDL_Event event;
    levelfile *lev;
    Menupoints *levels;
    file *record;
    int i, j = 0, delet;
    int q = 0;

    levels = (Menupoints*) malloc(levelnum * sizeof(Menupoints));

    lev = levelfile_in(0, levelnum);

    for(i = j * 10; (i < levelnum) && (i < (j+1) * 10); i++){
        if(i < j * 10 + 5){
            levels[i].x1 = 150;
            levels[i].x2 = 450;
            levels[i].y1 = 10 + (i - j * 10)*90;
            levels[i].y2 = levels[i].y1 + 60;
        }else{
            levels[i].x1 = 600;
            levels[i].x2 = 900;
            levels[i].y1 = 10 + (i - j * 10 - 5)*90;
            levels[i].y2 = levels[i].y1 + 60;
        }
        sprintf(levels[i].menuname, "%d.level", i+1);
        levels[i].namex = levels[i].x1 + 90;
        if((i + 1) % 10 == 0)
            j++;
    }

    i = 0;
    j = 0;
    if(levelnum <= (j + 1)*10){
        menudraw(screen, levels, levelnum, win, i, 1);
    }else{
        menudraw(screen, levels, (j + 1) * 10, win, i, 1);
    }

    while (!q){
        SDL_WaitEvent(&event);

        switch(event.type){
            case SDL_QUIT:
                q = 1;
            break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    q = 1;
                if(event.key.keysym.sym == SDLK_UP){
                    if(i != 0){
                        if(i == j * 10){
                            j--;
                            menudraw(screen, levels, (j + 1) * 10, win, j * 10, 1);
                            rectangleRGBA(screen, levels[j * 10].x1, levels[j * 10].y1, levels[j * 10].x2, levels[j * 10].y2, 255, 255, 255, 255);
                        }
                        rectangleRGBA(screen, levels[i].x1, levels[i].y1, levels[i].x2, levels[i].y2, 255, 255, 255, 255);
                        i--;
                        rectangleRGBA(screen, levels[i].x1, levels[i].y1, levels[i].x2, levels[i].y2, 255, 0, 0, 255);
                        SDL_Flip(screen);
                    }
                }
                if(event.key.keysym.sym == SDLK_DOWN){
                    if(i < levelnum - 1){
                        if(i == (j+1) * 10 - 1){
                            j++;
                            i++;
                            if(levelnum <= (j + 1)*10)
                                menudraw(screen, levels, levelnum, win, j*10, 1);
                            else
                                menudraw(screen, levels, (j + 1) * 10, win, j*10, 1);
                        }else{
                            rectangleRGBA(screen, levels[i].x1, levels[i].y1, levels[i].x2, levels[i].y2, 255, 255, 255, 255);
                            i++;
                            rectangleRGBA(screen, levels[i].x1, levels[i].y1, levels[i].x2, levels[i].y2, 255, 0, 0, 255);
                            SDL_Flip(screen);
                        }
                    }
                }
                if(event.key.keysym.sym == SDLK_RIGHT){
                    if((j+1) * 10 < levelnum){
                        j++;
                        i = j * 10;
                        if(levelnum <= (j + 1)*10){
                            menudraw(screen, levels, levelnum, win, i, 1);
                        }else{
                            menudraw(screen, levels, (j + 1) * 10, win, i, 1);
                        }
                    }
                }
                if(event.key.keysym.sym == SDLK_LEFT){
                    if(j){
                        j--;
                        i = j * 10;
                        menudraw(screen, levels, (j + 1) * 10, win, i, 1);
                    }
                }
                if((event.key.keysym.sym == '\r') && (i >= 3)){
                    delet = i;
                    q = 1;
                    fp = fopen("level.txt", "wt");
                    fprintf(fp, "%d\n", levelnum - 1);
                    for(i = 0; i < levelnum; i++)
                        if (i != delet){
                            if(i)
                                fprintf(fp, "\n");
                            fprintf(fp, "%d", lev[i].barpcs);
                            for(j = 0; j < lev[i].barpcs; j++)
                                fprintf(fp, " %f %f %f %f", lev[i].bar[j].x1, lev[i].bar[j].x2, lev[i].bar[j].y1, lev[i].bar[j].y2);
                            fprintf(fp, "\n%d", lev[i].partpcs);
                            for(j = 0; j < lev[i].partpcs; j++)
                                fprintf(fp, " %f %f %f %f", lev[i].part[j].x, lev[i].part[j].y, lev[i].part[j].r, lev[i].part[j].polarity);
                            fprintf(fp, "\n%f %f %f", lev[i].goal.x, lev[i].goal.y, lev[i].goal.r);
                        }
                    fclose(fp);
                    record = (file*) malloc(levelnum * sizeof(file));
                    fp = fopen("Records.txt", "rt");
                    if(fp != NULL){
                        file_in(fp, levelnum, record);
                        fclose(fp);
                        fp = fopen("Records.txt", "wt");
                        for(i = 0; i < levelnum; i++)
                            if(i != delet)
                                fprintf(fp, "%d\t%s\n", record[i].shot, record[i].us);
                        fclose(fp);
                        free(record);
                    }
                }
            break;
        }
    }
    free(levels);
    free_level(lev, levelnum);
}

void levelbuild(SDL_Surface *screen, Window window, const char* user, int levelnum){

    SDL_Event ev;
    Point goal;
    int quit = 0;
    Point use[4];
    Barrier bar[1];
    Barlist *barl, *b;
    particles *used, *p;
    int barpcs = 1, usepcs = 4;
    int bclick = 0;
    int deleted;
    int i;
    int diff = 0;
    Point temp;
    Barrier btemp;
    Levels lev;

    barl = NULL;
    p = used = NULL;
    temp.click = 0;

    goal.x = 1000;
    goal.y = 200;
    goal.r = 20;
    goal.click = 0;

    lev.radius = 30;
    lev.length = 20;
    lev.left_zone = 50;
    lev.up_zone = 50;

    use[0].x = 20;
    use[0].y = 150;
    use[0].r = 5;
    use[0].polarity = -1;
    use[1].x = 20;
    use[1].y = 250;
    use[1].r = 5;
    use[1].polarity = 1;
    use[2].x = 20;
    use[2].y = 350;
    use[2].r = 10;
    use[2].polarity = -1;
    use[3].x = 20;
    use[3].y = 450;
    use[3].r = 10;
    use[3].polarity = 1;

    bar[0].x1 = 10;
    bar[0].y1 = 50;
    bar[0].x2 = 40;
    bar[0].y2 = 80;

    for(i = 0; i < usepcs; i++)
        use[i].click = 0;

    for(p = used; p != NULL; p = p->next)
        p->element.click = 0;

    level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);

    while(!quit){

        SDL_WaitEvent(&ev);
        switch(ev.type){
            case SDL_QUIT:
                if (diff){
                    quit = save_level(window, used, barl, goal, screen, levelnum);
                    if(!quit)
                        level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                }
                else
                    quit = 1;
            break;
            case SDL_KEYDOWN:
                if(ev.key.keysym.sym == SDLK_ESCAPE){
                    if (diff){
                        quit = save_level(window, used, barl, goal, screen, levelnum);
                        if(!quit)
                            level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                    }
                    else
                        quit = 1;
                }
            break;
            case SDL_MOUSEBUTTONDOWN:
                if(ev.button.button  == SDL_BUTTON_LEFT){
                    if((ev.button.x >= bar[0].x1 && ev.button.x <= bar[0].x2) && (ev.button.y >= bar[0].y1 && ev.button.y <= bar[0].y2)){
                        bclick = 1;
                        btemp.x1 = bar[0].x1;
                        btemp.y1 = bar[0].y1;
                        btemp.x2 = bar[0].x2;
                        btemp.y2 = bar[0].y2;
                    }
                    i = 0;
                    while((i < usepcs) && !temp.click){
                        if((abs(ev.button.x - use[i].x) < use[i].r) && (abs(ev.button.y - use[i].y) <use[i].r)){
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
                        if((abs(ev.button.x - p->element.x) < p->element.r) && (abs(ev.button.y - p->element.y) < p->element.r)){
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
                    b = barl;
                    while(b != NULL && !bclick){
                        if((ev.button.x >= b->element.x1 && ev.button.x <= b->element.x2) && (ev.button.y >= b->element.y1 && ev.button.y <= b->element.y2)){
                            btemp.x1 = b->element.x1;
                            btemp.y1 = b->element.y1;
                            btemp.x2 = b->element.x2;
                            btemp.y2 = b->element.y2;
                            barl = bar_del(barl, b);
                            bclick = 1;
                        }
                        else
                            b = b->next;
                    }
                    if((abs(ev.button.x - goal.x) < goal.r) && (abs(ev.button.y - goal.y) < goal.r)){
                        goal.click = 1;
                    }
                }
                if (ev.button.button == SDL_BUTTON_RIGHT){
                    deleted = 0;
                    for(b = barl; !deleted && b != NULL; b = b->next)
                        if((ev.button.x >= b->element.x1 && ev.button.x <= b->element.x2) && (ev.button.y >= b->element.y1 && ev.button.y <= b->element.y2)){
                            barl = bar_del(barl, b);
                            deleted = 1;
                            level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                        }
                    for(p = used; !deleted && p != NULL; p = p->next)
                        if((abs(ev.button.x - p->element.x) < p->element.r) && (abs(ev.button.y - p->element.y) < p->element.r)){
                            used = del(used, p);
                            deleted = 1;
                            level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                        }
                }
            break;
            case SDL_MOUSEBUTTONUP:
                        if (ev.button.button == SDL_BUTTON_LEFT){
                            if(bclick){
                                bclick = 0;
                                if ((btemp.x1 > lev.left_zone) && (btemp.x2 < window.width)
                                && (btemp.y2 < window.high) && (btemp.y1 >= lev.up_zone)){
                                    barl = new_bar(barl, btemp);
                                    diff = 1;
                                }

                            }
                            if(temp.click){
                                temp.click = 0;
                                if (temp.x > lev.left_zone + temp.r){
                                    used = new_used_element(used, temp);
                                    diff = 1;
                                }
                            }
                            if(goal.click){
                                goal.click = 0;
                                diff = 1;
                            }

                            level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                        }
            break;
            case SDL_MOUSEMOTION:
                    if (bclick){
                        level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                        btemp.x1 += ev.motion.xrel;
                        btemp.y1 += ev.motion.yrel;
                        btemp.x2 += ev.motion.xrel;
                        btemp.y2 += ev.motion.yrel;
                        boxRGBA(screen, btemp.x1, btemp.y1, btemp.x2, btemp.y2, 255, 0, 0, 150);
                        SDL_Flip(screen);
                    }
                    if ((temp.click)
                    && (ev.motion.x + temp.r < window.width) && (ev.motion.x - temp.r > 0)
                    && (ev.motion.y + temp.r < window.high) && (ev.motion.y - temp.r > lev.up_zone)){
                        level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                        temp.x = ev.motion.x;
                        temp.y = ev.motion.y;
                        if(temp.polarity == -1)
                            filledCircleRGBA(screen, temp.x, temp.y, temp.r, 0, 0, 255, 255);
                        else
                            filledCircleRGBA(screen, temp.x, temp.y, temp.r, 255, 0, 0, 255);
                        SDL_Flip(screen);
                    }
                    if(goal.click){
                        goal.x += ev.motion.xrel;
                        goal.y += ev.motion.yrel;
                        level_draw(&lev, window, 0, goal, user, used, use, NULL, 0, usepcs, bar, barpcs, 0, levelnum, screen, barl, 1);
                    }
            break;
        }
    }
    bar_free(barl);
    part_free(used);
}
