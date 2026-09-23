#ifndef GRIDWINDOW_H
#define GRIDWINDOW_H

#include <QWidget>
#include <QHash>
#include <QMutex>
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
    void onStarPlaced(int key);
    void onStarsAdded();

private:
    void dijkstraAlgorithm(int startKey);
    void searchStars(int key);
    void deleteStars(int key);
    void loadLogFile(const QString& filePath);

    QHash<int, QPushButton*> buttonHash;  // arayüz: sadece ana thread kullanır
    QHash<int, bool> stars;               // veri: hangi hücrede yıldız var
    QMutex starsMutex;                    // stars tablosunu thread'ler arasında korur
    int threadCount = 3;
    int finishedThreads = 0;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::logger> error_logger;
    QTextEdit* textEdit;
};

#endif // GRIDWINDOW_H
