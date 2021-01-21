#include "application.h"
#include "gef.h"

#include "aud.h"

int main(int argc, char** argv) {
    application app = {0};
    application_init(&app);

    while (app.keep_going) {
        gef_start_frame(&app.gc);
        application_handle_input(&app);
        application_draw(&app);
        gef_end_frame(&app.gc);
    }

    return 0;
}