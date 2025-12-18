#pragma once

#include "MAVLinkProtocol.h"
#include "Vehicle.h"
#include "QmlObjectListModel.h"
#include "Fact.h"
#include "FactMetaData.h"
#include <QObject>
#include <Qstring>
#include <QMetaObject>
#include <QStringListModels>
#include <QList>

class Vehicle;

class Swarm:public QStringListModel
{
    Q_OBJECT

public:
    Swarm();
    virtual ~Swarm();
    


}