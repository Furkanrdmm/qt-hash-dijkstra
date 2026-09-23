#ifndef STARADDER_H
#define STARADDER_H

#include <QThread>
#include <QHash>
#include <QPushButton>
#include <memory>
#include <spdlog/spdlog.h>

class StarAdder : public QThread {
    Q_OBJECT
public:
    StarAdder(QHash<int, QPushButton*>* buttons, int count, int id, std::shared_ptr<spdlog::logger> log, std::shared_ptr<spdlog::logger> err_log);

protected:
    void run() override;

signals:
    void starsAdded();

private:
    void addStar(int key);

    QHash<int, QPushButton*>* buttonHash;
    int starCount;
    int threadId;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::logger> error_logger;
};

#endif // STARADDER_H
