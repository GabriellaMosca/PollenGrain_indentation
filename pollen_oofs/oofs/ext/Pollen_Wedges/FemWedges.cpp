#include "FemWedges.hpp"

namespace mdx
{

  bool FemWedgesIndentationSequence::initialize(QWidget *parent)
  {

    mesh = currentMesh();
    if(!mesh)
      throw QString("%1::run Invalid mesh").arg(name());
    ccName = mesh->ccName();
    if(ccName.isEmpty())
      throw QString("%1::initialize Invalid cell complex").arg(name());
    CCStructure cs = mesh->ccStructure(ccName);
    CCIndexDataAttr indexAttr = mesh->indexAttr();

    if(pressureList.isEmpty())
      pressureList = parm("Fem List of Pressure Values").split(",");

    if(eIsoList.isEmpty())
      eIsoList = parm("Fem List of Young moduli Eiso").split(",");

    if(eFiberList.isEmpty())
      eFiberList = parm("Fem List of Young moduli Efiber").split(",");


    if (vertexesToIndentList.isEmpty())
    {
      if(!parm("Fem List Vertexes to Indent").isEmpty())
        vertexesToIndentList = parm("Fem List Vertexes to Indent").split(",");
      //if no index is specified, add the selected vertexes to the list of points to be indented
      else
      {
          for(CCIndex v : cs.cellsOfDimension(0))
          {
            if ((indexAttr)[v].selected)
            {
              int vtxValue = v.value;
              QString vtxId = QString::number(vtxValue);
              vertexesToIndentList.push_back(vtxId);
            }
          }
  
      }
    }

    // Get the Fem Wedges process, Pressure and Material and initialize them 

    if(parm("Fem Set Pressure").isEmpty())
       throw QString("%1::initialize Fem Wedges process empty").arg(name());
    if(!getProcess(parm("Fem Set Pressure"), setPressure))
       throw QString("%1::initialize Fem Set Pressure process can't be retrieved").arg(name());

    if(parm("Fem Set Material").isEmpty())
       throw QString("%1::initialize Fem Set Material process empty").arg(name());
    if(!getProcess(parm("Fem Set Material"), setMaterial))
      throw QString("%1::initialize Fem Set Material process can't be retrieved").arg(name());

    if(parm("Fem Wedges").isEmpty())
      throw QString("%1::initialize Fem Wedges process empty").arg(name());
    if(!getProcess(parm("Fem Wedges"), femWedges))
      throw QString("%1::initialize Unable to make Fem Wedges process: %2").arg(name()).arg(parm("Fem Wedges"));
    femWedges->initialize(parent);

    // initialize the indeter step to zero 
     mesh->attributes().attrMap<QString, int>("IndentStep")["IndentStep"] = 0;
    return true;
  }

   bool FemWedgesIndentationSequence::step()
   {
    if (!getProcess("Mesh/System/Save", saveMesh))
      throw QString("%1::step Save Mesh can't be retrieved").arg(name());
    saveMesh->run(mesh, "CurrentMesh.mdxm", false);

    if(pressureList.size() != 0 and eIsoList.size() != 0 and eFiberList.size() != 0 and vertexesToIndentList.size() != 0 ){
      for (pressureIterator; pressureIterator<pressureList.size(); ++pressureIterator){
          materialIterator = 0;
          for (materialIterator; materialIterator < eIsoList.size(); ++materialIterator)
          { 
            setPressure->setParm("Pressure", pressureList[pressureIterator].trimmed());
            if(!getProcess("Mesh/Selection/Load Selection", loadSelection))
              throw QString("%1::step Load Selection process can't be retrieved").arg(name());
            CCIndexDataAttr &indexAttr = mesh->indexAttr();
            CCStructure &cs = mesh->ccStructure(ccName);
            bool loaded = loadSelection->run(indexAttr, parm("Filename for pressure faces")); 
            setPressure->run();
            if(!getProcess("Mesh/Selection/Clear Selection", clearSelection))
              throw QString("%1::step Clear Selection process can't be retrieved").arg(name());
            bool cleared = clearSelection->run(cs, indexAttr);

            setMaterial->setParm("Young E1E3", eIsoList[materialIterator].trimmed());
            setMaterial->setParm("Young E2", eFiberList[materialIterator].trimmed());
            setMaterial->run();
            mdxInfo << "material Properties assigned" << endl;
            if(parm("Fem Wedges").isEmpty())
              throw QString("%1::step Fem Wedges process empty").arg(name());
            femWedges->initialize(nullptr);
            mdxInfo << "Fem wedges initialized" << endl;
            //now let it converge with no dirichlet for indentation and save the mesh
            // need to retrieve femIndent first
            if(parm("Fem Indent").isEmpty())
                throw QString("%1::step Indent process empty").arg(name());
              if(!getProcess(parm("Fem Indent"), femIndent))
                throw QString("%1::step Unable to make indentation process: %2").arg(name()).arg(parm("Fem Indent"));
            femIndent->setParm("Indent Steps", "0");
            
            femWedges->run();
            //after inflation, apply Dirichlet Boundary Conditions to the mesh
             
            bool loaded2 = loadSelection->run(indexAttr, parm("Filename for Dirichlet set")); 
            if(!getProcess(parm("Name of Set Dirichlet Process"), setDirichlet))
              throw QString("%1::set Load Dirichlet process can't be retrieved").arg(name());
            setDirichlet->run();
            bool cleared2 = clearSelection->run(cs, indexAttr);
 
            QString headingName = "Pollen_inflation_";
            QString pressureVal = "P="+QString(pressureList[pressureIterator].trimmed());
            QString materialVal = "_E_iso="+QString(eIsoList[materialIterator].trimmed())+"_E_fiber="+QString(eFiberList[materialIterator].trimmed());
            QString fileName;
            fileName.reserve(headingName.length() + pressureVal.length() + materialVal.length());
            fileName.append(headingName);
            fileName.append(pressureVal);
            fileName.append(materialVal+".mdxm");
            if (!getProcess("Mesh/System/Save", saveMesh))
              throw QString("%1::step Save Mesh can't be retrieved").arg(name());
            //saveMesh->run(mesh, fileName, false);

            //now apply dirichlet condition to the inflated mesh
            pointIndIterator = 0;
            for (pointIndIterator; pointIndIterator<vertexesToIndentList.size(); ++pointIndIterator)
            {
              mdxInfo << "I am at pressure " << pressureIterator << endl;
              mdxInfo << "I am at Material " << materialIterator << endl;
              mdxInfo << "I am at node " << pointIndIterator << endl;

              // Get the Fem Membranes process and re-initialize it
              if(parm("Fem Wedges").isEmpty())
                throw QString("%1::step Fem Wedges process empty").arg(name());
              femWedges->initialize(nullptr);
              //put the desired vtx from the vertexesToIndentList into 11 Fem Indent "Indent Vertex" parameter

              femIndent->setParm("Indent Vertex", vertexesToIndentList[pointIndIterator].trimmed());
              mdxInfo << "Indentation Point assigned" << endl;

              QString headingName2 = "Pollen_indentation_";
              QString vertexIndex = "_vertexIndex=";
              QString fileName2;
              fileName2.reserve(headingName2.length() + pressureVal.length() + materialVal.length() + vertexIndex.length());
              fileName2.append(headingName2);
              fileName2.append(pressureVal);
              fileName2.append(materialVal);
              fileName2.append(vertexIndex);
   
              femIndent->setParm("File Name", fileName2);
              //you do not have to initialize it, it is done in FemWedges process already
              femWedges->run();

              if(!getProcess("Mesh/System/Load", loadMesh))
              throw QString("%1::step Load Mesh process can't be retrieved").arg(name());

              loadMesh->run(*mesh, "CurrentMesh.mdxm", false, false);
              bool resetted = femIndent->setIndentStep(0);
              mdxInfo << "the indenter is at step " << mesh->attributes().attrMap<QString, int>("IndentStep")["IndentStep"] << endl;
            }
            femWedges->rewind(nullptr);
            bool resetted2 = femIndent->setIndentStep(0);
 
          }
        }
      return false;
    }
    else
      throw QString("%1::step Indentation Vertex List or Pressure list or Material list is empty , please fill all the lists, or do not use this process").arg(name());
  }



  bool FemWedges::initialize(QWidget *parent)
  {
    // Get the solver process
    if(parm("Fem Solver").isEmpty())
      throw QString("%1::initialize Solver process empty").arg(name());
    if(!getProcess(parm("Fem Solver"), femSolver))
      throw QString("%1::initialize Unable to make solver process: %2").arg(name()).arg(parm("Fem Solver"));
    femSolver->initialize(parent);

    /*if(parm("Fem Extend").isEmpty())
      throw QString("%1::initialize Extend process empty").arg(name());
    if(!getProcess(parm("Fem Extend"), femExtend))
      throw QString("%1::initialize Unable to make extensometer process: %2").arg(name()).arg(parm("Fem Extend"));
    femExtend->initialize(parent);*/

    
    // Get the indentation process
    if(parm("Fem Indent").isEmpty())
      throw QString("%1::initialize Indent process empty").arg(name());
    if(!getProcess(parm("Fem Indent"), femIndent))
      throw QString("%1::initialize Unable to make indentation process: %2").arg(name()).arg(parm("Fem Indent"));
    femIndent->initialize(parent);
   

    return true;
  }

  bool FemWedges::step()
  {
    mesh = currentMesh();
    if(!mesh)
      throw QString("%1::run Invalid mesh").arg(name());
    ccName = mesh->ccName();
    if(ccName.isEmpty())
      throw QString("%1::initialize Invalid cell complex").arg(name());


    if(!femSolver)
      throw QString("%1:step Solver process not set").arg(name());
    //if(!femExtend)
    //  throw QString("%1:step Extend process not set").arg(name());
    if(!femIndent)
      throw QString("%1:step Indent process not set").arg(name());


    // Run the fem simulation
    bool converged = !femSolver->step();
    int &indentStep = mesh->attributes().attrMap<QString, int>("IndentStep")["IndentStep"];
    if(converged){
      return femIndent->step();

    }
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

  REGISTER_PROCESS(FemWedgesIndentationSequence);
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
  //REGISTER_PROCESS(FemWedgesExtend);
  REGISTER_PROCESS(FemMembraneIndent);

  REGISTER_PROCESS(FemWedgesVectorRender);
  REGISTER_PROCESS(FemWedgeVisDirections);
  REGISTER_PROCESS(FemWedgeComputeCurrentDirections);
  REGISTER_PROCESS(FemWedgeVisTransIsoMaterial);
}
