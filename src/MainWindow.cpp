#include "MainWindow.h"
#include "ScreenItem.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadScreens();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle("Horizon - Display Manager");
    resize(1000, 700);

    // Apply VS Code Modern Dark Theme
    QString qss = R"(
        QMainWindow { background-color: #1e1e1e; }
        QWidget { background-color: #1e1e1e; color: #cccccc; }
        QGraphicsView { background-color: #1e1e1e; border: none; }
        QToolBar { background-color: #333333; border-bottom: 1px solid #252526; spacing: 10px; padding: 5px; }
        QToolButton { color: #cccccc; background-color: transparent; border: none; padding: 6px; border-radius: 4px; }
        QToolButton:hover { background-color: #454545; }
        QToolButton:pressed { background-color: #3a3d41; }
        QMessageBox { background-color: #252526; color: #cccccc; }
        QMessageBox QPushButton { background-color: #007acc; color: white; border: none; padding: 6px 15px; border-radius: 2px; }
        QMessageBox QPushButton:hover { background-color: #0098ff; }
    )";
    qApp->setStyleSheet(qss);

    // Central Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);
    
    QAction *reloadAction = toolbar->addAction("Reload");
    QAction *applyAction = toolbar->addAction("Apply");
    QAction *saveAction = toolbar->addAction("Save As...");
    
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer); // Push actions to left or add specific layout if needed

    // Graphics View for Monitors
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#1e1e1e")));
    layout->addWidget(view);

    connect(reloadAction, &QAction::triggered, this, &MainWindow::reloadScreens);
    connect(applyAction, &QAction::triggered, this, &MainWindow::applyConfig);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveConfig);
}

void MainWindow::loadScreens()
{
    scene->clear();
    std::vector<ScreenInfo> screens = m_manager.getScreens();
    
    // Determine bounding rect to center view? For now just place them
    for (const auto &screen : screens) {
        if (!screen.active) continue; // Skip inactive for now
        
        ScreenItem *item = new ScreenItem(screen);
        // Position is scaled by 1/10
        item->setPos(screen.x / 10.0, screen.y / 10.0);
        scene->addItem(item);
    }
}

void MainWindow::reloadScreens()
{
    // Reload logic
    // We instantiate a new manager to force re-parsing (simple stateless approach for now)
    // Actually getScreens() in current manager re-parses.
    loadScreens();
}

void MainWindow::applyConfig()
{
    std::vector<ScreenInfo> currentConfig;
    
    // Gather state from scene
    for (QGraphicsItem *item : scene->items()) {
        ScreenItem *screenItem = dynamic_cast<ScreenItem*>(item);
        if (screenItem) {
            ScreenInfo info;
            info.name = screenItem->getName();
            info.width = screenItem->getWidth();
            info.height = screenItem->getHeight();
            info.x = static_cast<int>(screenItem->pos().x() * 10);
            info.y = static_cast<int>(screenItem->pos().y() * 10);
            info.currentRate = screenItem->getRate();
            info.isPrimary = screenItem->isPrimary();
            info.active = true;
            info.connected = true;
            currentConfig.push_back(info);
        }
    }
    
    QString cmd = m_manager.generateCommand(currentConfig);
    qDebug() << "Applying command:" << cmd;
    
    // Execute
    int result = system(cmd.toStdString().c_str());
    if (result == 0) {
        QMessageBox::information(this, "Horizon", "Configuration Applied Successfully!");
    } else {
        QMessageBox::critical(this, "Horizon", "Failed to apply configuration.");
    }
}

void MainWindow::saveConfig()
{
    std::vector<ScreenInfo> currentConfig;
    for (QGraphicsItem *item : scene->items()) {
        ScreenItem *screenItem = dynamic_cast<ScreenItem*>(item);
        if (screenItem) {
            ScreenInfo info;
            info.name = screenItem->getName();
            info.width = screenItem->getWidth();
            info.height = screenItem->getHeight();
            info.x = static_cast<int>(screenItem->pos().x() * 10);
            info.y = static_cast<int>(screenItem->pos().y() * 10);
            info.currentRate = screenItem->getRate();
            info.isPrimary = screenItem->isPrimary();
            info.active = true;
            info.connected = true;
            currentConfig.push_back(info);
        }
    }
    
    QString cmd = m_manager.generateCommand(currentConfig);

    QString fileName = QFileDialog::getSaveFileName(this, "Save Script", "layout.sh", "Shell Script (*.sh)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "#!/bin/sh\n# Generated by Horizon\n";
            out << cmd << "\n";
            file.close();
            
            // Make executable
            QFile::setPermissions(fileName, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ReadOther);
        }
    }
}
