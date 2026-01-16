#include "XrandrManager.h"
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

XrandrManager::XrandrManager() {
    parseXrandrOutput();
}

std::vector<ScreenInfo> XrandrManager::getScreens() {
    // If empty re-parse? For now just return
    if (screens.empty()) parseXrandrOutput();
    return screens;
}

void XrandrManager::parseXrandrOutput() {
    screens.clear();
    
    QProcess process;
    process.start("xrandr", QStringList() << "--query");
    process.waitForFinished();
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');
    
    // Regex for connected line: e.g. "HDMI-0 connected 1920x1080+0+0 ..."
    // A simplified regex to capture name and primary resolution if active
    QRegularExpression reConnected(R"(^(\S+) connected)");
    QRegularExpression reGeometry(R"((\d+)x(\d+)\+(\d+)\+(\d+))");

    ScreenInfo currentScreen;
    bool parsingScreen = false;

    for (const QString &line : lines) {
        QRegularExpressionMatch match = reConnected.match(line);
        if (match.hasMatch()) {
            if (parsingScreen) {
                screens.push_back(currentScreen);
            }
            
            parsingScreen = true;
            currentScreen = ScreenInfo();
            currentScreen.name = match.captured(1);
            currentScreen.connected = true;
            
            // Check if active (has geometry string)
            QRegularExpressionMatch geoMatch = reGeometry.match(line);
            if (geoMatch.hasMatch()) {
                currentScreen.active = true;
                currentScreen.width = geoMatch.captured(1).toInt();
                currentScreen.height = geoMatch.captured(2).toInt();
                currentScreen.x = geoMatch.captured(3).toInt();
                currentScreen.y = geoMatch.captured(4).toInt();
            } else {
                currentScreen.active = false;
            }
        }
        // If we find resolution lines indented, we could pick the preferred one if not active
        // skipping for MVP, assuming active or just parsing basic "connected"
    }
    if (parsingScreen) {
        screens.push_back(currentScreen);
    }
}

QString XrandrManager::generateCommand(const std::vector<ScreenInfo>& screens) {
    QString cmd = "xrandr";
    for (const auto &s : screens) {
        cmd += " --output " + s.name;
        if (s.active) {
            cmd += QString(" --mode %1x%2 --pos %3x%4").arg(s.width).arg(s.height).arg(s.x).arg(s.y);
        } else {
            cmd += " --off";
        }
    }
    return cmd;
}
