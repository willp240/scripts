#include <iostream>
#include <vector>

#include <RAT/DB.hh>
#include <RAT/DBLink.hh>
#include <RAT/DS/Run.hh>
#include <RAT/GeoUtils.hh>

int main( ){
  int runnum = 271388;
  RAT::DB *db = RAT::DB::Get();
  //db->LoadDefaults();
  db->SetServer("postgres://snoplus@pgsql.snopl.us:5400/ratdb");
  RAT::DS::Run run;
  run.SetRunID(runnum);
  db->BeginOfRun(run);

  double avoffset = RAT::GeoUtil::UpdateAVOffsetFromDB();

  std::cout << runnum << " " << avoffset << std::endl;
  return 0;
}

