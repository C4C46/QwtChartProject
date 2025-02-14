﻿#pragma once
#include <qobject.h>
#include <QMap>
#include <QVariantList>
#include <QMutexLocker>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <queue>
#define METER_THRESHOLD 200.0 //设置米数，自动清空（趋势图和数据表格）
#define XAISX_RANGE 5 //设置X轴的范围
class lpTendencyChartUpdateThread : public QObject
{
	Q_OBJECT
public:
	lpTendencyChartUpdateThread();
	~lpTendencyChartUpdateThread();

	void threadStart();

signals:
	void sgSendData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);//发送处理完成的数据内容

public slots:
	void onDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache);//接收数据信息

private:
	QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> m_dataCache;//存储接收到的数据信息
	std::queue <QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>>> m_dataQueue;
	//mutable QMutex m_mutex;
	bool m_processFlag = false;//数据处理标志

};

