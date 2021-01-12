#include "application.h"
#include "gef.h"

int main(int argc, char** argv) {
    application app = application_init();

    while (app.keep_going) {
        gef_start_frame(&app.gc);
        application_handle_input(&app);
        application_step(&app);
        application_draw(&app);
        gef_end_frame(&app.gc);
    }

    return 0;
}