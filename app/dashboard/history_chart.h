#ifndef HISTORYCHART_H
#define HISTORYCHART_H

#include <QWidget>
#include <QDebug>
#include <QtCharts>
#include <QTimer>

#include "Utils/format_util.h"
#include "dashboard/signal_mapper.h"
#include "utilities/utilities.h"


namespace Ui {
    class HistoryChart;
}

class HistoryChart : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryChart(const QString &title, const int &seriesCount, QCategoryAxis* categoriAxisY = nullptr, QWidget *parent = nullptr);
    ~HistoryChart();

    QVector<QSplineSeries *> getSeriesList() const;
    QCategoryAxis *getAxisY();
    void setYMax(const int &value);
    void setSeriesList(const QVector<QSplineSeries *> &seriesList);
    void setCategoryAxisYLabels();

private slots:
    void on_checkHistoryTitle_clicked(bool checked);

private:
    Ui::HistoryChart *ui;

    QString mTitle;
    int mSeriesCount;
    QChartView *mChartView;
    QChart *mChart;
    QVector<QSplineSeries *> mSeriesList;
    QCategoryAxis *mAxisY;

    void init();

};

#endif // HISTORYCHART_H
