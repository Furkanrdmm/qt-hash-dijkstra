#ifndef STARADDER_H
#define STARADDER_H

#include <QThread>
#include <QHash>
#include <QMutex>
#include <memory>
#include <spdlog/spdlog.h>

// Arka planda çalışıp hash tablosuna rastgele yıldız yerleştiren thread.
// Arayüze (butonlara) dokunmaz; yerleştirdiği her yıldızı starPlaced sinyaliyle
// ana thread'e bildirir. Paylaşılan tabloya erişim mutex ile korunur.
class StarAdder : public QThread {
    Q_OBJECT
public:
    StarAdder(QHash<int, bool>* stars, QMutex* mutex, int count, int id, std::shared_ptr<spdlog::logger> log, std::shared_ptr<spdlog::logger> err_log);

protected:
    void run() override;

signals:
    void starPlaced(int key);
    void starsAdded();

private:
    void addStar(int key);

    QHash<int, bool>* stars;
    QMutex* mutex;
    int starCount;
    int threadId;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::logger> error_logger;
};

#endif // STARADDER_H
