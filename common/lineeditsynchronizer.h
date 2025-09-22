// LineEditSynchronizer.h
#ifndef LINEEDITSYNCHRONIZER_H
#define LINEEDITSYNCHRONIZER_H

#include "statesynchronizer.h"
#include <QLineEdit>

class LineEditSynchronizer : public StateSynchronizer<QLineEdit>
{
public:
    explicit LineEditSynchronizer(QObject *parent = nullptr);
};

#endif // LINEEDITSYNCHRONIZER_H
