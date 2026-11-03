#include <GLFW/glfw3.h>

class Draw {
    public:
    // Creates a window of the given width, height and title and calls the given drawing function 
    Draw(int width, int height, const char *windowTitle, void func(Draw &d));
    
    // Draws a point at coordinates (x, y)
    void point(float x, float y);
    
    // Draws a line from coordinates (x1, y1) to (x2, y2)
    void line(float x1, float y1, float x2, float y2);
    
    // Sets the drawing color; components range from 0 to 1
    void color(float r, float g, float b);
    
    // Returns the time in seconds since starting the program
    double getTime();

    private:
    // Callback functions cannot be class instance function so must be static
    static void keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
};