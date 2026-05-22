#include "SliderBar.hpp"
#include "BandSlider.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QSignalBlocker>
#include <cmath>

static constexpr double C = 3.0e8;

namespace fmcw_gui {

// ---------------------------------------------------------------------------
// Log helpers (reused from ParameterPanel for f0)
// ---------------------------------------------------------------------------
static double logIntToPhys(int v, int vMin, int vMax, double physMin, double physMax)
{
    const double t = static_cast<double>(v - vMin) / (vMax - vMin);
    return std::pow(10.0, std::log10(physMin) + t * (std::log10(physMax) - std::log10(physMin)));
}

static int logPhysToInt(double phys, int vMin, int vMax, double physMin, double physMax)
{
    const double t = (std::log10(phys) - std::log10(physMin))
                   / (std::log10(physMax) - std::log10(physMin));
    return qBound(vMin, static_cast<int>(std::round(vMin + t * (vMax - vMin))), vMax);
}

// ---------------------------------------------------------------------------
SliderBar::SliderBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(100);
    auto* outer = new QHBoxLayout(this);
    outer->setContentsMargins(8, 4, 8, 4);
    outer->setSpacing(0);
}

void SliderBar::addSlider(const SliderSpec& spec)
{
    // Container for one slider column
    auto* col = new QWidget(this);
    col->setMinimumWidth(80);
    auto* vbox = new QVBoxLayout(col);
    vbox->setContentsMargins(4, 2, 4, 2);
    vbox->setSpacing(1);

    // Label above
    auto* topLabel = new QLabel(spec.label, col);
    topLabel->setAlignment(Qt::AlignHCenter);
    topLabel->setStyleSheet("font-size: 11px; font-weight: bold;");

    // Slider — use BandSlider for f0, plain QSlider for others
    QSlider* slider = nullptr;
    if (spec.id == "f0") {
        auto* bs = new BandSlider(Qt::Horizontal, col);
        slider = bs;
    } else {
        slider = new QSlider(Qt::Horizontal, col);
    }
    slider->setMinimum(spec.min);
    slider->setMaximum(spec.max);
    slider->setValue(spec.defaultVal);
    slider->setObjectName(spec.id);

    // Value label below
    auto* valLabel = new QLabel(col);
    valLabel->setAlignment(Qt::AlignHCenter);
    valLabel->setStyleSheet("font-size: 10px;");

    vbox->addWidget(topLabel);
    vbox->addWidget(slider);
    vbox->addWidget(valLabel);

    // Separator between sliders
    if (!m_entries.isEmpty()) {
        auto* sep = new QFrame(this);
        sep->setFrameShape(QFrame::VLine);
        sep->setFrameShadow(QFrame::Sunken);
        layout()->addWidget(sep);
    }
    layout()->addWidget(col);

    Entry e;
    e.spec     = spec;
    e.slider   = slider;
    e.valLabel = valLabel;
    m_entries.insert(spec.id, e);

    // Initialise value label
    valLabel->setText(formatValue(e, spec.defaultVal));

    // Connect slider → emit
    connect(slider, &QSlider::valueChanged, this, [this, spec](int v) {
        auto& entry = m_entries[spec.id];
        entry.valLabel->setText(formatValue(entry, v));
        emit valueChanged(spec.id, intToPhys(entry, v));
    });
}

void SliderBar::setAdasBand(int loInt, int hiInt, const QColor& colour)
{
    if (!m_entries.contains("f0")) return;
    auto* bs = qobject_cast<BandSlider*>(m_entries["f0"].slider);
    if (bs) bs->setBand(loInt, hiInt, colour, "ADAS");
    if (bs) bs->setSnapValue(m_entries["f0"].spec.defaultVal);
}

double SliderBar::value(const QString& id) const
{
    if (!m_entries.contains(id)) return 0.0;
    const auto& e = m_entries[id];
    return intToPhys(e, e.slider->value());
}

void SliderBar::setValue(const QString& id, double physicalVal)
{
    if (!m_entries.contains(id)) return;
    auto& e = m_entries[id];
    QSignalBlocker b(e.slider);
    e.slider->setValue(physToInt(e, physicalVal));
    e.valLabel->setText(formatValue(e, e.slider->value()));
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
double SliderBar::intToPhys(const Entry& e, int v) const
{
    if (e.spec.logScale) {
        return logIntToPhys(v, e.spec.min, e.spec.max,
                            e.spec.min * e.spec.scale,
                            e.spec.max * e.spec.scale);
    }
    return v * e.spec.scale;
}

int SliderBar::physToInt(const Entry& e, double phys) const
{
    if (e.spec.logScale) {
        return logPhysToInt(phys,
                            e.spec.min, e.spec.max,
                            e.spec.min * e.spec.scale,
                            e.spec.max * e.spec.scale);
    }
    return qBound(e.spec.min,
                  static_cast<int>(std::round(phys / e.spec.scale)),
                  e.spec.max);
}

QString SliderBar::formatValue(const Entry& e, int intVal) const
{
    const double phys = intToPhys(e, intVal);
    // Auto-scale units for f0
    if (e.spec.id == "f0") {
        if (phys >= 1000.0)
            return QString("%1 THz").arg(phys / 1000.0, 0, 'f', 2);
        return QString("%1 GHz").arg(phys, 0, 'f', 1);
    }
    return QString("%1 %2").arg(phys, 0, 'f', e.spec.decimals).arg(e.spec.unit);
}

}  // namespace fmcw_gui
