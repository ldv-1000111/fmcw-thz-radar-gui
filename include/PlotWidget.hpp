#pragma once

#include <QWidget>
#include <QVector>
#include <QTabWidget>
#include <QPen>
#include <QColor>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

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
    QChartView*           m_ifView{nullptr};
    QChartView*           m_specView{nullptr};
    QLineSeries*          m_ifSeries{nullptr};
    QLineSeries*          m_specSeries{nullptr};

    void buildIfChart();
    void buildSpecChart();
};

}  // namespace fmcw_gui
