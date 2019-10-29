#ifndef _PLOT_H_
#define _PLOT_H_

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class Plot : public QwtPlot {

public:
    typedef struct _Params {
        float x_min;
        float x_max;
        float x_step;

        float y_min;
        float y_max;
        float y_step;
    }Params;

public:
    Plot(const Plot::Params &params, QWidget *parent = NULL);

    void update(float *data, int count);

protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    void populate();
    void update_gradient();

private:
    Plot::Params m_params;
    QwtPlotCurve *m_curve;
};

#endif
