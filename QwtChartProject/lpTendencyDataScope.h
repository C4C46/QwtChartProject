#pragma once

#include <QObject>
#include <QTableWidget>
#include <QList>
#include <QHeaderView>
#include <QEvent>
#include <QScrollBar>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <qDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include "lpTendencyChartUpdateThread.h"


class lpTendencyDataScope : public QObject
{
	Q_OBJECT


public:
	lpTendencyDataScope(QTableWidget* tableWidget, QObject* parent = nullptr);
	lpTendencyDataScope::~lpTendencyDataScope();
	void setColumnNames(const QStringList &names);//表格表头加载布局
	void addData(const QString & curveName, double x, double y, const QVariantList & warningValue, const QVariantList & alarmValue);//添加数据缓存
	void setColumnVisibility(const QString & curveName, bool visible);//子类名称勾选状态
	void saveColumnConfig();//保存列布局
	void loadColumnConfig();//读取列布局
	void clearTable();//清空表格数据内容
	void initializeRows(int startM, int endM);//初始化表格步长
	int clearTableWidgetItems(QTableWidget* tableWidget);
	void loadLithiumRowsConfig();//加载锂电表格配置文件

signals:
	void sgDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache);
	void sginitialDisplayStatus(const QStringList &names);

public slots:
	void onSendData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);//更新表格中的数据内容
	void clearAllWidgetInfo();

protected:
	bool eventFilter(QObject *obj, QEvent *event);//表格功能设置

private:
	QTableWidget* data_tableWidget;


	QMap<int, int> m_xDataToRowMap;	// 使用哈希表来存储xData与行号的映射，减少查找时间
	QStringList m_columnNames;//存储列名
	//QMap<QString, QVariant> m_settingsCache;  // 缓存设置
	bool m_autoScrollEnabledVertical = true;  // 默认启用垂直自动滚动
	bool m_autoScrollEnabledHorizontal = true; //默认启用水平自动滚动
	int reloadCount = 0; // 记录表格重新加载次数的成员变量
	QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> m_dataCache; // 数据缓存
	lpTendencyChartUpdateThread* m_UpdateThread;//表格数据处理工作线程
	QThread *m_thread;
	bool m_enableDataLoading = true;//先显示界面再加载数据
	QMap<int, int> columnWidths; // 存储每列的宽度
	int m_stepM; // 存储表格步长
	int m_numColumns;//存储表格列数
	double m_lastClearedMeter = 0.0; // 记录上次清空时的米数
	double m_maxMeter = 0.0;
    int m_previousHighlightedRow = 0;//记录更新光标行
};

