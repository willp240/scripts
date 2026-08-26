#!/usr/bin/python

#############################################
# Calculate the livetime for an input run list.
# The following information is output:
# Total livetime
# Average livetime
# Livetime info per run
############################################
# Information needed to run this script:
#   A run list, usually the golden run list is used.
#   A rat environment.
###############################################

from __future__ import print_function
from __future__ import absolute_import
from __future__ import division
from __future__ import unicode_literals

from datetime import datetime, timedelta
try:
    input = raw_input  # Python 2
    import pytz as timezone
except NameError:
    from datetime import timezone
    pass  # Python 3
import argparse
import os
import sys
import glob
import numpy as np
from rat import ratdb
import rat

# contact ratdb
RATDB_USER='snoplus'
RATDB_PASS='dontestopmenow'
RATDB_SERVER='pgsql.snopl.us'
RATDB_PORT=5400

livetime_dic = {
'muon': 0.0,
'raw_livetime': 0.0,
'missedmuon': 0.0,
'livetime': 0.0,
'overlap': 0.0,
}

data_files = []

#Format KEY=run, VAL=2D array of vetos for run with data
#Veto data array in the form: GTID, Nhit, clockCount50, clockCount10, UTDays, UTSecs, UTNSecs, Veto length (in ns)
veto_dic = {}

do_file_checks = True
do_muon_veto = True
do_pot_muon_veto = True
do_bespoke_veto = True

# Constant to try and handle the big time shift that occurred for the 10 MHz clock in some data
BIG_TIME_SHIFT = datetime(2590, 3, 25, hour=5, minute=22, second=12, tzinfo=timezone.utc) - datetime(2023, 11, 7, hour=15, minute=22, second=15, tzinfo=timezone(timedelta(hours=-5), name='ET'))

def make_veto_dic(veto_dir, veto_tag=''):
    #Bespoke veto outputs are assumed to be contained in text files with
    #pattern below. The veto_tag variable can be used to differentiate
    #but avoid putting duplicate veto processing
    #for a single run in the same directory, or any other
    #files besides veto outputs for that matter.

    #This check is done when parsing the command line arguments,
    #but do it again for safety
    veto_dir = veto_dir if veto_dir[-1] == '/' else veto_dir + '/'
    veto_files = glob.glob(veto_dir + '*' + veto_tag + '*vetooutput.txt')

    for vfi in veto_files:
        veto_data = np.loadtxt(vfi,dtype=float,delimiter=' ', skiprows=1)
        if veto_data.ndim == 1 and len(veto_data) != 0:
            veto_dic[veto_data[0]] = [veto_data[1:]]
        else:
            for veto in veto_data:
                if veto[0] not in veto_dic:
                    veto_dic[veto[0]] = [veto[1:]]
                else:
                    veto_dic[veto[0]].append(veto[1:])


def parse_data_dirs(data_dir_arg):
    '''
    Convert comma/colon separated data directory arguments into a clean list.
    '''
    data_dirs = []
    for chunk in data_dir_arg.split(','):
        for path in chunk.split(os.pathsep):
            path = path.strip()
            if path:
                data_dirs.append(path if path[-1] == '/' else path + '/')
    return data_dirs


def fetch_tables(run_array, run_table_pass, muon_table_pass, ratdb_tag):
    '''
    Load in the RUN, LAST_MUON, and TPMUONFOLLOWER RATDB tables for ALL
    runs in the run_array at once! This should save a whole lot of time.
    ...Well, if you want to get LOADS of runs at once, the RATDB server won't
    like us very much, so we will split the requests up in that case.
    '''
    run_list = run_array.tolist()
    MAX_RUNS_PER_CALL = 500
    run_list_chunked = [run_list[i:i + MAX_RUNS_PER_CALL] for i in range(0, len(run_list), MAX_RUNS_PER_CALL)]
    ratdbLink = ratdb.RATDBConnector('postgres://'+RATDB_USER+':'+RATDB_PASS+'@'+RATDB_SERVER+'/ratdb', tag=ratdb_tag)
    print("LOADING ALL RUN TABLES...")
    run_tables = {}
    for idx, runs in enumerate(run_list_chunked):
        print(f"\tloading chunk {idx+1}/{len(run_list_chunked)}...")
        t = ratdbLink.fetchmult(obj_type='RUN', run_list=runs, pass_num=run_table_pass)
        run_tables.update(t)
    print("...LOADED.")
    if do_muon_veto:
        print("LOADING ALL LAST_MUON TABLES...")
        lastmuon_tables = {}
        for idx, runs in enumerate(run_list_chunked):
            print(f"\tloading chunk {idx+1}/{len(run_list_chunked)}...")
            t = ratdbLink.fetchmult(obj_type='LAST_MUON', run_list=runs, pass_num=muon_table_pass)
            lastmuon_tables.update(t)
        print("...LOADED.")
        print("LOADING ALL TPMUONFOLLOWER TABLES...")
        muon_tables = {}
        for idx, runs in enumerate(run_list_chunked):
            print(f"\tloading chunk {idx+1}/{len(run_list_chunked)}...")
            t = ratdbLink.fetchmult(obj_type='TPMUONFOLLOWER', run_list=runs, pass_num=muon_table_pass)
            muon_tables.update(t)
        print("...LOADED.")
    else:
        lastmuon_tables = None
        muon_tables = None

    return run_tables, lastmuon_tables, muon_tables


def get_tables_run(run_number, run_tables, lastmuon_tables, muon_tables):
    '''
    Return the RUN, LAST_MUON, and TPMUONFOLLOWER tables for the run requested.
    '''
    run_table = run_tables[(run_number, run_number)]
    if do_muon_veto:
        try:
            lastmuon_table = lastmuon_tables[(run_number, run_number)]
        except KeyError:
            # For whatever reason, LAST_MUON table not ran. That's okay, we can around this.
            lastmuon_table = None
        muon_table = muon_tables[(run_number, run_number)]
    else:
        lastmuon_table = None
        muon_table = None
    return run_table, lastmuon_table, muon_table


def determine_missing_subruns(run_number, subfile_list):
    '''
    Look at data files, determine if we have any subruns for this run missing.
    '''
    run_str = 'r{:010d}'.format(run_number)
    subrun_list = [run_str + '_s' + subfi.split('_')[-1].split('.zdab')[0] for subfi in subfile_list]

    missing_subs = []
    for subrun in subrun_list:
        if subrun not in data_files:
            print('Warning: Run ' + str(run_number) + ': Subrun not found: ' + subrun)
            missing_subs.append(subrun)
    return missing_subs


def calc_lastmuon_time(lastmuon_table, veto_times, epoch_time, muon_short_length, start_time):
    '''
    Determine when the last muon occured.
    '''
    if lastmuon_table is not None:
        lastmuon_table_data = lastmuon_table['data']
        if lastmuon_table_data['day_last_muon'] != -1:
            lm_day = lastmuon_table_data['day_last_muon']
            lm_sec = lastmuon_table_data['sec_last_muon']
            lm_ns = lastmuon_table_data['ns_last_muon']
            lastmuon_start_time = epoch_time + timedelta(days=lm_day, seconds=lm_sec, microseconds=lm_ns*1e-3)
        else:
            lastmuon_start_time = epoch_time
        lastmuon_end_time = lastmuon_start_time + muon_short_length
        lastmuon_start_time = correct_big_time_shift(lastmuon_start_time)
        lastmuon_end_time = correct_big_time_shift(lastmuon_end_time)
        if lastmuon_end_time > start_time:
            veto_times.append((lastmuon_start_time, lastmuon_end_time))
    else:
        # LAST_MUON table not present for this run.
        # Assume worst-case scenario: a muon happened right at run boundary!
        lastmuon_start_time = epoch_time
        lastmuon_start_time = correct_big_time_shift(lastmuon_start_time)
    
    return veto_times, lastmuon_start_time

def calc_pot_lastmuon_time(lastmuon_table, veto_times, epoch_time, muon_short_length, start_time, normal_lastmuon_start_time):
    '''
    Determine when the last potential muon occured.
    '''
    if lastmuon_table is not None:
        lastmuon_table_data = lastmuon_table['data']
        if 'day_last_potential_muon' not in lastmuon_table_data:
            # edge case: day_last_potential_muon not present in LAST_MUON table.
            # Just use value from main muon tagging
            return veto_times, normal_lastmuon_start_time
        if lastmuon_table_data['day_last_potential_muon'] != -1:
            lm_day = lastmuon_table_data['day_last_potential_muon']
            lm_sec = lastmuon_table_data['sec_last_potential_muon']
            lm_ns = lastmuon_table_data['ns_last_potential_muon']
            lastmuon_start_time = epoch_time + timedelta(days=lm_day, seconds=lm_sec, microseconds=lm_ns*1e-3)
        else:
            lastmuon_start_time = epoch_time
        lastmuon_end_time = lastmuon_start_time + muon_short_length
        lastmuon_start_time = correct_big_time_shift(lastmuon_start_time)
        lastmuon_end_time = correct_big_time_shift(lastmuon_end_time)
        if lastmuon_end_time > start_time:
            veto_times.append((lastmuon_start_time, lastmuon_end_time))
    else:
        # LAST_MUON table not present for this run.
        # Assume worst-case scenario: a muon happened right at run boundary!
        lastmuon_start_time = epoch_time
        lastmuon_start_time = correct_big_time_shift(lastmuon_start_time)
    
    return veto_times, lastmuon_start_time


def get_muon_veto_times(muon_table, lastmuon_start_time, muon_short_length, epoch_time, start_time, stop_time, run_number, veto_times):
    '''
    Extract the list of veto periods from the TPMUONFOLLOWER table.
    '''
    # Firstly, get the raw data out from the table
    if 'data' not in muon_table:
        # The TPMUONFOLLOWER table is empty/doesn't have the right entries!
        raise RuntimeError("Empty TPMUONFOLLOWER table")
    
    tpmuon_table_data = muon_table['data']
    
    if 'day_muons' not in tpmuon_table_data or 'sec_muons' not in tpmuon_table_data or 'ns_muons' not in tpmuon_table_data or 'gtid_muons' not in tpmuon_table_data:
        # The TPMUONFOLLOWER table is empty/doesn't have the right entries!
        raise RuntimeError("Empty TPMUONFOLLOWER table")
    
    tpmuon_days = tpmuon_table_data['day_muons']
    tpmuon_secs = tpmuon_table_data['sec_muons']
    tpmuon_ns = tpmuon_table_data['ns_muons']
    tpmuon_gtid = tpmuon_table_data['gtid_muons']

    is_bad_clock = False

    # Some basic sanity checks
    if tpmuon_days == -1:
        # Empty table! No tags.
        return veto_times, is_bad_clock
    if len(tpmuon_days) != len(tpmuon_secs) or len(tpmuon_days) != len(tpmuon_ns):
        print('Run ' + str(run_number) + ': Muon time arrays not the same length. Weird!!!')
    
    # Now loop over the tags to extract the timing info
    muon_veto_start_time_prev = lastmuon_start_time
    for day, sec, ns, gtid in zip(tpmuon_days, tpmuon_secs, tpmuon_ns, tpmuon_gtid):
        muon_veto_start_time = epoch_time + timedelta(days=day, seconds=sec, microseconds=ns*1e-3)
        muon_veto_start_time = correct_big_time_shift(muon_veto_start_time)
        # Annoying edge-case: 10 MHz clock can be "bad", being exactly 5 hours out,
        # relative to the start/end times in the RUN table
        # If we spot that, we can correct for it at least.
        if gtid == -1 and muon_veto_start_time <  start_time - timedelta(days=0, seconds=4.8*60*60, microseconds=0):
            is_bad_clock = True
            muon_veto_start_time += timedelta(days=0, seconds=5*60*60, microseconds=0)
            print('Warning: Run ' + str(run_number) + ': Initial muon veto time off by 5 hours (10MHz clock failure). Attempting to correct!!')
        #...Otherwise, treat the veto as you'd expect!
        muon_veto_end_time = muon_veto_start_time + muon_short_length

        # More edge-cases to worry about:
        # - If the muon times are not in order in the table, something weird has happened
        if muon_veto_start_time < muon_veto_start_time_prev:
            print('Run ' + str(run_number) + ': Muon veto start time decreased compared to previous muon veto in table. Weird!!!')
            print(muon_veto_start_time_prev, muon_veto_start_time)
        # - If the muon starts after the end of the run (ignore this muon)
        if muon_veto_start_time > stop_time:
            print('Warning: Run ' + str(run_number) + ': Muon veto start time, ' + str(muon_veto_start_time) + ', is after stop time of current run. Skipping.')
            continue
        # - If the muon ends before the start of the run (ignore this muon)
        if muon_veto_end_time < start_time:
            print('Warning: Run ' + str(run_number) + ': Muon veto end time, ' + str(muon_veto_end_time) + ', is before start time of current run. Skipping.')
            continue
        
        # add the veto interval to the list
        veto_times.append((muon_veto_start_time, muon_veto_end_time))
        muon_veto_start_time_prev = muon_veto_start_time
    
    return veto_times, is_bad_clock


def get_pot_muon_veto_times(muon_table, lastmuon_start_time, muon_short_length, epoch_time, start_time, stop_time, run_number, veto_times, is_bad_clock):
    '''
    Extract the list of potential muon veto periods from the TPMUONFOLLOWER table.
    '''
    # Firstly, get the raw data out from the table
    if 'data' not in muon_table:
        # The TPMUONFOLLOWER table is empty/doesn't have the right entries!
        print(muon_table)
        raise RuntimeError("Empty TPMUONFOLLOWER table")
    
    tpmuon_table_data = muon_table['data']
    
    if 'day_potential_muons' not in tpmuon_table_data or 'sec_potential_muons' not in tpmuon_table_data or 'ns_potential_muons' not in tpmuon_table_data or 'gtid_potential_muons' not in tpmuon_table_data:
        # The TPMUONFOLLOWER table is empty/doesn't have the right entries!
        print(muon_table)
        raise RuntimeError("Empty TPMUONFOLLOWER table")
    
    tpmuon_days = tpmuon_table_data['day_potential_muons']
    tpmuon_secs = tpmuon_table_data['sec_potential_muons']
    tpmuon_ns = tpmuon_table_data['ns_potential_muons']
    tpmuon_gtid = tpmuon_table_data['gtid_potential_muons']

    # Some basic sanity checks
    if tpmuon_days == -1:
        # Empty table! No tags.
        return veto_times, is_bad_clock
    if len(tpmuon_days) != len(tpmuon_secs) or len(tpmuon_days) != len(tpmuon_ns):
        print('Run ' + str(run_number) + ': Muon time arrays not the same length. Weird!!!')
    
    # Now loop over the tags to extract the timing info
    muon_veto_start_time_prev = lastmuon_start_time
    for day, sec, ns, gtid in zip(tpmuon_days, tpmuon_secs, tpmuon_ns, tpmuon_gtid):
        muon_veto_start_time = epoch_time + timedelta(days=day, seconds=sec, microseconds=ns*1e-3)
        muon_veto_start_time = correct_big_time_shift(muon_veto_start_time)
        # Annoying edge-case: 10 MHz clock can be "bad", being exactly 5 hours out,
        # relative to the start/end times in the RUN table
        # If we spot that, we can correct for it at least.
        if gtid == -1 and muon_veto_start_time <  start_time - timedelta(days=0, seconds=4.8*60*60, microseconds=0):
            is_bad_clock = True
            muon_veto_start_time += timedelta(days=0, seconds=5*60*60, microseconds=0)
            print('Warning: Run ' + str(run_number) + ': Initial potential muon veto time off by 5 hours (10MHz clock failure). Attempting to correct!!')
        #...Otherwise, treat the veto as you'd expect!
        muon_veto_end_time = muon_veto_start_time + muon_short_length

        # More edge-cases to worry about:
        # - If the muon times are not in order in the table, something weird has happened
        if muon_veto_start_time < muon_veto_start_time_prev:
            print('Run ' + str(run_number) + ': Potential muon veto start time decreased compared to previous muon veto in table. Weird!!!')
            print(muon_veto_start_time_prev, muon_veto_start_time)
        # - If the muon starts after the end of the run (ignore this muon)
        if muon_veto_start_time > stop_time:
            print('Warning: Run ' + str(run_number) + ': Potential muon veto start time, '+ str(muon_veto_start_time) + ', is after stop time of current run. Skipping.')
            continue
        # - If the muon ends before the start of the run (ignore this muon)
        if muon_veto_end_time < start_time:
            print('Warning: Run ' + str(run_number) + ': Potential muon veto end time, '+ str(muon_veto_end_time) +', is before start time of current run. Skipping.')
            continue
        
        # add the veto interval to the list
        veto_times.append((muon_veto_start_time, muon_veto_end_time))
        muon_veto_start_time_prev = muon_veto_start_time
    
    return veto_times, is_bad_clock


def get_bespoke_veto_times(run_number, veto_dic, veto_times, epoch_time, start_time, stop_time):
    '''
    Extract the list of "bespoke" veto times from the dictionary.
    '''
    # First look at the bespoke vetos from the /previous/ run, if there are any
    veto_start_time_prev = epoch_time
    table_veto_prev = veto_dic[run_number-1] if run_number-1 in veto_dic else []
    if table_veto_prev == []:
        print('Run ' + str(run_number) + ': Previous veto table empty!')
    # Check to see if any of those vetos enter into /this/ run!
    for veto in table_veto_prev:
        veto_start_time = epoch_time + timedelta(days=veto[4], seconds=veto[5], microseconds=veto[6]*1e-3)
        veto_start_time = correct_big_time_shift(veto_start_time)
        veto_end_time = veto_start_time + timedelta(microseconds=veto[7]*1e-3)
        # Check for possible cases where we don't care:
        if veto[4] == 0 and veto[5] == 0 and veto[6] == 0:
            print('Warning: Run ' + str(run_number) + ': Previous run bespoke veto time not valid. Skipping.')
            continue
        if veto_start_time > start_time:
            print('Warning: Run ' + str(run_number) + ': Previous run bespoke veto start time is after start time of current run. Skipping.')
            continue
        if veto_start_time > stop_time:
            print('Warning: Run ' + str(run_number) + ': Previous run bespoke veto start time is after stop time of current run. Skipping.')
            continue
        if veto_start_time < veto_start_time_prev:
            print('Run ' + str(run_number) + ': Previous run bespoke veto start time decreased compared to previous bespoke veto in table. Weird!!!')
        if veto_end_time > start_time:
            # Okay, we do care here!
            veto_times.append((veto_start_time, veto_end_time))
            veto_start_time_prev = veto_start_time

    # Now look at /this/ run's bespoke vetos, add them to the list
    table_veto = veto_dic[run_number] if run_number in veto_dic else []
    if table_veto == []:
        print('Run ' + str(run_number) + ': Veto table empty!')

    for veto in table_veto:
        veto_start_time = epoch_time + timedelta(days=veto[4], seconds=veto[5], microseconds=veto[6]*1e-3)
        veto_start_time = correct_big_time_shift(veto_start_time)
        veto_end_time = veto_start_time + timedelta(microseconds=veto[7]*1e-3)
        # Handle edge cases:
        # - If UT time of event is all zeroes, ignore
        if veto[4] == 0 and veto[5] == 0 and veto[6] == 0:
            print('Warning: Run ' + str(run_number) + ': Current run bespoke veto time not valid. Skipping.')
            continue
        # - If beginning of veto happens after end of run, ignore
        if veto_end_time < start_time:
            print('Warning: Run ' + str(run_number) + ': Current run bespoke veto end time is before the start time of current run. Skipping.')
            continue
        # - If end of veto happens before start of run, ignore
        if veto_start_time > stop_time:
            print('Warning: Run ' + str(run_number) + ': Current run bespoke veto start time is after stop time of current run. Skipping.')
            continue
        # - If this veto starts before the last one, that's a bit odd. Fire a warning.
        if veto_start_time < veto_start_time_prev:
            print('Run ' + str(run_number) + ': Current run bespoke veto start time decreased compared to previous bespoke veto in table. Weird!!!')
        # Add the veto to the list
        veto_times.append((veto_start_time, veto_end_time))
        veto_start_time_prev = veto_start_time
    
    return veto_times


def get_time_correction(veto_times, time_correction, start_time, stop_time):
    '''
    Determine the livetime loss due to all the veto periods.
    We're being careful here to deal correctly with overlapping veto periods,
    and the run boundaries.
    '''
    veto_chain_start_time = veto_times[0][0] if veto_times[0][0] > start_time else start_time
    veto_chain_end_time = veto_times[0][1] if veto_times[0][1] < stop_time else stop_time
    for veto in veto_times[1:]:
        #The proposed next veto in the chain starts at the next veto time (vetoes are sorted in ascending order)
        #and ends either at the next veto end or the end of the run, whichever is sooner
        next_veto_start = veto[0]
        next_veto_end = veto[1] if veto[1] < stop_time else stop_time

        #First, check whether the proposed start time is less than the end of the current chain
        #and whether the proposed end time is later than the end of the current chain
        if next_veto_start < veto_chain_end_time and next_veto_end >= veto_chain_end_time:
            #If so, the new chain end time is the next veto end (start remains the same)
            veto_chain_end_time = next_veto_end
        #Second, check whether the proposed start time is less than the end of the current chain
        #and whether the proposed end time is sooner than the end of the current chain
        elif next_veto_start < veto_chain_end_time and next_veto_end < veto_chain_end_time:
            #If so, this veto is entirely within the chain already, so we just continue
            continue
        #Last, this means the next veto start is after the current chain ends
        #So we add the time correction and start a new chain starting with this veto
        else:
            time_correction += (veto_chain_end_time - veto_chain_start_time)
            veto_chain_start_time = next_veto_start
            veto_chain_end_time = next_veto_end
        #If the veto chain is ending equal to or past the run end time,
        #then no more deadtime can be accrued and we end the loop
        if veto_chain_end_time >= stop_time:
            veto_chain_end_time = stop_time
            break

    #Need to close out loop since last chain will not be added
    time_correction += (veto_chain_end_time - veto_chain_start_time)

    return time_correction


def correct_big_time_shift(time):
    '''
    If time given is way into the future, correct it with a big fixed time shift,
    that has been determined by hand - looking at run 353207.
    '''
    if time > datetime(2590, 1, 1, tzinfo=timezone.utc):
        time -= BIG_TIME_SHIFT
    return time


def calc_livetime(run_number, run_table, lastmuon_table, muon_table):
    '''
    Check if the run table exists for a specified run.
    If it does, calculate the various livetime values and write to a file.
    '''
    # First: extract basic info from RUN table
    if 'data' not in run_table:
        # RUN table doesn't exist/ not populated correctly
        print(run_table)
        return False, 'Table Not Found!', [], False
    
    run_table_data = run_table['data']
    
    if 'subfile_list' not in run_table_data or 'start_day' not in run_table_data or 'start_sec' not in run_table_data or 'start_nsc' not in run_table_data or 'stop_day' not in run_table_data or 'stop_sec' not in run_table_data or 'stop_nsc' not in run_table_data:
        # RUN table doesn't exist/ not populated correctly
        print(run_table_data)
        return False, 'Table Not Found!', [], False
    
    subfile_list = run_table_data['subfile_list']        
    start_day = run_table_data['start_day']
    start_sec = run_table_data['start_sec']
    start_nsc = run_table_data['start_nsc']
    stop_day = run_table_data['stop_day']
    stop_sec = run_table_data['stop_sec']
    stop_nsc = run_table_data['stop_nsc']

    print('Run ' + str(run_number) + ': Beginning checks!')

    # Check if there are any missing subrun files (If user wants to)
    missing_subruns = []
    if do_file_checks:
        missing_subruns = determine_missing_subruns(run_number, subfile_list)

    #Check for clock badness
    is_bad_clock = False

    #~~~~~~~~ STEP 1: CALCULATE RAW LIVETIME~~~~~~~~~~~~~~~~
    #SNO+ time counts from 1 Jan 2010 at 0:00 GMT (UTC 0)
    epoch_time = datetime(2010, 1, 1, tzinfo=timezone.utc)
    start_time = epoch_time + timedelta(days=start_day, seconds=start_sec, microseconds=start_nsc*1e-3)
    stop_time = epoch_time + timedelta(days=stop_day, seconds=stop_sec, microseconds=stop_nsc*1e-3)
    raw_run_len = (stop_time - start_time).total_seconds()/60/60/24
    print(start_time)
    print(stop_time)
    start_time = correct_big_time_shift(start_time)
    stop_time = correct_big_time_shift(stop_time)
    print(start_time)
    print(stop_time)

    time_correction = timedelta()
    
    #~~~~~~~~ STEP 2: Do veto corrections (muon + high nhit) ~~~~~~
    #         (If user wants to)
    veto_times = []

    #Gathers muon veto times from LAST_MUON and TPMUONFOLLOWER tables
    if do_muon_veto:
        #Currently only consider short time, as long is unused as of Jan 2024
        #Will need to add new veto logic for long if revived
        muon_short_length = timedelta(seconds=20)
        # First work out when last muon occured before start of this run
        veto_times, lastmuon_start_time = calc_lastmuon_time(lastmuon_table, veto_times, epoch_time, muon_short_length, start_time)
        # Then look at the tagged muons from TPMUONFOLLOWER
        try:
            veto_times, is_bad_clock = get_muon_veto_times(muon_table, lastmuon_start_time, muon_short_length, epoch_time, start_time, stop_time, run_number, veto_times)
        except RuntimeError:
            # If TPMUONFOLLOWER table not present/empty etc, exit early
            return False, 'Table Not Found!', [], False
    #Gathers potential muon veto times, also from the LAST_MUON and TPMUONFOLLOWER tables
    if do_pot_muon_veto:
        #Currently only consider short time, as long is unused as of Jan 2024
        #Will need to add new veto logic for long if revived
        muon_short_length = timedelta(seconds=20)
        # First work out when last potential muon occured before start of this run
        veto_times, pot_lastmuon_start_time = calc_pot_lastmuon_time(lastmuon_table, veto_times, epoch_time, muon_short_length, start_time, lastmuon_start_time)
        # Then look at the tagged potential muons from TPMUONFOLLOWER
        try:
            veto_times, is_bad_clock = get_pot_muon_veto_times(muon_table, pot_lastmuon_start_time, muon_short_length, epoch_time, start_time, stop_time, run_number, veto_times, is_bad_clock)
        except RuntimeError:
            # If TPMUONFOLLOWER table not present/empty etc, exit early
            return False, 'Table Not Found!', [], False
    #Gathers bespoke veto times from specified output files
    if do_bespoke_veto:
        veto_times = get_bespoke_veto_times(run_number, veto_dic, veto_times, epoch_time, start_time, stop_time)

    #Sort the vetos so that they are in time order
    #Assumes that all vetos should be treated equally
    #Can add hiearchical logic later if needed
    veto_times = sorted(veto_times)

    print('Start: ' + str(start_time))
    print('End: ' + str(stop_time))
    print ('Veto periods:')
    [print(str(veto_time[0]) + ' to ' + str(veto_time[1])) for veto_time in veto_times]

    # If there are any veto periods, work out the livetime loss
    if len(veto_times) != 0:
        time_correction = get_time_correction(veto_times, time_correction, start_time, stop_time)
    # convert into days, calculate net livetime
    time_correction_len = time_correction.total_seconds()/60/60/24
    corrected_run_len = raw_run_len - time_correction_len
    # write result to dictionary, do final checks
    livetime_dic['raw_livetime'] += raw_run_len
    livetime_dic['livetime'] += corrected_run_len
    writestring = str(run_number) + ' ' + str(raw_run_len) + ' ' + str(corrected_run_len)
    if raw_run_len < corrected_run_len:
        print('Run ' + str(run_number) + ': Raw livetime less than corrected livetime. Weird!!!')
        is_bad_clock = True
    return True, writestring, missing_subruns, is_bad_clock


if __name__=='__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--input', '-i',type = str,
                        help='Input filelist containing line separated run numbers.')
    parser.add_argument('--data_dir', '-d', type = str, default = os.getcwd(),
                        help='Directory/directories containing data files (assumed ntuple, default is current directory). Multiple directories may be separated by commas or colons.')
    parser.add_argument('--lower_range', '-l', type = int, default = 0,
                        help='Lowest run of range to calculate livetime.')
    parser.add_argument('--upper_range', '-u', type = int, default = 9999999,
                        help='Uppermost run of range to calculate livetime.')
    parser.add_argument('--run_table_pass', type = int, default = -1,
                        help='Pass to use for the RUN table. Default is -1, which loads most recent table.')
    parser.add_argument('--muon_table_pass', type = int, default = -1,
                        help='Pass to use for the LAST_MUON, TPMUONFOLLOWER, and  tables. Default is -1, which loads most recent table.')
    parser.add_argument('--ratdb_tag', type=str, default=None,
                        help='Name of the RATDB tag to use.')
    parser.add_argument('--tag', '-t', type = str, default = 'defaultlivetime',
                        help='Name of the livetime calculation trial defined by user.')
    parser.add_argument('--output_dir', '-o', type = str, default = os.getcwd(),
                        help='Directory to place output files in, by default the current directory.')
    parser.add_argument('--veto_dir', type = str, default = os.getcwd(),
                        help='Directory to search for bespoke veto text files, by default the current directory.')
    parser.add_argument('--veto_tag', type = str, default = '',
                        help='Identifying tag to search for bespoke veto text files, by default empty string.')
    parser.add_argument('--turn_off_file_checks', action='store_true',
                        help='Whether to turn off checks for missing files or subruns and multiple passes of same file (default is ON)')
    parser.add_argument('--turn_off_muon_veto', action='store_true',
                        help='Whether to turn off deadtime calculation from muon veto (default is ON)')
    parser.add_argument('--turn_off_pot_muon_veto', action='store_true',
                        help='Whether to turn off deadtime calculation from potential muon veto (default is ON)')
    parser.add_argument('--turn_off_bespoke_veto', action='store_true',
                        help='Whether to turn off deadtime calculation from bespoke veto (default is ON)')
    args = parser.parse_args()

    do_file_checks = not args.turn_off_file_checks
    if do_file_checks:
        print('Performing file checks.')
    else:
        print('Not performing file checks.')
    do_muon_veto = not args.turn_off_muon_veto
    if do_muon_veto:
        print('Including muon veto in livetime calculation.')
    else:
        print('Not including muon veto in livetime calculation.')
    do_pot_muon_veto = not args.turn_off_pot_muon_veto
    if do_pot_muon_veto:
        print('Including potential muon veto in livetime calculation.')
    else:
        print('Not including potential muon veto in livetime calculation.')
    do_bespoke_veto = not args.turn_off_bespoke_veto
    if do_bespoke_veto:
        print('Including bespoke veto in livetime calculation.')
    else:
        print('Not including bespoke veto in livetime calculation.')

    if not RATDB_PASS:
        print('No RATDB password specified. Please fill in.')
        sys.exit()

    # Create a new directory to write out the livetime information
    outdir = args.output_dir + '/' + args.tag + '/'
    if not os.path.exists(outdir):
        os.mkdir(outdir)

    # Grab data files in appropriate directory/directories and truncate off extra info
    data_dirs = parse_data_dirs(args.data_dir)
    data_files = []
    for data_dir in data_dirs:
        data_files.extend(glob.glob(data_dir + 'Analysis*.ntuple.root'))
    data_files = ['r0'+fi.split('.ntuple.root')[0].split('_r0')[-1] for fi in data_files]

    # Open livetime information files
    fmisrun = open(outdir + 'missing_runs_' + str(args.tag) + '.txt','w')
    fmissub = open(outdir + 'missing_subruns_' + str(args.tag) + '.txt','w')
    fmulpass = open(outdir + 'multiple_passes_' + str(args.tag) + '.txt','w')
    fbadclock = open(outdir + 'bad_clock_runs_' + str(args.tag) + '.txt','w')
    ftot = open(outdir + 'livetime_total_'   + str(args.tag) + '.txt','w')
    favg = open(outdir + 'livetime_average_' + str(args.tag) + '.txt','w')
    frun = open(outdir + 'livetime_per_run_' + str(args.tag) + '.txt','w')

    # Get the run list from the input file
    with open(args.input, 'r') as fruns:
        run_list = [int(line) for line in fruns]

    # Subset of the run list using the option run-range
    lower_run = args.lower_range
    upper_run = args.upper_range
    run_array = np.array(run_list)
    run_array = run_array[np.logical_and(run_array>=lower_run, run_array<=upper_run)]
    total_run = run_array.flatten().shape[0]

    if do_file_checks:
        #Check that there are files for every run in the list and that there are no overlapping passes
        for i, run in enumerate(run_array):
            run_files = [fi for fi in data_files if 'r0000'+str(run) in fi]
            #Check whether files exist for run
            if len(run_files) == 0:
                print('No files for run: ' + str(run))
                fmisrun.write(str(run) + '\n')

            #Check no overlapping passes
            subruns = set([int(fi.split('_s')[-1].split('_p')[0]) for fi in run_files])
            pass_files = [[fi for fi in run_files if 'r0000'+str(run)+'_s'+'{:03d}'.format(subrun) in fi] for subrun in subruns]
            multi_pass = []
            pass_file_lens = [(print('Warning: Multiple passes: ' + str(files)), multi_pass.append(files)) for files in pass_files if len(files) > 1]
            [fmulpass.write(' '.join(files) + '\n') for files in multi_pass]
    data_files = [fi.split('_p')[0] for fi in data_files]

    #Make veto dictionary from offline files
    veto_dir = args.veto_dir if args.veto_dir[-1] == '/' else args.veto_dir + '/'
    make_veto_dic(args.veto_dir, args.veto_tag)

    # Access the RATDB database all in one go for all the runs for the tables we need
    # Takes more memory, but is a LOT faster (I'm tired of waiting hours for calculating the livetime)
    run_tables, lastmuon_tables, muon_tables = fetch_tables(run_array, args.run_table_pass, args.muon_table_pass, args.ratdb_tag)
    # Loop over the runs and get the information from the livetime table
    print('Calculating livetime for run-list...')
    print('Format: Index, Run, Raw Livetime (days), Corrected Livetime (days), Missing Subruns, Bad Clock Flag')
    print('---------------------------------------------------------------------------------------------------')
    for i, run in enumerate(run_array):
        run_table, lastmuon_table, muon_table = get_tables_run(run, run_tables, lastmuon_tables, muon_tables)
        table_flag, writestring, missing_subruns, is_bad_clock = calc_livetime(run, run_table, lastmuon_table, muon_table)
        print(i, run, writestring, missing_subruns, is_bad_clock)

        if len(missing_subruns) != 0:
            print('Warning: Run ' + str(run) + ' has missing subruns in specified directory. Review output file.')
            fmissub.write('\n'.join(missing_subruns) + '\n')
        if table_flag:
            frun.write(writestring + '\n')
            if is_bad_clock:
                print('Warning: Run ' + str(run) + ' was flagged as having bad clock behavior.\n'
                      'Correction may have been attempted or veto was skipped. '
                      'Check calculator log for details.')
                fbadclock.write(str(run) + '\n')
        else:
            print('Warning: Run ' + str(run) + ' was not found in specified directory. Review output file.')
            fmisrun.write(str(run) + '\n')
        print('---------------------------------------------------------------------------------------------------')
        print('---------------------------------------------------------------------------------------------------')

    print('Completed livetime calculations!')
    print('Raw livetime total: ' + str(livetime_dic['raw_livetime']) + ' - Corrected livetime total: ' + str(livetime_dic['livetime']))
    print('Check output carefully for warnings!')
    for key, val in livetime_dic.items():
        ftot.write(str(key) + ': ' + str(val) + '\n')
        favg.write(str(key) + ': ' + str(val / total_run) + '\n')

    fmissub.close()
    fbadclock.close()
    fmulpass.close()
    fmisrun.close()
    frun.close()
    ftot.close()
    favg.close()
    print('Finished!')
