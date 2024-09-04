#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUBBLE 0
#define INSERTION 1

#define ARR_LENGTH 200
#define SEP 2

typedef struct {
    uint16_t val;
    bool red, green;
} elem;

elem arr[ARR_LENGTH];

const int window_width = 1800;
const int window_height = 1200;

const int bar_width = window_width / ARR_LENGTH;
int bar_height;

int bar_x;
int bar_y;

void scc(int code) {
    if (code < 0) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

void *scp(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

int gen() { return rand() * 1.0 / RAND_MAX * ARR_LENGTH; }

void init_arr() {
    bool chosen[ARR_LENGTH] = {0};

    int val;
    for (size_t i = 0; i < ARR_LENGTH; ++i) {
        for (val = gen(); chosen[val]; val = gen());
        arr[i].val = val + 1;
        chosen[val] = true;
    }
}

typedef struct {
    uint16_t i, j;
    bool comp, swap, stop;
} bubble_sort_state;

bubble_sort_state init_bubble_sort() {
    arr[0].red = true;
    arr[1].red = true;
    return (bubble_sort_state){
        .i = 0,
        .j = 1,
        .comp = true,
        .swap = false,
        .stop = false,
    };
}

void bubble_sort_next(bubble_sort_state *bss) {
    if (bss->stop) return;
    if (bss->comp) {
        if (arr[bss->j].val < arr[bss->j - 1].val) {
            bss->swap = true;
            bss->comp = false;
            return;
        }
    } else if (bss->swap) {
        elem temp = arr[bss->j];
        arr[bss->j] = arr[(bss->j) - 1];
        arr[bss->j - 1] = temp;
        bss->swap = false;
        bss->comp = true;
    }
    arr[bss->j - 1].red = false;
    arr[bss->j].red = false;
    if (++(bss->j) < ARR_LENGTH - (bss->i)) {
        arr[bss->j - 1].red = true;
        arr[bss->j].red = true;
        return;
    }

    bss->j = 1;
    arr[bss->j - 1].red = true;
    arr[bss->j].red = true;

    if (++(bss->i) < ARR_LENGTH) return;

    bss->stop = true;
    arr[bss->j - 1].red = false;
    arr[bss->j].red = false;
    return;
}

void bubble_sort() {
    for (size_t i = 0; i < ARR_LENGTH; ++i) {
        for (size_t j = 1; j < ARR_LENGTH - i; ++j) {
            if (arr[j].val < arr[j - 1].val) {
                elem temp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = temp;
            }
        }
    }
}

void insertion_sort() {
    for (size_t i = 1; i < ARR_LENGTH; ++i) {
        elem tmp = arr[i];
        size_t j = i;
        for (; j > 0 && arr[j - 1].val > tmp.val; --j) {
            arr[j] = arr[j - 1];
        }
        arr[j] = tmp;
    }
}

typedef struct {
    uint16_t i, j;
    bool comp, set, swap, stop;
    elem tmp;
} insertion_sort_state;

insertion_sort_state init_insertion_sort() {
    arr[0].green = true;
    return (insertion_sort_state){
        .i = 1,
        .j = 1,

        .comp = true,
        .set = false,
        .swap = false,
        .stop = false,

        .tmp = arr[1],
    };
}

void insertion_sort_next(insertion_sort_state *iss) {
    if (iss->stop) return;

    if (iss->comp) {
        if (iss->j == 0 || arr[iss->j - 1].val <= iss->tmp.val) {
            iss->comp = false;
            iss->swap = true;
            iss->set = false;
            return;
        }

        iss->comp = false;
        iss->swap = false;
        iss->set = true;
        return;
    }

    if (iss->set) {
        arr[iss->j - 1].red = false;
        arr[iss->j] = arr[(iss->j) - 1];

        --iss->j;

        arr[iss->j - 1].red = true;

        iss->comp = true;
        iss->swap = false;
        iss->set = false;

        return;
    }

    if (iss->swap) {
        arr[iss->j - 1].red = false;
        arr[iss->j] = iss->tmp;

        iss->comp = true;
        iss->set = false;
        iss->swap = false;

        arr[iss->i - 1].green = false;
        ++iss->i;
        if (iss->i >= ARR_LENGTH) {
            iss->stop = true;
            arr[iss->i - 1].green = false;
            return;
        }

        arr[iss->i - 1].green = true;
        iss->tmp = arr[iss->i];

        iss->j = iss->i;

        return;
    }
}

int main(void) {
    srand(time(NULL));

    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *window = scp(SDL_CreateWindow("sorter", 0, 0, window_width,
                                              window_height, SDL_WINDOW_SHOWN));

    SDL_Renderer *renderer =
        scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    init_arr();

#if BUBBLE
    bubble_sort_state bss = init_bubble_sort();
#elif INSERTION
    insertion_sort_state iss = init_insertion_sort();
#endif

    /* insertion_sort(); */

    bool quit = false;
    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
            }
            break;
        }
        scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
        scc(SDL_RenderClear(renderer));

        uint16_t val;
        SDL_Rect bar;
        for (size_t i = 0; i < ARR_LENGTH; ++i) {
            val = arr[i].val;
            bar_height = window_height / ARR_LENGTH * val;

            bar_x = i * bar_width;
            bar_y = window_height - bar_height;

            bar = (SDL_Rect){
                .x = bar_x,
                .y = bar_y,
                .w = bar_width - SEP,
                .h = bar_height,
            };

            if (arr[i].red) {
                scc(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
            } else if (arr[i].green) {
                scc(SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255));
            } else {
                scc(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255));
            }
            scc(SDL_RenderFillRect(renderer, &bar));
        }

#if BUBBLE
        bubble_sort_next(&bss);
#elif INSERTION
        insertion_sort_next(&iss);
#endif

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
