#include <time.h>

#include "utils/input.h"
#include "utils/draw.h"
#include "utils/audio.h"

#include "splash.h"
#include "title.h"
#include "game.h"
#include "score.h"

int main() {
    srand(time(NULL));
    
    xbox_draw_init();
    xbox_text_init(80);
    
    xbox_audio_init();
    
    input_init();
    
    // Show a static splash on screen for a few seconds
    splash_run();
    
    title_run();
    game_run();
    
    // input_free(); // BUG: This can actually cause a crash on exit sometimes!
    xbox_audio_free();
    
    xbox_text_free();
    xbox_draw_free();
    
    return 0;
}
