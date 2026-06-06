#pragma once
#include <QStringList>

struct TimeControl {
	int initial;
	int increment;
};

inline TimeControl parseTimeControl(const QString& text)
{
    QStringList parts = text.split('+');

    TimeControl time;

    time.initial = parts[0].toInt() * 60 * 1000;
    time.increment = parts.size() > 1
        ? parts[1].toInt() * 1000
        : 0;

    return time;
}