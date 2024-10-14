#include "lpTendencyManageThread.h"
#pragma execution_character_set("utf-8")

lpTendencyManageThread::lpTendencyManageThread()
{

}


lpTendencyManageThread::~lpTendencyManageThread()
{

}

void lpTendencyManageThread::stopThread()
{
	/*QMutexLocker locker(&m_mutex);*/
}

void lpTendencyManageThread::threadStart()
{
        if (!m_tagQueue.empty()) {
        QList<lpbs::LithiumChannelRegionInfo_Tag> currentTagList = m_tagQueue.front();
        m_tagQueue.pop(); // 取出队列前端元素并移除
        for (const auto& tag : currentTagList) {
			qint64 xOrigin = tag.posMeterMm;
			qint64 x = xOrigin - static_cast<qint64>(m_relativeStartPosMm);
			if (x >= 0)
			{
				if (!tag.channelIdInfoMp.isEmpty()) {
					for (auto it = tag.channelIdInfoMp.begin(); it != tag.channelIdInfoMp.end(); ++it) {
						int channelId = it.key();
						QVariantMap info = it.value();
						for (auto detailIt = info.begin(); detailIt != info.end(); ++detailIt) {
							QString name = detailIt.key(); // 获取名称
							double width = detailIt.value().toDouble(); // 获取宽度值
							emit sgupdateScope(name, qRound64(x / 1000.0), width); // 更新表格
							emit sgupdateChart(name, qRound64(x / 1000.0), width); // 更新趋势图
						}
					}
				}

				// 解析 channelAllDetectTypeExtendInfo，居中度
				QVariantMap extendInfo = tag.channelAllDetectTypeExtendInfo;
				if (!extendInfo.isEmpty()) {
					for (auto it = extendInfo.begin(); it != extendInfo.end(); ++it) {
						QString name = it.key(); // 获取检测类型名称
						double value = it.value().toDouble(); // 获取对应的值
						emit sgupdateScope(name, qRound64(x / 1000.0), value); // 更新表格
						emit sgupdateChart(name, qRound64(x / 1000.0), value); // 更新趋势图
					}
				}

				// 解析 meterAllDetectTypeAndDetailsInfo，对齐度
				QVariantMap extendInfoalignment = tag.meterAllDetectTypeAndDetailsInfo;
				if (!extendInfoalignment.isEmpty()) {
					for (auto italignment = extendInfoalignment.begin(); italignment != extendInfoalignment.end(); ++italignment) {
						QString name = italignment.key(); // 获取检测类型名称
						double value = italignment.value().toDouble(); // 获取对应的值
						emit sgupdateScope(name, qRound64(x / 1000.0), value); // 更新表格
						emit sgupdateChart(name, qRound64(x / 1000.0), value); // 更新趋势图
					}
				}
			}
			else
			{
				qWarning() << "static_cast<double>(tag.posMeterMm)=" << static_cast<double>(tag.posMeterMm) << tag.channelAllDetectTypeExtendInfo.size() << tag.channelIdInfoMp.size() << tag.meterAllDetectTypeAndDetailsInfo;
			}
		}

    }
	m_isProcessing = false; // 标记处理结束
}

void lpTendencyManageThread::LithiumRecipeProcess()
{
	QJsonDocument doc = QJsonDocument::fromJson(m_obj.toUtf8());
	QJsonObject root = doc.object();
	QJsonObject channels = root["channels"].toObject();
	QJsonObject channelsNum = root["channels_num"].toObject();

	QString filePath = QCoreApplication::applicationDirPath() + "/config/LithiumConfig/category_codes.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		qWarning("Cannot open file for reading");
		return;
	}
	QByteArray data = file.readAll();
	file.close();

	QJsonDocument docConfig = QJsonDocument::fromJson(data);
	QJsonObject rootConfig = docConfig.object();

	// 使用配置文件中的数据填充 QMap
	QMap<QString, QString> channelNames;
	for (auto key : rootConfig.keys()) {
		channelNames[rootConfig[key].toString()] = key;
	}

	//// 通道名称映射
	//QMap<QString, QString> channelNames = {
	//	{"9001", "陶瓷"},
	//	{"9004", "电浆"},
	//	{"9003_1", "极耳"},
	//	{"9003_2", "间隙"}
	//};



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
					m_outputNames.append(aSideName);
					m_outputNames.append(bSideName);
					continue;
				}
				else if (name == "对齐度")
				{
					QString a_bAglineName = QString("(通道:%1)A/B面%2区%3").arg(i).arg(channelName).arg(name);
					m_outputNames.append(a_bAglineName);
					continue;
				}
			}
		}
	}
    // 添加居中度名称
    QString centeringA = "A面居中度";
    QString centeringB = "B面居中度";
    m_outputNames.append(centeringA);
    m_outputNames.append(centeringB);

	QMap<QString, QVariant> warningValues;
	QMap<QString, QVariant> alarmValues;
	QMap<QString, QVariant> yAxisRanges;
	QMap<QString, QVariantMap> combinedValues;
	for (auto channelId : channels.keys()) {
		QString channelName = channelNames[channelId];
		QJsonArray categories = channels[channelId].toObject()["categories"].toArray();
		for (const QJsonValue &categoryVal : categories) {
			QJsonObject category = categoryVal.toObject();
			QString name = category["name"].toString();

			QJsonArray alarmValue = category["settings"].toObject()["alarmValue"].toArray();
			double alarmHigh = alarmValue[0].toDouble();
			double alarmLow = alarmValue[1].toDouble();
            double AlarmNum = alarmHigh + alarmLow;
			QJsonArray warningValue;
            warningValue.append(alarmHigh - 5);
            warningValue.append(alarmLow + 5);


            // 计算 yAxisRange
            QJsonArray yAxisRange;
            if (AlarmNum > 10)
            {
                yAxisRange.append(alarmHigh + 3);
                yAxisRange.append(alarmLow - 3);
            }
            else
            {
                yAxisRange.append(alarmHigh + 1);
                yAxisRange.append(alarmLow - 1);
            }


			QString key = QString("%1区%2").arg(channelName).arg(name);
			QVariantMap values;
			values["warningValue"] = QVariant(warningValue.toVariantList());
			values["alarmValue"] = QVariant(alarmValue.toVariantList());
			values["yAxisRange"] = QVariant(yAxisRange.toVariantList());

			combinedValues[key] = values;
		}
	}

	emit sgCombinedValues(combinedValues);

	emit sgChartRecipe(m_outputNames);
	emit sgScopeRecipe(m_outputNames);
	m_outputNames.clear();
	m_obj.clear();
}

void lpTendencyManageThread::onUpdateDataScope(const QList<lpbs::LithiumChannelRegionInfo_Tag> tagList)
{
	// QMutexLocker locker(&m_mutex);
    m_tagQueue.push(tagList);
    if (!m_isProcessing) {
        m_isProcessing = true;
        threadStart(); // 如果当前没有处理，则开始处理
    }

}

void lpTendencyManageThread::onUpdateRelativeStartPosMm(float relativeStartPosMm)
{
	m_relativeStartPosMm = relativeStartPosMm;
}

void lpTendencyManageThread::onLithiumRecipe(QString obj)
{
	QMutexLocker locker(&m_mutex);
	m_obj = obj;
	LithiumRecipeProcess();
}

void lpTendencyManageThread::clearQueue()
{
    QMutexLocker locker(&m_mutex); // 确保线程安全
    std::queue<QList<lpbs::LithiumChannelRegionInfo_Tag>> empty;
    std::swap(m_tagQueue, empty); // 直接清空队列
    qDebug() << "m_tagQueue:" << m_tagQueue.size();
}
