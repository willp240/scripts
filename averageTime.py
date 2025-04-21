import os
import re

def get_scall_times(directory):
    scall_times = []
    pattern = re.compile(r'scintFitter:\s+DSEvent:\s+\d+\s+calls,\s+([\d\.]+)s/call,')
    
    for filename in os.listdir(directory):
        filepath = os.path.join(directory, filename)
        if os.path.isfile(filepath):
            with open(filepath, 'r') as file:
                for line in file:
                    match = pattern.search(line)
                    if match:
                        scall_times.append(float(match.group(1)))
                        print(match.group(1), filename)
                        break  # Stop reading once we've found the relevant line
    
    if scall_times:
        average_scall = sum(scall_times) / len(scall_times)
        print(f'Average s/call: {average_scall:.5f}')
    else:
        print('No matching lines found.')

# Change this to your target directory
directory = "/data/snoplus/parkerw/ratSimulations/Mar31_SFtime/output"
get_scall_times(directory)
