#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_math.h>

#include "plot.h"

using namespace std;

Plot::Plot(const Plot::Params &params, QWidget *parent) 
    : QwtPlot(parent), m_params(params) {
    setAutoFillBackground(true);
    setPalette(QPalette(QColor(165, 193, 228)));
    update_gradient();
    setAutoReplot(false);

    insertLegend(new QwtLegend(), QwtPlot::RightLegend);

    /* X 轴 */
    setAxisTitle(xBottom, "x -->");
    setAxisScale(xBottom, m_params.x_min, m_params.x_max, m_params.x_step);

    /* Y 轴 */
    setAxisTitle(yLeft, "y -->");
    setAxisScale(yLeft, m_params.y_min, m_params.y_max, m_params.y_step);

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth(1);
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);
    canvas->setBorderRadius(15);

    QPalette canvasPalette(Qt::white);
    canvasPalette.setColor(QPalette::Foreground, QColor(133, 190, 232));
    canvas->setPalette(canvasPalette);

    setCanvas(canvas);

    (void) new QwtPlotPanner(canvas);

    (void) new QwtPlotMagnifier(canvas);

    m_curve = new QwtPlotCurve();

    populate();
}

void Plot::update(float *data, int count) {
    QPolygonF m_points;
    for (int i = 0; i < count; i++)
        m_points << QPointF(i, data[i]);
    m_curve->setSamples(m_points);
    update_gradient();
    replot();
}

void Plot::populate() {
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
    m_curve->setPen(Qt::red);
    m_curve->attach(this);

    /* line: y = 0 */
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
    mY->setLineStyle(QwtPlotMarker::HLine);
    mY->setYValue(0.0);
    mY->attach(this);
}

void Plot::update_gradient() {
    QPalette pal = palette();

    const QColor buttonColor = pal.color(QPalette::Button);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.7, buttonColor);
    gradient.setColorAt(1.0, buttonColor);

    pal.setBrush(QPalette::Window, gradient);
    setPalette(pal);
}

void Plot::resizeEvent(QResizeEvent *event) {
    QwtPlot::resizeEvent(event);

    update_gradient();
}
