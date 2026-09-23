#include "GridWindow.h"
#include "StarAdder.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QSet>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <spdlog/sinks/rotating_file_sink.h>
#include <limits>
#include <queue>

GridWindow::GridWindow() {
    try {
        logger = spdlog::rotating_logger_mt("log_file", "logs/my_log.txt", 1024 * 1024 * 5, 3);
        error_logger = spdlog::rotating_logger_mt("error_log", "logs/error_log.txt", 1024 * 1024 * 5, 3);

        logger->set_level(spdlog::level::info);
        error_logger->set_level(spdlog::level::err);

        logger->warn("==== Yeni loglama işlemi başladı ====");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QGridLayout *gridLayout = new QGridLayout();

        int gridSize = 10;

        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                QPushButton* button = new QPushButton();
                button->setFixedSize(30, 30);
                button->setText("");
                gridLayout->addWidget(button, i, j);

                int key = i * gridSize + j;
                buttonHash.insert(key, button);
            }
        }

        mainLayout->addLayout(gridLayout);

        textEdit = new QTextEdit();
        textEdit->setFixedHeight(200);
        mainLayout->addWidget(textEdit);

        loadLogFile("logs/my_log.txt");

        setLayout(mainLayout);

        int count = 30;
        int threadCount = 1;
        for (int i = 0; i < threadCount; ++i) {
            StarAdder* staradder = new StarAdder(&buttonHash, count / threadCount, i + 1, logger, error_logger);
            connect(staradder, &StarAdder::starsAdded, this, &GridWindow::onStarsAdded);
            staradder->start();
        }
    } catch (const std::exception& ex) {
        error_logger->error("Bir hata oluştu: {}", ex.what());
    } catch (...) {
        error_logger->error("Bilinmeyen bir hata oluştu.");
    }
}

void GridWindow::onStarsAdded() {
    static bool processed = false;
    if (!processed) {
        searchStars(30);

        deleteStars(75);
        dijkstraAlgorithm(0);

        processed = true;
    }
}

void GridWindow::dijkstraAlgorithm(int startKey) {
    const int INF = std::numeric_limits<int>::max();
    QHash<int,int> distances;
    QHash<int,int> previous;
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<>> pq;

    QList<int> keys = buttonHash.keys();
    for (int i = 0; i < keys.size(); ++i) {
        int key = keys[i];
        distances[key] = INF;
        previous[key] = -1;
    }

    distances[startKey] = 0;
    pq.push({0,startKey});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        int directions[4] = {-1, 1, -10, 10}; // Hareket yönleri için bir dizi tanımlandı.
        for (int i = 0; i < 4; ++i) {
            int direction = directions[i]; // Her döngüde farklı bir yönü alıyoruz.
            int v = u + direction;

            if (buttonHash.contains(v)) {
                int weight = 1;
                if (distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    previous[v] = u;
                    pq.push({distances[v], v});
                }
            }
        }
    }

    QString result = "Yıldızlar Arası En Kısa Yollar:\n";
    QSet<int> starKeys;
    auto buttonValues = buttonHash.values();
    for (int i = 0; i < buttonValues.size(); ++i) {
        QPushButton* button = buttonValues[i];
        if (button->text() == "*") {
            starKeys.insert(buttonHash.key(button));
        }
    }

    auto starKeyList = starKeys.values();
    for (int i = 0; i < starKeyList.size(); ++i) {
        int starKey = starKeyList[i];
        if (starKey == startKey) continue; // Başlangıç düğümünü atla

        if (distances[starKey] == INF) {
            result += QString("0'dan %1'e ulaşım yok. \n").arg(starKey);
        } else {
            QString path;
            int current = starKey;
            while (current != startKey && current != -1) {
                path.prepend(QString::number(current) + "->");
                current = previous[current];
            }
            path.prepend(QString::number(startKey));
            result += QString("0'dan %1'e olan mesafe =%2 || Yolu:%3\n")
                          .arg(starKey)
                          .arg(distances[starKey])
                          .arg(path);
        }
    }

    logger->info(result.toStdString());
    qDebug() << result;
    textEdit->append(result);
}

void GridWindow::searchStars(int key) {
    key = key % buttonHash.size();

    if (buttonHash.contains(key) && buttonHash[key]->text() == "*") {
        logger->info("Search| {}. keyde yıldız bulundu.", key);
        qDebug() << "Search|" << key << ". keyde yıldız bulundu.";
    } else {
        logger->warn("Search| {}. keyde yıldız bulunamadı.", key);
        qDebug() << "Search|" << key << ". keyde yıldız bulunamadı.";
    }
}

void GridWindow::deleteStars(int key) {
    key = key % buttonHash.size();
    if (buttonHash.contains(key) && buttonHash[key]->text() == "*") {
        buttonHash[key]->setText("");
        logger->info("Delete| {}. keyde yıldız bulundu ve silindi.", key);
        qDebug() << "Delete|" << key << ". keyde yıldız bulundu ve silindi.";
    } else {
        logger->warn("Delete| {}. keyde yıldız bulunamadı. Ve silme işlemi yapılamadı", key);
        qDebug() << "Delete|" << key << ". keyde yıldız bulunamadı ve silme işlemi yapılamadı..";
    }
}

void GridWindow::loadLogFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString logs;
        QString line;
        QStringList listLogs;
        bool islast = false;

        while (!in.atEnd()) {
            line = in.readLine();
            if (line.contains("==== Yeni loglama işlemi başladı ====")) {
                islast = true;
                listLogs.clear();
            }
            if (islast && line.contains("[warning]")) {
                listLogs.append(line);
            }
        }

        logs = listLogs.join("\n");
        textEdit->setText(logs);
        file.close();
    }
}
