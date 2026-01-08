#include "SwarmOperationHandler.h"
#include <QDebug>

SwarmOperationHandler::SwarmOperationHandler(QObject *parent)
    : QObject(parent)
    , _processTimer(new QTimer(this))
    , _aggregationTimer(new QTimer(this))
{
    // 设置队列处理定时器
    connect(_processTimer, &QTimer::timeout, this, &SwarmOperationHandler::processAckQueue);
    _processTimer->setInterval(QUEUE_PROCESS_INTERVAL);
    _processTimer->start();

    // 设置聚合检查定时器
    connect(_aggregationTimer, &QTimer::timeout, this, &SwarmOperationHandler::checkAggregation);
    _aggregationTimer->setInterval(AGGREGATION_WINDOW);
}

SwarmOperationHandler::~SwarmOperationHandler()
{
    _processTimer->stop();
    _aggregationTimer->stop();
}

void SwarmOperationHandler::handleOperationAck(int sysId, int opType, int result, int oldValue, int newValue)
{
    qDebug() << "[SwarmOperationHandler] 收到ACK: sysId=" << sysId
             << ", opType=" << opType
             << ", result=" << result
             << ", oldValue=" << oldValue
             << ", newValue=" << newValue;

    SwarmOperationAckData data;
    data.sysId = sysId;
    data.opType = opType;
    data.result = result;
    data.oldValue = oldValue;
    data.newValue = newValue;
    data.timestamp = QDateTime::currentMSecsSinceEpoch();

    // 添加到聚合缓冲区
    _aggregationBuffer.append(data);

    // 启动聚合定时器（如果未启动）
    if (!_aggregationTimer->isActive()) {
        _aggregationTimer->start();
    }

    // 如果队列已满，移除最旧的消息
    while (_ackQueue.size() >= MAX_QUEUE_SIZE) {
        _ackQueue.dequeue();
    }

    // 添加到队列
    _ackQueue.enqueue(data);
}

void SwarmOperationHandler::processAckQueue()
{
    if (_ackQueue.isEmpty()) {
        return;
    }

    // 取出队首消息
    SwarmOperationAckData data = _ackQueue.dequeue();

    // 格式化消息
    QString message = formatAckMessage(data);

    // 发送信号通知QML
    emit operationAckReceived(data.sysId, data.opType, data.result,
                              data.oldValue, data.newValue, message);
}

void SwarmOperationHandler::checkAggregation()
{
    _aggregationTimer->stop();

    if (_aggregationBuffer.isEmpty()) {
        return;
    }

    // 检查是否有多个相同类型的操作（组合并/拆分）
    QMap<int, QList<int>> groupChangeMap; // newValue -> list of sysIds

    for (const auto& data : _aggregationBuffer) {
        if (data.opType == OP_GROUP_CHANGE && data.result == RESULT_SUCCESS) {
            groupChangeMap[data.newValue].append(data.sysId);
        }
    }

    // 如果有多架飞机切换到同一组，发送聚合消息
    for (auto it = groupChangeMap.begin(); it != groupChangeMap.end(); ++it) {
        if (it.value().size() > 1) {
            QStringList sysIdList;
            for (int sysId : it.value()) {
                sysIdList.append(QString::number(sysId));
            }
            QString message = QString("飞机 %1 已加入第%2组")
                              .arg(sysIdList.join(", "))
                              .arg(it.key());
            emit aggregatedAckReceived(message, it.value().size());
        }
    }

    // 清空聚合缓冲区
    _aggregationBuffer.clear();
}

QString SwarmOperationHandler::formatAckMessage(const SwarmOperationAckData& data) const
{
    QString message;

    if (data.result == RESULT_SUCCESS) {
        if (data.opType == OP_GROUP_CHANGE) {
            message = QString("飞机%1: 组号从%2切换到%3 成功")
                      .arg(data.sysId)
                      .arg(data.oldValue)
                      .arg(data.newValue);
        } else if (data.opType == OP_LEADER_CHANGE) {
            QString oldRole = getRoleText(data.oldValue);
            QString newRole = getRoleText(data.newValue);
            message = QString("飞机%1: 角色从%2切换到%3 成功")
                      .arg(data.sysId)
                      .arg(oldRole)
                      .arg(newRole);
        }
    } else {
        if (data.opType == OP_GROUP_CHANGE) {
            message = QString("飞机%1: 组号切换失败").arg(data.sysId);
        } else if (data.opType == OP_LEADER_CHANGE) {
            message = QString("飞机%1: 角色切换失败").arg(data.sysId);
        }
    }

    return message;
}

QString SwarmOperationHandler::getOperationTypeText(int opType) const
{
    switch (opType) {
    case OP_GROUP_CHANGE:
        return "组号变更";
    case OP_LEADER_CHANGE:
        return "角色变更";
    default:
        return "未知操作";
    }
}

QString SwarmOperationHandler::getResultText(int result) const
{
    return (result == RESULT_SUCCESS) ? "成功" : "失败";
}

QString SwarmOperationHandler::getRoleText(int value) const
{
    return (value == 1) ? "主机" : "从机";
}
