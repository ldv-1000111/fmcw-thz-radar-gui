#include "PlotWidget.hpp"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace fmcw_gui {

PlotWidget::PlotWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    buildIfChart();
    buildSpecChart();
}

// ---------------------------------------------------------------------------
// IF signal chart
// ---------------------------------------------------------------------------
void PlotWidget::buildIfChart()
{
    m_ifSeries = new QtCharts::QLineSeries();
    m_ifSeries->setName("IF signal (real)");
    m_ifSeries->setPen(QPen(QColor("#185FA5"), 1.0));

    auto* chart = new QtCharts::QChart();
    chart->addSeries(m_ifSeries);
    chart->setTitle("IF Signal — Time Domain");
    chart->legend()->hide();

    auto* axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("Time (µs)");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_ifSeries->attachAxis(axisX);

    auto* axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("Amplitude");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_ifSeries->attachAxis(axisY);

    m_ifView = new QtCharts::QChartView(chart, this);
    m_ifView->setRenderHint(QPainter::Antialiasing);

    m_tabs->addTab(m_ifView, "IF Signal (time)");
}

// ---------------------------------------------------------------------------
// Range spectrum chart
// ---------------------------------------------------------------------------
void PlotWidget::buildSpecChart()
{
    m_specSeries = new QtCharts::QLineSeries();
    m_specSeries->setName("Range spectrum");
    m_specSeries->setPen(QPen(QColor("#1D9E75"), 1.5));

    auto* chart = new QtCharts::QChart();
    chart->addSeries(m_specSeries);
    chart->setTitle("IF Spectrum — Range Domain");
    chart->legend()->hide();

    auto* axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("Range (m)");
    axisX->setLabelFormat("%.0f");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_specSeries->attachAxis(axisX);

    auto* axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("Magnitude (dB)");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_specSeries->attachAxis(axisY);

    m_specView = new QtCharts::QChartView(chart, this);
    m_specView->setRenderHint(QPainter::Antialiasing);

    m_tabs->addTab(m_specView, "Spectrum (range)");
}

// ---------------------------------------------------------------------------
// Data update helpers
// ---------------------------------------------------------------------------
void PlotWidget::updateIfSignal(const QVector<double>& timeUs,
                                 const QVector<double>& amplitude)
{
    m_ifSeries->clear();
    const int N = qMin(timeUs.size(), amplitude.size());
    QList<QPointF> pts;
    pts.reserve(N);
    for (int i = 0; i < N; ++i)
        pts.append(QPointF(timeUs[i], amplitude[i]));
    m_ifSeries->replace(pts);
    m_ifSeries->chart()->axes(Qt::Horizontal).first()->setRange(
        timeUs.isEmpty() ? 0 : timeUs.first(),
        timeUs.isEmpty() ? 1 : timeUs.last());
}

void PlotWidget::updateSpectrum(const QVector<double>& rangeM,
                                 const QVector<double>& magDb)
{
    m_specSeries->clear();
    const int N = qMin(rangeM.size(), magDb.size());
    QList<QPointF> pts;
    pts.reserve(N);
    for (int i = 0; i < N; ++i)
        pts.append(QPointF(rangeM[i], magDb[i]));
    m_specSeries->replace(pts);
    if (!rangeM.isEmpty())
        m_specSeries->chart()->axes(Qt::Horizontal).first()->setRange(
            rangeM.first(), rangeM.last());
}

}  // namespace fmcw_gui
