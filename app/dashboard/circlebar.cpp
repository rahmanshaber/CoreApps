#include "circlebar.h"
#include "ui_circlebar.h"


CircleBar::CircleBar(const QString &title, const QStringList &colors, QWidget *parent) :QWidget(parent),ui(new Ui::CircleBar),
    mColors(colors),
    mChart(new QChart),
    mChartView(new QChartView(mChart)),
    mSeries(new QPieSeries(this))
{
    ui->setupUi(this);

    ui->lblCircleChartTitle->setText(title);

    init();
}

CircleBar::~CircleBar()
{
    delete ui;
    delete mChart;
}

void CircleBar::init()
{
    QColor transparent("transparent");

    // series settings
    mSeries->setHoleSize(0.67);
    mSeries->setPieSize(165);
    mSeries->setPieStartAngle(-115);
    mSeries->setPieEndAngle(115);
    mSeries->setLabelsVisible(false);
    mSeries->append("Used", 0);
    mSeries->append("Free", 0);
    mSeries->slices().first()->setBorderColor(transparent);
    mSeries->slices().last()->setBorderColor(transparent);
    QConicalGradient gradient;
    gradient.setAngle(115);
    for (int i = 0; i < mColors.count(); ++i) {
        gradient.setColorAt(i, QColor(mColors.at(i)));
    }
    mSeries->slices().first()->setBrush(gradient);

    // chart settings
    mChart->setBackgroundBrush(QBrush(transparent));
    mChart->setContentsMargins(-20, -20, -20, -65);
    mChart->addSeries(mSeries);
    mChart->legend()->hide();

    // chartview settings
    mChartView->setRenderHint(QPainter::Antialiasing);

    ui->layoutCircleBar->insertWidget(1, mChartView);

//    connect(SignalMapper::ins(), &SignalMapper::sigChangedAppTheme, [=] {
//        mChartView->setBackgroundBrush(QColor("#232729"));
//        mSeries->slices().last()->setColor("#1b252f");
        mChartView->setBackgroundBrush(QColor(getStylesheetValue()->value("@color01").toString()));
        mSeries->slices().last()->setColor("#1b252f");
//    });
}

void CircleBar::setValue(const int &value, const QString &valueText)
{
    mSeries->slices().first()->setValue(value);
    mSeries->slices().last()->setValue(100 - value);

    ui->lblCircleChartValue->setText(valueText);
}

