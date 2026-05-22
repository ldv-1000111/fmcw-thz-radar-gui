#include "PlotWidget.hpp"

#include <QVBoxLayout>

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
    m_ifSeries = new QLineSeries();
    m_ifSeries->setName("IF signal (real)");
    m_ifSeries->setPen(QPen(QColor("#185FA5"), 1.0));

    auto* chart = new QChart();
    chart->addSeries(m_ifSeries);
    chart->setTitle("IF Signal — Time Domain");
    chart->legend()->hide();

    auto* axisX = new QValueAxis();
    axisX->setTitleText("Time (µs)");
    axisX->setLabelFormat("%.1f");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_ifSeries->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setTitleText("Amplitude");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_ifSeries->attachAxis(axisY);

    m_ifView = new QChartView(chart, this);
    m_ifView->setRenderHint(QPainter::Antialiasing);

    m_tabs->addTab(m_ifView, "IF Signal (time)");
}

// ---------------------------------------------------------------------------
// Range spectrum chart
// ---------------------------------------------------------------------------
void PlotWidget::buildSpecChart()
{
    m_specSeries = new QLineSeries();
    m_specSeries->setName("Range spectrum");
    m_specSeries->setPen(QPen(QColor("#1D9E75"), 1.5));

    auto* chart = new QChart();
    chart->addSeries(m_specSeries);
    chart->setTitle("IF Spectrum — Range Domain");
    chart->legend()->hide();

    auto* axisX = new QValueAxis();
    axisX->setTitleText("Range (m)");
    axisX->setLabelFormat("%.0f");
    chart->addAxis(axisX, Qt::AlignBottom);
    m_specSeries->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setTitleText("Magnitude (dB)");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_specSeries->attachAxis(axisY);

    m_specView = new QChartView(chart, this);
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
    double yMin =  1e30, yMax = -1e30;
    for (int i = 0; i < N; ++i) {
        pts.append(QPointF(timeUs[i], amplitude[i]));
        if (amplitude[i] < yMin) yMin = amplitude[i];
        if (amplitude[i] > yMax) yMax = amplitude[i];
    }
    m_ifSeries->replace(pts);
    if (!timeUs.isEmpty()) {
        m_ifSeries->chart()->axes(Qt::Horizontal).first()->setRange(
            timeUs.first(), timeUs.last());
        const double margin = (yMax - yMin) * 0.1 + 1e-9;
        m_ifSeries->chart()->axes(Qt::Vertical).first()->setRange(
            yMin - margin, yMax + margin);
    }
}

void PlotWidget::updateSpectrum(const QVector<double>& rangeM,
                                 const QVector<double>& magDb)
{
    m_specSeries->clear();
    const int N = qMin(rangeM.size(), magDb.size());
    QList<QPointF> pts;
    pts.reserve(N);
    double yMin =  1e30, yMax = -1e30;
    for (int i = 0; i < N; ++i) {
        pts.append(QPointF(rangeM[i], magDb[i]));
        if (magDb[i] < yMin) yMin = magDb[i];
        if (magDb[i] > yMax) yMax = magDb[i];
    }
    m_specSeries->replace(pts);
    if (!rangeM.isEmpty()) {
        m_specSeries->chart()->axes(Qt::Horizontal).first()->setRange(
            rangeM.first(), rangeM.last());
        const double margin = (yMax - yMin) * 0.1 + 1e-9;
        m_specSeries->chart()->axes(Qt::Vertical).first()->setRange(
            yMin - margin, yMax + margin);
    }
}

}  // namespace fmcw_gui
