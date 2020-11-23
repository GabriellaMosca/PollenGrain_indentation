#include "FemWedges.hpp"

namespace mdx
{

  bool FemWedges::initialize(QWidget *parent)
  {
    // Get the solver process
    if(parm("Fem Solver").isEmpty())
      throw QString("%1::initialize Solver process empty").arg(name());
    if(!getProcess(parm("Fem Solver"), femSolver))
      throw QString("%1::initialize Unable to make solver process: %2").arg(name()).arg(parm("Fem Solver"));
    femSolver->initialize(parent);

    if(parm("Fem Extend").isEmpty())
      throw QString("%1::initialize Extend process empty").arg(name());
    if(!getProcess(parm("Fem Extend"), femExtend))
      throw QString("%1::initialize Unable to make extensometer process: %2").arg(name()).arg(parm("Fem Extend"));
    femExtend->initialize(parent);


    return true;
  }

  bool FemWedges::step()
  {

    if(!femSolver)
      throw QString("%1:step Solver process not set").arg(name());
    if(!femExtend)
      throw QString("%1:step Extend process not set").arg(name());

    // Run the fem simulation
    bool converged = !femSolver->step();
    if(converged)
      return femExtend->step();
      //return femIndent->step();

    return true;
  }

  bool FemWedges::rewind(QWidget *parent)
  {
    // To rewind, we'll reload the mesh
    Mesh *mesh = currentMesh();
    if(!mesh or mesh->file().isEmpty())
      throw(QString("No current mesh, cannot rewind"));
    MeshLoad meshLoad(*this);
    meshLoad.setParm("File Name", mesh->file());

    return meshLoad.run();
  }

  REGISTER_PROCESS(FemWedges);
  REGISTER_PROCESS(FemWedgesSolver);
  REGISTER_PROCESS(FemWedgesRefCfg);
  REGISTER_PROCESS(FemWedgesStressStrain);
  REGISTER_PROCESS(FemWedgesWedgeDerivs);
  REGISTER_PROCESS(FemWedgesSetMaterial);
  REGISTER_PROCESS(FemWedgeAnisoDir);
  REGISTER_PROCESS(FemWedgesPressureDerivs);
  REGISTER_PROCESS(FemWedgesSetPressure);
  REGISTER_PROCESS(FemWedgesSetDirichlet);
  REGISTER_PROCESS(FemWedgesDirichletDerivs);
  REGISTER_PROCESS(FemWedgesExtend);
  REGISTER_PROCESS(FemWedgesVectorRender);
  REGISTER_PROCESS(FemWedgeVisDirections);
  REGISTER_PROCESS(FemWedgeComputeCurrentDirections);
  REGISTER_PROCESS(FemWedgeVisTransIsoMaterial);
}
