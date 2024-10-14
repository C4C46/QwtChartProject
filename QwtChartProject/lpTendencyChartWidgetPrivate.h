#pragma once
#include "lpTendencyChartWidget.h"
#include "ui_lpTendencyChartWidget.h"
#include "lpTendencyChartConfig.h"
#include "lpTendencyChartUpdateThread.h"
#include "lpTendencyDataScope.h"
#include "lpTendencyDataChart.h"
#include "lpTendencyManageThread.h"
#include <QThread>
#include <LithiumCeramicMsg.h>
using namespace lpbs;
class lpTendencyChartWidgetPrivate : public QObject
{
	Q_OBJECT
public:
	lpTendencyChartWidgetPrivate(lpTendencyChartWidget *parent);
	~lpTendencyChartWidgetPrivate();
	void init();
	void handleIntervalPBClicked();//参数设置
	void toggleTableVisibility();//趋势勾选指标控制是否隐藏
	void AlignPBClicked();//对齐度设置
	void initialDisplayStatus(const QStringList &names);//趋势表格数据内容显示筛选

	QWidget* getTopWidget();  // 提供访问 top_widget 的接口
	QWidget* getDownWidget();// 提供访问 down_widget 的接口

signals:
	void sgUpdateDataLithium(QList<lpbs::LithiumChannelRegionInfo_Tag> tagList);
	void sgRelativeStartPosMm(float relativeStartPosMm);
	void sgUpdateData(const QString &curveName, double x, double y);
	void sgClearScope();
	void sgClearChart();
	void sgLithiumRecipe(QString obj);//UI同步趋势图和趋势表格

public slots:
	void updateDataScope(const QString &curveName, double x, double y);//接收数据更新数据表格
	void updateDataChart(const QString &curveName, double x, double y);//接收数据更新趋势图
private:
	Ui::lpTendencyChartWidget ui;
	lpTendencyChartConfig *m_ChartConfig{nullptr};
	lpTendencyDataScope *m_dataScope{nullptr};
	lpTendencyDataChart *m_dataChart{nullptr};
	lpTendencyManageThread *m_manageThread{ nullptr };
	QThread *m_thread{ nullptr };
	QStringList m_curveNames;//获取曲线名称
	QStringList m_allCurveNames;// 获取所有曲线名称





};

