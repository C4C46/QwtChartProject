#pragma once
#include <qobject.h>
#include <QMap>
#include <QVariantList>
#include <QMutexLocker>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class lpTendencyPlotThread : public QObject
{
	Q_OBJECT
public:
	lpTendencyPlotThread();
	~lpTendencyPlotThread();

	void threadStart();
	void ScopeRecipeProcess();

signals:
	void sgChildNames(QStringList &newNames);
public slots:
	void onPlotThread(QString obj);

private:
	mutable QMutex m_mutex;
	QStringList m_outputChartNames;  // 用于存储生成的名称
	QString m_obj;

};

