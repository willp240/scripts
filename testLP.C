void testLP(  )
{ 

    RAT::DB::Get()->SetAirplaneModeStatus(true);

    // RAT::DU::DSReader dsReader( "test.root" );

    RAT::DU::Utility::LoadDBAndBeginRun();
    
    RAT::LP::LightPathStraightScint::BeginOfRun();

    const RAT::DU::EffectiveVelocity& effVelocity = RAT::DU::Utility::Get()->GetEffectiveVelocity(); // To get the group velocity
    const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo(); // The PMT positions etc..
    
}
