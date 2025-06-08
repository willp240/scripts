#include <TObject.h>
#include <TMath.h>

#include <RAT/zdab_convert.hh>
#include <RAT/PZdabFile.hh>
#include <RAT/DB.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/TrigHeader.hh>
#include <RAT/DS/ECAHeader.hh>
#include <RAT/DS/Digitiser.hh>
#include <RAT/DS/PMT.hh>
#include <RAT/DS/UniversalTime.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/MetaInformation.hh>
#include <RAT/Log.hh>
#include <stlplus3/portability/dprintf.hpp>

using RAT::warn;
using RAT::info;
using RAT::detail;
using RAT::debug;
using RAT::Log;

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

extern "C" {
#include <stdint.h>
}

namespace ratzdab {

/* RATDB instance to get at PMTINFO, since PMT type is needed to
   populate the RAT DS. */
class RATDB {
    public:
        RATDB() {
            RAT::DB* db = RAT::DB::Get();
            assert(db);

            pmtinfo = db->GetLink("PMTINFO");
            assert(pmtinfo);
            daq_rl = db->GetLink("DAQ_RUN_LEVEL");
            assert(daq_rl);
            // cache these for performance
            pmttype = pmtinfo->GetIArray("pmt_type");
            caen_channels = daq_rl->GetIArray("trigSums");
        }

        virtual ~RATDB() {}

        RAT::DBLinkPtr pmtinfo;
        RAT::DBLinkPtr daq_rl;
        std::vector<int> pmttype;
        std::vector<int> caen_channels;
};

/* initialize static maps */

std::map<int, int> get_pdg_to_snoman_map() {
    std::map<int, int> m;
    m[0] = 1;  // Photon, G4 also uses 0 for geantino and unknown
    m[22] = 2;  // Gamma
    m[11] = 20;  // Electron
    m[-11] = 21;  // Positron
    m[13] = 22;  // Mu-
    m[-13] = 23;  // Mu+
    m[15] = 24;  // Tau-
    m[-15] = 25;  // Tau+
    m[12] = 30;  // Nu-e
    m[-12] = 31;  // Nu-e-bar
    m[14] = 32;  // Nu-mu
    m[-14] = 33;  // Nu-mu-bar
    m[16] = 34;  // Nu-tau
    m[-16] = 35;  // Nu-tau-bar
    m[111] = 40;  // Pi0
    m[211] = 41;  // Pi+
    m[-211] = 42;  // Pi-
    m[311] = 50;  // K0
    m[-311] = 51;  // K0-bar
    m[321] = 52;  // K+
    m[-321] = 53;  // K-
    m[2212] = 80;  // Proton
    m[2112] = 81;  // Neutron
    return m;
}
static std::map<int, int> pdg_to_snoman = get_pdg_to_snoman_map();

std::map<int, float> get_pdg_to_mass_map() {
    std::map<int, float> m;
    m[0] = 0;  // Photon, G4 also uses 0 for geantino and unknown
    m[22] = 0;  // Gamma
    m[11] = 0.510998928;  // Electron
    m[-11] = 0.510998928;  // Positron
    m[13] = 105.6583715;  // Mu-
    m[-13] = 105.6583715;  // Mu+
    m[15] = 1776.82;  // Tau-
    m[-15] = 1776.82;  // Tau+
    m[12] = 0;  // Nu-e
    m[-12] = 0;  // Nu-e-bar
    m[14] = 0;  // Nu-mu
    m[-14] = 0;  // Nu-mu-bar
    m[16] = 0;  // Nu-tau
    m[-16] = 0;  // Nu-tau-bar
    m[111] = 134.9766;  // Pi0
    m[211] = 139.57018;  // Pi+
    m[-211] = 139.57018;  // Pi-
    m[311] = 497.648;  // K0
    m[-311] = 497.648;  // K0-bar
    m[321] = 493.667;  // K+
    m[-321] = 493.667;  // K-
    m[2212] = 938.272046;  // Proton
    m[2112] = 939.565378;  // Neutron
    return m;
}
static std::map<int, float> pdg_to_mass = get_pdg_to_mass_map();

// Prints a message about the unexpected subrecord named 'type' if we
// haven't warned about that one before. Meant for when we find an
// unexpected, yet valid, subrecord type. We don't want to abort, and we
// don't want to flood the user with messages, but we do want to remark
// upon it. Not written to be efficient, since it isn't supposed to be
// called under normal circumstances.
static void unexpected_subrecord_warn(const char * const type)
{
  static std::vector<std::string> said;
  if(std::find(said.begin(), said.end(), type) == said.end()){
    warn << "Warning: Got " << type << " subrecord, which is never "
            "expected\nin SNO+ ZDAB data files.  Corrupt input?\n";
    said.push_back(type);
  }
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                        Unpacking functions                         //
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// unpack::event
RAT::DS::Entry* unpack::event(PmtEventRecord* const pev) {
    RAT::DS::Entry* ds = new RAT::DS::Entry;
    RAT::DS::EV ev;
    static RATDB ratdb;

    const unsigned nhit = pev->NPmtHit;
    const uint32_t run_id = pev->RunNumber;
    const uint16_t subrun_id = pev->DaqStatus; // seriously
    //uint32_t evorder = pev->EvNumber;  // unused in RAT
    //uint16_t datatype = pev->DataType;  // unused here...?
    uint32_t mtc_words[6];
    memcpy(mtc_words, &pev->TriggerCardData, 6*sizeof(uint32_t));
    const uint32_t gtid = static_cast<uint32_t>(UNPK_MTC_GT_ID(mtc_words));
    const uint64_t clockcount10 =
      (static_cast<uint64_t>(UNPK_MTC_BC10_2(mtc_words)) << 32) |
      (static_cast<uint64_t>(UNPK_MTC_BC10_1(mtc_words)));
    const uint64_t clockcount50 =
      (static_cast<uint64_t>(UNPK_MTC_BC50_2(mtc_words)) << 11) |
      (static_cast<uint64_t>(UNPK_MTC_BC50_1(mtc_words)));
    const uint32_t trig_error = UNPK_MTC_ERROR(mtc_words);
    const uint32_t esumpeak = UNPK_MTC_PEAK(mtc_words);
    const uint32_t esumdiff = UNPK_MTC_DIFF(mtc_words);
    const uint32_t esumint = UNPK_MTC_INT(mtc_words);
    const bool polling = static_cast<bool>(UNPK_MTC_POLLING(mtc_words));

    // trigger word
    const uint32_t trig_type = ((mtc_words[3] & 0xff000000) >> 24) |
                               ((mtc_words[4] & 0x7ffff) << 8);

    ds->SetRunID(run_id);
    ds->SetSubRunID(subrun_id);

    ev.SetClockStat10(0);  // FIXME: what is this?
    ev.SetTrigError(trig_error);
    ev.SetESumPeak(esumpeak);
    ev.SetESumDerivative(esumdiff);
    ev.SetESumIntegral(esumint);
    ev.SetTrigType(trig_type);
    ev.SetGTID(gtid);
    ev.SetClockCount50(clockcount50);
    ev.SetClockCount10(clockcount10);
    ev.SetPollingEvent(polling);

    // set ut from 10mhz clock
    const uint64_t total = clockcount10 * 100;
    const uint64_t ns = total % (static_cast<uint64_t>(1e9));
    uint64_t s = total / 1e9;
    const uint64_t d = s / 86400;
    s -= (86400 * d);

    ev.SetUniversalTime(RAT::DS::UniversalTime(d, s, ns));

    // loop over sub fields and extract extra info
    PMTCal* calhits = static_cast<PMTCal*>(NULL);
    MonteCarloHeader* mcdata = static_cast<MonteCarloHeader*>(NULL);
    FittedEvent* fitdata = static_cast<FittedEvent*>(NULL);
    uint32_t* caen_data = static_cast<uint32_t*>(NULL);
    uint32_t* tubii_data = static_cast<uint32_t*>(NULL);
    uint32_t* extrahitinfo = static_cast<uint32_t*>(NULL);
    uint32_t* extraeventinfo = static_cast<uint32_t*>(NULL);
    unsigned nfits = 0;

    uint32_t* sub_header = &pev->CalPckType;
    while (*sub_header & SUB_NOT_LAST) {
        sub_header += (*sub_header & SUB_LENGTH_MASK);
        const uint32_t subtype_id = *sub_header >> SUB_TYPE_BITNUM;

        switch(subtype_id){
            case SUB_TYPE_CALIBRATED:
            unexpected_subrecord_warn("SUB_TYPE_CALIBRATED");
            calhits = reinterpret_cast<PMTCal*>(sub_header + 1);
            break;

            case SUB_TYPE_MONTE_CARLO:
            unexpected_subrecord_warn("SUB_TYPE_MONTE_CARLO");
            mcdata = reinterpret_cast<MonteCarloHeader*>(sub_header + 1);
            break;

            case SUB_TYPE_FIT:
            unexpected_subrecord_warn("SUB_TYPE_FIT");
            fitdata = reinterpret_cast<FittedEvent*>(sub_header + 1);
            nfits = ((*sub_header & SUB_LENGTH_MASK) *
                    sizeof(uint32_t) - sizeof(SubFieldHeader)) /
                    sizeof(FittedEvent);
            break;

            case SUB_TYPE_CAEN:
            // CAEN sub-records *are* expected in SNO+ data
            caen_data = static_cast<uint32_t*>(sub_header + 1);
            break;

            case SUB_TYPE_HIT_DATA:
            unexpected_subrecord_warn("SUB_TYPE_HIT_DATA");
            extrahitinfo = static_cast<uint32_t*>(sub_header + 1);
            break;

            case SUB_TYPE_EVENT_DATA:
            unexpected_subrecord_warn("SUB_TYPE_EVENT_DATA");
            extraeventinfo = static_cast<uint32_t*>(sub_header + 1);
            break;

            case SUB_TYPE_TUBII:
            // TUBii sub-record is also expected in SNO+ data
            tubii_data = static_cast<uint32_t*>(sub_header + 1);
            break;

            default:
            warn << stlplus::dformat("ERROR: Got subrecord with unknown type "
                                     "0x%x. Corrupt input?\n", subtype_id);
            break;
        }
    }

    // pmt hit data
    uint32_t* hits = reinterpret_cast<uint32_t*>(pev + 1);
    RAT::DS::UncalPMTs& uncalPMTs = ev.GetUncalPMTs();
    RAT::DS::CalPMTs calPMTs;
    for (unsigned i=0; i<nhit; i++, hits += 3) {
        const unsigned crate_id = UNPK_CRATE_ID(hits);
        const unsigned board_id = UNPK_BOARD_ID(hits);
        const unsigned channel_id = UNPK_CHANNEL_ID(hits);

        const int lcn = 16 * 32 * crate_id + 32 * board_id + channel_id;
        const int type = ratdb.pmttype[lcn];

        RAT::DS::PMTUncal pmtuncal = unpack::pmt(hits, lcn);
        uncalPMTs.AddPMT( pmtuncal, static_cast<RAT::DU::PMTInfo::EPMTType>( type ) );

        if (calhits)
          {
            RAT::DS::PMTCal pmtcal;
            pmtcal.SetID(pmtuncal.GetID());
            pmtcal.SetChanFlags(pmtuncal.GetChanFlags());
            pmtcal.SetCellID(pmtuncal.GetCellID());

            pmtcal.SetQHS(calhits->qhs);
            pmtcal.SetQHL(calhits->qhl);
            pmtcal.SetQLX(calhits->qlx);
            pmtcal.SetTime(calhits->tac);
            calPMTs.AddPMT( pmtcal, static_cast<RAT::DU::PMTInfo::EPMTType>( type ) );
          }
    }
    ev.SetNhits( uncalPMTs.GetCount() );
    if( calPMTs.GetAllCount() > 0 )
      ev.AddCalPMTs( calPMTs, -1 ); // FIXME choose an appropriate calibration type for this, -1 is unknown?

    // caen data
    if (caen_data) {
      RAT::DS::Digitiser dig = unpack::caen(caen_data);
      ev.SetDigitiser(dig);
    }

    // tubii data
    if (tubii_data) {
      uint32_t tubiiWord = unpack::tubii(tubii_data);
      ev.SetTubiiTrig(tubiiWord);
    }

    // extra hit info -- no place in RAT DS currently
    if (extrahitinfo) {
        warn << "ratzdab::unpack::event: Extra hit data not converted "
          "since\nthere is nowhere in RAT::DS to put it." << RAT::newline;
    }

    // extra event data -- no place in RAT DS currently
    if (extraeventinfo) {
        ExtraEventData* eed = reinterpret_cast<ExtraEventData*>(extraeventinfo);
        const float value = eed->value;
        warn << "ratzdab::unpack::event: Extra event data with value "
             << value << " not\nconverted since there is nowhere in "
             "RAT::DS to put it." << RAT::newline;
    }

    // monte carlo data
    if (mcdata) {
        MonteCarloHeader* mchdr = mcdata;
        MonteCarloVertex* mcvtx = reinterpret_cast<MonteCarloVertex*>(mchdr+1);
        for (unsigned i=0; i<mchdr->nVertices; i++, ++mcvtx) {
            RAT::DS::MCParticle p;
            const int snoman_code = mcvtx->particle;

            // convert cm -> mm
            p.SetPosition(TVector3(mcvtx->x * 10, mcvtx->y * 10, mcvtx->z * 10));

            // convert SNOMAN code to PDG
            int pdgcode = 0;
            bool found_code = false;
            std::map<int, int>::const_iterator it_code;
            for (it_code  = ratzdab::pdg_to_snoman.begin();
                 it_code != ratzdab::pdg_to_snoman.end();
                 it_code++) {
                if (it_code->second == snoman_code) {
                    pdgcode = it_code->first;
                    found_code = true;
                    break;
                }
            }
            if (!found_code) {
                warn << "ratzdab::unpack::event: No PDG code available for "
                  "SNOMAN code " << snoman_code << ", using zero." << RAT::newline;
            }
            p.SetPDGCode(pdgcode);

            // convert total energy to kinetic
            float mass = 0;
            std::map<int, float>::const_iterator it_mass =
              ratzdab::pdg_to_mass.find(pdgcode);
            if (it_mass != ratzdab::pdg_to_mass.end()) {
                mass = it_mass->second;
            }
            else {
                warn << "ratzdab::unpack::event: No mass code available for "
                  "PDG code " << pdgcode << ", using zero." << RAT::newline;
            }
            float momentum = mcvtx->energy * mcvtx->energy - mass * mass;
            float ke = momentum * momentum / (2 * mass);
            p.SetKineticEnergy(ke);

            // convert direction cosines to cartesian momentum vector
            double px = momentum * TMath::Cos(mcvtx->u);
            double py = momentum * TMath::Cos(mcvtx->v);
            double pz = momentum * TMath::Cos(mcvtx->w);
            p.SetMomentum(TVector3(px, py, pz));

            p.SetTime(mcvtx->time);
            ds->GetMC().AddMCParticle(p);
        }
    }

    // fit results
    if (fitdata) {
      if(nfits > 1000){
        warn << stlplus::dformat("SUB_TYPE_FIT subrecord claims it has %u "
                                 "fits.\nDoubtful. Skipping this subrecord.\n",
                                 nfits);
      }
      else{
        const size_t pass = RAT::MetaInformation::Get()->GetCurrentPass();
        for (unsigned i=0; i<nfits; i++, ++fitdata) {
          RAT::DS::FitVertex fv;
          fv.SetPosition(TVector3(fitdata->x, fitdata->y, fitdata->z));
          fv.SetDirection(TVector3(fitdata->u, fitdata->v, fitdata->w));
          fv.SetTime(fitdata->time);

          RAT::DS::FitResult fr;
          fr.SetFOM("quality", fitdata->quality);
          fr.SetVertex(0, fv);

          ev.SetFitResult(pass, fitdata->name, fr);
        }
      }
    }

    ds->AddEV( ev );
    return ds;
}

////////////////////////////////////////////////////////////////////////
// unpack::hrdr
RAT::DS::Run* unpack::rhdr(RunRecord* const r) {
    RAT::DS::Run* run = new RAT::DS::Run;

    run->SetDate(r->Date);
    run->SetTime(r->Time);
    run->SetRunID(r->RunNumber);
    run->SetSubRunID(r->SubRunNumber);
    run->SetRunType(r->RunMask);
    run->SetFirstGTID(r->FirstGTID); // First GTID
    run->SetValidGTID(r->ValidGTID); // First valid GTID (HW settings known)
    uint32_t version = r->Spares[0]; // version stored in format 0x04010300 = 4.1.3
    int v1 = (version >> 24);
    int v2 = (version >> 16)%16;
    int v3 = (version >> 8)%16;
    char builderversion[32];
    sprintf(builderversion, "%i.%i.%i",v1,v2,v3);
    run->SetBuilderVersion((std::string)builderversion);
    return run;

}

////////////////////////////////////////////////////////////////////////
// unpack::trig
RAT::DS::TrigHeader* unpack::trig(TriggerInfo* const t) {
    RAT::DS::TrigHeader* triginfo = new RAT::DS::TrigHeader;

    triginfo->SetTrigMask(t->TriggerMask);
    triginfo->SetPulserRate(t->PulserRate);
    triginfo->SetMTC_CSR(t->ControlRegister);
    triginfo->SetLockoutWidth(t->reg_LockoutWidth);
    triginfo->SetPrescaleFrequency(t->reg_Prescale);

    // triggers are separate members in TriggerInfo, array in TrigHeader
    triginfo->SetTriggerThreshold(0, t->n100lo);
    triginfo->SetTriggerThreshold(1, t->n100med);
    triginfo->SetTriggerThreshold(2, t->n100hi);
    triginfo->SetTriggerThreshold(3, t->n20);
    triginfo->SetTriggerThreshold(4, t->n20lb);
    triginfo->SetTriggerThreshold(5, t->esumlo);
    triginfo->SetTriggerThreshold(6, t->esumhi);
    triginfo->SetTriggerThreshold(7, t->owln);
    triginfo->SetTriggerThreshold(8, t->owlelo);
    triginfo->SetTriggerThreshold(9, t->owlehi);
    triginfo->SetTrigZeroOffset(0, t->n100lo_zero);
    triginfo->SetTrigZeroOffset(1, t->n100med_zero);
    triginfo->SetTrigZeroOffset(2, t->n100hi_zero);
    triginfo->SetTrigZeroOffset(3, t->n20_zero);
    triginfo->SetTrigZeroOffset(4, t->n20lb_zero);
    triginfo->SetTrigZeroOffset(5, t->esumlo_zero);
    triginfo->SetTrigZeroOffset(6, t->esumhi_zero);
    triginfo->SetTrigZeroOffset(7, t->owln_zero);
    triginfo->SetTrigZeroOffset(8, t->owlelo_zero);
    triginfo->SetTrigZeroOffset(9, t->owlehi_zero);

    return triginfo;
}

////////////////////////////////////////////////////////////////////////
// unpack::eped
RAT::DS::ECAHeader* unpack::eped(EpedRecord* const e) {

    if (e->Flag == 0x01000000) { //orca subrun record
        TObject* feped = new TObject();
        return reinterpret_cast<RAT::DS::ECAHeader*>(feped);
    }

    uint32_t used_pattern;
    uint32_t used_caltype;
    //If it is SNO+ data
    used_pattern = e->halfCrateID;
    used_caltype = e->CalibrationType;
    //else
    if(e->Flag > 0 && e->Flag < 5 ){ //means that it is SNO data
      //Convert SNO pattern in SNO+ pattern
      if(used_pattern & 128){ //check bit 7
        used_pattern &= ~(1 << 7); //unset bit 7
        used_pattern *=2;
        used_pattern++;
      }
      else{
        used_pattern *=2;
      }
      //Convert SNO calibration type in SNO+ type
      if(used_caltype == 3) used_caltype = 16; //pedestal half-crates calibration
      else if(used_caltype == 2) used_caltype = 26; //tslope half-crates calibration

    }

    RAT::DS::ECAHeader* eped = new RAT::DS::ECAHeader;
    eped->SetGTDelayCoarse(e->ped_delay_coarse);
    eped->SetGTDelayFine(e->ped_delay_fine);
    eped->SetChargePedestalAmplitude(e->qinj_dacsetting);
    eped->SetChargePedestalWidth(e->ped_width);
    eped->SetCalType(used_caltype);
    eped->SetPatternID(used_pattern);
    eped->SetFlag(e->Flag);

    return eped;
}

////////////////////////////////////////////////////////////////////////
// unpack::caen
RAT::DS::Digitiser unpack::caen(uint32_t* const c) {
    RAT::DS::Digitiser d;

    static RATDB ratdb;
    uint32_t channel_mask = UNPK_CAEN_CHANNEL_MASK(c);
    uint32_t magic = UNPK_CAEN_MAGIC(c);
    uint32_t pattern = UNPK_CAEN_PATTERN(c);
    uint32_t event_count = UNPK_CAEN_EVENT_COUNT(c);
    uint32_t clock = UNPK_CAEN_TRIGGER_TIME(c);
    uint32_t pack_flag = UNPK_CAEN_PACK_FLAG(c);

    d.SetBit24(pack_flag);
    d.SetDataFormat(magic);  // FIXME?
    d.SetEventID(event_count);
    d.SetTrigTime(clock);
    d.SetIOPins(pattern);

    unsigned nchannels = 0;
    for (int i=0; i<8; i++) {
        if (channel_mask & (1 << i)) {
            nchannels++;
        }
    }

    if (nchannels > 0) {
        int trace_length = (UNPK_CAEN_WORD_COUNT(c) - 4) / nchannels; // trace_length is the length of the trace in 32bit integers
        int trace_samples = trace_length * 2;  // trace_samples is the number of samples in each trace
        d.SetNWords(trace_length);

        unsigned n = 0;
        for (unsigned i=0; i<8; i++) {
            if (!(channel_mask & (1 << i))) {
                continue;
            }
            std::vector<uint16_t> trace(trace_samples);

            uint32_t* pword = c + 4 + n * trace_length ;
            for (int j=0; j<trace_samples; pword++) {
                //for even j's
                trace[j++] = *(pword) & 0xffff;
                //for odd j's
                trace[j++] = (*(pword) >> 16) & 0xffff;

            }
            n++;

            int id = ratdb.caen_channels[i];
            // If the ratdb table doesn't know what this is just index it by
            // caen channel number
            if(!id){
                id = i;
            }
            d.SetWaveform(id, trace);
        }
    }

    return d;
}

////////////////////////////////////////////////////////////////////////
// unpack::tubii
uint32_t unpack::tubii(uint32_t* const t) {
    uint32_t tubii_word = *t & 0xffffff;  // TUBii's trig word is 24 bit
    return tubii_word;
}

////////////////////////////////////////////////////////////////////////
// unpack::pmt
RAT::DS::PMTUncal unpack::pmt(uint32_t* const hits, const int lcn) {
    const int cell = UNPK_CELL_ID(hits);

    // This can be also be written as
    // (*hits >> 30 & 0x3) | ((*(hits+1)) >> 26 & 0x3c)
    // but the performance gain is unmeasurably small.
    const char flags = (UNPK_CGT_ES_16(hits)    << 0) |
                       (UNPK_CGT_ES_24(hits)    << 1) |
                       (UNPK_MISSED_COUNT(hits) << 2) |
                       (UNPK_NC_CC(hits)        << 3) |
                       (UNPK_LGI_SELECT(hits)   << 4) |
                       (UNPK_CMOS_ES_16(hits)   << 5);

    const uint32_t qhs = UNPK_QHS(hits);
    const uint32_t qhl = UNPK_QHL(hits);
    const uint32_t qlx = UNPK_QLX(hits);
    const uint32_t tac = UNPK_TAC(hits);

    RAT::DS::PMTUncal p;

    p.SetID(lcn);
    p.SetCellID(cell);
    p.SetChanFlags(flags);
    p.SetQHS(qhs);
    p.SetQHL(qhl);
    p.SetQLX(qlx);
    p.SetTime(tac);

    return p;
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                         Packing functions                          //
//                                                                    //
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// pack::event
PmtEventRecord* pack::event(RAT::DS::Entry *ds, int ev_id) {
    if (!ds || ev_id > (int)ds->GetEVCount() - 1) {
        return static_cast<PmtEventRecord*>(NULL);
    }
    RAT::DS::EV& ev = ds->GetEV(ev_id);

    int npmts = ev.GetUncalPMTs().GetAllCount();

    // calculate buffer size (length)
    // there is no GenericRecordHeader here, as they are only used in
    // dispatched data, not in files
    int length = sizeof(PmtEventRecord) + npmts * sizeof(FECReadoutData);

    int nfits = 0;
    int caen_length = 0;

    // calibrated pmt data
    if (ev.GetCalPMTs().GetAllCount() > 0) {
        assert(ev.GetCalPMTs().GetAllCount() == ev.GetUncalPMTs().GetAllCount());
        length += npmts * sizeof(PMTCal) + sizeof(SubFieldHeader);
    }

    // monte carlo data
    if (ds->MCExists()) {
        RAT::DS::MC& mc = ds->GetMC();
        std::vector<unsigned int> mcTrackIDs = mc.GetMCTrackIDs();
        int nvertices = 0;
        if (mc.GetMCTrackCount() > 0) {
            for (size_t itrack=0; itrack < mcTrackIDs.size(); itrack++) {
                RAT::DS::MCTrack& track = mc.GetMCTrack(mcTrackIDs.at(itrack));
                nvertices += track.GetMCTrackStepCount();
            }
        }
        else {
            nvertices = mc.GetMCParticleCount();
        }
        length += nvertices * sizeof(MonteCarloVertex)
                  + sizeof(MonteCarloHeader) + sizeof(SubFieldHeader);
    }

    // fits
    nfits = ev.GetFitNames().size();
    if (nfits > 0) {
        length += nfits * (sizeof(FittedEvent) + sizeof(SubFieldHeader));
    }

    // caen data
    if(ev.DigitiserExists()){
        int caen_total_sample_count = 0;
        RAT::DS::Digitiser & digitizer = ev.GetDigitiser();
        for (int i=0; i < 8; i++)
          if (digitizer.ExistsWaveform(i))
            caen_total_sample_count+=digitizer.GetWaveform(i).size();

        if(caen_total_sample_count){
          const int caen_header_length = 4 * sizeof(uint32_t);
          caen_length = caen_header_length
                      + caen_total_sample_count * sizeof(uint16_t);
          length += sizeof(SubFieldHeader) + caen_length;
        }
    }

    // allocate PmtEventRecord atop char buffer
    char* buffer = new char[length];
    PmtEventRecord* r = reinterpret_cast<PmtEventRecord*>(buffer);

    // get pointer to first MTC and FEC words
    uint32_t* mtc_word = reinterpret_cast<uint32_t*>(&(r->TriggerCardData));
    uint32_t* fec_word = reinterpret_cast<uint32_t*>(r + 1);

    r->RunNumber = ds->GetRunID();
    //r->EvNumber = ev.GetGTID();  // event order not implemented in RAT
    r->NPmtHit = npmts;

    // from xsnoed... ???
    r->PmtEventRecordInfo = PMT_EVR_RECTYPE | PMT_EVR_NOT_MC | PMT_EVR_ZDAB_VER;

    r->DataType = PMT_EVR_DATA_TYPE;
    r->DaqStatus = ds->GetSubRunID();
    r->CalPckType = PMT_EVR_PCK_TYPE | PMT_EVR_CAL_TYPE;

    PMTCal* cal_pmt = static_cast<PMTCal*>(NULL);

    uint32_t* sub_header = &(r->CalPckType);

    // must set the size of this sub-field before calling AddSubField()
    // (from the sub-field header to the end)
    *sub_header |= (static_cast<uint32_t*>(fec_word + npmts * 3) - sub_header);

    // calibrated hit information
    if (ev.GetCalPMTs().GetAllCount() > 0) {
        PZdabFile::AddSubField(&sub_header, SUB_TYPE_CALIBRATED,
                               npmts * sizeof(PMTCal));
        cal_pmt = reinterpret_cast<PMTCal*>(sub_header + 1);
    }

    // add monte carlo data
    if (ds->MCExists()) {
        RAT::DS::MC& mc = ds->GetMC();
        // if tracking is enabled, add track steps. otherwise, just add
        // the primary vertex MCParticles
        if (mc.GetMCTrackCount() > 0) {
            // build table to look up sequential vertex id by track & step index
            std::vector<unsigned int> mcTrackIDs = mc.GetMCTrackIDs();
            std::map<std::pair<int, int>, int> step_to_vertex_id;
            std::map<int, int> track_id_to_index;
            int nvertices = 0;
            for (size_t itrack=0; itrack < mcTrackIDs.size(); itrack++) {
                RAT::DS::MCTrack& track = mc.GetMCTrack(mcTrackIDs.at(itrack));
                track_id_to_index[track.GetTrackID()] = itrack;
                for (size_t s=0; s<track.GetMCTrackStepCount(); s++) {
                    step_to_vertex_id[std::make_pair(itrack, s)] = nvertices++;
                }
            }
            //assert(nvertices == step_to_vertex_id.size());

            if (nvertices > 0) {
                PZdabFile::AddSubField(&sub_header,
                  SUB_TYPE_MONTE_CARLO,
                  sizeof(MonteCarloHeader)+nvertices*sizeof(MonteCarloVertex));

                // get pointer to start of monte-carlo data and vertices
                MonteCarloHeader* mc_hdr =
                  reinterpret_cast<MonteCarloHeader*>(sub_header + 1);
                MonteCarloVertex* mc_vtx =
                  reinterpret_cast<MonteCarloVertex*>(mc_hdr + 1);

                // fill in the monte carlo data
                mc_hdr->nVertices = nvertices;

                int vertex_id = 0;
                for (size_t itrack=1; itrack <= mc.GetMCTrackCount(); itrack++) {
                    RAT::DS::MCTrack& track = mc.GetMCTrack(itrack);
                    for (size_t istep = 0; istep<track.GetMCTrackStepCount();istep++) {
                        RAT::DS::MCTrackStep& step=track.GetMCTrackStep(istep);

                        int pdgcode = track.GetPDGCode();
                        float momentum = step.GetMomentum().Mag();

                        // convert kinetic to total energy
                        double mass = 0;
                        std::map<int, float>::const_iterator it_mass =
                          ratzdab::pdg_to_mass.find(pdgcode);
                        if (it_mass != ratzdab::pdg_to_mass.end()) {
                            mass = it_mass->second;
                        }
                        else {
                            warn << "ratzdab::pack::event: No mass available "
                                 "for PDG code " << pdgcode << ", using zero."
                                 << RAT::newline;
                        }

                        // all MeV, c=1
                        mc_vtx->energy=TMath::Sqrt(mass*mass+momentum*momentum);

                        // convert mm -> cm
                        mc_vtx->x = step.GetPosition()[0] / 10;
                        mc_vtx->y = step.GetPosition()[1] / 10;
                        mc_vtx->z = step.GetPosition()[2] / 10;

                        // u, v, w are direction cosines
                        mc_vtx->u = step.GetMomentum()[0] / momentum;
                        mc_vtx->v = step.GetMomentum()[1] / momentum;
                        mc_vtx->w = step.GetMomentum()[2] / momentum;

                        // convert from PDG to SNOMAN code
                        int code = 0;  // 0 indicates unknown particle
                        std::map<int, int>::const_iterator it_code =
                          ratzdab::pdg_to_snoman.find(pdgcode);
                        if (it_code != ratzdab::pdg_to_snoman.end()) {
                            code = it_code->second;
                        }
                        mc_vtx->particle = code;

                        // interaction code; FIXME: 100 = start
                        mc_vtx->int_code = 100;

                        // parent is previous step, or if this is the first
                        // step in the track, look up parent track and
                        // fuzzy match to determine which step
                        int parent = vertex_id - 1;
                        if (istep == 0) {
                            int parent_track_id = track.GetParentID();
                            std::map<int, int>::iterator it_ptid =
                              track_id_to_index.find(parent_track_id);
                            if (it_ptid == track_id_to_index.end()) {
                                parent = -1;
                            }
                            else {
                                RAT::DS::MCTrack& parent_track =
                                  mc.GetMCTrack(it_ptid->second);

                                assert(parent_track.GetMCTrackStepCount() > 0);
                                double closest_distance =
                                  (parent_track.GetMCTrackStep(0).GetPosition()
                                  - step.GetPosition()).Mag();
                                int closest_step_id = 0;
                                for (size_t jstep = 0;
                                     jstep<parent_track.GetMCTrackStepCount();
                                     jstep++) {
                                    RAT::DS::MCTrackStep& parent_step =
                                      parent_track.GetMCTrackStep(jstep);
                                    double dist = (parent_step.GetPosition() -
                                                   step.GetPosition()).Mag();
                                    if (dist < closest_distance) {
                                        closest_distance = dist;
                                        closest_step_id = jstep;
                                    }
                                }
                                parent = step_to_vertex_id[std::make_pair(
                                         it_ptid->second, closest_step_id)];
                            }
                        }

                        mc_vtx->parent = parent;

                        mc_vtx->time = step.GetGlobalTime();
                        mc_vtx->flags = 0;

                        vertex_id++;
                        mc_vtx++;
                    }
                }
            }
        }
        else {
            int nvertices = mc.GetMCParticleCount();
            if (nvertices > 0) {
                PZdabFile::AddSubField(&sub_header,
                  SUB_TYPE_MONTE_CARLO,
                  sizeof(MonteCarloHeader)+nvertices*sizeof(MonteCarloVertex));

                // get pointer to start of monte-carlo data and vertices
                MonteCarloHeader* mc_hdr =
                  reinterpret_cast<MonteCarloHeader*>(sub_header + 1);
                MonteCarloVertex* mc_vtx =
                  reinterpret_cast<MonteCarloVertex*>(mc_hdr + 1);

                // fill in the monte carlo data values
                mc_hdr->nVertices = nvertices;
                for (int i=0; i<nvertices; i++, mc_vtx++) {
                    RAT::DS::MCParticle& p = mc.GetMCParticle(i);
                    int pdgcode = p.GetPDGCode();
                    float momentum = p.GetMomentum().Mag();

                    // convert kinetic to total energy
                    double mass = 0;
                    std::map<int, float>::const_iterator it_mass =
                      ratzdab::pdg_to_mass.find(pdgcode);
                    if (it_mass != ratzdab::pdg_to_mass.end()) {
                        mass = it_mass->second;
                    }
                    else {
                        warn << "ratzdab::pack::event: No mass available for "
                          "PDG code " << pdgcode << ", using zero." << RAT::newline;
                    }

                    // all MeV, c=1
                    mc_vtx->energy = TMath::Sqrt(mass*mass + momentum*momentum);

                    // convert mm -> cm
                    mc_vtx->x = p.GetPosition()[0] / 10;
                    mc_vtx->y = p.GetPosition()[1] / 10;
                    mc_vtx->z = p.GetPosition()[2] / 10;

                    // u, v, w are direction cosines
                    mc_vtx->u = p.GetMomentum()[0] / momentum;
                    mc_vtx->v = p.GetMomentum()[1] / momentum;
                    mc_vtx->w = p.GetMomentum()[2] / momentum;

                    // convert from PDG to SNOMAN code
                    int code = 0;  // 0 indicates unknown particle
                    std::map<int, int>::const_iterator it_code =
                      ratzdab::pdg_to_snoman.find(pdgcode);
                    if (it_code != ratzdab::pdg_to_snoman.end()) {
                        code = it_code->second;
                    }
                    mc_vtx->particle = code;

                    // interaction code; FIXME: 100 = start
                    mc_vtx->int_code = 100;

                    mc_vtx->parent = -1;  // p->GetIndex() in QSNO
                    mc_vtx->time = p.GetTime();
                    mc_vtx->flags = 0;
                }
            }
        }
    }

    // add all available fits
    const std::vector<std::string>& fitNames = ev.GetFitNames();
    for( size_t iFit = 0; iFit < fitNames.size(); iFit++ ) {
        PZdabFile::AddSubField(&sub_header, SUB_TYPE_FIT, sizeof(FittedEvent));

        // get pointer to start of fit data
        FittedEvent* fit = reinterpret_cast<FittedEvent*>(sub_header + 1);
        const RAT::DS::FitResult& rfit = ev.GetFitResult( fitNames[iFit] );

        if (rfit.GetVertexCount() == 0) {
            continue;
        }

        // FIXME can zdab handle >1 vertices?
        RAT::DS::FitVertex rv = rfit.GetVertex(0);

        if (rv.ContainsPosition()) {
            fit->x = rv.GetPosition()[0]/10;
            fit->y = rv.GetPosition()[1]/10;
            fit->z = rv.GetPosition()[2]/10;
        }
        else {
            fit->x = -9999;
            fit->y = -9999;
            fit->z = -9999;
        }

        if (rv.ContainsDirection()) {
            fit->u = rv.GetDirection()[0];
            fit->v = rv.GetDirection()[1];
            fit->w = rv.GetDirection()[2];
        }
        else {
            fit->u = 0;
            fit->v = 0;
            fit->w = 0;
        }

        if (rv.ContainsTime()) {
            fit->time = rv.GetTime();
        }
        else {
            fit-> time = -9999;
        }

        try{
          fit->quality = rfit.GetFOM(fitNames[iFit]);
        }
        catch(std::out_of_range &o){
          fit->quality = 0;
        }

        fit->npmts = npmts;
        fit->spare = 0;
        char buff[256];
        snprintf(buff, 256, "%s", fitNames[iFit].c_str());
        memset(fit->name, 0, 32);
        strncpy(fit->name, buff, 31);  // copy fit name (31 chars max)
    }

    // caen digitizer waveforms
    if (caen_length) {
        PZdabFile::AddSubField(&sub_header, SUB_TYPE_CAEN, caen_length);
        uint32_t* caen = sub_header + 1;

        RAT::DS::Digitiser& d = ev.GetDigitiser();
        uint16_t magic = d.GetDataFormat();  // FIXME ???

        uint16_t channel_mask = 0;
        for (int i=0; i<8; i++)
            if(d.ExistsWaveform(i))
              channel_mask |= (1 << i);

        uint16_t pattern = d.GetIOPins();
        uint16_t pack_flag = d.GetBit24();
        uint32_t word_count = d.GetNWords();
        uint32_t board_id = d.GetBoardID();
        uint32_t event_id = d.GetEventID();
        uint32_t trigger_time = d.GetTrigTime();

        // pack header
        *(caen++) = ((magic & 0xf) << 28) | word_count;
        *(caen++) = (board_id << 25) | ((pack_flag & 1) << 24) |
                    ((pattern & 0xffff) << 8) | (channel_mask & 0xff);
        *(caen++) = event_id & 0xffffff;
        *(caen++) = trigger_time;

        // copy samples
        uint16_t* psample = reinterpret_cast<uint16_t*>(caen);
        for (int i = 0; i < 8; i++) {
            if (d.ExistsWaveform(i)) {
                const std::vector<uint16_t> s = d.GetWaveform(i);
                for (unsigned int j = 0; j < s.size(); j++)
                    *(psample++) = s[j];
            }
        }
    }

    // trigger
    uint32_t gtid = ev.GetGTID();
    uint32_t trigger = ev.GetTrigType();
    uint32_t error = ev.GetTrigError();

    uint32_t peak = ev.GetESumPeak() & 0x3ff;
    uint32_t inte = ev.GetESumIntegral() & 0x3ff;
    uint32_t diff = ev.GetESumDerivative() & 0x3ff;

    *(mtc_word++) = static_cast<uint32_t>(ev.GetClockCount10());
    *(mtc_word++) = ((ev.GetClockCount10() >> 32) & 0x7fffff) |
                    ((ev.GetClockCount50() & 0x7ff) << 21);
    *(mtc_word++) = static_cast<uint32_t>(ev.GetClockCount50() >> 11);
    *(mtc_word++) = ((trigger & 0x000000ff) << 24) | (gtid & 0xffffff);
    *(mtc_word++) = ((trigger & 0x07ffff00) >> 8) | (peak << 19) | (diff << 29);
    *(mtc_word++) = ((error & 0x3fff) << 17) | (inte << 7) | (diff >> 3);

    // pmts
    for (int i=0; i<npmts; i++) {
        const RAT::DS::PMTUncal& pmt = ev.GetUncalPMTs().GetAllPMT(i);

        uint32_t crate_id = static_cast<uint32_t>((pmt.GetID() >> 9) & 0x1f);
        uint32_t card_id = static_cast<uint32_t>((pmt.GetID() >> 5) & 0xf);
        uint32_t channel_id = static_cast<uint32_t>(pmt.GetID() & 0x1f);
        uint32_t cell_id = static_cast<uint32_t>(pmt.GetCellID());

        *(fec_word++) = (card_id << 26) | (crate_id << 21) |
                        (channel_id << 16) | (gtid & 0xffffUL);

        *(fec_word++) = ((static_cast<uint32_t>(pmt.GetQHS()) ^ 0x800) << 16) |
             (cell_id << 12) | ((static_cast<uint32_t>(pmt.GetQLX()) ^ 0x800));

        *(fec_word++) = ((gtid & 0x00f00000UL) << 8) |
            ((static_cast<uint32_t>(pmt.GetTime()) ^ 0x800) << 16) |
            ((gtid & 0x000f0000UL) >> 4) |
            ((static_cast<uint32_t>(pmt.GetQHL()) ^ 0x800));

        // fill in calibrated PmtEventRecord entries if available
        if (cal_pmt) {
            const RAT::DS::PMTCal& rpmtcal = ev.GetCalPMTs().GetAllPMT(i);
            cal_pmt->tac = rpmtcal.GetTime();
            cal_pmt->qhs = rpmtcal.GetQHS();
            cal_pmt->qhl = rpmtcal.GetQHL();
            cal_pmt->qlx = rpmtcal.GetQLX();
            cal_pmt++;
        }
    }

    return r;
}

////////////////////////////////////////////////////////////////////////
// pack::rhdr
RunRecord* pack::rhdr(RAT::DS::Run* const run) {
    int i;

    RunRecord* rr = new RunRecord;

    rr->Date = run->GetDate();
    rr->Time = run->GetTime();
    rr->SubRunNumber = run->GetSubRunID();
    rr->RunNumber = run->GetRunID();
    rr->CalibrationTrialNumber = 0; /* RAT doesn't currently store this. */
    rr->SourceMask = 0x0;//FIXME PHIL from db run->GetSrcMask();
    rr->RunMask = run->GetRunType();
    rr->GTCrateMsk = 0x0;//FIXME PHIL from db run->GetCrateMask();
    rr->FirstGTID = run->GetFirstGTID();
    rr->ValidGTID = run->GetValidGTID();

    for (i = 0; i < 8; i++) {
        rr->Spares[i] = 0;
    }

    return rr;
}

////////////////////////////////////////////////////////////////////////
// pack::trig
TriggerInfo* pack::trig(RAT::DS::TrigHeader* const triginfo,
                        const uint32_t gtid) {
    TriggerInfo* ti = new TriggerInfo;

    ti->TriggerMask = triginfo->GetTrigMask();

    ti->n100lo  = triginfo->GetTriggerThreshold(0);
    ti->n100med = triginfo->GetTriggerThreshold(1);
    ti->n100hi  = triginfo->GetTriggerThreshold(2);
    ti->n20     = triginfo->GetTriggerThreshold(3);
    ti->n20lb   = triginfo->GetTriggerThreshold(4);
    ti->esumlo  = triginfo->GetTriggerThreshold(5);
    ti->esumhi  = triginfo->GetTriggerThreshold(6);
    ti->owln    = triginfo->GetTriggerThreshold(7);
    ti->owlelo  = triginfo->GetTriggerThreshold(8);
    ti->owlehi  = triginfo->GetTriggerThreshold(9);

    ti->n100lo_zero  = triginfo->GetTriggerZeroOffset(0);
    ti->n100med_zero = triginfo->GetTriggerZeroOffset(1);
    ti->n100hi_zero  = triginfo->GetTriggerZeroOffset(2);
    ti->n20_zero     = triginfo->GetTriggerZeroOffset(3);
    ti->n20lb_zero   = triginfo->GetTriggerZeroOffset(4);
    ti->esumlo_zero  = triginfo->GetTriggerZeroOffset(5);
    ti->esumhi_zero  = triginfo->GetTriggerZeroOffset(6);
    ti->owln_zero    = triginfo->GetTriggerZeroOffset(7);
    ti->owlelo_zero  = triginfo->GetTriggerZeroOffset(8);
    ti->owlehi_zero  = triginfo->GetTriggerZeroOffset(9);

    ti->PulserRate = triginfo->GetPulserRate();
    ti->ControlRegister = triginfo->GetMTC_CSR();
    ti->reg_LockoutWidth = triginfo->GetLockoutWidth();
    ti->reg_Prescale = triginfo->GetPrescaleFrequency();
    ti->GTID = gtid;

    return ti;
}

////////////////////////////////////////////////////////////////////////
// pack::eped
EpedRecord* pack::eped(RAT::DS::ECAHeader* const epedinfo,
                       const uint32_t gtid) {
    EpedRecord* er = new EpedRecord;

    er->ped_width = epedinfo->GetChargePedestalWidth();
    er->ped_delay_coarse = epedinfo->GetGTDelayCoarse();
    er->ped_delay_fine = epedinfo->GetGTDelayFine();
    er->qinj_dacsetting = epedinfo->GetChargePedestalAmplitude();
    er->halfCrateID = epedinfo->GetPatternID();
    er->CalibrationType = epedinfo->GetCalType();
    er->GTID = gtid;
    er->Flag = 0;

    return er;
}

}  // namespace ratzdab
