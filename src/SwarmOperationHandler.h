#ifndef SWARM_OPERATION_HANDLER_H
#define SWARM_OPERATION_HANDLER_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QDateTime>

// 操作类型常量
#define OP_GROUP_CHANGE  1
#define OP_LEADER_CHANGE 2

// 结果常量
#define RESULT_SUCCESS 0
#define RESULT_FAILED  1

/**
 * @brief 操作确认消息结构
 */
struct SwarmOperationAckData {
    int sysId;          // 飞机系统ID
    int opType;         // 操作类型
    int result;         // 操作结果
    int oldValue;       // 旧值
    int newValue;       // 新值
    qint64 timestamp;   // 接收时间戳
};

/**
 * @brief 集群操作反馈处理类
 *
 * 负责接收和处理来自PX4的SWARM_OPERATION_ACK消息，
 * 并通过信号通知QML层显示弹窗。
 */
class SwarmOperationHandler : public QObject
{
    Q_OBJECT

public:
    explicit SwarmOperationHandler(QObject *parent = nullptr);
    ~SwarmOperationHandler();

    /**
     * @brief 处理收到的操作确认消息
     * @param sysId 飞机系统ID
     * @param opType 操作类型
     * @param result 操作结果
     * @param oldValue 旧值
     * @param newValue 新值
     */
    Q_INVOKABLE void handleOperationAck(int sysId, int opType, int result, int oldValue, int newValue);

    /**
     * @brief 获取操作类型的中文描述
     */
    Q_INVOKABLE QString getOperationTypeText(int opType) const;

    /**
     * @brief 获取结果的中文描述
     */
    Q_INVOKABLE QString getResultText(int result) const;

    /**
     * @brief 获取角色描述（主机/从机）
     */
    Q_INVOKABLE QString getRoleText(int value) const;

signals:
    /**
     * @brief 操作确认信号，通知QML显示弹窗
     * @param sysId 飞机系统ID
     * @param opType 操作类型
     * @param result 操作结果
     * @param oldValue 旧值
     * @param newValue 新值
     * @param message 格式化的消息文本
     */
    void operationAckReceived(int sysId, int opType, int result, int oldValue, int newValue, QString message);

    /**
     * @brief 聚合消息信号，用于组合并/拆分等批量操作
     * @param message 聚合后的消息文本
     * @param count 涉及的飞机数量
     */
    void aggregatedAckReceived(QString message, int count);

private slots:
    void processAckQueue();
    void checkAggregation();

private:
    QString formatAckMessage(const SwarmOperationAckData& data) const;

    QQueue<SwarmOperationAckData> _ackQueue;        // ACK消息队列
    QList<SwarmOperationAckData> _aggregationBuffer; // 聚合缓冲区
    QTimer* _processTimer;                           // 队列处理定时器
    QTimer* _aggregationTimer;                       // 聚合检查定时器

    static constexpr int QUEUE_PROCESS_INTERVAL = 500;   // 队列处理间隔(ms)
    static constexpr int AGGREGATION_WINDOW = 2000;      // 聚合时间窗口(ms)
    static constexpr int MAX_QUEUE_SIZE = 10;            // 最大队列长度
};

#endif // SWARM_OPERATION_HANDLER_H
