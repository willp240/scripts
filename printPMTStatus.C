#include <iostream>
#include <vector>

#include <RAT/DB.hh>
#include <RAT/DBLink.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DU/ChanHWStatus.hh>

void printPMTStatus(const int run = 300000,
                    const char* server = "postgres://snoplus@pgsql.snopl.us:5400/ratdb",
                    const bool print_online = false)
{
  RAT::DB* db = RAT::DB::Get();
  db->LoadDefaults();
  db->SetServer(server);

  RAT::DS::Run ratRun;
  ratRun.SetRunID(run);
  db->BeginOfRun(ratRun);

  db->Load(db->GetLink("DETECTOR")->GetS("geo_file"));
  db->Load(db->GetLink("DETECTOR")->GetS("pmt_info_file"));
  RAT::DU::Utility::Get()->BeginOfRun();

  RAT::DBLinkPtr dqxx = db->GetLink("PMT_DQXX");
  const std::vector<int> run_range = dqxx->GetIArray("run_range");

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  const RAT::DU::ChanHWStatus& chs = RAT::DU::Utility::Get()->GetChanHWStatus();

  std::vector<unsigned int> online_ids;
  std::vector<unsigned int> offline_ids;

  for (unsigned int lcn = 0; lcn < pmtInfo.GetCount(); ++lcn) {
    if (chs.IsTubeOnline(lcn))
      online_ids.push_back(lcn);
    else
      offline_ids.push_back(lcn);
  }

  std::cout << "Offline channel IDs (LCN):" << std::endl;
  for (size_t i = 0; i < offline_ids.size(); ++i) {
    std::cout << offline_ids[i] << std::endl;
  }

  if (print_online) {
    std::cout << "Online channel IDs (LCN):" << std::endl;
    for (size_t i = 0; i < online_ids.size(); ++i) {
      std::cout << online_ids[i] << std::endl;
    }
  }
}