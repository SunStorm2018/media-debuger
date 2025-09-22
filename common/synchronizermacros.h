// SynchronizerMacros.h
#ifndef SYNCHRONIZERMACROS_H
#define SYNCHRONIZERMACROS_H

#include "lineeditsynchronizer.h"
#include "checkboxsynchronizer.h"
#include "spinboxsynchronizer.h"

// Synchronize multiple QLineEdit widgets
#define SYNC_LINEEDITS(...) \
do { \
        static LineEditSynchronizer synchronizer; \
        const int count = sizeof((QLineEdit*[]){__VA_ARGS__}) / sizeof(QLineEdit*); \
        QLineEdit* edits[] = {__VA_ARGS__}; \
        for (int i = 0; i < count; ++i) { \
            if (edits[i]) synchronizer.addObject(edits[i]); \
    } \
} while (0)

// Synchronize multiple QCheckBox widgets
#define SYNC_CHECKBOXES(...) \
    do { \
        static CheckBoxSynchronizer synchronizer; \
        const int count = sizeof((QCheckBox*[]){__VA_ARGS__}) / sizeof(QCheckBox*); \
        QCheckBox* checkboxes[] = {__VA_ARGS__}; \
        for (int i = 0; i < count; ++i) { \
            if (checkboxes[i]) synchronizer.addObject(checkboxes[i]); \
    } \
} while (0)

// Synchronize multiple QSpinBox widgets
#define SYNC_SPINBOXES(...) \
    do { \
            static SpinBoxSynchronizer synchronizer; \
            const int count = sizeof((QSpinBox*[]){__VA_ARGS__}) / sizeof(QSpinBox*); \
            QSpinBox* spinboxes[] = {__VA_ARGS__}; \
            for (int i = 0; i < count; ++i) { \
                if (spinboxes[i]) synchronizer.addObject(spinboxes[i]); \
        } \
    } while (0)

// Generic synchronization macro (requires manual accessor specification)
#define SYNC_OBJECTS(type, getter, setter, signal, ...) \
        do { \
            static StateSynchronizer<type> synchronizer; \
            synchronizer.setAccessors(getter, setter, signal); \
            const int count = sizeof((type*[]){__VA_ARGS__}) / sizeof(type*); \
            type* objects[] = {__VA_ARGS__}; \
            for (int i = 0; i < count; ++i) { \
                if (objects[i]) synchronizer.addObject(objects[i]); \
        } \
    } while (0)

#endif // SYNCHRONIZERMACROS_H
