#include "StarAdder.h"

#include <QRandomGenerator>
#include <QDebug>

StarAdder::StarAdder(QHash<int, QPushButton*>* buttons, int count, int id, std::shared_ptr<spdlog::logger> log, std::shared_ptr<spdlog::logger> err_log)
    : buttonHash(buttons), starCount(count), threadId(id), logger(log), error_logger(err_log) {}

void StarAdder::run() {
    logger->info("Thread {} baslatıldı.", threadId);
    qDebug() << "Thread" << threadId << "başlatıldı.";

    int added = 0;
    try {
        while (added < starCount) {
            int originalkey = QRandomGenerator::global()->bounded(0, 2500);
            int key = originalkey % buttonHash->size();
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
    int originalKey = key;

    while (buttonHash->contains(key) && buttonHash->value(key)->text() == "*") {
        key = (key + 1) % buttonHash->size();
        qDebug() << "Thread" << threadId << "Anahtar dolu yeni key:" << key;
        if (key == originalKey) {
            logger->warn("Tüm butonlar dolu yıldız eklenmedi çıkış yapıldı.");
            qDebug() << "Thread " << threadId << "Tüm butonlar dolu çıkış";
            return;
        }
    }

    if (buttonHash->contains(key)) {
        buttonHash->value(key)->setText("*");
        logger->info("Thread:{} Yıldız ekliyor key : {}", threadId, key);
        qDebug() << "Thread" << threadId << "Yıldız ekliyor Key:" << key;
    }
}
