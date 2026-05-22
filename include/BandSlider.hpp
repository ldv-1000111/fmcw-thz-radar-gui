#pragma once

#include <QSlider>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QMouseEvent>

namespace fmcw_gui {

/// QSlider subclass that paints a coloured band overlay between two
/// marked positions, and optionally snaps to a centre value on double-click.
///
/// Used for f₀ to highlight the Teradar ADAS operating band (100–330 GHz).
class BandSlider : public QSlider
{
    Q_OBJECT

public:
    explicit BandSlider(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QSlider(orientation, parent) {}

    /// Set the band to highlight (in slider integer units).
    void setBand(int lo, int hi, const QColor& colour, const QString& label)
    {
        m_bandLo    = lo;
        m_bandHi    = hi;
        m_bandColor = colour;
        m_bandLabel = label;
        update();
    }

    /// If set, double-click snaps the slider to this value.
    void setSnapValue(int v) { m_snapValue = v; }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        // Draw the standard slider first
        QSlider::paintEvent(event);

        if (m_bandLo < 0 || m_bandHi < 0) return;

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // Compute pixel positions of band edges
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        const QRect groove = style()->subControlRect(
            QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);

        const int range   = maximum() - minimum();
        const double xLo  = groove.left() +
            (double)(m_bandLo - minimum()) / range * groove.width();
        const double xHi  = groove.left() +
            (double)(m_bandHi - minimum()) / range * groove.width();

        // Band rectangle — full groove height
        QRectF band(xLo, groove.top(), xHi - xLo, groove.height());

        // Fill with semi-transparent colour
        QColor fill = m_bandColor;
        fill.setAlpha(80);
        p.fillRect(band, fill);

        // Border
        p.setPen(QPen(m_bandColor, 1.0));
        p.drawRect(band);

        // Label above the band
        if (!m_bandLabel.isEmpty()) {
            p.setPen(m_bandColor);
            QFont f = p.font();
            f.setPointSize(7);
            f.setBold(true);
            p.setFont(f);
            p.drawText(
                QRectF(xLo, groove.top() - 14, xHi - xLo, 13),
                Qt::AlignHCenter | Qt::AlignBottom,
                m_bandLabel);
        }
    }

    void mouseDoubleClickEvent(QMouseEvent* event) override
    {
        if (m_snapValue >= 0) {
            setValue(m_snapValue);
            QToolTip::showText(event->globalPosition().toPoint(),
                               QString("Snapped to %1").arg(m_bandLabel),
                               this);
        }
        QSlider::mouseDoubleClickEvent(event);
    }

private:
    int     m_bandLo{-1};
    int     m_bandHi{-1};
    int     m_snapValue{-1};
    QColor  m_bandColor{Qt::green};
    QString m_bandLabel;
};

}  // namespace fmcw_gui
