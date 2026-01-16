#include "XrandrManager.h"
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

XrandrManager::XrandrManager() {
    parseXrandrOutput();
}

std::vector<ScreenInfo> XrandrManager::getScreens() {
    // Always re-parse to get fresh data
    parseXrandrOutput();
    return screens;
}

void XrandrManager::parseXrandrOutput() {
    screens.clear();
    
    QProcess process;
    process.start("xrandr", QStringList() << "--query");
    process.waitForFinished();
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');
    
    // Regex for connected line: e.g. "HDMI-0 connected primary 1920x1080+0+0 ..."
    QRegularExpression reConnected(R"(^(\S+) (connected|disconnected))");
    QRegularExpression reGeometry(R"((\d+)x(\d+)\+(\d+)\+(\d+))");
    
    // Regex for mode line: "   1920x1080     60.00*+  50.00"
    QRegularExpression reMode(R"(^\s+(\d+)x(\d+)\s+(.+))");

    ScreenInfo currentScreen;
    bool parsingScreen = false;
    currentScreen.connected = false;

    for (const QString &line : lines) {
        QRegularExpressionMatch matchConn = reConnected.match(line);
        if (matchConn.hasMatch()) {
            if (parsingScreen && currentScreen.connected) {
                screens.push_back(currentScreen);
            }
            
            parsingScreen = true;
            currentScreen = ScreenInfo();
            currentScreen.name = matchConn.captured(1);
            currentScreen.connected = (matchConn.captured(2) == "connected");
            currentScreen.active = false;
            currentScreen.isPrimary = line.contains("primary");
            currentScreen.currentRate = 0.0;
            currentScreen.width = 0;
            currentScreen.height = 0;
            currentScreen.x = 0;
            currentScreen.y = 0;

            if (currentScreen.connected) {
                // Check if active (has geometry string)
                QRegularExpressionMatch geoMatch = reGeometry.match(line);
                if (geoMatch.hasMatch()) {
                    currentScreen.active = true;
                    currentScreen.width = geoMatch.captured(1).toInt();
                    currentScreen.height = geoMatch.captured(2).toInt();
                    currentScreen.x = geoMatch.captured(3).toInt();
                    currentScreen.y = geoMatch.captured(4).toInt();
                }
            }
            continue; // Move to next line (modes usually follow)
        }

        if (parsingScreen && currentScreen.connected) {
            QRegularExpressionMatch matchMode = reMode.match(line);
            if (matchMode.hasMatch()) {
                ResolutionMode mode;
                mode.width = matchMode.captured(1).toInt();
                mode.height = matchMode.captured(2).toInt();
                
                QString ratesStr = matchMode.captured(3);
                QStringList ratesParts = ratesStr.split(' ', Qt::SkipEmptyParts);
                
                for (const QString &rPart : ratesParts) {
                    QString cleanRate = rPart;
                    bool isCurrent = cleanRate.contains('*');
                    cleanRate.remove('*');
                    cleanRate.remove('+');
                    
                    double rate = cleanRate.toDouble();
                    if (rate > 0) {
                        if (isCurrent) {
                            currentScreen.currentRate = rate;
                        }
                        
                        // Check for duplicates before adding
                        bool exists = false;
                        for (double existing : mode.rates) {
                            if (std::abs(existing - rate) < 0.01) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            mode.rates.push_back(rate);
                        }
                    }
                }
                currentScreen.modes.push_back(mode);
            }
        }
    }
    if (parsingScreen && currentScreen.connected) {
        screens.push_back(currentScreen);
    }
}

QString XrandrManager::generateCommand(const std::vector<ScreenInfo>& screens) {
    QString cmd = "xrandr";
    bool primarySet = false;
    
    // First pass to check if multiple primaries set? (Should enforce one in UI)
    // We just respect the list provided.
    
    for (const auto &s : screens) {
        cmd += " --output " + s.name;
        if (s.active) {
            cmd += QString(" --mode %1x%2 --pos %3x%4 --rate %5")
                    .arg(s.width)
                    .arg(s.height)
                    .arg(s.x)
                    .arg(s.y)
                    .arg(s.currentRate, 0, 'f', 2);
            
            if (s.isPrimary && !primarySet) {
                cmd += " --primary";
                primarySet = true;
            }
        } else {
            cmd += " --off";
        }
    }
    return cmd;
}