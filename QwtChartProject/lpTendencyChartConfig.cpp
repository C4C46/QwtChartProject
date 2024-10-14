#include "lpTendencyChartConfig.h"
#pragma execution_character_set("utf-8")

lpTendencyChartConfig::lpTendencyChartConfig(QTreeWidget *treeWidget, QObject *parent /*= nullptr*/)
	: QObject(parent), m_treeWidget(treeWidget)
{
	m_treeWidget->setColumnCount(2); // 设置为2列，一列是名称，一列是单选按钮
	m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Fixed);
	m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_treeWidget->setHeaderLabels(QStringList() << "" << "趋势勾选指标"); // 第一列留空，第二列设置为“趋势勾选指标”
	//m_treeWidget->header()->hide(); // 隐藏表头
	m_treeWidget->setRootIsDecorated(false); // 隐藏树状结构的展开/收缩箭头
	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->setExclusive(true);
}


QStringList lpTendencyChartConfig::getCurveNames() const {
	QStringList selectedCurveNames;
	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
			if (checkBox && checkBox->isChecked()) {
				selectedCurveNames.append(childItem->text(1)); // 添加选中的子项名称
			}
		}
	}

	return selectedCurveNames;
}


QMap<QString, bool> lpTendencyChartConfig::getInitialCurveDisplayStatus() const {
	QMap<QString, bool> displayStatus;
	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
			if (checkBox) {
				displayStatus[childItem->text(1)] = checkBox->isChecked();
			}
		}
	}
	return displayStatus;
}



QString lpTendencyChartConfig::getParentNameForCurve(const QString& curveName) const {
	if (m_configDoc.isNull() || !m_configDoc.isObject()) {
		return QString();
	}

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString parentName = categoryObj["name"].toString();
		QJsonArray children = categoryObj["children"].toArray();

		for (const QJsonValue& childVal : children) {
			QJsonObject childObj = childVal.toObject();
			QString childName = childObj["name"].toString();
			if (childName == curveName) {
				return parentName; // 返回匹配曲线名称的父类名称
			}
		}
	}

	return QString(); // 如果没有找到匹配项，返回空字符串
}

QStringList lpTendencyChartConfig::getParentCategoryNames() const {
	QStringList parentNames;

	if (m_configDoc.isNull() || !m_configDoc.isObject()) {
		return parentNames;
	}

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString categoryName = categoryObj["name"].toString();
		// 添加父类名称到列表中
		parentNames.append(categoryName);
	}

	return parentNames;
}

QStringList lpTendencyChartConfig::getAllCurveNames() const {
	QStringList allCurveNames;
	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			allCurveNames.append(childItem->text(1)); // 添加所有子项名称
		}
	}
	return allCurveNames;
}

QStringList lpTendencyChartConfig::getAllCurveNamesExceptParent() const {
	QStringList allCurveNames;
	if (m_configDoc.isNull() || !m_configDoc.isObject()) {
		return allCurveNames;
	}

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString parentName = categoryObj["name"].toString();
		// 排除所有包含“对齐度”字样的父类名称
		if (!parentName.contains("对齐度")) {
			QJsonArray children = categoryObj["children"].toArray();
			for (const QJsonValue& childVal : children) {
				QJsonObject childObj = childVal.toObject();
				allCurveNames.append(childObj["name"].toString());
			}
		}
	}

	return allCurveNames;
}

QVariantMap lpTendencyChartConfig::getSettingDefaultValue(const QString& settingName) {
	QVariantMap settingDefaults;


	if (m_configDoc.isNull() || !m_configDoc.isObject()) {
		return settingDefaults;
	}

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString categoryName = categoryObj["name"].toString();


		if (categoryName == settingName) {
			//qDebug() << "Object:" << categoryObj;
			// 找到了匹配的设置项，读取其默认值
			if (categoryObj.contains("settings")) {
				QJsonObject settingsObj = categoryObj["settings"].toObject();
				// 从settingsObj中读取yAxisRange
				if (settingsObj.contains("yAxisRange")) {
					QJsonArray yAxisRange = settingsObj["yAxisRange"].toArray();
					settingDefaults["yAxisRange"] = QVariantList{ yAxisRange.at(0).toDouble(), yAxisRange.at(1).toDouble() };
				}
				// 从settingsObj中读取warningValue
				if (settingsObj.contains("warningValue")) {
					QJsonArray warningValue = settingsObj["warningValue"].toArray();
					settingDefaults["warningValue"] = QVariantList{ warningValue.at(0).toDouble(), warningValue.at(1).toDouble() };
				}
				// 从settingsObj中读取alarmValue
				if (settingsObj.contains("alarmValue")) {
					QJsonArray alarmValue = settingsObj["alarmValue"].toArray();
					settingDefaults["alarmValue"] = QVariantList{ alarmValue.at(0).toDouble(), alarmValue.at(1).toDouble() };
				}
			}
			break; // 找到后退出循环
		}
	}

	return settingDefaults;
}


void lpTendencyChartConfig::loadConfig(const QString &filePath) {
	QString readPath = QCoreApplication::applicationDirPath() + "/" + filePath;
	QFile file(readPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QByteArray fileData = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(fileData);

	if (doc.isNull() || !doc.isObject())
		return;

	m_configDoc = doc;

	//// 打印整个文档以验证其内容
	//qDebug() << "Config loaded:" << configDoc.toJson(QJsonDocument::Compact);

	QJsonObject jsonObject = doc.object();
	QJsonArray categoriesArray = jsonObject["categories"].toArray();

	m_treeWidget->setColumnWidth(0, 35);
	m_treeWidget->setColumnWidth(1, 220); // 为名称列设置更大的宽度

	QFont font = m_treeWidget->font();
	font.setPointSize(font.pointSize() + 2); // 增加字体大小
	m_treeWidget->setFont(font);


	for (int i = 0; i < categoriesArray.size(); ++i) {
		QJsonObject categoryObject = categoriesArray[i].toObject();
		QString name = categoryObject["name"].toString();
		m_curveNames.append(name);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_treeWidget);
		parentItem->setText(1, name); // 将名称设置在右侧列
		parentItem->setToolTip(1, name);//设置工具提示完整内容

		QRadioButton *radioButton = new QRadioButton();
		radioButton->setStyleSheet("QRadioButton::indicator { border: 2px solid white; border-radius: 6px; }"
			"QRadioButton::indicator::unchecked { background: none; }");
		m_treeWidget->setItemWidget(parentItem, 0, radioButton); // 将单选按钮设置在左侧列
		m_buttonGroup->addButton(radioButton, i);


		// 根据配置文件设置单选按钮的选中状态
		bool selected = categoryObject["selected"].toBool();
		radioButton->setChecked(selected);

		QJsonArray childrenArray = categoryObject["children"].toArray();
		for (int j = 0; j < childrenArray.size(); ++j) {
			QJsonObject childObject = childrenArray[j].toObject();
			QString childName = childObject["name"].toString();
			bool display = childObject["display"].toBool();

			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(1, childName); // 子项名称也在右侧列
			childItem->setToolTip(1, childName);

			QColor childTextColor = QColor(211, 211, 211);
			childItem->setTextColor(1, childTextColor);
			QCheckBox *checkBox = new QCheckBox();
			checkBox->setChecked(display); // 根据配置文件设置复选框的状态
			checkBox->setEnabled(selected); // 根据父项的选中状态启用或禁用复选框
			m_treeWidget->setItemWidget(childItem, 0, checkBox); // 将复选框设置在左侧列

			// 设置子项的缩进级别和字体样式
			QFont childFont = childItem->font(1);
			childFont.setPointSize(childFont.pointSize() + 1);
			childItem->setFont(3, childFont);
			


			QObject::connect(checkBox, &QCheckBox::toggled, [this, childName](bool checked) {
				emit curveDisplayChanged(childName, checked);
			});
		}

		// 当单选按钮状态改变时，更新子复选框的可用状态
		QObject::connect(radioButton, &QRadioButton::toggled, [this, parentItem, categoryObject](bool checked) {
			if (checked) {
				// 发出Y轴范围改变的信号
				QVariantMap settingDefaults = getSettingDefaultValue(parentItem->text(1));
				if (settingDefaults.contains("yAxisRange")) {
					emit yAxisRangeChanged(settingDefaults["yAxisRange"].toList());
				}
				if (settingDefaults.contains("warningValue")) {
					emit warningValueChanged(settingDefaults["warningValue"].toList());
				}
				if (settingDefaults.contains("alarmValue")) {
					emit alarmValueChanged(settingDefaults["alarmValue"].toList());
				}
			}

			for (int j = 0; j < parentItem->childCount(); ++j) {
				QTreeWidgetItem *childItem = parentItem->child(j);
				QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
				if (checkBox) {
					checkBox->setEnabled(checked);
					if (!checked) checkBox->setChecked(false); // 如果父项未选中，也取消子项的勾选状态
				}
			}
		});
		//// 如果父项被选中，则展开该项
		//if (selected) {
		//	m_treeWidget->expandItem(parentItem);
		//}

	}

	connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [this](QAbstractButton *button) {
		int id = m_buttonGroup->id(button);
		for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
			QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
			if (i == id) {
				// 如果是当前点击的项，确保它是展开的
				m_treeWidget->expandItem(item);
				// 勾选所有子项的复选框
				for (int j = 0; j < item->childCount(); ++j) {
					QTreeWidgetItem *childItem = item->child(j);
					QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
					if (checkBox) checkBox->setChecked(true);
				}
			}
			else {
				// 其他项则收起
				m_treeWidget->collapseItem(item);
				// 取消勾选所有子项的复选框
				for (int j = 0; j < item->childCount(); ++j) {
					QTreeWidgetItem *childItem = item->child(j);
					QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
					if (checkBox) checkBox->setChecked(false);
				}
			}
		}

		//QString selectedCategory = curveNames.at(id);
		//emit curveDisplayChanged(selectedCategory, true);
	});
}



void lpTendencyChartConfig::saveConfig()
{
	QString savePath = QCoreApplication::applicationDirPath() + "/config/Event.json";
	QFile file(savePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QJsonObject rootObject;
	QJsonArray categoriesArray;

	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		QJsonObject categoryObject;
		categoryObject["name"] = parentItem->text(1); // 使用右侧列的文本作为名称

		QRadioButton *radioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(parentItem, 0));
		categoryObject["selected"] = radioButton && radioButton->isChecked(); // 检查单选按钮是否被选中

		QVariantMap settingDefaults = getSettingDefaultValue(parentItem->text(1));
		QJsonObject settingsObject; // 创建一个新的JSON对象来存储设置
		if (!settingDefaults.isEmpty()) {
			if (settingDefaults.contains("yAxisRange")) {
				settingsObject["yAxisRange"] = QJsonArray::fromVariantList(settingDefaults["yAxisRange"].toList());
			}
			if (settingDefaults.contains("warningValue")) {
				settingsObject["warningValue"] = QJsonArray::fromVariantList(settingDefaults["warningValue"].toList());
			}
			if (settingDefaults.contains("alarmValue")) {
				settingsObject["alarmValue"] = QJsonArray::fromVariantList(settingDefaults["alarmValue"].toList());
			}
		}
		categoryObject["settings"] = settingsObject; // 将设置对象添加到类别对象中

		QJsonArray childrenArray;
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			QJsonObject childObject;
			childObject["name"] = childItem->text(1); // 子项名称

			QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
			childObject["display"] = checkBox && checkBox->isChecked(); // 检查复选框是否被勾选

			childrenArray.append(childObject);
		}

		categoryObject["children"] = childrenArray;
		categoriesArray.append(categoryObject);
	}

	rootObject["categories"] = categoriesArray;

	QJsonDocument doc(rootObject);
    m_jsonString = doc.toJson(QJsonDocument::Indented);

	file.write(doc.toJson());
	file.close();

    saveLithiumChannelSplitConfig();
}

void lpTendencyChartConfig::saveLithiumChannelSplitConfig()
{
    QString savePath = QCoreApplication::applicationDirPath() + "/config/LithiumChannelSplit.json";
    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QJsonObject rootObject;
    QJsonObject channelAObject;
    QJsonObject channelBObject;
    int maxChannelNumber = 0; // 用于存储电浆区的最大通道数

    QJsonDocument eventDoc = QJsonDocument::fromJson(m_jsonString.toUtf8());
    QJsonObject eventRoot = eventDoc.object();
    QJsonArray categories = eventRoot["categories"].toArray();

    // 第一次遍历：确定最大通道数
    for (const QJsonValue &categoryValue : categories) {
        QJsonObject category = categoryValue.toObject();
        QJsonArray children = category["children"].toArray();

        for (const QJsonValue &childValue : children) {
            QJsonObject child = childValue.toObject();
            QString childName = child["name"].toString();

            if (childName.contains("电浆区宽度")) {
                QRegExp regex("\\(通道:(\\d+)\\)");
                if (regex.indexIn(childName) != -1) {
                    int channel = regex.cap(1).toInt();
                    if (channel > maxChannelNumber) {
                        maxChannelNumber = channel;
                    }
                }
            }
        }
    }

    // 第二次遍历：构建 JSON 对象
    for (const QJsonValue &categoryValue : categories) {
        QJsonObject category = categoryValue.toObject();
        QJsonArray children = category["children"].toArray();
        for (const QJsonValue &childValue : children) {
            QJsonObject child = childValue.toObject();
            QString childName = child["name"].toString();

            if (!childName.contains("宽度")) continue;

            QRegExp regex("\\(通道:(\\d+)\\)([AB])面(.*)");
            if (regex.indexIn(childName) != -1) {
                QString channel = regex.cap(1);
                QString side = regex.cap(2);
                QString type = regex.cap(3).trimmed();

                QJsonObject childObject;
                childObject["name"] = childName;

                QJsonObject *channelObject = (side == "A") ? &channelAObject : &channelBObject;

                int channelNum = channel.toInt();
                QString channelKey;
                // 根据类型决定如何计算 channelKey
                if (type.contains("陶瓷区")) {
                    channelKey = QString::number((channelNum - 1) / 2 + 1); // 陶瓷区每两个通道一组
                }
                else if (type.contains("极耳区") && channelNum == maxChannelNumber + 1) {
                    channelKey = QString::number(maxChannelNumber); // 将极耳区的最后一个通道放入电浆区的最大通道组
                }
                else {
                    channelKey = QString::number(channelNum); // 电浆区和极耳区每个通道单独一组
                }

                QJsonArray channelArray = channelObject->value(channelKey).toArray();
                // 根据类型插入到合适的位置
                if (type.contains("陶瓷区")) {
                    // 陶瓷区条目按顺序添加
                    int insertIndex = 0;
                    while (insertIndex < channelArray.size() && channelArray[insertIndex].toObject()["name"].toString().contains("陶瓷区")) {
                        ++insertIndex;
                    }
                    channelArray.insert(insertIndex, childObject);
                }
                else if (type.contains("电浆区")) {
                    // 电浆区条目插入在陶瓷区之后
                    int insertIndex = 0;
                    while (insertIndex < channelArray.size() && !channelArray[insertIndex].toObject()["name"].toString().contains("极耳区")) {
                        ++insertIndex;
                    }
                    channelArray.insert(insertIndex, childObject);
                }
                else {
                    channelArray.append(childObject); // 极耳区条目最后添加
                }
                channelObject->insert(channelKey, channelArray);
            }
        }
    }

    rootObject["channel_A"] = channelAObject;
    rootObject["channel_B"] = channelBObject;


    QJsonDocument doc(rootObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented); // 获取格式化的 JSON 字符串

    // 使用正则表达式调整 name 字段的格式
    QRegExp regex("\\{\n\\s+\"name\": \"([^\"]+)\"\n\\s+\\}");
    jsonString.replace(regex, "{\"name\": \"\\1\"}");

    file.write(jsonString.toUtf8()); // 写入调整后的 JSON 字符串
    file.close();
}

void lpTendencyChartConfig::addChannelItem(QJsonObject *channelObject, const QString &channel, const QJsonObject &item)
{
    if (!channelObject->contains(channel)) {
        channelObject->insert(channel, QJsonArray());
    }
    channelObject->operator[](channel).toArray().append(item);
}



QString lpTendencyChartConfig::getSelectedParentNames() const {

	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
		QRadioButton *radioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(item, 0));
		if (radioButton && radioButton->isChecked())
		{
			return item->text(1);//返回選擇的父類名稱
		}
	}

	return QString();

}


void lpTendencyChartConfig::updateSetting(const QString &settingName, const QString &key, const QVariantList &value) {
	if (m_configDoc.isNull() || !m_configDoc.isObject()) return;

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (int i = 0; i < categories.size(); ++i) {
		QJsonObject categoryObj = categories[i].toObject();
		if (categoryObj["name"].toString() == settingName) {
			QJsonObject settingsObj = categoryObj["settings"].toObject();
			settingsObj[key] = QJsonArray::fromVariantList(value);
			categoryObj["settings"] = settingsObj;
			categories[i] = categoryObj;
			break;
		}
	}

	rootObj["categories"] = categories;
	m_configDoc.setObject(rootObj);

}


void lpTendencyChartConfig::addNewChildToCategory(const QString& categoryName, const QString& childName, bool display, bool *shouldEmit) {
	if (m_configDoc.isNull() || !m_configDoc.isObject()) return;

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (int i = 0; i < categories.size(); ++i) {
		QJsonObject categoryObj = categories[i].toObject();
		QString jsonname = categoryObj["name"].toString();
		qDebug() << "jsonname:" << jsonname;
		if (categoryObj["name"].toString() == categoryName) {
			QJsonArray children = categoryObj["children"].toArray();
			// 检查是否已存在同名子类
			bool childExists = false;
			for (const QJsonValue& childVal : children) {
				QJsonObject childObj = childVal.toObject();
				if (childObj["name"].toString() == childName) {
					QMessageBox::warning(nullptr, "对齐度设置", "当前对齐度名称已存在！");
					childExists = true;
					break;
				}
			}
			if (!childExists)
			{
				QJsonObject newChild;
				newChild["name"] = childName;

				// 获取父类的勾选状态
				QRadioButton *radioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(m_treeWidget->topLevelItem(i), 0));
				bool parentChecked = radioButton && radioButton->isChecked();


				newChild["display"] = parentChecked && display;

				children.append(newChild);
				categoryObj["children"] = children;
				categories[i] = categoryObj;




				// 更新界面
				QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
				QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
				childItem->setText(1, childName);
				QFont childFont = childItem->font(1);
				childFont.setPointSize(childFont.pointSize() + 1); // 设置字体大小
				childItem->setFont(1, childFont); // 应用字体样式

				QCheckBox *checkBox = new QCheckBox();
				checkBox->setChecked(parentChecked && display);
				checkBox->setEnabled(parentChecked); // 根据父类的状态启用或禁用复选框
				m_treeWidget->setItemWidget(childItem, 0, checkBox);

				//// 如果需要显示，则添加曲线到图表
				//if (display) {
				//	emit curveDisplayChanged(childName, true);
				//}

				QObject::connect(checkBox, &QCheckBox::toggled, [this, childName](bool checked) {
					emit curveDisplayChanged(childName, checked);
				});

				if (display && parentChecked)
				{
					emit curveDisplayChanged(childName, true);
				}
				break;
			}

		}
	}

	rootObj["categories"] = categories;
	m_configDoc.setObject(rootObj);
}

void lpTendencyChartConfig::removeChildFromCategory(const QString& categoryName, const QString& childName) {
	if (m_configDoc.isNull() || !m_configDoc.isObject()) return;

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();
	bool found = false;

	for (int i = 0; i < categories.size(); ++i) {
		QJsonObject categoryObj = categories[i].toObject();
		if (categoryObj["name"].toString() == categoryName) {
			QJsonArray children = categoryObj["children"].toArray();
			for (int j = 0; j < children.size(); ++j) {
				QJsonObject childObj = children[j].toObject();
				if (childObj["name"].toString() == childName) {
					children.removeAt(j);
					categoryObj["children"] = children;
					categories[i] = categoryObj;
					found = true;
					break;
				}
			}
		}
		if (found) break;
	}

	if (found) {
		rootObj["categories"] = categories;
		m_configDoc.setObject(rootObj);

		// 更新界面
		QTreeWidgetItem *parentItem = nullptr;
		for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
			QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
			if (item->text(1) == categoryName) {
				parentItem = item;
				break;
			}
		}

		if (parentItem) {
			for (int j = 0; j < parentItem->childCount(); ++j) {
				QTreeWidgetItem *childItem = parentItem->child(j);
				if (childItem->text(1) == childName) {
					delete parentItem->takeChild(j); // 正确删除子项
					break;
				}

			}
		}
		// 通知趋势图组件删除曲线
		emit curveDisplayChanged(childName, false);
	}
}


void lpTendencyChartConfig::updateChildNames(const QStringList &newNames){
	// 遍历所有父类
	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		QString parentName = parentItem->text(1); // 获取父类名称

	  // 获取父类项的单选框，并检查其是否被选中
		QRadioButton *parentRadioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(parentItem, 0));
		bool isParentChecked = parentRadioButton ? parentRadioButton->isChecked() : false;


		parentItem->takeChildren(); // 清空子项列表


		// 遍历传入的新名称列表
		for (const QString &name : newNames) {
			// 检查子类名称是否包含父类名称
			if (name.contains(parentName) || (parentName.contains("极耳区居中度") && (name.contains("A面居中度") || name.contains("B面居中度")))) {
				// 创建新的子项
				QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
				childItem->setText(1, name); // 设置子项的名称
				childItem->setToolTip(1, name); // 设置工具提示为完整内容

				// 创建并设置复选框
				QCheckBox *checkBox = new QCheckBox();
				m_treeWidget->setItemWidget(childItem, 0, checkBox); // 将复选框设置在左侧列

				// 设置复选框的勾选状态和可用性
				checkBox->setChecked(isParentChecked);
				checkBox->setEnabled(isParentChecked);

				// 连接复选框的状态改变信号
				QObject::connect(checkBox, &QCheckBox::toggled, [this, name](bool checked) {
					emit curveDisplayChanged(name, checked);
				});
			}
		}
	}
	emit sgAllCurveNames(newNames);
	saveConfig();
}


void lpTendencyChartConfig::onCombinedValues(const QMap<QString, QVariantMap>& combinedValues)
{
	if (m_configDoc.isNull() || !m_configDoc.isObject()) return;

	QJsonObject rootObj = m_configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (int i = 0; i < categories.size(); ++i) {
		QJsonObject categoryObj = categories[i].toObject();
		QString categoryName = categoryObj["name"].toString();

		if (combinedValues.contains(categoryName)) {
			QVariantMap values = combinedValues[categoryName];
			QJsonObject settingsObj = categoryObj["settings"].toObject();

			if (values.contains("warningValue")) {
				settingsObj["warningValue"] = QJsonArray::fromVariantList(values["warningValue"].toList());
			}
			if (values.contains("alarmValue")) {
				settingsObj["alarmValue"] = QJsonArray::fromVariantList(values["alarmValue"].toList());
			}
			if (values.contains("yAxisRange")) {
				settingsObj["yAxisRange"] = QJsonArray::fromVariantList(values["yAxisRange"].toList());
			}

			categoryObj["settings"] = settingsObj;
			categories[i] = categoryObj;
		}
	}

	rootObj["categories"] = categories;
	m_configDoc.setObject(rootObj);

	// Optionally save the updated document to file
	saveConfig();

}