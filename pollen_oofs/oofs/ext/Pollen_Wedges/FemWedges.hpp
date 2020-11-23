#ifndef FEM_MEMBRANES_HPP
#define FEM_MEMBRANES_HPP

#include <MDXProcessFem.hpp>
#include <MeshProcessSystem.hpp>

namespace mdx
{

 class FemWedges : public Process
  {
  public:
    FemWedges(const Process &proc) : Process(proc) 
    {
      setName("Model/CCF/00 Fem Wedges");
      setDesc("Fem Simulation of extensometer");

      addParm("Fem Solver", "Name of Fem solver process", "Model/CCF/01 FEM Solver");
      addParm("Fem Extend", "Name of the extensometer process", "Model/CCF/11 Fem Extend");
    }
    bool initialize(QWidget *parent);
    bool step();
    bool rewind(QWidget *parent);

  private:
    fem::FemSolver *femSolver = 0;
    fem::ExtensometerDirichlet *femExtend = 0;
  };

  	
  class FemWedgesSolver : public fem::FemSolver
  {
  public:
    FemWedgesSolver(const Process &proc) : fem::FemSolver(proc) 
    {
      setName("Model/CCF/01 FEM Solver");
      setDesc("FEM Simulation using wedge elements");

      // Update parameters with our own defaults
      setParmDefault("Stress-Strain", "Model/CCF/05 Stress-Strain");

      // Add derivatives processes
      addParm("Element Derivs","Process for element derivatives", "Model/CCF/03 Wedge Derivs");
      addParm("Pressure Derivs","Process for pressure derivatives", "Model/CCF/08 Pressure Derivs");
      addParm("Dirichlet Derivs","Process for Dirichlet derivatives", "Model/CCF/10 Dirichlet Derivs");
    }


    bool rewind(QWidget *parent)
    {
      // To rewind, we'll reload the mesh
      Mesh *mesh = currentMesh();
      if(!mesh or mesh->file().isEmpty())
        throw(QString("No current mesh, cannot rewind"));
      MeshLoad meshLoad(*this);
      meshLoad.setParm("File Name", mesh->file());
      return meshLoad.run();
    }
  };

  class FemWedgesWedgeDerivs : public fem::ElementDerivs
  {
  public:
    FemWedgesWedgeDerivs(const Process &proc) : ElementDerivs(proc) 
    {
      setName("Model/CCF/03 Wedge Derivs");

      setParmDefault("Element Type", "Linear Wedge");
      setParmDefault("Element Attribute", "Wedge Element");
    }
  };

  class FemWedgesRefCfg : public fem::SetRefCfg
  {
  public:
    FemWedgesRefCfg(const Process &proc) : SetRefCfg(proc) 
    {
      setName("Model/CCF/04 Reference Configuration");

      setParmDefault("Element Type", "Linear Wedge");
      setParmDefault("Element Attribute", "Wedge Element");
    }
  };

  class FemWedgesStressStrain : public fem::StressStrain
  {
  public:
    FemWedgesStressStrain(const Process &proc) : StressStrain(proc) 
    {
      setName("Model/CCF/05 Stress-Strain");

      setParmDefault("Element Type", "Linear Wedge");
      setParmDefault("Element Attribute", "Wedge Element");
    }
  };

  class FemWedgesSetMaterial : public fem::SetTransIsoMaterial
  {
  public:
    FemWedgesSetMaterial(const Process &proc) : SetTransIsoMaterial(proc) 
    {
      setName("Model/CCF/06 Material Properties");
    }
  };


  class FemWedgeAnisoDir : public fem::SetAnisoDir
  {
  public:
    FemWedgeAnisoDir(const Process &proc) : SetAnisoDir(proc) 
    {
      setName("Model/CCF/07 Set Aniso Dir");

      setParmDefault("Element Type", "Linear Wedge");
      setParmDefault("Element Attribute", "Wedge Element");
    }
  };

  class FemWedgesSetPressure : public fem::SetPressure
  {
  public:
    FemWedgesSetPressure(const Process &proc) : fem::SetPressure(proc) 
    {
      setName("Model/CCF/08 Set Pressure");
    }
  };

  class FemWedgesPressureDerivs : public fem::PressureDerivs
  {
  public:
    FemWedgesPressureDerivs(const Process &proc) : fem::PressureDerivs(proc) 
    {
      setName("Model/CCF/08 Pressure Derivs");
    }
  };

  class FemWedgesSetDirichlet : public fem::SetDirichlet
  {
  public:
    FemWedgesSetDirichlet(const Process &proc) : SetDirichlet(proc) 
    {
      setName("Model/CCF/09 Set Dirichlet");
    }
  };

  class FemWedgesDirichletDerivs : public fem::DirichletDerivs
  {
  public:
    FemWedgesDirichletDerivs(const Process &proc) : DirichletDerivs(proc) 
    {
      setName("Model/CCF/10 Dirichlet Derivs");
    }
  };
  
  class FemWedgesExtend : public fem::ExtensometerDirichlet
  {
  public:
    FemWedgesExtend(const Process &proc) : fem::ExtensometerDirichlet(proc) 
    {
      setName("Model/CCF/11 Fem Extend");
      setDesc("Fem Simulation of extension");

      setParmDefault("Fem Solver", "Model/CCF/01 FEM Solver");
      setParmDefault("Dirichlet Derivs", "Model/CCF/10 Dirichlet Derivs");
    }
  };
   
  class FemWedgesVectorRender : public fem::VectorRender
  {
  public:
    FemWedgesVectorRender(const Process &proc) : VectorRender(proc) 
    {
      setName("Model/CCF/15 Vector Render");
    }
  };

  class FemWedgesSelectFaces : public Process
  {
  public:
    FemWedgesSelectFaces(const Process &proc) : Process(proc) 
    {
      setName("Model/CCF/16 Select Faces");
      setDesc("Select all faces >= to a distance from the origin");

      addParm("Distance", "Distance over which faces will be selected", "49.0");
    }
    bool run();
  };

  class FemWedgesVisDirichlet : public fem::VisDirichlet
  {
  public:
    FemWedgesVisDirichlet(const Process &process) : fem::VisDirichlet(process) 
    {
      setName("Model/CCF/12 Visualize Dirichlet");
    }
  };

  class FemWedgeVisDirections : public fem::VisDirections
  {
    public:
      FemWedgeVisDirections(const Process &process) : fem::VisDirections(process) 
      {
        setName("Model/CCF/13b Visualize Element Directions");
	      
      }
  };

  class FemWedgeComputeCurrentDirections : public fem::ComputeCurrentDirections
  {
    public:
      FemWedgeComputeCurrentDirections(const Process &process) : fem::ComputeCurrentDirections(process)
      {
        setName("Model/CCF/13a Compute Current Element Directions");
      }
  };
  class FemWedgeVisTransIsoMaterial : public fem::VisTransIsoMaterial
  {
    public:
      FemWedgeVisTransIsoMaterial(const Process &process) : fem::VisTransIsoMaterial(process)
      {
        setName("Model/CCF/14 Visusalize Material");
      }
  };

  
 }
#endif

