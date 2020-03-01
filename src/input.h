#ifndef Input_h
#define Input_h

#include "math/vector.hpp"

struct GLFWwindow;

class Input
{
public:
    enum class Mouse
    {
        Left,
        Middle,
        Right,
        Other,
        Total,
    };
    enum class Key
    {
        Other,
        Total,
    };

public:
    static void Init(GLFWwindow* window);
    static void Update();
    static void Purge();
    static Vec2 MouseMotion();
    static bool MousePressed(Mouse mouse_button);
    static bool MouseReleased(Mouse mouse_button);
    static bool MouseDown(Mouse mouse_button);
    static bool KeyPressed();
    static bool KeyReleased();
    static bool KeyDown();
private:
    static void MouseButtonCallack(
        GLFWwindow* window,
        int button,
        int action,
        int mods);
    static void CursorPosCallback(
        GLFWwindow* window,
        double x_pos,
        double y_pos);
    static Mouse GLFWMouseToMouse(int button);
    static int MouseToGLFWMouse(Mouse button);
    static GLFWwindow* _window;
    static Vec2 _mouse_position;
    static Vec2 _mouse_motion;
    // TODO: These should all be bit fields if they aren't
    // converted to that already by the compiler.
    static bool _mouse_pressed[(int)Mouse::Total];
    static bool _mouse_released[(int)Mouse::Total];
    static bool _key_pressed[(int)Key::Total];
    static bool _key_released[(int)Key::Total];

    static const int _invalid_glfw_button = -1;
};

#endif