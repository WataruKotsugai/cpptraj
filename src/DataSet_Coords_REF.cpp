#include "DataSet_Coords_REF.h"
#include "CpptrajStdio.h"
#include "Trajin_Single.h"

void DataSet_Coords_REF::Info() const {
  //if (!tag_.empty())
  //  mprintf(" %s", tag_.c_str());
  if (!Meta().Fname().empty() && Meta().Name() != Meta().Fname().Full())
    mprintf(" '%s'", Meta().Fname().full());
  mprintf(" (%i atoms)", Top().Natom());
}

int DataSet_Coords_REF::LoadRefFromFile(FileName const& fname, Topology const& parmIn, int dbg)
{
  ArgList blank;
  return LoadRefFromFile(fname, "", parmIn, blank, dbg);
}

int DataSet_Coords_REF::LoadRefFromFile(FileName const& fname, std::string const& nameIn,
                                        Topology const& parmIn, ArgList& argIn, int dbg)
{
  // Set up trajectory - false = do not modify box info
  Trajin_Single traj;
  traj.SetDebug( dbg );
  if ( traj.SetupTrajRead( fname.Full(), argIn, (Topology*)&parmIn, false ) ) { // FIXME: Fix cast
    mprinterr("Error: reference: Could not set up trajectory.\n");
    return 1;
  }
  // Check number of frames to be read
  int trajFrames = traj.TotalReadFrames();
  if (trajFrames < 1) {
    mprinterr("Error: No frames could be read for reference '%s'\n", traj.TrajFilename().full());
    return 1;
  } else if (trajFrames > 1)
    mprintf("Warning: Reference has %i frames, only reading frame %i\n",
            trajFrames, traj.Start()+1);
  // Start trajectory read
  if ( traj.BeginTraj(false) ) {
    mprinterr("Error: Could not open reference '%s'\n.", traj.TrajFilename().full());
    return 1;
  }
  // Set up reference frame
  if (frame_.SetupFrameV(parmIn.Atoms(), traj.TrajCoordInfo()))
    return 1;
  // Read reference frame
  traj.ReadTrajFrame( traj.Start(), frame_ );
  traj.EndTraj();
  SetTopology( parmIn );
  if (SetMeta( MetaData(fname, nameIn, traj.Start()+1) )) return 1;
  return 0;
}

int DataSet_Coords_REF::SetRefFromCoords(DataSet_Coords* CRD, std::string const& nameIn, int fnum)
{
  if (CRD==0) return 1;
  frame_ = CRD->AllocateFrame();
  CRD->GetFrame( fnum, frame_ );
  SetTopology( CRD->Top() );
  std::string setname;
  if (nameIn.empty())
    setname = CRD->Meta().Name();
  else
    setname = nameIn;
  if (SetMeta( MetaData(setname, fnum+1) )) return 1;
  return 0;
}

int DataSet_Coords_REF::StripRef(std::string const& maskexpr) {
  if (maskexpr.empty()) return 1;
  AtomMask stripMask( maskexpr );
  if (Top().SetupIntegerMask( stripMask )) return 1;
  return StripRef( stripMask );
}

/** Currently used by 'reference' and 'atommap' */
int DataSet_Coords_REF::StripRef(AtomMask const& stripMask) {
    Frame stripFrame( frame_, stripMask );
    Topology* stripParm = Top().modifyStateByMask( stripMask );
    if (stripParm == 0) {
      mprinterr("Error: Could not create stripped reference topology.\n");
      return 1;
    }
    stripParm->Brief("Stripped ref parm:");
    frame_ = stripFrame;
    SetTopology( *stripParm );
    delete stripParm; // OK to free, parm has been copied by SetTopology.
    return 0;
}
