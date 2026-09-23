#ifndef GRIDWINDOW_H
#define GRIDWINDOW_H

#include <QWidget>
#include <QHash>
#include <QPushButton>
#include <QTextEdit>
#include <QString>
#include <memory>
#include <spdlog/spdlog.h>

class GridWindow : public QWidget {
    Q_OBJECT
public:
    GridWindow();

private slots:
    void onStarsAdded();

private:
    void dijkstraAlgorithm(int startKey);
    void searchStars(int key);
    void deleteStars(int key);
    void loadLogFile(const QString& filePath);

    QHash<int, QPushButton*> buttonHash;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::logger> error_logger;
    QTextEdit* textEdit;
};

#endif // GRIDWINDOW_H
