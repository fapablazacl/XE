#include <GL/gl.h>

int main(void) {
    /* Verify GL symbols are available (link-time check) */
    (void)glGetString;
    (void)glClear;
    return 0;
}
