

//#include "lptendencychartwidget_global.h"

#pragma once

#include <QtCore/qglobal.h>
#include <QtWidgets>
#include <QMap>
#include <QVariantMap>
/*#include <LithiumCeramicMsg.h>*/
#ifndef BUILD_STATIC
# if defined(LPTENDENCYCHART_LIB)
#  define LPTENDENCYCHARTWIDGET_EXPORT Q_DECL_EXPORT
# else
#  define LPTENDENCYCHARTWIDGET_EXPORT Q_DECL_IMPORT
# endif
#else
# define LPTENDENCYCHARTWIDGET_EXPORT
#endif


//struct LithiumChannelRegionInfo_Tag
//{
//	int msgId = -1; // 消息id
//	qint64 posMeterMm{ -1 };
//	QMap<int, QVariantMap> channelIdInfoMp; // <通道，<xxx宽度，宽度值>>
//	QMap<int, QVariantMap> channelIdAlignmentInfoMp; // <通道，<xxx对齐度，对齐度值>>
//	QVariantMap meterAllDetectTypeAndDetailsInfo;
//	QVariantMap channelAllDetectTypeExtendInfo; // <米数，<名称，宽度>>
//};


class lpTendencyChartWidgetPrivate;


class LPTENDENCYCHARTWIDGET_EXPORT lpTendencyChartWidget : public QWidget 
{
	Q_OBJECT
public:
	lpTendencyChartWidget(QWidget *parent=nullptr);
	//lpTendencyChartWidget(QWidget *parent);
	~lpTendencyChartWidget();

	//锂电
	virtual void initLithiumChart(QWidget *pWidget = nullptr);
	virtual void initLithiumScope(QWidget *pWidget = nullptr);


	//通用
	void DataScope(const QString &curveName, double x, double y);//接收数据更新数据表格
	void DataChart(const QString &curveName, double x, double y);//接收数据更新趋势图
signals:
    void sgNewData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);
public slots:
/*	void updateLithiumChart(QList<lpbs::LithiumChannelRegionInfo_Tag> tagList);*/
	void clearLithiumData();//清空趋势图和表格数据
	void clearLithiumDataofCutDoff(float relativeStartPosMm);
	void onSaveLithiumRecipe(QString fileName, QString obj);//与配方保存内容实时联动
private:
	QSharedPointer<lpTendencyChartWidgetPrivate> d_ptr;//管理趋势图表格
};

