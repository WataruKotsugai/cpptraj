#ifndef INC_ACTION_RMSD_H
#define INC_ACTION_RMSD_H
#include "Action.h"
#include "Range.h"
#include "ReferenceAction.h"
#include "DataSet_1D.h"
/// Action to calculate the RMSD between frame and a reference frame.
class Action_Rmsd: public Action {
  public:
    Action_Rmsd();
    DispatchObject* Alloc() const { return (DispatchObject*)new Action_Rmsd(); }
    void Help() const;
  private:
    Action::RetType Init(ArgList&, ActionInit&, int);
    Action::RetType Setup(ActionSetup&);
    Action::RetType DoAction(int, ActionFrame&);
    void Print();

    // PerResRMSD -------------
    /// Set up per-residue RMSD calc
    int perResSetup(Topology const&, Topology const&);
    bool perres_;                      ///< If true calculate per-residue rmsd
    struct perResType {
      AtomMask tgtResMask_; ///< Target mask for residue
      AtomMask refResMask_; ///< Reference mask for residue
      DataSet_1D* data_;    ///< Hold residue RMSD for each frame
      bool isActive_;       ///< If true both masks were successfully set up.
    };
    typedef std::vector<perResType> perResArray;
    perResArray ResidueRMS_;           ///< Hold residue RMSDs
    Range TgtRange_;                   ///< Residues to calculate perRes rmsd for
    Range RefRange_;                   ///< Residues in reference corresponding to those in ResRange
    DataFile* perresout_;              ///< Per res RMSD data output file
    std::string perresmask_;           ///< Additional mask to apply to residue masks
    bool perrescenter_;                ///< Move residues to common COM before rms calc
    bool perresinvert_;                ///< If true rows will contain set info instead of cols
    DataFile* perresavg_;              ///< Hold per residue average filename
    Frame ResTgtFrame_;                ///< Hold residue target coords
    Frame ResRefFrame_;                ///< Hold residue reference coords.
    // TODO: Replace these with new DataSet type
    DataSetList* masterDSL_;
    // ------------------------
    int debug_;
    ReferenceAction REF_; ///< Hold reference frame/traj/options
    AtomMask tgtMask_; ///< Mask of selected target atoms.
    bool fit_;         ///< If true, best-fit RMS.
    bool rotate_;      ///< If true, rotate coordinates according to best-fit.
    bool useMass_;     ///< If true, mass-weight calculation.
    Vec3 tgtTrans_;    ///< Hold translation to origin.
    Matrix_3x3 rot_;   ///< Hold best-fit rotation matrix.
    Frame tgtFrame_;   ///< Hold selected target atoms.
    DataSet* rmsd_;
    DataSet* rmatrices_;
};
#endif
