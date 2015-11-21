#include "plotwidget.h"
#include "models/pvmodel.h"
#include <qwtplot3d/qwt3d_function.h>
#include <cmath>

using namespace Qwt3D;

class PVModule : public Function
{
public:

    PVModule(SurfacePlot* pw) :
        Function(pw)
    {
        chain.addCell("cell 1", PVCell(6, 3, 1.75));
        chain.addCell("cell 2", PVCell(6, 3, 1.75));
        chain.addCell("cell 3", PVCell(6, 3, 1.75, 0.5));
        chain.addCell("cell 4", PVCell(6, 3, 1.75));
    }

    double operator()(double exposure, double current)
    {
        chain.setExposure(0.01 * exposure);
        return chain.calculateVoltage(current);
    }
private:
    PVChain chain;
};

PlotWidget::PlotWidget()
{
  PVModule pvmodule(this);

  pvmodule.setMesh(20,20);
  pvmodule.setDomain(0.0, 100.0, 0.0, 3.0);

  pvmodule.create();

  setRotation(30,0,15);
  setScale(0.4,10,1);
  setShift(0.15,0,0);
  setZoom(0.9);

  for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
  {
    coordinates()->axes[i].setMajors(7);
    coordinates()->axes[i].setMinors(4);
  }

  coordinates()->axes[X1].setLabelString("exposure (%)");
  coordinates()->axes[Y1].setLabelString("voltage (V)");
  coordinates()->axes[Z1].setLabelString("current (A)");

  setCoordinateStyle(BOX);
  setOrtho(false);

  updateData();
  updateGL();
}
