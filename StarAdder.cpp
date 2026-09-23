#include "StarAdder.h"

#include <QRandomGenerator>
#include <QDebug>

StarAdder::StarAdder(QHash<int, bool>* stars, QMutex* mutex, int count, int id, std::shared_ptr<spdlog::logger> log, std::shared_ptr<spdlog::logger> err_log)
    : stars(stars), mutex(mutex), starCount(count), threadId(id), logger(log), error_logger(err_log) {}

void StarAdder::run() {
    logger->info("Thread {} baslatıldı.", threadId);
    qDebug() << "Thread" << threadId << "başlatıldı.";

    int added = 0;
    try {
        while (added < starCount) {
            int originalkey = QRandomGenerator::global()->bounded(0, 2500);
            int key = originalkey % stars->size();
            logger->info("Thread {} Orjinal Key: {} | Mod sonrası Key: {}", threadId, originalkey, key);
            qDebug() << "Thread" << threadId << "Orijinal key:" << originalkey << "Mod sonrası key:" << key;
            addStar(key);
            QThread::msleep(10);
            added++;
        }
        logger->info("Thread {} tamamlandı", threadId);
        qDebug() << "Thread" << threadId << "tamamlandı.";
    } catch (const std::exception& ex) {
        error_logger->error("Thread {}'de bir hata oluştu: {}", threadId, ex.what());
        logger->info("Thread {} hata nedeniyle sonlandırıldı.", threadId);
    } catch (...) {
        error_logger->error("Thread {}'de bilinmeyen bir hata oluştu.", threadId);
        logger->info("Thread {} bilinmeyen hata nedeniyle sonlandırıldı.", threadId);
    }

    emit starsAdded();
}

void StarAdder::addStar(int key) {
    // Boş hücre arama ve işaretleme tek adımda yapılmalı; aksi halde iki thread
    // aynı boş hücreyi görüp oraya aynı anda yıldız koyabilir (race condition).
    QMutexLocker locker(mutex);

    int originalKey = key;

    while (stars->contains(key) && stars->value(key)) {
        key = (key + 1) % stars->size();
        qDebug() << "Thread" << threadId << "Anahtar dolu yeni key:" << key;
        if (key == originalKey) {
            logger->warn("Tüm butonlar dolu yıldız eklenmedi çıkış yapıldı.");
            qDebug() << "Thread " << threadId << "Tüm butonlar dolu çıkış";
            return;
        }
    }

    if (stars->contains(key)) {
        (*stars)[key] = true;
        logger->info("Thread:{} Yıldız ekliyor key : {}", threadId, key);
        qDebug() << "Thread" << threadId << "Yıldız ekliyor Key:" << key;
        // Butonu bu thread değil, sinyali alan ana thread günceller
        emit starPlaced(key);
    }
}
