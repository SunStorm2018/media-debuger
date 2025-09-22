// SpinBoxSynchronizer.h
#ifndef SPINBOXSYNCHRONIZER_H
#define SPINBOXSYNCHRONIZER_H

#include "statesynchronizer.h"

#include <QSpinBox>

class SpinBoxSynchronizer : public StateSynchronizer<QSpinBox>
{
public:
    explicit SpinBoxSynchronizer(QObject *parent = nullptr);
};

#endif // SPINBOXSYNCHRONIZER_H
