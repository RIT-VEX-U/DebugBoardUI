#include <string>
namespace Platform {
using Data = void *;

Data init(std::string window_title);
void prerender(Data);
void postrender(Data);
void cleanup(Data cleanup);
bool shouldclose(Data);
} // namespace  Platform
