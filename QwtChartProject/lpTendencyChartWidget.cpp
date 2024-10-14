#include "lpTendencyChartWidget.h"
#include "lpTendencyChartWidgetPrivate.h"
lpTendencyChartWidget::lpTendencyChartWidget(QWidget* parent):
	QWidget(parent), d_ptr(new lpTendencyChartWidgetPrivate(this))
{

}


lpTendencyChartWidget::~lpTendencyChartWidget()
{

}

void lpTendencyChartWidget::initLithiumChart(QWidget * pWidget)
{
	//if (!pWidget) {
	//	pWidget = new QWidget(this); // 如果没有提供widget，则创建一个新的
	//}

	QVBoxLayout *layout = new QVBoxLayout(pWidget); // 创建布局
	layout->addWidget(d_ptr->getTopWidget()); // 将 top_widget 添加到布局中
	pWidget->setLayout(layout); // 设置widget的布局
}

void lpTendencyChartWidget::initLithiumScope(QWidget * pWidget)
{

	//if (!pWidget) {
	//	pWidget = new QWidget(this); // 如果没有提供widget，则创建一个新的
	//}

	QVBoxLayout *layout = new QVBoxLayout(pWidget); // 创建布局
	layout->addWidget(d_ptr->getDownWidget()); // 将 down_widget 添加到布局中
	pWidget->setLayout(layout); // 设置widget的布局
}

void lpTendencyChartWidget::updateLithiumChart( QList<lpbs::LithiumChannelRegionInfo_Tag> tagList)
{
	emit d_ptr->sgUpdateDataLithium(tagList);

}

void lpTendencyChartWidget::clearLithiumData()
{

	d_ptr->emit sgClearScope();
	d_ptr->emit sgClearChart();
}


void lpTendencyChartWidget::clearLithiumDataofCutDoff(float relativeStartPosMm)
{
	d_ptr->emit sgRelativeStartPosMm(relativeStartPosMm);
	d_ptr->emit sgClearScope();
	d_ptr->emit sgClearChart();
}

void lpTendencyChartWidget::DataScope(const QString & curveName, double x, double y)
{
	d_ptr->emit sgUpdateData(curveName, x, y);//更新表格
}

void lpTendencyChartWidget::DataChart(const QString & curveName, double x, double y)
{

	d_ptr->emit sgUpdateData(curveName, x, y);//更新趋势图

}

void lpTendencyChartWidget::onSaveLithiumRecipe(QString fileName, QString obj)
{
	d_ptr->emit sgLithiumRecipe(obj);
}


