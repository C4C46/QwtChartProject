#pragma once
#include <qobject.h>
#include <QMap>
#include <QVariantList>
#include <QMutexLocker>
#include <QMutex>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <queue>
#include "lpTendencyChartWidget.h"
/*#include <LithiumCeramicMsg.h>*/

//struct LithiumChannelRegionInfo_Tag
//{
//	int msgId = -1; // 消息id
//	qint64 posMeterMm{ -1 };
//	QMap<int, QVariantMap> channelIdInfoMp; // <通道，<xxx宽度，宽度值>>
//	QMap<int, QVariantMap> channelIdAlignmentInfoMp; // <通道，<xxx对齐度，对齐度值>>
//	QVariantMap meterAllDetectTypeAndDetailsInfo;
//	QVariantMap channelAllDetectTypeExtendInfo; // <米数，<名称，宽度>>
//};
/*using namespace lpbs;*/
class lpTendencyManageThread : public QObject
{
	Q_OBJECT

public:
	lpTendencyManageThread();
	~lpTendencyManageThread();

	void stopThread();
	void threadStart();
	void LithiumRecipeProcess();

signals:
	void sgupdateScope(const QString &curveName, double x, double y);
	void sgupdateChart(const QString &curveName, double x, double y);
	void sgChartRecipe(const QStringList &newNames);
	void sgScopeRecipe(const QStringList &newNames);
	void sgCombinedValues(const QMap<QString, QVariantMap> &combinedValues);//发送对应的报警值和Y的显示范围

public slots:
/*	void onUpdateDataScope( QList<lpbs::LithiumChannelRegionInfo_Tag> tagList);*/
	void onUpdateRelativeStartPosMm(float relativeStartPosMm);
	void onLithiumRecipe(QString obj);
    void clearQueue();
private:
	mutable QMutex m_mutex;
	//lpbs::LithiumChannelRegionInfo_Tag m_currentTag;
	//QList<lpbs::LithiumChannelRegionInfo_Tag> m_tagList;
/*    std::queue<QList<lpbs::LithiumChannelRegionInfo_Tag>> m_tagQueue;*/
	QStringList m_outputNames;  // 用于存储obj拼接生成的名称
	QString m_obj;//保存UI进行修改后的信息内容
    bool m_isProcessing = false;//标记是否正在处理
	std::atomic<float> m_relativeStartPosMm = { 0 };

};

