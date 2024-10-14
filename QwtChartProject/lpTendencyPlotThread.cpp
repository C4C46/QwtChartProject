#include "lpTendencyPlotThread.h"

lpTendencyPlotThread::lpTendencyPlotThread()
{

}

lpTendencyPlotThread::~lpTendencyPlotThread()
{

}

void lpTendencyPlotThread::threadStart()
{

	QJsonDocument doc = QJsonDocument::fromJson(m_obj.toUtf8());
	QJsonObject root = doc.object();
	QJsonObject channels = root["channels"].toObject();
	QJsonObject channelsNum = root["channels_num"].toObject();


	// 通道名称映射
	QMap<QString, QString> channelNames = {
		{"9001", "陶瓷"},
		{"9004", "电浆"},
		{"9003_1", "极耳"},
		{"9003_2", "间隙"}
	};

	// 添加居中度名称
	QString centeringA = "A面居中度";
	QString centeringB = "B面居中度";
	m_outputChartNames.append(centeringA);
	m_outputChartNames.append(centeringB);

	// 遍历 channels_num 来生成输出
	for (auto it = channelsNum.begin(); it != channelsNum.end(); ++it) {
		QString channelId = it.key();
		int count = it.value().toInt();
		QString channelName = channelNames[channelId];
		QJsonArray categories = channels[channelId].toObject()["categories"].toArray();


		for (int i = 1; i <= count; i++) {
			for (const QJsonValue &categoryVal : categories) {
				QJsonObject category = categoryVal.toObject();
				QString name = category["name"].toString();
				if (name == "宽度")
				{
					QString aSideName = QString("(通道:%1)A面%2区%3").arg(i).arg(channelName).arg(name);
					QString bSideName = QString("(通道:%1)B面%2区%3").arg(i).arg(channelName).arg(name);
					m_outputChartNames.append(aSideName);
					m_outputChartNames.append(bSideName);
					continue;
				}
				else if (name == "对齐度")
				{
					QString a_bAglineName = QString("(通道:%1)A/B面%2区%3").arg(i).arg(channelName).arg(name);
					m_outputChartNames.append(a_bAglineName);
					continue;
				}
			}
		}
	}

	emit sgChildNames(m_outputChartNames);
	m_outputChartNames.clear();
	m_obj.clear();
}

void lpTendencyPlotThread::onPlotThread(QString obj)
{
	m_obj = obj;
	threadStart();
}


