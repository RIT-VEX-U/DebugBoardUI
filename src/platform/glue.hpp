#include <string>
namespace Platform {
using Data = void *;

/**
 * Perform initial setup of the window.
 * Should be called as the first thing in main
 * Sets up rendering, initialize any OS support, etc
 * @param window_title the title of the OS window
 * @return platform data. Keep this and pass it in to further Platform functions
 */
Data init(const std::string &window_title);
/**
 * Called before beginning rendering for a given iteration of the game loop
 * Prepares for ImGui functions to be called
 */
void prerender(Data);
/**
 * Called after building the ImGui UI
 */
void postrender(Data);
void cleanup(Data);
/**
 * Checks if the windows is requesting to close
 * If the X button on the OS window is pressed, this will return true
 */
bool shouldclose(Data);
} // namespace  Platform
