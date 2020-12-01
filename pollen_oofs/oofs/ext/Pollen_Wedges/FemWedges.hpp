#ifndef FEM_MEMBRANES_HPP
#define FEM_MEMBRANES_HPP

#include <MDXProcessFem.hpp>
#include <MeshProcessSystem.hpp>

namespace mdx
{

  class FemWedges;
  class FemWedgesSetMaterial;
  class FemWedgesSetPressure;
  class FemWedgesSetDirichlet;
  class FemWedgesSolver;

  class FemWedgesIndentationSequence : public Process
  {
  public: 
   FemWedgesIndentationSequence(const Process &proc) : Process(proc) 
    {
      setName("Model/CCF/00 Main Processes/1 Fem Wedges Indentation Sequence");
      setDesc("Sequence of Fem Simulation of indentation");

      addParm("Fem Wedges", "Name of Fem Wedges main process", "Model/CCF/00 Main Processes/2 Fem Wedges");
      addParm("Fem Solver", "Name of the Solver process", "Model/CCF/01 Fem Solver");
      addParm("Fem Indent", "Name of indentation process", "Model/CCF/12 Fem Indent");
      addParm("Fem Set Pressure", "Name of the process that assigns pressure", "Model/CCF/08 Set Pressure");
      addParm("Filename for pressure faces", "Name of the file containing the selected faces for pressure assignment", "FacesPressure.txt");
      addParm("Fem Set Material", "Name of the process that assigns material properties", "Model/CCF/06 Material Properties");
      addParm("Name of Set Dirichlet Process", "Name of the Process setting Dirichlet conditions", "Model/CCF/09 Set Dirichlet");
      addParm("Filename for Dirichlet set", "Name of the file containing the selected nodes for Dirichlet conditions", "DirichletSet.txt");
      addParm("Fem List Vertexes to Indent", "List of vertex indexes to indent", "13739");
      addParm("Fem List of Pressure Values", "List of pressure values to assign to the template", "0.1, 0.2, 0.3, 0.4");
      addParm("Fem List of Young moduli Eiso", "List of Young Moduli to be assigned to the isotropic plane", "50, 100, 200, 300");
      addParm("Fem List of Young moduli Efiber", "List of Young Moduli to be assigned to the isotropic fiber plane (coupled in pair with Eiso)", "50, 100, 200, 300");
    }
    bool initialize(QWidget *parent);
    bool step();

  private:
    FemWedges *femWedges = 0;
    fem::IndentDirichlet *femIndent = 0;
    FemWedgesSolver *FEMsolverProc = 0;
    QStringList vertexesToIndentList;
    QStringList pressureList;
    QStringList eIsoList;
    QStringList eFiberList;
    int pointIndIterator = 0;
    int pressureIterator = 0;
    int materialIterator = 0;
    FemWedgesSetMaterial *setMaterial = 0;
    FemWedgesSetPressure *setPressure = 0;
    MeshLoadSelection *loadSelection = 0;
    MeshClearSelection *clearSelection = 0;
    FemWedgesSetDirichlet *setDirichlet = 0;
    MeshSave *saveMesh = 0;
    MeshLoad *loadMesh = 0; 
    Mesh *mesh = 0;
    QString ccName;
  };


 class FemWedges : public Process
  {
  public:
    FemWedges(const Process &proc) : Process(proc) 
    {
      setName("Model/CCF/00 Main Processes/2 Fem Wedges");
      setDesc("Fem Simulation of Indentation");

      addParm("Fem Solver", "Name of Fem solver process", "Model/CCF/01 Fem Solver");
      //addParm("Fem Extend", "Name of the extensometer process", "Model/CCF/11 Fem Extend");
      addParm("Fem Indent", "Name of indentation process", "Model/CCF/12 Fem Indent");

    }
    bool initialize(QWidget *parent);
    bool step();
    bool rewind(QWidget *parent);

  private:
    fem::FemSolver *femSolver = 0;
    //fem::ExtensometerDirichlet *femExtend = 0;
    fem::IndentDirichlet *femIndent = 0;
    Mesh *mesh = 0;
    QString ccName;


  };

  	
  class FemWedgesSolver : public fem::FemSolver
  {
  public:
    FemWedgesSolver(const Process &proc) : fem::FemSolver(proc) 
    {
      setName("Model/CCF/01 Fem Solver");
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

      setParmDefault("Fem Solver", "Model/CCF/01 Fem Solver");
      setParmDefault("Dirichlet Derivs", "Model/CCF/10 Dirichlet Derivs");
    }
  };

  class FemMembraneIndent : public fem::IndentDirichlet
  {
  public:
    FemMembraneIndent(const Process &proc) : fem::IndentDirichlet(proc) 
    {
      setName("Model/CCF/12 Fem Indent");
      setDesc("Fem Simulation of indentation");

      setParmDefault("Fem Solver", "Model/CCF/01 Fem Solver");
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

