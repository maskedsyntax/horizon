#ifndef XRANDRMANAGER_H
#define XRANDRMANAGER_H

#include <QString>
#include <vector>

struct ResolutionMode {
    int width;
    int height;
    std::vector<double> rates;
};

struct ScreenInfo {
    QString name;
    int width;
    int height;
    int x;
    int y;
    double currentRate;
    bool connected;
    bool active;
    bool isPrimary;
    std::vector<ResolutionMode> modes;
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
