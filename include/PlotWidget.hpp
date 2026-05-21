#pragma once

#include <QWidget>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace QtCharts {
class QChartView;
class QLineSeries;
}

namespace fmcw_gui {

/// Two-tab plot widget:
///   Tab 0 — IF signal (time domain, real part)
///   Tab 1 — Range spectrum (single-sided magnitude in dB)
class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget* parent = nullptr);

    void updateIfSignal(const QVector<double>& timeUs,
                        const QVector<double>& amplitude);

    void updateSpectrum(const QVector<double>& rangeM,
                        const QVector<double>& magDb);

private:
    QTabWidget*           m_tabs{nullptr};
    QtCharts::QChartView* m_ifView{nullptr};
    QtCharts::QChartView* m_specView{nullptr};
    QtCharts::QLineSeries* m_ifSeries{nullptr};
    QtCharts::QLineSeries* m_specSeries{nullptr};

    void buildIfChart();
    void buildSpecChart();
};

}  // namespace fmcw_gui
