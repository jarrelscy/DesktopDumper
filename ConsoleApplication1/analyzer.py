import sys
import json
from datetime import datetime
import pprint
def get_time(inst):
    return datetime.strptime(inst['time'], '%Y-%m-%d.%H:%M:%S')
    
if __name__ == '__main__':
    filename = sys.argv[1]
    print ('Use: python analyzer.py output.txt (or other output file created by ConsoleApplication1.exe)')
    data = json.loads('['+open(filename, 'r').read().strip(',\n')+']')
    total_time = {}
    for i in range(len(data)-1):
        inst = data[i]
        nextinst = data[i+1]
        interval = (get_time(nextinst) - get_time(inst)).total_seconds()
        if inst['active_desktop'] in inst:
            desktop_windows = [d for d in inst[inst['active_desktop']] if 'desktop_' in d[:8] and d[-10:] == ' - Notepad']
            if len(desktop_windows) > 0:
                info = desktop_windows[0]
                info = info.replace('.', '_')
                info = info.split('_')[1]
            else:
                info = 'unknown'
            if info not in total_time:
                total_time[info] = 0
            total_time[info] += interval
    pprint.pprint(total_time)
        