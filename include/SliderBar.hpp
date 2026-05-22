#pragma once

#include <QWidget>
#include <QMap>
#include <QString>
#include <functional>

class QSlider;
class QLabel;

namespace fmcw_gui {

/// Horizontal strip of labelled sliders displayed at the bottom of the
/// main window.  Each slider shows:
///   - a short label above  (e.g. "f₀")
///   - the slider itself
///   - the current value below  (e.g. "300.0 GHz")
///
/// SliderBar owns no physics — it emits valueChanged(id, value) and
/// ParameterPanel reads the values via getters.
class SliderBar : public QWidget
{
    Q_OBJECT

public:
    explicit SliderBar(QWidget* parent = nullptr);

    // --- configuration -------------------------------------------------------
    struct SliderSpec {
        QString  id;
        QString  label;
        QString  unit;
        int      min;       // integer slider units
        int      max;
        int      defaultVal;
        double   scale;     // displayed value = intVal * scale
        int      decimals;
        bool     logScale{false};  // true only for f0
    };

    void addSlider(const SliderSpec& spec);

    // --- accessors -----------------------------------------------------------
    double value(const QString& id) const;   // returns physical value
    void   setValue(const QString& id, double physicalVal);

    // --- ADAS band marker on f0 ----------------------------------------------
    void setAdasBand(int loInt, int hiInt, const QColor& colour);

signals:
    void valueChanged(const QString& id, double physicalValue);

private:
    struct Entry {
        SliderSpec   spec;
        QSlider*     slider{nullptr};
        QLabel*      valLabel{nullptr};
    };

    QMap<QString, Entry> m_entries;

    QString formatValue(const Entry& e, int intVal) const;
    int     physToInt(const Entry& e, double phys) const;
    double  intToPhys(const Entry& e, int intVal)  const;
};

}  // namespace fmcw_gui
