#ifndef STEP_H
#define STEP_H

#include <QObject>

class Step : public QObject
{
    Q_OBJECT
public:
    explicit Step(QObject *parent = 0);
    ~Step();

    int _movedId;
    int _killId;
    int _angleFrom;
    int _rFrom;
    int _angleTo;
    int _rTo;

signals:

public slots:
};

#endif // STEP_H
