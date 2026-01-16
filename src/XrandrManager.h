#ifndef XRANDRMANAGER_H
#define XRANDRMANAGER_H

#include <QString>
#include <vector>

struct ScreenInfo {
    QString name;
    int width;
    int height;
    int x;
    int y;
    bool connected;
    bool active;
};

class XrandrManager {
public:
    XrandrManager();
    std::vector<ScreenInfo> getScreens();
    QString generateCommand(const std::vector<ScreenInfo>& screens);

private:
    void parseXrandrOutput();
    std::vector<ScreenInfo> screens;
};

#endif // XRANDRMANAGER_H
