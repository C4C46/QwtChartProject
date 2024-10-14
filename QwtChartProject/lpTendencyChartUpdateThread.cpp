#include "lpTendencyChartUpdateThread.h"
#pragma execution_character_set("utf-8")

lpTendencyChartUpdateThread::lpTendencyChartUpdateThread()
{



}



lpTendencyChartUpdateThread::~lpTendencyChartUpdateThread()
{

	m_dataCache.clear();
}

void lpTendencyChartUpdateThread::threadStart()
{
	while (!m_dataQueue.empty())
	{
		m_dataCache = m_dataQueue.front();
		m_dataQueue.pop();

		for (auto &curveName : m_dataCache.keys()) {
			for (auto &data : m_dataCache[curveName]) {
				double x = data.first;
				double y = data.second.first;
				QVariantList warningValue = data.second.second[0].toList();
				QVariantList alarmValue = data.second.second[1].toList();
				emit sgSendData(curveName, x, y, warningValue, alarmValue);

			}

		}
	}
	//qDebug() << "threadStartThreadID:" << QThread::currentThreadId();
	
	m_dataCache.clear(); // 清空缓存
	m_processFlag = false;
}





void lpTendencyChartUpdateThread::onDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache)
{
	//qDebug() << "onDataCache called with dataCache size:" << dataCache.size();

	m_dataQueue.push(dataCache);
	if (!m_processFlag)
	{
		m_processFlag = true;
		threadStart();

	}



}