#include "basecleaner.h"

BaseCleaner::BaseCleaner(QObject *parent) : QObject(parent) {}

qint64 BaseCleaner::totalSize() const {
    qint64 total = 0;
    for (const auto &e : m_entries) total += e.sizeBytes;
    return total;
}

void BaseCleaner::emitLog(const QString &msg) {
    emit logMessage(msg);
}
